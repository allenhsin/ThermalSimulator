
#include <iostream>

#include <bitset>
#include "BitVector.h"
#include "Bit.h"

namespace VerilogParser
{
    using std::bitset;
    using std::vector;
    using std::string;

    BitVector::BitVector(int offset_)
        : m_bits_(), m_offset_(offset_)
    {}

    // Allow copy constructor
    BitVector::BitVector(const BitVector& bits_)
        : m_bits_(), m_offset_(bits_.m_offset_)
    {
        for (vector<Bit*>::const_iterator it = bits_.m_bits_.begin();
            it != bits_.m_bits_.end(); ++it)
        {
            m_bits_.push_back((*it)->clone());
        }
    }
    
    BitVector::~BitVector()
    {
        clearPtrVector<Bit>(&m_bits_);
    }
    
    void BitVector::append(const BitVector& bits_)
    {
        for (int i = bits_.low(); i <= bits_.high(); ++i)
            m_bits_.push_back(bits_[i]->clone()); 
    }
    
    Bit*& BitVector::operator[](int idx_)
    {
        if (idx_ < low() || idx_ > high())
            throw VerilogException("Index out of bounds");
        return m_bits_[idx_ - m_offset_]; 
    }
    
    const Bit* BitVector::operator[](int idx_) const 
    { 
        if (idx_ < low() || idx_ > high())
            throw VerilogException("Index out of bounds");
        return m_bits_[idx_ - m_offset_]; 
    }
    
    bool BitVector::isConst() const
    {
        for (unsigned int i = 0; i < m_bits_.size(); ++i)
            if (!m_bits_[i]->isConstant()) return false;
        return true;
    }
    
    unsigned long BitVector::getConstNum() const
    {
        if (!isConst())
            throw VerilogException("Expected constant target, got non-constant");        
        if (m_bits_.size() > MAX_WIDTH)
            throw VerilogException("Numerical max-width exceeded: " + makeString(m_bits_.size()));
        
        bitset<MAX_WIDTH> bits;
        for (unsigned int i = 0; i < m_bits_.size(); ++i)
            bits[i] = ((ConBit*) m_bits_[i])->getBit();
        return bits.to_ulong();
    }

    string BitVector::getConstStr() const
    {
        if (!isConst())
            throw VerilogException("Expected constant target, got non-constant");        

        string bit_str = "";
        for (unsigned int i = 0; i < m_bits_.size(); ++i)
        {
            ConBit* bit = (ConBit*) m_bits_[i];
            if (bit->getBit()) bit_str.push_back('1');
            else bit_str.push_back('0');
        }
        // Pad the string until it is a nice multiple of 8
        while (bit_str.size() % 8 != 0)
            bit_str.push_back('0');
            
        string out = "";
        for (unsigned int i = 0; i < bit_str.size(); i+=8)
        {
            bitset<8> c_bits(bit_str.substr(i, i+8));
            out = (char) c_bits.to_ulong() + out;
        }
        return out;
    }
    
    string BitVector::toString() const
    {
        string out = "{";
        for(int i = m_bits_.size() - 1; i >= 0; --i)
            out += m_bits_[i]->toString() + ",";        
        unsigned int pos = out.find_last_of(',');
        if (pos < out.length()) out[pos] = '}';
        else out += '}';
        
        return out;
    }
    
    BitVector* BitVector::makeBitVector(unsigned long num_, unsigned long width_)
    {
        BitVector* out = new BitVector();
        
        bitset<MAX_WIDTH> bits(num_);
        for (unsigned int i = 0; i < width_; ++i)
            out->m_bits_.push_back(new ConBit(bits[i]));
            
        return out;
    }
    
    BitVector* BitVector::makeBitVector(const string& str_)
    {
        BitVector* out = new BitVector();

        for (string::const_reverse_iterator it = str_.rbegin(); it != str_.rend(); ++it)
        {
            bitset<8> bits((unsigned long) *it);
            for (unsigned int i = 0; i < 8; ++i)
                out->m_bits_.push_back(new ConBit(bits[i]));
        }
        
        return out;
    }
    
} // namespace VerilogParser
