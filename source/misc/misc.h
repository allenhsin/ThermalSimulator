
#ifndef __THERMAL_MISC_H__
#define __THERMAL_MISC_H__

#include <string>

#include "config.hpp"

#define DELTA           1e-6
#define MAX(x,y)        (((x)>(y))?(x):(y))
#define MIN(x,y)        (((x)<(y))?(x):(y))

namespace Thermal
{
    class Misc
    {
    public:
        static void setConfig(std::string config_file, config::Config*& cfg_ref, int argc, char** argv);

        static void initDouble1DVector(vector<double>& vec_ref, int dim_1);
        static void initDouble2DVector(vecotr< vector<double> >& vec_ref, int dim_1, int dim_2);
        static void initDouble3DVector(vecotr< vector< vector<double> > >& vec_ref, int dim_1, int dim_2, int dim_3);
        static void initInt1DVector(vector<int>& vec_ref, int dim_1);
        static void initInt2DVector(vecotr< vector<int> >& vec_ref, int dim_1, int dim_2);
        static bool eq(double x, double y);
        static void swapDoubleValue(double *a, double *b);

    }; // class Misc

} // namespace Thermal

#endif // __THERMAL_MISC_H__

