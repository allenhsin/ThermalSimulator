
#include <cassert>
#include <stdio.h>
#include <cstdlib>

#include "../../../include/libutil/LibUtil.h"
#include "VerilogNumber.h"

namespace Thermal
{
    using namespace std;
    using namespace LibUtil;

    VerilogNumber::VerilogNumber(const string& value_, char base_, const string& width_)
    {
        m_width_ = (unsigned int) strtol(width_.c_str(), NULL, 10);
        
        // Prune '_' from verilog numbers
        string pruned_val = "";
        for(unsigned int i = 0; i < value_.size(); ++i)
            if (value_[i] != '_')
                pruned_val += value_[i];
        
        // Check to see if we can use the simplified unsigned int representation
        if (m_width_ <= 32)
        {
            // Just use the strtol thing
            m_use_uint_ = true;
            switch(base_)
            {
                case 'b': m_value_uint_ = (unsigned int) strtol(pruned_val.c_str(), NULL, 2);
                    break;
                case 'd': m_value_uint_ = (unsigned int) strtol(pruned_val.c_str(), NULL, 10);
                    break;
                case 'h': m_value_uint_ = (unsigned int) strtol(pruned_val.c_str(), NULL, 16);
                    break;
            }
        }
        else
        {
            // If not, try to convert to a binary string
            m_use_uint_ = false;
            throw "Numerical constants with bit widths > 32 are unsupported at this moment";
        }
    }

    VerilogNumber::~VerilogNumber()
    {}
    
    unsigned int VerilogNumber::getUInt() const
    {
        if (m_use_uint_)
            return m_value_uint_;
        
        throw "Numerical constants with bit widths > 32 are unsupported at this moment";
        return 0;
    }
    
    // const string& VerilogNumber::makeBinString(const string& value_, char base_)
    // {
        // if (base_ == 'h')
        // {
            // string bin_str = "";
            // for(int i = 0; i < value_.size(); ++i)
            // {
                // strtoi(value_[i], NULL, 16);
            // }
        // }
    // }
    
    // Copy constructor
    VerilogNumber::VerilogNumber(const VerilogNumber& num_)
        : m_use_uint_(num_.m_use_uint_), m_width_(num_.m_width_), m_value_uint_(num_.m_value_uint_)
    {}
    
} // namespace Thermal
