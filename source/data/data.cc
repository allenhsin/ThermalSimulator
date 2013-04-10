
#include "source/data/data.h" 

#include <cassert>
#include <stddef.h>

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

    Data::Data()
    {
        _temperature.clear();
        _accumulated_energy_consumption.clear();
    }

    Data::~Data()
    {}



} // namespace Thermal

