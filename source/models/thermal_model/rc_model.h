
#ifndef __THERMAL_RC_MODEL_H__
#define __THERMAL_RC_MODEL_H__

#include <vector>
#include <string>

#include "source/models/thermal_model/floorplan.h"

namespace Thermal
{   
    class RCModelLayer
    {
    public:
        // total no. of layers in the chip
        int n_layers;

        std::vector<double> thickness;
        std::vector<double> thermal_conductance;
        std::vector<double> specific_heat;
    };

    class RCModelHolder
    {
    public:
        // main matrices
        // conductance matrix
        std::vector< std::vector<double> >  b;
        // diagonal capacitance matrix stored as a 1-d std::vector
        std::vector<double>                 a; 
        // intermediate vectors and matrices
        std::vector<double>                 g_amb;
        // LUP decomposition of b
        std::vector< std::vector< std::vector<double> > > lu_step;
        std::vector< std::vector<int> >     p_step;
        std::vector< std::vector<double> >  geq_step;
        std::vector<double>                 time_steps;

        RCModelLayer layer;
    
        // total no. of nodes
        int n_nodes;
    
        // flags
        int r_ready;    // are the R's initialized?
        int c_ready;    // are the C's initialized?
    };

    class RCModel
    {
    public:
        RCModel();
        ~RCModel();

        void setFloorplanHolder(const FloorplanHolder* floorplan_holder)
        { _floorplan_holder = floorplan_holder; }

        const RCModelHolder* getRCModelHolder(){ return _rc_model_holder; }

        void allocateRCModelHolder(std::string layer_def_file_name, double sampling_intvl);
        void populateR(double r_convec);
        void populateC(double c_convec);
        void precomputeStepLupDcmp();

        void printRCModelToFile(std::string rc_model_file_name);

    protected:
        void   loadLayerDefinitionFile(std::string layer_def_file_name);
        double getR(double conductivity, double thickness, double area);
        double getCap(double sp_heat, double thickness, double area);
    
    private:
        RCModelHolder*          _rc_model_holder;
        const FloorplanHolder*  _floorplan_holder;

    }; // class RCModel

} // namespace Thermal

#endif // __THERMAL_RC_MODEL_H__

