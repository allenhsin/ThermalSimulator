
#include <cassert>
#include <stdio.h>

#include "source/data/data.h"
#include "source/models/physical_model/device_models/data_generator.h"
#include "source/models/performance_model/bit_sequence.h"
#include "libutil/LibUtil.h"

namespace Thermal
{
    DataGenerator::DataGenerator()
        : _name                     ("")
        , _bit_period               (1.0)
        , _current_bit              (false)
        , _time_since_last_bit      (0)
    {}

    DataGenerator::~DataGenerator()
    {}

    void DataGenerator::setName(std::string name)
    {
        _name = name;
    }
    
    void DataGenerator::setBitPeriod(double bit_period)
    {
        if( bit_period < 0.0 )
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Bit period cannot be negative.\n");
            
        _bit_period = bit_period;
    }

    bool DataGenerator::getBit(Time elapsed_time)
    {
        _time_since_last_bit += elapsed_time;
        
        if (_time_since_last_bit >= _bit_period)
        {
            _current_bit = Data::getSingleton()->getBitSequenceData(_name)->getNextBit();
            _time_since_last_bit = _time_since_last_bit - _bit_period;
        }
        
        return _current_bit;
    }

} // namespace Thermal
