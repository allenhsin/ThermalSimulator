#ifndef VERILOG_PARSER_NUMBER_H
#define VERILOG_PARSER_NUMBER_H

#include <string>
#include <vector>
#include <bitset>
#include <limits>

#include "VerilogMisc.h"

// A class representing a verilog number
// Note: I am restricting myself to the maximum number of bits of an unsigned long!
namespace VerilogParser
{
    class Number
    {
        public:
            static const char DEFAULT_BASE = 'd';
            static const unsigned long MAX_WIDTH = std::numeric_limits<unsigned long>::digits;;
            static const unsigned long DEFAULT_WIDTH = MAX_WIDTH;
    
        public:
            Number(const std::string& num_str_);
            Number(unsigned long num_);
            virtual ~Number();
            
        public:
            // Copy constructor
            Number(const Number& num_);
            
        public:            
            unsigned long toULong() const;            
            std::string toString() const;            
            
        private:
            // Prunes the '_' fillers in verilog numbers
            static std::string pruneNum(const std::string& input_);
            
        private:
            // bit width of the number
            unsigned long m_width_;
            // value of the number stored as a maximum size bitset
            std::bitset<MAX_WIDTH> m_value_;
    };
}
#endif
