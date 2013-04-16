
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

    // temperature data -------------------------------------------------------

    bool Data::hasTemperatureData(string key)
    { return _temperature.count(key); }

    void Data::addTemperatureData(string key, double value)
    {
        if(hasTemperatureData(key))
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Key \"" + key + "\" already exists in the temperature data.\n");
        _temperature[key] = value;
    }

    void Data::setTemperatureData(string key, double value)
    {
        if(!hasTemperatureData(key))
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Key \"" + key + "\" already exists in the temperature data.\n");
        _temperature[key] = value;
    }

    double Data::getTemperatureData(string key)
    {
        if(!hasTemperatureData(key))
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Key \"" + key + "\" does not exist in the temperature data.\n");
        return _temperature[key];
    }

    unsigned int Data::getTemperatureDataSize()
    { return _temperature.size(); }

    // ------------------------------------------------------------------------


    // energy data ------------------------------------------------------------

    bool Data::hasEnergyData(string key)
    { return _energy.count(key); }

    void Data::addEnergyData(string key, double value)
    {
        if(hasEnergyData(key))
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Key \"" + key + "\" already exists in the energy data.\n");
        _energy[key] = value;
    }

    void Data::setEnergyData(string key, double value)
    {
        if(!hasEnergyData(key))
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Key \"" + key + "\" already exists in the energy data.\n");
        _energy[key] = value;
    }

    double Data::getEnergyData(string key)
    {
        if(!hasEnergyData(key))
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Key \"" + key + "\" already exists in the energy data.\n");
        return _energy[key];
    }

    unsigned int Data::getEnergyDataSize()
    { return _energy.size(); }

    // ------------------------------------------------------------------------

    Data::Data()
    {
        _temperature.clear();
        _energy.clear();
    }

    Data::~Data()
    {}

} // namespace Thermal

