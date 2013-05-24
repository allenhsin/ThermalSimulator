
#include <cassert>
#include <stdio.h>

#include "RawNet.h"

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
    
    ElabItems* RawNet::elaborate(VerilogScope* scope_) const
    {
        ElabItems* elab_nets = new ElabItems();
        //TODO
        return elab_nets;
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
