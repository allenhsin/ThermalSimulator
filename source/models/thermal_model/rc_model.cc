
#include <stdio.h>
#include <stddef.h>
#include <cassert>

#include "source/models/thermal_model/rc_model.h"
#include "source/models/thermal_model/thermal_util.h"
#include "source/models/thermal_model/thermal_constants.h"
#include "source/misc/misc.h"
#include "libutil/LibUtil.h"

using std::vector;
using std::string;
using LibUtil::String;

namespace Thermal
{

    RCModel::RCModel()
        : _rc_model_holder  ( new RCModelHolder() )
        , _floorplan_holder (NULL)
    {}

    RCModel::~RCModel()
    { delete _rc_model_holder; }
    
    double RCModel::getR(double conductivity, double thickness, double area)
    { return thickness / (conductivity * area); }

    double RCModel::getCap(double sp_heat, double thickness, double area)
    { return sp_heat * thickness * area; }

    void RCModel::loadLayerDefinitionFile(string layer_def_file_name)
    {
        assert(_rc_model_holder);

        char    line[LINE_SIZE];
        char    line_copy[LINE_SIZE];
        char*   line_token;
        int     line_number = 0;

        double  current_thickness = 0;
        double  current_thermal_conductance = 0;
        double  current_specific_heat = 0;
        int     n_layers = 0;

        vector<double>& thickness           = _rc_model_holder->layer.thickness;
        vector<double>& thermal_conductance = _rc_model_holder->layer.thermal_conductance;
        vector<double>& specific_heat       = _rc_model_holder->layer.specific_heat;
        thickness.clear();
        thermal_conductance.clear();
        specific_heat.clear();

        FILE* def_file = fopen(layer_def_file_name.c_str(), "r");
        if (!def_file)
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Cannot open layer def file.\n");

        while(!feof(def_file))
        {
            // read a line from the file
            fgets(line, LINE_SIZE, def_file);
            line_number++;
            if (feof(def_file))
                break;

            strcpy(line_copy, line);
            line_token = strtok(line, " \r\t\n");

            // ignore comments and empty lines
            if      (!line_token || line_token[0] == '#')
                continue;
            else
            {
                if (sscanf(line_copy, "%lf%lf%lf", &current_thickness, &current_thermal_conductance, &current_specific_heat) == 3) 
                {
                    thickness.push_back(current_thickness);
                    thermal_conductance.push_back(current_thermal_conductance);
                    specific_heat.push_back(current_specific_heat);
                    n_layers++;
                }
                else
                    LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Wrong layer def file format at line " + (String) line_number + ".\n");
                
                Misc::isEndOfLine(2);
            }
        } // while

        _rc_model_holder->layer.n_layers = n_layers;

        fclose(def_file);
    }

    void RCModel::allocateRCModelHolder(string layer_def_file_name, double sampling_intvl)
    {
        assert(_floorplan_holder);
        assert(_rc_model_holder);
        
        // setup layer definitions
        loadLayerDefinitionFile(layer_def_file_name);

        // number floorplan units
        int n_units = _floorplan_holder->_n_units;
        // number of layers = 
        //   number of layers in the process file + 1 more for the air around the chip
        int n_layers = _rc_model_holder->layer.n_layers + 1;
        // number total rc model nodes
        int n_nodes = n_layers * n_units;
        
        _rc_model_holder->n_nodes = n_nodes;
        
        // ready are all not set
        _rc_model_holder->r_ready = 0;
        _rc_model_holder->c_ready = 0;

        Misc::initDouble2DVector(   _rc_model_holder->b,            n_nodes, n_nodes); // conductance matrix
        Misc::initDouble1DVector(   _rc_model_holder->a,            n_nodes); // vertical Cs - diagonal matrix stored as a 1-d vector
        Misc::initDouble1DVector(   _rc_model_holder->g_amb,        n_units); // vertical conductances to ambient
        Misc::initDouble3DVector(   _rc_model_holder->lu_step,      N_TIME_STEPS, n_nodes, n_nodes); // for precomputed lupdcmp at several different time step sizes
        Misc::initInt2DVector   (   _rc_model_holder->p_step,       N_TIME_STEPS, n_nodes);
        Misc::initDouble2DVector(   _rc_model_holder->geq_step,     N_TIME_STEPS, n_nodes);
        Misc::initDouble1DVector(   _rc_model_holder->time_steps,   N_TIME_STEPS);

        double max_time_step = (sampling_intvl>(2*MAX_TIME_STEP))? MAX_TIME_STEP : (sampling_intvl/2);
        for(int i = 0; i < N_TIME_STEPS; ++i)
        {
            _rc_model_holder->time_steps[i] = max_time_step;
            max_time_step /= 4;
        }

    } // allocateRCModelHolder

    // creates matrices  B and invB: BT = Power in the steady state. 
    void RCModel::populateR(double r_convec)
    {
        assert(_floorplan_holder);
        assert(_rc_model_holder);
        
        int i = 0, j = 0, k = 0;
        int n_units  = _floorplan_holder->_n_units;            // number units
        int n_layers = _rc_model_holder->layer.n_layers + 1;   // number layers
        int n_nodes  = _rc_model_holder->n_nodes;              // number nodes
        double chip_area = _floorplan_holder->_total_width * _floorplan_holder->_total_height; // chip area

        vector< vector<double> >    gx;
        vector< vector<double> >    gy;
        vector< vector<double> >    gz;
        vector< vector<double> >    len;
        vector< vector<double> >    g;
        Misc::initDouble2DVector( gx,   n_layers, n_units); // lumped conductances in x direction
        Misc::initDouble2DVector( gy,   n_layers, n_units); // lumped conductances in y direction
        Misc::initDouble2DVector( gz,   n_layers, n_units); // lumped conductances in z direction
        Misc::initDouble2DVector( len,  n_units,  n_units); // len[i][j] = length of shared edge between i & j
        Misc::initDouble2DVector( g,    n_nodes,  n_nodes); // g[i][j] = conductance between nodes i & j

        vector< vector<double> >& b     = _rc_model_holder->b;
        vector<double>& g_amb           = _rc_model_holder->g_amb;
        vector<double>& thickness       = _rc_model_holder->layer.thickness;
        vector<double>& thermal_conductance = _rc_model_holder->layer.thermal_conductance;

        // modeling each unit in 3D T-model with the node in the center and 
        // 2 x-axis, 2 y-axis and 2 z-axis resistances and 1 cap connecting from
        // the node to the ground
        // for each unit
        double unit_width = 0;
        double unit_height = 0;
        double unit_area = 0;
        for (i = 0; i < n_units; i++) 
        {   
            unit_width  =   _floorplan_holder->_flp_units[i]._width;
            unit_height =   _floorplan_holder->_flp_units[i]._height;
            unit_area   =   unit_width * unit_height;
            // (n_units-1) layers (exclude the air layer)
            for (k = 0; k < (n_layers-1); ++k)
            {
                gx[k][i] = 1.00/getR(thermal_conductance[k], (unit_width  / 2.00), (unit_height * thickness[k]));
                gy[k][i] = 1.00/getR(thermal_conductance[k], (unit_height / 2.00), (unit_width  * thickness[k]));
                gz[k][i] = 1.00/getR(thermal_conductance[k], (thickness[k] / 2.00), unit_area);
            }
            // air layer
            gx[n_layers-1][i] = 0;
            gy[n_layers-1][i] = 0;
            gz[n_layers-1][i] = 1.0/( (r_convec/2.00) * (chip_area / unit_area) ); // r_convec is devided by 2 since it's T-model
        }
    
        // shared lengths between units at the same layer
        for (i = 0; i < n_units; i++) 
            for (j = i; j < n_units; j++) 
                len[i][j] = len[j][i] = Floorplan::getSharedLength(_floorplan_holder, i, j);
    
        double side_length = 0;
        // for each unit "Ui"
        for (i = 0; i < n_units; i++) 
        {
            // first find out the conductance from "Ui" to "Uj" for all layers, "Uj" being another unit at the same layer
            for (j = 0; j < n_units; j++) 
            {
                // "Ui" and "Uj" are horizontally adjacent to each other at the same layer
                if (Floorplan::isHorizAdj(_floorplan_holder, i, j)) 
                {
                    side_length = _floorplan_holder->_flp_units[i]._height;
                    // for each layer
                    for (k = 0; k < (n_layers-1); ++k)
                        g[k*n_units + i][k*n_units + j] = gx[k][i] * (len[i][j] / side_length);
                }
                // "Ui" and "Uj" are vertically adjacent to each other at the same layer
                else if (Floorplan::isVertAdj(_floorplan_holder, i,j))  
                {
                    side_length = _floorplan_holder->_flp_units[i]._width;
                    // for each layer
                    for (k = 0; k < (n_layers-1); ++k)
                        g[k*n_units + i][k*n_units + j] = gy[k][i] * (len[i][j] / side_length);
                }
                // "Ui" and "Uj" are not adjacent to each other at the same layer
                else
                {
                    for (k = 0; k < (n_layers-1); ++k)
                        g[k*n_units + i][k*n_units + j] = 0.00;
                }

                // at the air layer, nodes are not conducted with each other at the same layer
                g[(n_layers-1)*n_units + i][(n_layers-1)*n_units + j] = 0.00;
            }
            
            // second, find out the conductances between "Ui"s at different layers
            for (k = 0; k < (n_layers-1); ++k)
            {
                g[k*n_units + i][(k+1)*n_units + i] = gz[k][i];
                g[(k+1)*n_units + i][k*n_units + i] = gz[k+1][i];
            }
            // from air layer to ambient
            g_amb[i] = gz[n_layers-1][i];
        }
    
        // calculate matrix B such that BT = POWER in steady state  
        // non-diagonal elements     
        for (i = 0; i < n_nodes; i++)
            for (j = 0; j < i; j++)
                if ((g[i][j] == 0.0) || (g[j][i] == 0.0))
                    b[i][j] = b[j][i] = 0.00;
                else
                    b[i][j] = b[j][i] = -1.0/( (1.0/g[i][j]) + (1.0/g[j][i]) );
        // diagonal elements               
        for (i = 0; i < n_nodes; i++) 
        {
            // nodes at the air layer
            if (i >= (n_layers-1)*n_units) 
                b[i][i] = g_amb[i%n_units];
            // all other nodes that are not connected to the ambient
            else
                b[i][i] = 0.00;
            // sum up the conductances     
            for(j=0; j < n_nodes; j++)
                if (i != j)
                    b[i][i] -= b[i][j];
        }
        
        // done  
        _rc_model_holder->r_ready = true;
    } // populateR


    // creates 2 matrices: invA, C: dT + A^-1*BT = A^-1*Power, 
    // C = A^-1 * B. note that A is a diagonal matrix (no lateral
    // capacitances. all capacitances are to ground).
    void RCModel::populateC(double c_convec)
    {
        assert(_floorplan_holder);
        assert(_rc_model_holder);

        int i = 0, k = 0;
        int n_units  = _floorplan_holder->_n_units; // number units
        int n_layers = _rc_model_holder->layer.n_layers + 1;   // number layers
        double chip_area = _floorplan_holder->_total_width * _floorplan_holder->_total_height; // chip area

        vector<double>& a               = _rc_model_holder->a;
        vector<double>& thickness       = _rc_model_holder->layer.thickness;
        vector<double>& specific_heat   = _rc_model_holder->layer.specific_heat;
    
        if (!_rc_model_holder->r_ready)
            LibUtil::Log::printFatalLine(std::cerr, "\nR model not ready\n");
            
        // thermal capacitance of each unit
        double unit_area = 0;
        for (i = 0; i < n_units; i++) 
        {
            unit_area = (_floorplan_holder->_flp_units[i]._height * _floorplan_holder->_flp_units[i]._width);
            
            // at every layer excluding air layer
            for (k=0; k<(n_layers-1); ++k)
                a[k*n_units + i] = getCap(specific_heat[k], thickness[k], unit_area);
            // at air layer
            a[(n_layers-1)*n_units + i] = c_convec * (unit_area/chip_area);
        }
    
        //  done     
        _rc_model_holder->c_ready = true;
    } // populateC


    void RCModel::precomputeStepLupDcmp()
    {
        int n_nodes                         = _rc_model_holder->n_nodes;
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
    
            for (j=0; j<n_nodes; ++j)
            {
                geq_step[i][j] = a[j]/time_steps[i];
                lu_step[i][j][j] += geq_step[i][j];
            }
    
            ThermalUtil::lupDecomposition(lu_step[i], n_nodes, p_step[i]);
        }
    }

    void RCModel::printRCModelToFile(string rc_model_file_name)
    {
        assert(_rc_model_holder);
        assert(_floorplan_holder);

        FILE* fp = fopen(rc_model_file_name.c_str(), "w");

        int n_units = _floorplan_holder->_n_units;
        int n_nodes = _rc_model_holder->n_nodes;

        fprintf(fp, "printing block model information...\n");
        fprintf(fp, "n_nodes: %d\n", n_nodes);
        fprintf(fp, "n_units: %d\n", n_units);
        fprintf(fp, "r_ready: %d\n", _rc_model_holder->r_ready);
        fprintf(fp, "c_ready: %d\n", _rc_model_holder->c_ready);
    
        fprintf(fp, "printing matrix b:\n");
        Misc::printDouble2DVector(_rc_model_holder->b, n_nodes, n_nodes, fp);
        fprintf(fp, "printing vector a:\n");
        Misc::printDouble1DVector(_rc_model_holder->a, n_nodes, fp);
        fprintf(fp, "printing vector g_amb:\n");
        Misc::printDouble1DVector(_rc_model_holder->g_amb, n_units, fp);

        fclose(fp);
    }
    
} // namespace Thermal

