#ifndef THERMAL_PARSER_VERILOG_NUMBER_H
#define THERMAL_PARSER_VERILOG_NUMBER_H

#include <string>
#include <vector>

#include "VerilogMisc.h"

// A class representing a verilog module
namespace Thermal
{
    class VerilogNumber
    {
        public:
            static const char DEFAULT_BASE;
            static const unsigned int DEFAULT_WIDTH;
            static const unsigned int UINT_MAX_BITS;
    
        public:
            VerilogNumber(const std::string& num_str_);
            virtual ~VerilogNumber();
            
        public:
            // Copy constructor
            VerilogNumber(const VerilogNumber& num_);
            
        public:
            // const string& getBinString() const;
            // const string& getHexString() const;
            // const string& getUIntString() const;
            
            unsigned int getUInt() const;
            
            // Prunes the '_' fillers in verilog numbers
            static std::string pruneNum(const std::string& input_);
            
            std::string toString() const;
            
            // static const string& makeBinString(const string& value_, char base_);
            
        private:
            // using efficient representation
            bool m_use_uint_;
            // bit width of the number
            unsigned int m_width_;
            // value of the integer stored in two different formats
            unsigned int m_value_uint_;
            // std::string m_value_str_;
    };
}
#endif
