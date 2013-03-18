
#ifndef __THERMAL_RC_MODEL_H__
#define __THERMAL_RC_MODEL_H__

#include <vector>

#include "source/models/thermal_model/thermal_parameters.h"
#include "source/models/thermal_model/floorplan.h"


namespace Thermal
{   

    struct PackageRCModelHolder
    {
        // lateral resistances
        // peripheral spreader nodes
        double r_sp1_x;
        double r_sp1_y;
        // sink's inner periphery
        double r_hs1_x;
        double r_hs1_y;
        double r_hs2_x;
        double r_hs2_y;
        // sink's outer periphery
        double r_hs;
     
        // vertical resistances
        // peripheral spreader nodes
        double r_sp_per_x;
        double r_sp_per_y;
        // sink's inner periphery
        double r_hs_c_per_x;
        double r_hs_c_per_y;
        // sink's outer periphery
        double r_hs_per;
    
        // vertical capacitances
        // peripheral spreader nodes
        double c_sp_per_x;
        double c_sp_per_y;
        // sink's inner periphery
        double c_hs_c_per_x;
        double c_hs_c_per_y;
        // sink's outer periphery
        double c_hs_per;
    
        // vertical R's and C's to ambient
        // sink's inner periphery
        double r_amb_c_per_x;
        double c_amb_c_per_x;
        double r_amb_c_per_y;
        double c_amb_c_per_y;
        // sink's outer periphery
        double r_amb_per;
        double c_amb_per;
        
        // secondary path R's and C's
        
        // lateral resistances
        // peripheral package substrate nodes
        double r_sub1_x;
        double r_sub1_y;
        // peripheral solder ball nodes
        double r_solder1_x;
        double r_solder1_y;
        // PCB's inner periphery
        double r_pcb1_x;
        double r_pcb1_y;
        double r_pcb2_x;
        double r_pcb2_y;
        // PCB's outer periphery
        double r_pcb;
        
        // vertical resistances
        // peripheral package substrate nodes
        double r_sub_per_x;
        double r_sub_per_y;
        // peripheral solder ball nodes
        double r_solder_per_x;
        double r_solder_per_y;
        // PCB's inner periphery
        double r_pcb_c_per_x;
        double r_pcb_c_per_y;
        // PCB's outer periphery
        double r_pcb_per;
    
        // vertical capacitances
        // peripheral package substrate nodes
        double c_sub_per_x;
        double c_sub_per_y;
        // peripheral solder ballnodes
        double c_solder_per_x;
        double c_solder_per_y;
        // PCB's inner periphery
        double c_pcb_c_per_x;
        double c_pcb_c_per_y;
        // PCB's outer periphery
        double c_pcb_per;
    
        // vertical R's and C's to ambient at PCB
        // PCB's inner periphery
        double r_amb_sec_c_per_x;
        double c_amb_sec_c_per_x;
        double r_amb_sec_c_per_y;
        double c_amb_sec_c_per_y;
        // PCB's outer periphery
        double r_amb_sec_per;
        double c_amb_sec_per;
    };

    struct RCModelHolder
    {
        // main matrices
        // conductance matrix
        std::vector< std::vector<double> > b;
        // LUP decomposition of b
        std::vector< std::vector<double> > lu; 
        std::vector< std::vector< std::vector<double> > > lu_step;
        std::vector<int> p;
        std::vector< std::vector<int> > p_step;
    
        std::vector< std::vector<double> > geq_step;
        std::vector<double> time_steps;
    
        // diagonal capacitance matrix stored as a 1-d std::vector
        std::vector<double> a; 
        // inverse of the above
        std::vector<double> inva;
        // c = inva * b
        std::vector< std::vector<double> > c;
    
        // package parameters
        PackageRCModelHolder pack;
    
        // intermediate std::vectors and matrices
        std::vector<double> gx; 
        std::vector<double> gy;
        std::vector<double> gx_int; 
        std::vector<double> gy_int;
        std::vector<double> gx_sp;
        std::vector<double> gy_sp;
        std::vector<double> gx_hs; 
        std::vector<double> gy_hs;
        std::vector<double> g_amb;
        std::vector<double> t_vector;
        std::vector< std::vector<double> > len;
        std::vector< std::vector<double> > g;
        std::vector< std::vector<int> > border;
    
        // total no. of nodes
        int n_nodes;
        // total no. of blocks
        int n_units;
        // to allow for resizing
        int base_n_units;
    
        // flags
        int r_ready;    // are the R's initialized?
        int c_ready;    // are the C's initialized?
    };

    class RCModel
    {
    public:
        RCModel();
        ~RCModel();

        void setThermalParameters(ThermalParameters* thermal_params);
        void setFloorplanHolder(FloorplanHolder* floorplan_holder);

        RCModelHolder* getRCModelHolder(){ return _rc_model_holder; }

        void allocateRCModelHolder();
        void populateR();
        void populateC();
        void precomputeStepLupDcmp();

    protected:
        void populatePackageR(double chip_width, double chip_height);
        void populatePackageC(double chip_width, double chip_height);

        double getR(double conductivity, double thickness, double area);
        double getCap(double sp_heat, double thickness, double area);

    private:
        RCModelHolder*      _rc_model_holder;

        ThermalParameters*  _thermal_params;
        FloorplanHolder*    _floorplan_holder;


    }; // class RCModel

} // namespace Thermal

#endif // __THERMAL_RC_MODEL_H__

