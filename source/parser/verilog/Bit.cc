
#include <iostream>

#include <bitset>
#include "Bit.h"
#include "elaborated/ElabNet.h"

namespace VerilogParser
{
    using std::vector;
    using std::string;

    Bit::Bit(bool constant_)
        : m_constant_(constant_)
    {}
    
    Bit::~Bit()
    {}
        
    Bit::Bit(const Bit& bit_)
        : m_constant_(bit_.m_constant_)
    {}
        

    ConBit::ConBit(bool bit_)
        : Bit(true), m_bit_(bit_)
    {}
    
    ConBit::~ConBit()
    {}
        
    ConBit::ConBit(const ConBit& bit_)
        : Bit(bit_), m_bit_(bit_.m_bit_)
    {}

    string ConBit::toString() const
    {
        if (m_bit_) return "1";
        return "0";
    }
    
    NetBit::NetBit(const ElabNet* net_)
        : Bit(false), m_net_(net_)
    {}
    
    NetBit::~NetBit()
    {}
        
    NetBit::NetBit(const NetBit& bit_)
        : Bit(bit_), m_net_(bit_.m_net_)
    {}
    
    string NetBit::toString() const
    {
        return m_net_->toString();
    }

} // namespace VerilogParser
