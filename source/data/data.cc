
#include "source/data/data.h" 

#include <cassert>
#include <stddef.h>
#include "libutil/LibUtil.h"

using std::map;
using std::string;

namespace Thermal
{
    Data* Data::_data_singleton = NULL;

    void Data::allocate()
    {
        assert(_data_singleton==NULL);
        _data_singleton = new Data();
        assert(_data_singleton);
    }

    void Data::release()
    {
        assert(_data_singleton);
        delete _data_singleton;
        _data_singleton = NULL;
    }
    
    Data* Data::getSingleton()
    { 
        assert(_data_singleton);
        return _data_singleton;
    }

    map<string, double>* Data::getDataPointer(DataType data_type)
    {
        switch (data_type)
        {
        case TEMPERATURE_DATA:
            return &_temperature;
        case ACCUMULATED_ENERGY_DATA:
            return &_accumulated_energy_consumption;
        default:
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Unrecognized data type.\n");
            return NULL;
        }
    }

    bool Data::hasData(map<string, double>& data, string key)
    { return data.count(key); }

    void Data::addData(DataType data_type, string key, double value)
    {
        map<string, double>* data = getDataPointer(data_type);
        
        if(hasData(*data, key))
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Key \"" + key + "\" already exists in the data.\n");

        (*data)[key] = value;
    }

    void Data::setData(DataType data_type, string key, double value)
    {
        map<string, double>* data = getDataPointer(data_type);
        
        if(!hasData(*data, key))
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Key \"" + key + "\" does not exist in the data.\n");

        (*data)[key] = value;
    }

    double Data::getData(DataType data_type, string key)
    {
        map<string, double>* data = getDataPointer(data_type);
        
        if(!hasData(*data, key))
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Key \"" + key + "\" does not exist in the data.\n");

        return (*data)[key];
    }

    unsigned int Data::getDataSize(DataType data_type)
    {
        map<string, double>* data = getDataPointer(data_type);
        return data->size();
    }

    Data::Data()
    {
        _temperature.clear();
        _accumulated_energy_consumption.clear();
        _modulator_driver_bits.clear();
    }

    Data::~Data()
    {}



} // namespace Thermal

