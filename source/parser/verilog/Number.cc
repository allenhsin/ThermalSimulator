
#include <cstdlib>
#include <limits>
#include <sstream>
#include <iostream>

#include "Number.h"

// A class representing a verilog number

namespace VerilogParser
{
    using std::string;
    using std::bitset;

    Number::Number(const string& num_str_)
    {
        // Prune all '_' fillers
        string pruned_num_str = pruneNum(num_str_);
        // Default width and base
        m_width_ = DEFAULT_WIDTH;
        char base = DEFAULT_BASE;
        string& value = pruned_num_str;
        
        // search for the ' tick that separates the bit width from the base declaration
        unsigned long tick_pos = pruned_num_str.find_first_of('\'');        
        // if found, then
        if (tick_pos != string::npos)
        {
            m_width_ = strtoul(pruned_num_str.substr(0, tick_pos).c_str(), NULL, 10);
            base = pruned_num_str[tick_pos + 1];
            value = pruned_num_str.substr(tick_pos + 2);
        }

        // Check to see if the width is reasonable
        if ((m_width_ > 0) && (m_width_ <= MAX_WIDTH))
        {
            // Set a mask based on the width
            bitset<MAX_WIDTH> mask = (m_width_ == MAX_WIDTH) ? ~0 : (0x1ul << m_width_) - 1ul;
        
            switch(base)
            {
                case 'b': m_value_ = mask & bitset<MAX_WIDTH>(strtoul(value.c_str(), NULL, 2));
                    break;
                case 'o': m_value_ = mask & bitset<MAX_WIDTH>(strtoul(value.c_str(), NULL, 8));
                    break;
                case 'd': m_value_ = mask & bitset<MAX_WIDTH>(strtoul(value.c_str(), NULL, 10));
                    break;
                case 'h': m_value_ = mask & bitset<MAX_WIDTH>(strtoul(value.c_str(), NULL, 16));
                    break;
            }
        }
        else
        {
            // If not, error
            throw VerilogException("Invalid numerical constant width: " + num_str_);
        }
    }
    
    Number::Number(unsigned long num_)
    {
        m_width_ = DEFAULT_WIDTH;
        m_value_ = bitset<MAX_WIDTH>(num_);
    }
    
    Number::~Number()
    {}
    
    unsigned long Number::toULong() const
    {
        return m_value_.to_ulong();
    }
            
    string Number::toString() const
    {
        std::ostringstream ost;
        ost << m_value_.to_ulong();
        return ost.str();
    }    
    
    // Copy constructor
    Number::Number(const Number& num_)
        : m_width_(num_.m_width_), m_value_(num_.m_value_)
    {}
    
    string Number::pruneNum(const string& input_)
    {
        string output = "";
        for(string::const_iterator it = input_.begin(); it != input_.end(); ++it)
            if (*it != '_') output += *it;
        return output;
    }
    
} // namespace VerilogParser
