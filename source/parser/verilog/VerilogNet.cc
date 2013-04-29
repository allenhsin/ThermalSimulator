
#include <cassert>
#include <stdio.h>

#include "../../../include/libutil/LibUtil.h"
#include "VerilogNet.h"
#include "VerilogScope.h"

namespace VerilogParser
{
    using namespace std;
    using namespace LibUtil;

    VerilogNet::VerilogNet(
                const std::string& identifier_, 
                VerilogPortType port_type_,
                VerilogNetType net_type_,
                const VerilogRange& range_
            )
        : VerilogItem(ITEM_NET, identifier_),
        m_port_type_(port_type_),
        m_net_type_(net_type_),
        m_range_(range_)
    {}

    VerilogNet::~VerilogNet()
    {}
    
    void VerilogNet::elaborateItem(VerilogScope* scope_)
    {
        // Elaborate the bit ranges
        m_range_.first.elaborate(scope_);
        m_range_.second.elaborate(scope_);
        
        if(!m_range_.first.isConstExpr())
            throw VerilogException("bit range did not evaluate to a constant: " + m_range_.first.toString());        
        if(!m_range_.second.isConstExpr())
            throw VerilogException("bit range did not evaluate to a constant: " + m_range_.second.toString());
    }
    
    VerilogItems* VerilogNet::createVerilogNets(
            const VerilogVariables& identifiers_,
            VerilogPortType port_type_,
            VerilogNetType net_type_, 
            const VerilogRange& range_
        )
    {
        VerilogItems* new_nets = new VerilogItems();
        VerilogVariables::const_iterator it;
        for (it = identifiers_.begin(); it != identifiers_.end(); ++it)
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
    
    std::string VerilogNet::toString() const
    {
        return "Net -> " + getIdentifier() + " [" + m_range_.first.toString() + ":" + m_range_.second.toString() + "]";
    }
    
    VerilogItem* VerilogNet::clone() const
    {
        return new VerilogNet(*this);
    }
    
    VerilogNet::VerilogNet(const VerilogNet& net_)
        : VerilogItem(net_), m_port_type_(net_.m_port_type_), m_net_type_(net_.m_net_type_), m_range_(net_.m_range_)
    {}
    
     bool VerilogNet::isValidSubRange(const VerilogRange& range, const VerilogRange& sub_range)
     {
        if (sub_range.first.getConstExpr() < sub_range.second.getConstExpr())
            return false;
        if (range.first.getConstExpr() < sub_range.first.getConstExpr())
            return false;
        if (range.second.getConstExpr() > sub_range.second.getConstExpr())
            return false;
            
        return true;
     }
    
} // namespace VerilogParser
