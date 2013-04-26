
#include <cassert>
#include <stdio.h>
#include <cstdlib>

#include "../../../include/libutil/LibUtil.h"
#include "VerilogNumber.h"

namespace Thermal
{
    using namespace std;
    using namespace LibUtil;

    const char VerilogNumber::DEFAULT_BASE = 'd';
    const unsigned int VerilogNumber::DEFAULT_WIDTH = 32;
    const unsigned int VerilogNumber::UINT_MAX_BITS = 32;
    
    VerilogNumber::VerilogNumber(const string& num_str_)
    {
        // Prune all '_' fillers
        string pruned_num_str = pruneNum(num_str_);
        // Default width and base
        m_width_ = DEFAULT_WIDTH;
        char base = DEFAULT_BASE;
        string& value = pruned_num_str;
                
        // search for the ' tick that separates the bit width from the base declaration
        unsigned int tick_pos = pruned_num_str.find_first_of('\'');        
        // if found, then
        if (tick_pos != string::npos)
        {
            m_width_ = (unsigned int) strtoul(pruned_num_str.substr(0, tick_pos).c_str(), NULL, 10);
            base = pruned_num_str[tick_pos + 1];
            value = pruned_num_str.substr(tick_pos + 2);
        }

        // Set a mask based on the width
        unsigned long int mask = (m_width_ >= UINT_MAX_BITS) ? 
            ~0 : 
            ((0x1u << m_width_) - 1u);
        
        // Check to see if we can use the simplified unsigned int representation
        if (m_width_ <= UINT_MAX_BITS)
        {
            // Just use the strtoul thing
            m_use_uint_ = true;
            switch(base)
            {
                case 'b': m_value_uint_ = mask & (unsigned int) strtoul(value.c_str(), NULL, 2);
                    break;
                case 'o': m_value_uint_ = mask & (unsigned int) strtoul(value.c_str(), NULL, 8);
                    break;
                case 'd': m_value_uint_ = mask & (unsigned int) strtoul(value.c_str(), NULL, 10);
                    break;
                case 'h': m_value_uint_ = mask & (unsigned int) strtoul(value.c_str(), NULL, 16);
                    break;
            }
        }
        else
        {
            // If not, try to convert to a binary string
            m_use_uint_ = false;
            throw VerilogException("Numerical constants with bit widths > UINT_MAX_BITS are unsupported at this moment.");
        }
    }
    
    VerilogNumber::VerilogNumber(unsigned int num_)
    {
        m_use_uint_ = true;
        m_width_ = DEFAULT_WIDTH;
        m_value_uint_ = num_;
    }
    
    VerilogNumber::~VerilogNumber()
    {}
    
    unsigned int VerilogNumber::getUInt() const
    {
        if (m_use_uint_)
            return m_value_uint_;
        
        throw VerilogException("Numerical constants with bit widths > 32 are unsupported at this moment");
        return 0;
    }
            
    string VerilogNumber::toString() const
    {
        std::ostringstream ost;
        if (m_use_uint_)
        {
            ost << m_value_uint_;
            return ost.str();
        }        
        throw VerilogException("Numerical constants with bit widths > 32 are unsupported at this moment");
        return "";
    }
    
    
    string VerilogNumber::pruneNum(const string& input_)
    {
        string output = "";
        for(string::const_iterator it = input_.begin(); it != input_.end(); ++it)
            if (*it != '_') output += *it;
        return output;
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
