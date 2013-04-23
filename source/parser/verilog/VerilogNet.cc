
#include <cassert>
#include <stdio.h>

#include "../../../include/libutil/LibUtil.h"
#include "VerilogNet.h"

namespace Thermal
{
    using namespace std;
    using namespace LibUtil;

    VerilogNet::VerilogNet(
                const std::string& net_name_, 
                VerilogPortType port_type_,
                VerilogNetType net_type_,
                const VerilogRange range_
            )
        : VerilogItem(ITEM_NET),
        m_net_name_(net_name_),
        m_port_type_(port_type_),
        m_net_type_(net_type_),
        m_range_(range_)
    {}

    VerilogNet::~VerilogNet()
    {}
    
    VerilogItems* VerilogNet::createVerilogNets(
            const VerilogVariables* names_,
            VerilogPortType port_type_,
            VerilogNetType net_type_, 
            const VerilogRange range_
        )
    {
        VerilogItems* new_nets = new VerilogItems();
        VerilogVariables::const_iterator it;
        for (it = names_->begin(); it != names_->end(); ++it)
        {
            new_nets->push_back(new VerilogNet(
                    string(*it),
                    port_type_,
                    net_type_,
                    range_
                ));
        }            
        return new_nets;
    }
    
} // namespace Thermal
