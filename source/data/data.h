#ifndef __THERMAL_DATA_H__
#define __THERMAL_DATA_H__

#include <vector>
#include <map>

using std::vector;
using std::map;

namespace Thermal
{
    class Data
    {
    public:
        static void allocate();
        static void release();
        static Data* getSingleton();

        map<char*, double>& getTemperature()
        { return _temperature; }

        map<char*, double>& getPower()
        { return _power; }

    protected:
        Data();
        ~Data();

    private:
        static Data* _data_singleton; 
    
        map<char*, double> _temperature;
        map<char*, double> _power;
        



    }; // class Data

} // namespace Thermal

#endif // __THERMAL_DATA_H__
