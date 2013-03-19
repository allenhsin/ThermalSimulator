#ifndef __THERMAL_DATA_H__
#define __THERMAL_DATA_H__

#include <vector>
#include <map>
#include <string>
#include <stdio.h>
#include <stdlib.h>

namespace Thermal
{
    class Data
    {
    public:
        static void allocate();
        static void release();
        static Data* getSingleton();

        std::map<std::string, double>& getTemperature()
        { return _temperature; }

        std::map<std::string, double>& getPower()
        { return _power; }

    protected:
        Data();
        ~Data();

    private:
        static Data* _data_singleton; 
    
        std::map<std::string, double> _temperature;
        std::map<std::string, double> _power;
        
    }; // class Data

} // namespace Thermal

#endif // __THERMAL_DATA_H__
