#ifndef VERILOG_PARSER_BIT_VECTOR_H
#define VERILOG_PARSER_BIT_VECTOR_H

#include <string>
#include "VerilogMisc.h"

namespace VerilogParser
{
    class Bit;

    // A bit vector
    class BitVector
    {
        public:
            // Initialize the bit vector with some offset index
            BitVector(int offset_ = 0);
            ~BitVector();
            
        public:
            // Add an item to the back
            inline void addBack(Bit* bit_) { m_bits_.push_back(bit_); }
            // Append things from a BitVector (note that this makes a copy
            // of individual bit targets)
            void append(const BitVector& bits_);
            // Bit index accessors
            Bit*& operator[](int idx_);
            const Bit* operator[](int idx_) const;
            // Get the high and low indices
            inline int low() const { return m_offset_; }
            inline int high() const { return m_offset_ + m_bits_.size() - 1; }
            inline int width() const { return m_bits_.size(); }

            // Is the bit target a constant number/string
            bool isConst() const;
            // Return the bit target as a constant number
            unsigned long getConstNum() const;
            // Return the bit target as a constant string
            std::string getConstStr() const;
            // Return a string representation
            std::string toString() const;
            
        public:
            // Static creator methods for creating bit vectors from constants
            static BitVector* makeBitVector(unsigned long num_, unsigned long width_);
            static BitVector* makeBitVector(const std::string& str_);
            
        public:
            // Allow copy constructor
            BitVector(const BitVector& bits_);

        private:
            // Vector holding the bit targets
            std::vector<Bit*> m_bits_;
            // Index offset
            const int m_offset_;
    };
}
#endif
