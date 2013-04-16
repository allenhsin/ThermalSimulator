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

        // temperature data
        void            addTemperatureData(std::string key, double value=0);
        void            setTemperatureData(std::string key, double value);
        double          getTemperatureData(std::string key);
        unsigned int    getTemperatureDataSize();

        // energy data
        void            addEnergyData(std::string key, double value=0);
        void            setEnergyData(std::string key, double value);
        double          getEnergyData(std::string key);
        unsigned int    getEnergyDataSize();

    protected:
        Data();
        ~Data();
        
        // temperature data
        bool hasTemperatureData(std::string key);
        // energy data
        bool hasEnergyData(std::string key);

    private:
        static Data* _data_singleton; 
    
        std::map<std::string, double> _temperature;
        std::map<std::string, double> _energy;
        
    }; // class Data

} // namespace Thermal

#endif // __THERMAL_DATA_H__
