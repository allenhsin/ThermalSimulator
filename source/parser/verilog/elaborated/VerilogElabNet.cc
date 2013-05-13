
#include <cassert>
#include <stdio.h>

#include "VerilogElabNet.h"

namespace VerilogParser
{
    using namespace std;

    VerilogElabNet::VerilogElabNet(
                const std::string& identifier_, 
                VerilogPortType port_type_,
                VerilogNetType net_type_
            )
        : VerilogElabItem(ITEM_NET, identifier_),
        m_port_type_(port_type_),
        m_net_type_(net_type_)
    {}

    VerilogElabNet::~VerilogElabNet()
    {}
    
    std::string VerilogElabNet::toString() const
    {
        return "Net -> " + getIdentifier();
    }
    
    VerilogElabItem* VerilogElabNet::clone() const
    {
        return new VerilogElabNet(*this);
    }
    
    VerilogElabNet::VerilogElabNet(const VerilogElabNet& net_)
        : VerilogElabItem(net_), m_port_type_(net_.m_port_type_), m_net_type_(net_.m_net_type_)
    {}

    
} // namespace VerilogParser
