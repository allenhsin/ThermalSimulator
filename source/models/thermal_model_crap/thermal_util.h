
#ifndef __THERMAL_THERMAL_UTIL_H__
#define __THERMAL_THERMAL_UTIL_H__

#include <vector>


namespace Thermal
{
    class ThermalUtil
    {
    public:
        static void lupDecomposition(std::vector< std::vector<double> >& a, int n, std::vector<int>& p);
        static void luSolve(const std::vector< std::vector<double> >& a, int n, 
                            const std::vector<int>& p, std::vector<double>& b, 
                            std::vector<double>& x);
    }; // class ThermalUtil

} // namespace Thermal

#endif // __THERMAL_THERMAL_UTIL_H__

