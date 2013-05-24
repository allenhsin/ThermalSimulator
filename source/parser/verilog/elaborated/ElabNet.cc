
#include <cassert>
#include <stdio.h>

#include "ElabNet.h"

namespace VerilogParser
{
    using namespace std;

    ElabNet::ElabNet(const std::string& identifier_, PortType port_type_, NetType net_type_)
        : ElabItem(ELAB_ITEM_NET, identifier_),
        m_port_type_(port_type_),
        m_net_type_(net_type_)
    {}

    ElabNet::~ElabNet()
    {}
    
    std::string ElabNet::toString() const
    {
        return "Net -> " + getIdentifier();
    }
    
    ElabItem* ElabNet::clone() const
    {
        return new ElabNet(*this);
    }
    
    ElabNet::ElabNet(const ElabNet& net_)
        : ElabItem(net_), m_port_type_(net_.m_port_type_), m_net_type_(net_.m_net_type_)
    {}

    
} // namespace VerilogParser
