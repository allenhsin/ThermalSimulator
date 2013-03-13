
#ifndef __THERMAL_PACKAGE_H__
#define __THERMAL_PACKAGE_H__

#include "source/models/thermal_model/thermal_parameters.h"

namespace Thermal
{
    class Package
    {
    public:
        Package();
        ~Package();

        void updateRConvection(double sink_temp);

        void setThermalParameters(ThermalParameters* thermal_params);

    protected:
        void calculateRForcedConvection();
        void calculateRNaturalConvection(double sink_temp);

    private:

        // thermal parameters
        ThermalParameters* _thermal_params;

        // number of fins
	    double _n_fin;
	    // total surface area
        double _sur_area_fin;

        // number of pins
	    double _n_pin;
        // total surface area
	    double _sur_area_pin;

	    // flow charateristics
	    double _reynolds;
	    double _nusselt;
        // heat transfer coefficient
	    double _h_coeff;
        // air velocity
	    double _air_v;
        // lumped convection thermal resistance
	    double _r_th;

    }; // class Package

} // namespace Thermal

#endif // __THERMAL_PACKAGE_H__

