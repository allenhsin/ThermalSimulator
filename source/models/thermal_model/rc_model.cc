
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <cassert>
#include <vector>

#include "source/models/thermal_model/rc_model.h"
#include "source/models/thermal_model/floorplan.h"
#include "source/models/thermal_model/thermal_constants.h"
#include "source/misc/misc.h"
#include "libutil/Log.h"

using namespace std::vector;

namespace Thermal
{

    RCModel::RCModel()
        : _thermal_params   (NULL)
        , _floorplan_holder (NULL)
        , _rc_model_holder  (NULL)
    {}

    RCModel::~RCModel()
    {
        if(_rc_model_holder)
            delete _rc_model_holder;
    }

    void RCModel::setThermalParameters(ThermalParameters* thermal_params)
    { _thermal_params = thermal_params; }

    void RCModel::setFloorplanHolder(FloorplanHolder* floorplan_holder)
    { _floorplan_holder = floorplan_holder; }
    
    double RCModel::getR(double conductivity, double thickness, double area)
    { return thickness / (conductivity * area); }

    double RCModel::getCap(double sp_heat, double thickness, double area)
    { return C_FACTOR * sp_heat * thickness * area; }


    // LUP decomposition from the pseudocode given in the CLR 
    // 'Introduction to Algorithms' textbook. The matrix 'a' is
    // transformed into an in-place lower/upper triangular matrix
    // and the vector'p' carries the permutation vector such that
    // Pa = lu, where 'P' is the matrix form of 'p'. The 'spd' flag 
    // indicates that 'a' is symmetric and positive definite 
    void RCModel::lupDcmp(vector< vector<double> >& a, int n, vector<int>& p, int spd)
    {
        int i, j, k, pivot=0;
        double max = 0;
    
        /* start with identity permutation  */
        for (i=0; i < n; i++)
            p[i] = i;
    
        for (k=0; k < n-1; k++)  
        {
            max = 0;
            for (i = k; i < n; i++) 
            {
                if (fabs(a[i][k]) > max) 
                {
                    max = fabs(a[i][k]);
                    pivot = i;
                }
            }   
            if (eq (max, 0))
                fatal ("singular matrix in lupdcmp\n");
    
            /* bring pivot element to position  */
            swap_ival (&p[k], &p[pivot]);
            for (i=0; i < n; i++)
                Misc::swapDoubleValue(&a[k][i], &a[pivot][i]);
    
            for (i=k+1; i < n; i++) 
            {
                a[i][k] /= a[k][k];
                for (j=k+1; j < n; j++)
                    a[i][j] -= a[i][k] * a[k][j];
            }
        }
    }

    void RCModel::allocateRCModelHolder()
    {
        assert(_floorplan_holder);
        
        _rc_model_holder = new RCModelHolder();
        assert(_rc_model_holder);

        // number floorplan units
        int n = _floorplan_holder->_n_units;
        // number total rc model nodes
        int m = NL*n+EXTRA;
        
        _rc_model_holder->n_units = n;
        _rc_model_holder->base_n_units = n;
        _rc_model_holder->n_nodes = m;
        
        // ready are all not set
        _rc_model_holder->r_ready = 0;
        _rc_model_holder->c_ready = 0;

        Misc::initDouble2DVector(   _rc_model_holder->b,            m, m); // conductance matrix
        Misc::initDouble2DVector(   _rc_model_holder->lu,           m, m); // LUP decomposition of b
        Misc::initDouble3DVector(   _rc_model_holder->lu_step,      N_TIME_STEPS, m, m); // for precomputed lupdcmp at several different time step sizes
        Misc::initInt1DVector   (   _rc_model_holder->p,            m); // permutation vector for b's LUP decomposition
        Misc::initInt2DVector   (   _rc_model_holder->p_step,       N_TIME_STEPS, m);

        Misc::initDouble2DVector(   _rc_model_holder->geq_step,     N_TIME_STEPS, m);
        Misc::initDouble1DVector(   _rc_model_holder->time_steps,   N_TIME_STEPS);
        
        Misc::initDouble1DVector(   _rc_model_holder->a,            m); // vertical Cs - diagonal matrix stored as a 1-d vector
        Misc::initDouble1DVector(   _rc_model_holder->inva,         m); // inverse of a
        Misc::initDouble2DVector(   _rc_model_holder->c,            m ,m);
        
        Misc::initDouble1DVector(   _rc_model_holder->gx,           n); // lumped conductances in x direction
        Misc::initDouble1DVector(   _rc_model_holder->gy,           n); // lumped conductances in y direction
        Misc::initDouble1DVector(   _rc_model_holder->gx_int,       n); // lateral conductances in the interface layer
        Misc::initDouble1DVector(   _rc_model_holder->gy_int,       n);
        Misc::initDouble1DVector(   _rc_model_holder->gx_sp,        n); // lateral conductances in the spreader layer
        Misc::initDouble1DVector(   _rc_model_holder->gy_sp,        n);
        Misc::initDouble1DVector(   _rc_model_holder->gx_hs,        n); // lateral conductances in the heatsink layer
        Misc::initDouble1DVector(   _rc_model_holder->gy_hs,        n);
        Misc::initDouble1DVector(   _rc_model_holder->g_amb,        n+EXTRA); // vertical conductances to ambient
        Misc::initDouble1DVector(   _rc_model_holder->t_vector,     m); // scratch pad
       
        Misc::initDouble2DVector(   _rc_model_holder->len,          n, n); // len[i][j] = length of shared edge between i & j
        Misc::initDouble2DVector(   _rc_model_holder->g,            m, m); // g[i][j] = conductance between nodes i & j
        Misc::initInt2DVector   (   _rc_model_holder->border,       n ,4);
    } // allocateRCModelHolder

    void RCModel::populatePackageR(double chip_width, double chip_height)
    {
        assert(_thermal_params);
        assert(_rc_model_holder);

        double s_spreader   = _thermal_params->s_spreader;
        double t_spreader   = _thermal_params->t_spreader;
        double s_sink       = _thermal_params->s_sink;
        double t_sink       = _thermal_params->t_sink;
        double r_convec     = _thermal_params->r_convec;
     
        double s_sub        = _thermal_params->s_sub;
        double t_sub        = _thermal_params->t_sub;
        double s_solder     = _thermal_params->s_solder;
        double t_solder     = _thermal_params->t_solder;
        double s_pcb        = _thermal_params->s_pcb;
        double t_pcb        = _thermal_params->t_pcb;
        double r_convec_sec = _thermal_params->r_convec_sec;
        
        double k_sink       = _thermal_params->k_sink;
        double k_spreader   = _thermal_params->k_spreader;
     
    
        // lateral R's of spreader and sink  
        _rc_model_holder->pack.r_sp1_x = getR(k_spreader, (s_spreader-chip_width)/4.0, (s_spreader+3*chip_height)/4.0 * t_spreader);
        _rc_model_holder->pack.r_sp1_y = getR(k_spreader, (s_spreader-chip_height)/4.0, (s_spreader+3*chip_width)/4.0 * t_spreader);
        _rc_model_holder->pack.r_hs1_x = getR(k_sink, (s_spreader-chip_width)/4.0, (s_spreader+3*chip_height)/4.0 * t_sink);
        _rc_model_holder->pack.r_hs1_y = getR(k_sink, (s_spreader-chip_height)/4.0, (s_spreader+3*chip_width)/4.0 * t_sink);
        _rc_model_holder->pack.r_hs2_x = getR(k_sink, (s_spreader-chip_width)/4.0, (3*s_spreader+chip_height)/4.0 * t_sink);
        _rc_model_holder->pack.r_hs2_y = getR(k_sink, (s_spreader-chip_height)/4.0, (3*s_spreader+chip_width)/4.0 * t_sink);
        _rc_model_holder->pack.r_hs = getR(k_sink, (s_sink-s_spreader)/4.0, (s_sink+3*s_spreader)/4.0 * t_sink);
    
        // vertical R's of spreader and sink  
        _rc_model_holder->pack.r_sp_per_x = getR(k_spreader, t_spreader, (s_spreader+chip_height) * (s_spreader-chip_width) / 4.0);
        _rc_model_holder->pack.r_sp_per_y = getR(k_spreader, t_spreader, (s_spreader+chip_width) * (s_spreader-chip_height) / 4.0);
        _rc_model_holder->pack.r_hs_c_per_x = getR(k_sink, t_sink, (s_spreader+chip_height) * (s_spreader-chip_width) / 4.0);
        _rc_model_holder->pack.r_hs_c_per_y = getR(k_sink, t_sink, (s_spreader+chip_width) * (s_spreader-chip_height) / 4.0);
        _rc_model_holder->pack.r_hs_per = getR(k_sink, t_sink, (s_sink*s_sink - s_spreader*s_spreader) / 4.0);
        
        // vertical R's to ambient (divide r_convec proportional to area)  
        _rc_model_holder->pack.r_amb_c_per_x = r_convec * (s_sink * s_sink) / ((s_spreader+chip_height) * (s_spreader-chip_width) / 4.0);
        _rc_model_holder->pack.r_amb_c_per_y = r_convec * (s_sink * s_sink) / ((s_spreader+chip_width) * (s_spreader-chip_height) / 4.0);
        _rc_model_holder->pack.r_amb_per = r_convec * (s_sink * s_sink) / ((s_sink*s_sink - s_spreader*s_spreader) / 4.0);
        
        // lateral R's of package substrate, solder and PCB  
        _rc_model_holder->pack.r_sub1_x = getR(K_SUB, (s_sub-chip_width)/4.0, (s_sub+3*chip_height)/4.0 * t_sub);
        _rc_model_holder->pack.r_sub1_y = getR(K_SUB, (s_sub-chip_height)/4.0, (s_sub+3*chip_width)/4.0 * t_sub);
        _rc_model_holder->pack.r_solder1_x = getR(K_SOLDER, (s_solder-chip_width)/4.0, (s_solder+3*chip_height)/4.0 * t_solder);
        _rc_model_holder->pack.r_solder1_y = getR(K_SOLDER, (s_solder-chip_height)/4.0, (s_solder+3*chip_width)/4.0 * t_solder);
        _rc_model_holder->pack.r_pcb1_x = getR(K_PCB, (s_solder-chip_width)/4.0, (s_solder+3*chip_height)/4.0 * t_pcb);
        _rc_model_holder->pack.r_pcb1_y = getR(K_PCB, (s_solder-chip_height)/4.0, (s_solder+3*chip_width)/4.0 * t_pcb);
        _rc_model_holder->pack.r_pcb2_x = getR(K_PCB, (s_solder-chip_width)/4.0, (3*s_solder+chip_height)/4.0 * t_pcb);
        _rc_model_holder->pack.r_pcb2_y = getR(K_PCB, (s_solder-chip_height)/4.0, (3*s_solder+chip_width)/4.0 * t_pcb);
        _rc_model_holder->pack.r_pcb = getR(K_PCB, (s_pcb-s_solder)/4.0, (s_pcb+3*s_solder)/4.0 * t_pcb);
    
        // vertical R's of package substrate, solder balls and PCB  
        _rc_model_holder->pack.r_sub_per_x = getR(K_SUB, t_sub, (s_sub+chip_height) * (s_sub-chip_width) / 4.0);
        _rc_model_holder->pack.r_sub_per_y = getR(K_SUB, t_sub, (s_sub+chip_width) * (s_sub-chip_height) / 4.0);
        _rc_model_holder->pack.r_solder_per_x = getR(K_SOLDER, t_solder, (s_solder+chip_height) * (s_solder-chip_width) / 4.0);
        _rc_model_holder->pack.r_solder_per_y = getR(K_SOLDER, t_solder, (s_solder+chip_width) * (s_solder-chip_height) / 4.0);
        _rc_model_holder->pack.r_pcb_c_per_x = getR(K_PCB, t_pcb, (s_solder+chip_height) * (s_solder-chip_width) / 4.0);
        _rc_model_holder->pack.r_pcb_c_per_y = getR(K_PCB, t_pcb, (s_solder+chip_width) * (s_solder-chip_height) / 4.0);
        _rc_model_holder->pack.r_pcb_per = getR(K_PCB, t_pcb, (s_pcb*s_pcb - s_solder*s_solder) / 4.0);
        
        // vertical R's to ambient at PCB (divide r_convec_sec proportional to area)  
        _rc_model_holder->pack.r_amb_sec_c_per_x = r_convec_sec * (s_pcb * s_pcb) / ((s_solder+chip_height) * (s_solder-chip_width) / 4.0);
        _rc_model_holder->pack.r_amb_sec_c_per_y = r_convec_sec * (s_pcb * s_pcb) / ((s_solder+chip_width) * (s_solder-chip_height) / 4.0);
        _rc_model_holder->pack.r_amb_sec_per = r_convec_sec * (s_pcb * s_pcb) / ((s_pcb*s_pcb - s_solder*s_solder) / 4.0);

    } // populatePackageR

    void RCModel::populatePackageC(double chip_width, double chip_height)
    {
        assert(_thermal_params);
        assert(_rc_model_holder);

        double s_spreader   = _thermal_params->s_spreader;
        double t_spreader   = _thermal_params->t_spreader;
        double s_sink       = _thermal_params->s_sink;
        double t_sink       = _thermal_params->t_sink;
        double c_convec     = _thermal_params->c_convec;
     
        double s_sub        = _thermal_params->s_sub;
        double t_sub        = _thermal_params->t_sub;
        double s_solder     = _thermal_params->s_solder;
        double t_solder     = _thermal_params->t_solder;
        double s_pcb        = _thermal_params->s_pcb;
        double t_pcb        = _thermal_params->t_pcb;
        double c_convec_sec = _thermal_params->c_convec_sec;
        
        double p_sink       = _thermal_params->p_sink;
        double p_spreader   = _thermal_params->p_spreader; 
    
        // vertical C's of spreader and sink  
        _rc_model_holder->pack.c_sp_per_x = getCap(p_spreader, t_spreader, (s_spreader+chip_height) * (s_spreader-chip_width) / 4.0);
        _rc_model_holder->pack.c_sp_per_y = getCap(p_spreader, t_spreader, (s_spreader+chip_width) * (s_spreader-chip_height) / 4.0);
        _rc_model_holder->pack.c_hs_c_per_x = getCap(p_sink, t_sink, (s_spreader+chip_height) * (s_spreader-chip_width) / 4.0);
        _rc_model_holder->pack.c_hs_c_per_y = getCap(p_sink, t_sink, (s_spreader+chip_width) * (s_spreader-chip_height) / 4.0);
        _rc_model_holder->pack.c_hs_per = getCap(p_sink, t_sink, (s_sink*s_sink - s_spreader*s_spreader) / 4.0);
    
        // vertical C's to ambient (divide c_convec proportional to area)  
        _rc_model_holder->pack.c_amb_c_per_x = C_FACTOR * c_convec / (s_sink * s_sink) * ((s_spreader+chip_height) * (s_spreader-chip_width) / 4.0);
        _rc_model_holder->pack.c_amb_c_per_y = C_FACTOR * c_convec / (s_sink * s_sink) * ((s_spreader+chip_width) * (s_spreader-chip_height) / 4.0);
        _rc_model_holder->pack.c_amb_per = C_FACTOR * c_convec / (s_sink * s_sink) * ((s_sink*s_sink - s_spreader*s_spreader) / 4.0);
        
        // vertical C's of package substrate, solder balls, and PCB  
        _rc_model_holder->pack.c_sub_per_x = getCap(SPEC_HEAT_SUB, t_sub, (s_sub+chip_height) * (s_sub-chip_width) / 4.0);
        _rc_model_holder->pack.c_sub_per_y = getCap(SPEC_HEAT_SUB, t_sub, (s_sub+chip_width) * (s_sub-chip_height) / 4.0);
        _rc_model_holder->pack.c_solder_per_x = getCap(SPEC_HEAT_SOLDER, t_solder, (s_solder+chip_height) * (s_solder-chip_width) / 4.0);
        _rc_model_holder->pack.c_solder_per_y = getCap(SPEC_HEAT_SOLDER, t_solder, (s_solder+chip_width) * (s_solder-chip_height) / 4.0);
        _rc_model_holder->pack.c_pcb_c_per_x = getCap(SPEC_HEAT_PCB, t_pcb, (s_solder+chip_height) * (s_solder-chip_width) / 4.0);
        _rc_model_holder->pack.c_pcb_c_per_y = getCap(SPEC_HEAT_PCB, t_pcb, (s_solder+chip_width) * (s_solder-chip_height) / 4.0);
        _rc_model_holder->pack.c_pcb_per = getCap(SPEC_HEAT_PCB, t_pcb, (s_pcb*s_pcb - s_solder*s_solder) / 4.0);
    
        // vertical C's to ambient at PCB (divide c_convec_sec proportional to area)  
        _rc_model_holder->pack.c_amb_sec_c_per_x = C_FACTOR * c_convec_sec / (s_pcb * s_pcb) * ((s_solder+chip_height) * (s_solder-chip_width) / 4.0);
        _rc_model_holder->pack.c_amb_sec_c_per_y = C_FACTOR * c_convec_sec / (s_pcb * s_pcb) * ((s_solder+chip_width) * (s_solder-chip_height) / 4.0);
        _rc_model_holder->pack.c_amb_sec_per = C_FACTOR * c_convec_sec / (s_pcb * s_pcb) * ((s_pcb*s_pcb - s_solder*s_solder) / 4.0);

    } // populatePackageC


    // creates matrices  B and invB: BT = Power in the steady state. 
    // NOTE: EXTRA nodes: 4 heat spreader peripheral nodes, 4 heat 
    // sink inner peripheral nodes, 4 heat sink outer peripheral 
    // nodes(north, south, east and west) and 1 ambient node.
    void RCModel::populateR()
    {
        assert(_thermal_params);
        assert(_floorplan_holder);
        assert(_rc_model_holder);

        vector< vector<double> >& b     = _rc_model_holder->b;
        vector<double>& gx              = _rc_model_holder->gx;
        vector<double>& gy              = _rc_model_holder->gy;
        vector<double>& gx_int          = _rc_model_holder->gx_int;
        vector<double>& gy_int          = _rc_model_holder->gy_int;
        vector<double>& gx_sp           = _rc_model_holder->gx_sp;
        vector<double>& gy_sp           = _rc_model_holder->gy_sp;
        vector<double>& gx_hs           = _rc_model_holder->gx_hs;
        vector<double>& gy_hs           = _rc_model_holder->gy_hs;
        vector<double>& g_amb           = _rc_model_holder->g_amb;
        vector< vector<double> >& len   = _rc_model_holder->len;
        vector< vector<double> >& g     = _rc_model_holder->g;
        vector< vector<double> >& lu    = _rc_model_holder->lu;
        vector< vector<int> >& border   = _rc_model_holder->border;
        vector<int>& p                  = _rc_model_holder->p;
        double t_chip                   = _thermal_params->t_chip;
        double r_convec                 = _thermal_params->r_convec;
        double s_sink                   = _thermal_params->s_sink;
        double t_sink                   = _thermal_params->t_sink;
        double s_spreader               = _thermal_params->s_spreader;
        double t_spreader               = _thermal_params->t_spreader;
        double t_interface              = _thermal_params->t_interface;
        double k_chip                   = _thermal_params->k_chip;
        double k_sink                   = _thermal_params->k_sink;
        double k_spreader               = _thermal_params->k_spreader;
        double k_interface              = _thermal_params->k_interface;
     
        int i, j;
        int n = _floorplan_holder->_n_units;
        double gn_sp=0, gs_sp=0, ge_sp=0, gw_sp=0;
        double gn_hs=0, gs_hs=0, ge_hs=0, gw_hs=0;
        double r_amb;
    
        double w_chip = _floorplan_holder->_total_width;  // x-axis    
        double l_chip = _floorplan_holder->_total_height; // y-axis    
        
        // sanity check on floorplan sizes   
        if (w_chip > s_sink || l_chip > s_sink || w_chip > s_spreader || l_chip > s_spreader) 
                LibUtil::Log::printLine(stderr, "\ninordinate floorplan size!\n");
    
        // gx's and gy's of blocks   
        for (i = 0; i < n; i++) {
            // at the silicon layer  
            if (_thermal_params->block_omit_lateral) {
                gx[i] = gy[i] = 0;
            }
            else {
                gx[i] = 1.0/getR(k_chip, _floorplan_holder->_flp_units[i]._width / 2.0, _floorplan_holder->_flp_units[i]._height * t_chip);
                gy[i] = 1.0/getR(k_chip, _floorplan_holder->_flp_units[i]._height / 2.0, _floorplan_holder->_flp_units[i]._width * t_chip);
            }
    
            // at the interface layer    
            gx_int[i] = 1.0/getR(k_interface, _floorplan_holder->_flp_units[i]._width / 2.0, _floorplan_holder->_flp_units[i]._height * t_interface);
            gy_int[i] = 1.0/getR(k_interface, _floorplan_holder->_flp_units[i]._height / 2.0, _floorplan_holder->_flp_units[i]._width * t_interface);
    
            // at the spreader layer     
            gx_sp[i] = 1.0/getR(k_spreader, _floorplan_holder->_flp_units[i]._width / 2.0, _floorplan_holder->_flp_units[i]._height * t_spreader);
            gy_sp[i] = 1.0/getR(k_spreader, _floorplan_holder->_flp_units[i]._height / 2.0, _floorplan_holder->_flp_units[i]._width * t_spreader);
    
            // at the heatsink layer     
            gx_hs[i] = 1.0/getR(k_sink, _floorplan_holder->_flp_units[i]._width / 2.0, _floorplan_holder->_flp_units[i]._height * t_sink);
            gy_hs[i] = 1.0/getR(k_sink, _floorplan_holder->_flp_units[i]._height / 2.0, _floorplan_holder->_flp_units[i]._width * t_sink);
        }
    
        // shared lengths between blocks     
        for (i = 0; i < n; i++) 
            for (j = i; j < n; j++) 
                len[i][j] = len[j][i] = Floorplan::getSharedLength(_floorplan_holder, i, j);
    
        // package R's   
        populatePackageR(w_chip, l_chip);
    
        // short the R's from block centers to a particular chip edge    
        for (i = 0; i < n; i++) {
            if (Misc::eq(_floorplan_holder->_flp_units[i]._bottomy + _floorplan_holder->_flp_units[i]._height, l_chip)) {
                gn_sp += gy_sp[i];
                gn_hs += gy_hs[i];
                border[i][2] = 1;   // block is on northern border   
            } else
                border[i][2] = 0;
    
            if (Misc::eq(_floorplan_holder->_flp_units[i]._bottomy, 0)) {
                gs_sp += gy_sp[i];
                gs_hs += gy_hs[i];
                border[i][3] = 1;   // block is on southern border   
            } else
                border[i][3] = 0;
    
            if (Misc::eq(_floorplan_holder->_flp_units[i]._leftx + _floorplan_holder->_flp_units[i]._width, w_chip)) {
                ge_sp += gx_sp[i];
                ge_hs += gx_hs[i];
                border[i][1] = 1;   // block is on eastern border    
            } else 
                border[i][1] = 0;
    
            if (Misc::eq(_floorplan_holder->_flp_units[i]._leftx, 0)) {
                gw_sp += gx_sp[i];
                gw_hs += gx_hs[i];
                border[i][0] = 1;   // block is on western border    
            } else
                border[i][0] = 0;
        }
    
        // overall Rs between nodes  
        for (i = 0; i < n; i++) {
            double area = (_floorplan_holder->_flp_units[i]._height * _floorplan_holder->_flp_units[i]._width);
            // amongst functional units in the various layers    
            for (j = 0; j < n; j++) {
                double part = 0, part_int = 0, part_sp = 0, part_hs = 0;
                if (Floorplan::isHorizAdj(_floorplan_holder, i, j)) {
                    part = gx[i] / _floorplan_holder->_flp_units[i]._height;
                    part_int = gx_int[i] / _floorplan_holder->_flp_units[i]._height;
                    part_sp = gx_sp[i] / _floorplan_holder->_flp_units[i]._height;
                    part_hs = gx_hs[i] / _floorplan_holder->_flp_units[i]._height;
                }
                else if (Floorplan::isVertAdj(_floorplan_holder, i,j))  {
                    part = gy[i] / _floorplan_holder->_flp_units[i]._width;
                    part_int = gy_int[i] / _floorplan_holder->_flp_units[i]._width;
                    part_sp = gy_sp[i] / _floorplan_holder->_flp_units[i]._width;
                    part_hs = gy_hs[i] / _floorplan_holder->_flp_units[i]._width;
                }
                g[i][j] = part * len[i][j];
                g[IFACE*n+i][IFACE*n+j] = part_int * len[i][j];
                g[HSP*n+i][HSP*n+j] = part_sp * len[i][j];
                g[HSINK*n+i][HSINK*n+j] = part_hs * len[i][j];
            }
            // the 2.0 factor in the following equations is 
            // explained during the calculation of the B matrix
              
            // vertical g's in the silicon layer     
            g[i][IFACE*n+i]=g[IFACE*n+i][i]=2.0/getR(k_chip, t_chip, area);
            // vertical g's in the interface layer   
            g[IFACE*n+i][HSP*n+i]=g[HSP*n+i][IFACE*n+i]=2.0/getR(k_interface, t_interface, area);
            // vertical g's in the spreader layer    
            g[HSP*n+i][HSINK*n+i]=g[HSINK*n+i][HSP*n+i]=2.0/getR(k_spreader, t_spreader, area);
            // vertical g's in the heatsink core layer   
            // vertical R to ambient: divide r_convec proportional to area   
            r_amb = r_convec * (s_sink * s_sink) / area;
            g_amb[i] = 1.0 / (getR(k_sink, t_sink, area) + r_amb);
    
            // lateral g's from block center (spreader layer) to peripheral (n,s,e,w) spreader nodes     
            g[HSP*n+i][NL*n+SP_N]=g[NL*n+SP_N][HSP*n+i]=2.0*border[i][2] /
                                  ((1.0/gy_sp[i])+_rc_model_holder->pack.r_sp1_y*gn_sp/gy_sp[i]);
            g[HSP*n+i][NL*n+SP_S]=g[NL*n+SP_S][HSP*n+i]=2.0*border[i][3] /
                                  ((1.0/gy_sp[i])+_rc_model_holder->pack.r_sp1_y*gs_sp/gy_sp[i]);
            g[HSP*n+i][NL*n+SP_E]=g[NL*n+SP_E][HSP*n+i]=2.0*border[i][1] /
                                  ((1.0/gx_sp[i])+_rc_model_holder->pack.r_sp1_x*ge_sp/gx_sp[i]);
            g[HSP*n+i][NL*n+SP_W]=g[NL*n+SP_W][HSP*n+i]=2.0*border[i][0] /
                                  ((1.0/gx_sp[i])+_rc_model_holder->pack.r_sp1_x*gw_sp/gx_sp[i]);
            
            // lateral g's from block center (heatsink layer) to peripheral (n,s,e,w) heatsink nodes     
            g[HSINK*n+i][NL*n+SINK_C_N]=g[NL*n+SINK_C_N][HSINK*n+i]=2.0*border[i][2] /
                                        ((1.0/gy_hs[i])+_rc_model_holder->pack.r_hs1_y*gn_hs/gy_hs[i]);
            g[HSINK*n+i][NL*n+SINK_C_S]=g[NL*n+SINK_C_S][HSINK*n+i]=2.0*border[i][3] /
                                        ((1.0/gy_hs[i])+_rc_model_holder->pack.r_hs1_y*gs_hs/gy_hs[i]);
            g[HSINK*n+i][NL*n+SINK_C_E]=g[NL*n+SINK_C_E][HSINK*n+i]=2.0*border[i][1] /
                                        ((1.0/gx_hs[i])+_rc_model_holder->pack.r_hs1_x*ge_hs/gx_hs[i]);
            g[HSINK*n+i][NL*n+SINK_C_W]=g[NL*n+SINK_C_W][HSINK*n+i]=2.0*border[i][0] /
                                        ((1.0/gx_hs[i])+_rc_model_holder->pack.r_hs1_x*gw_hs/gx_hs[i]);
        }
        
        // g's from peripheral(n,s,e,w) nodes    
        // vertical g's between peripheral spreader nodes and center peripheral heatsink nodes  
        g[NL*n+SP_N][NL*n+SINK_C_N]=g[NL*n+SINK_C_N][NL*n+SP_N]=2.0/_rc_model_holder->pack.r_sp_per_y;
        g[NL*n+SP_S][NL*n+SINK_C_S]=g[NL*n+SINK_C_S][NL*n+SP_S]=2.0/_rc_model_holder->pack.r_sp_per_y;
        g[NL*n+SP_E][NL*n+SINK_C_E]=g[NL*n+SINK_C_E][NL*n+SP_E]=2.0/_rc_model_holder->pack.r_sp_per_x;
        g[NL*n+SP_W][NL*n+SINK_C_W]=g[NL*n+SINK_C_W][NL*n+SP_W]=2.0/_rc_model_holder->pack.r_sp_per_x;
        // lateral g's between peripheral outer sink nodes and center peripheral sink nodes  
        g[NL*n+SINK_C_N][NL*n+SINK_N]=g[NL*n+SINK_N][NL*n+SINK_C_N]=2.0/(_rc_model_holder->pack.r_hs + _rc_model_holder->pack.r_hs2_y);
        g[NL*n+SINK_C_S][NL*n+SINK_S]=g[NL*n+SINK_S][NL*n+SINK_C_S]=2.0/(_rc_model_holder->pack.r_hs + _rc_model_holder->pack.r_hs2_y);
        g[NL*n+SINK_C_E][NL*n+SINK_E]=g[NL*n+SINK_E][NL*n+SINK_C_E]=2.0/(_rc_model_holder->pack.r_hs + _rc_model_holder->pack.r_hs2_x);
        g[NL*n+SINK_C_W][NL*n+SINK_W]=g[NL*n+SINK_W][NL*n+SINK_C_W]=2.0/(_rc_model_holder->pack.r_hs + _rc_model_holder->pack.r_hs2_x);
        // vertical g's between inner peripheral sink nodes and ambient  
        g_amb[n+SINK_C_N] = g_amb[n+SINK_C_S] = 1.0 / (_rc_model_holder->pack.r_hs_c_per_y+_rc_model_holder->pack.r_amb_c_per_y);
        g_amb[n+SINK_C_E] = g_amb[n+SINK_C_W] = 1.0 / (_rc_model_holder->pack.r_hs_c_per_x+_rc_model_holder->pack.r_amb_c_per_x);
        // vertical g's between outer peripheral sink nodes and ambient  
        g_amb[n+SINK_N] = g_amb[n+SINK_S] = g_amb[n+SINK_E] =
                          g_amb[n+SINK_W] = 1.0 / (_rc_model_holder->pack.r_hs_per+_rc_model_holder->pack.r_amb_per);
    
        // calculate matrix B such that BT = POWER in steady state  
        // non-diagonal elements     
        for (i = 0; i < NL*n+EXTRA; i++)
            for (j = 0; j < i; j++)
                if ((g[i][j] == 0.0) || (g[j][i] == 0.0))
                    b[i][j] = b[j][i] = 0.0;
                else
                    // here is why the 2.0 factor comes when calculating g[][]   
                    b[i][j] = b[j][i] = -1.0/((1.0/g[i][j])+(1.0/g[j][i]));
        // diagonal elements               
        for (i = 0; i < NL*n+EXTRA; i++) {
            // functional blocks in the heat sink layer  
            if (i >= HSINK*n && i < NL*n) 
                b[i][i] = g_amb[i%n];
            // heat sink peripheral nodes    
            else if (i >= NL*n+SINK_C_W)
                b[i][i] = g_amb[n+i-NL*n];
            // all other nodes that are not connected to the ambient       
            else
                b[i][i] = 0.0;
            // sum up the conductances     
            for(j=0; j < NL*n+EXTRA; j++)
                if (i != j)
                    b[i][i] -= b[i][j];
        }
        
        // copy b to lu
        lu = b;
        // LUP decomp.
        lupDcmp(lu, NL*n+EXTRA, p, 1);

        // done  
        _rc_model_holder->r_ready = TRUE;
    } // populateR


    // creates 2 matrices: invA, C: dT + A^-1*BT = A^-1*Power, 
    // C = A^-1 * B. note that A is a diagonal matrix (no lateral
    // capacitances. all capacitances are to ground). also note that
    // it is stored as a 1-d vector. so, for computing the inverse, 
    // inva[i] = 1/a[i] is just enough. 
    void RCModel::populateC()
    {
        assert(_thermal_params);
        assert(_floorplan_holder);
        assert(_rc_model_holder);

        vector<double>& inva        = _rc_model_holder->inva;
        vector< vector<double> >& c = _rc_model_holder->c;
        vector< vector<double> >& b = _rc_model_holder->b;
        vector<double>& a           = _rc_model_holder->a;
        double t_chip               = _thermal_params->t_chip;
        double c_convec             = _thermal_params->c_convec;
        double s_sink               = _thermal_params->s_sink;
        double t_sink               = _thermal_params->t_sink;
        double t_spreader           = _thermal_params->t_spreader;
        double t_interface          = _thermal_params->t_interface;
        double p_chip               = _thermal_params->p_chip;
        double p_sink               = _thermal_params->p_sink;
        double p_spreader           = _thermal_params->p_spreader;
        double p_interface          = _thermal_params->p_interface;
        double c_amb;
    
        int i;
        int n = _floorplan_holder->_n_units;
    
        if (!_rc_model_holder->r_ready)
            fatal("R model not ready\n");
            
        double w_chip = _floorplan_holder->_total_width;  // x-axis    
        double l_chip = _floorplan_holder->_total_height; // y-axis    
    
        // package C's   
        populatePackageC(w_chip, l_chip);
        
        // functional block C's  
        for (i = 0; i < n; i++) {
            double area = (_floorplan_holder->_flp_units[i].height * _floorplan_holder->_flp_units[i].width);
            // C's from functional units to ground   
            a[i] = getCap(p_chip, t_chip, area);
            // C's from interface portion of the functional units to ground  
            a[IFACE*n+i] = getCap(p_interface, t_interface, area);
            // C's from spreader portion of the functional units to ground   
            a[HSP*n+i] = getCap(p_spreader, t_spreader, area);
            // C's from heatsink portion of the functional units to ground   
            // vertical C to ambient: divide c_convec proportional to area   
            c_amb = C_FACTOR * c_convec / (s_sink * s_sink) * area;
            a[HSINK*n+i] = getCap(p_sink, t_sink, area) + c_amb;
        }
    
        // C's from peripheral(n,s,e,w) nodes    
        // from peripheral spreader nodes to ground  
        a[NL*n+SP_N] = a[NL*n+SP_S] = _rc_model_holder->pack.c_sp_per_y;
        a[NL*n+SP_E] = a[NL*n+SP_W] = _rc_model_holder->pack.c_sp_per_x;
        // from center peripheral sink nodes to ground
        // NOTE: this treatment of capacitances (and 
        // the corresponding treatment of resistances 
        // in populate_R_model) as parallel (series)
        // is only approximate and is done in order
        // to avoid creating an extra layer of nodes
        a[NL*n+SINK_C_N] = a[NL*n+SINK_C_S] = _rc_model_holder->pack.c_hs_c_per_y + 
                                              _rc_model_holder->pack.c_amb_c_per_y;
        a[NL*n+SINK_C_E] = a[NL*n+SINK_C_W] = _rc_model_holder->pack.c_hs_c_per_x + 
                                              _rc_model_holder->pack.c_amb_c_per_x; 
        // from outer peripheral sink nodes to ground    
        a[NL*n+SINK_N] = a[NL*n+SINK_S] = a[NL*n+SINK_E] = a[NL*n+SINK_W] = 
                         _rc_model_holder->pack.c_hs_per + _rc_model_holder->pack.c_amb_per;
        
        //  done     
        _rc_model_holder->c_ready = TRUE;
    } // populateC


    void RCModel::precomputeStepLupDcmp()
    {
        int m                               = _rc_model_holder->n_nodes;
        vector< vector<double> >& b         = _rc_model_holder->b;
        vector<double>& a                   = _rc_model_holder->a;
        vector< vector<int> >& p_step       = _rc_model_holder->p_step;
        vector< vector<double> >& geq_step  = _rc_model_holder->geq_step;
        vector<double>& time_steps          = _rc_model_holder->time_steps;
        vector< vector< vector<double> > >& lu_step  = _rc_model_holder->lu_step;
        
        int i, j;
    
        for (i=0; i<N_TIME_STEPS; ++i)
        {   
            lu_step[i] = b;
    
            for (j=0; j<m; ++j)
            {
                geq_step[i][j] = a[j]/time_steps[i];
                lu_step[i][j][j] += geq_step[i][j];
            }
    
            lupdcmp(lu_step[i], m, p_step[i], 1);
        }
    }

} // namespace Thermal

