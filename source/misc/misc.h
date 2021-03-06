
#ifndef __THERMAL_MISC_H__
#define __THERMAL_MISC_H__

#include <string>
#include <stdio.h>
#include <vector>

#include "source/misc/common_types.h"
#include "source/misc/common_constants.h"
#include "source/misc/config_parser.h"

#define DELTA           1e-9
#define TIME_DELTA      1e-12
#define MAX(x,y)        (((x)>(y))?(x):(y))
#define MIN(x,y)        (((x)<(y))?(x):(y))

namespace Thermal
{
    class Misc
    {
    public:
        static void setConfig(std::string config_file, ConfigParser*& cfg_ref, int argc, char** argv);

        static void initDouble1DVector(std::vector<double>& vec_ref, int dim_1);
        static void initDouble2DVector(std::vector< std::vector<double> >& vec_ref, int dim_1, int dim_2);
        static void initDouble3DVector(std::vector< std::vector< std::vector<double> > >& vec_ref, int dim_1, int dim_2, int dim_3);
        static void initInt1DVector(std::vector<int>& vec_ref, int dim_1);
        static void initInt2DVector(std::vector< std::vector<int> >& vec_ref, int dim_1, int dim_2);
        static bool eq(double x, double y);
        static bool eqTime(Time x, Time y);
        static void swapDoubleValue(double *a, double *b);
        static void swapIntValue(int *a, int *b);
        static void printDouble1DVector(std::vector<double>& v, int n, FILE* fp);
        static void printDouble2DVector(std::vector< std::vector<double> >& m, int nr, int nc, FILE* fp);

        static void isEndOfLine(int remaining_token_to_read);
        static void isEndOfLineWithEqual(int remaining_token_to_read);

        static double genGaussianRandomNumber(double mean, double std_dev);
    }; // class Misc

} // namespace Thermal

#endif // __THERMAL_MISC_H__

