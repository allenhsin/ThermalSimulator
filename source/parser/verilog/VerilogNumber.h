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
            VerilogNumber(const std::string& value_, char base_ = 'd', const std::string& width_ = "32");
            virtual ~VerilogNumber();
            
        public:
            // Copy constructor
            VerilogNumber(const VerilogNumber& num_);
            
        public:
            // const string& getBinString() const;
            // const string& getHexString() const;
            // const string& getUIntString() const;
            
            unsigned int getUInt() const;            
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
