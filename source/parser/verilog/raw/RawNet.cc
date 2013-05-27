
#include <iostream>

#include "RawNet.h"
#include "../VerilogScope.h"
#include "../Bit.h"
#include "../BitVector.h"
#include "../elaborated/ElabNet.h"
#include "../elaborated/ElabModule.h"

namespace VerilogParser
{
    using std::string;

    RawNet::RawNet(
                const string& identifier_, 
                PortType port_type_,
                NetType net_type_,
                const BitRange& range_
            )
        : RawItem(RAW_ITEM_NET, identifier_),
        m_port_type_(port_type_),
        m_net_type_(net_type_),
        m_range_(range_)
    {}

    RawNet::~RawNet()
    {}
    
    RawItems* RawNet::createRawNets(
            const Identifiers& identifiers_,
            PortType port_type_,
            NetType net_type_, 
            const BitRange& range_
        )
    {
        RawItems* new_nets = new RawItems();
        Identifiers::const_iterator it;
        for (it = identifiers_.begin(); it != identifiers_.end(); ++it)
            new_nets->push_back(new RawNet(string(*it), port_type_, net_type_, range_));
        return new_nets;
    }
    
    void RawNet::elaborate(VerilogScope* scope_) const
    {
        // Get the low and high bit ranges
        int low = m_range_.low(scope_);
        int high = m_range_.high(scope_);

        // Create elaborated nets
        // Create the bit vector that links to the symbol
        BitVector* symb = new BitVector(low);
        
        // Create bits in the symbol that link to the new elaborated net
        for (int i = low; i <= high; ++i)
        {
            ElabNet* net = new ElabNet(getIdentifier(), scope_, i, m_port_type_, m_net_type_);
            scope_->getElabModule()->addItem(net);
            symb->addBack(new NetBit(net));
        }        
        // Add the symbol to the current scope
        scope_->add(getIdentifier(), symb);
    }
    
    string RawNet::toString() const
    {
        return RawItem::toString() + m_range_.toString();
    }

    RawItem* RawNet::clone() const
    {
        return new RawNet(*this);
    }
        
    RawNet::RawNet(const RawNet& net_)
        : RawItem(net_), m_port_type_(net_.m_port_type_), m_net_type_(net_.m_net_type_), m_range_(net_.m_range_)
    {}

} // namespace VerilogParser
