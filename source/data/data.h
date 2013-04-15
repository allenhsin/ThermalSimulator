#ifndef __THERMAL_DATA_H__
#define __THERMAL_DATA_H__

#include <vector>
#include <map>
#include <string>
#include <stdio.h>
#include <stdlib.h>

namespace Thermal
{
    enum DataType
    {
        TEMPERATURE_DATA,
        ACCUMULATED_ENERGY_DATA,
        MODULATOR_DRIVER_BITS_DATA,
        NUMBER_DATA_TYPES
    };

    class Data
    {
    public:
        static void allocate();
        static void release();
        static Data* getSingleton();

        void addData(DataType data_type, std::string key, double value=0);
        void setData(DataType data_type, std::string key, double value);
        double getData(DataType data_type, std::string key);
        unsigned int getDataSize(DataType data_type);

    protected:
        Data();
        ~Data();

        std::map<std::string, double>* getDataPointer(DataType data_type);
        bool hasData(std::map<std::string, double>& data, std::string key);

    private:
        static Data* _data_singleton; 
    
        std::map<std::string, double> _temperature;
        std::map<std::string, double> _accumulated_energy_consumption;
        std::map<std::string, double> _modulator_driver_bits;
        
    }; // class Data

} // namespace Thermal

#endif // __THERMAL_DATA_H__
