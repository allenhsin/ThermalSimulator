
#include <cassert>
#include <stdio.h>

#include "VerilogElabInstance.h"

namespace VerilogParser
{
    using std::string;

    VerilogElabInstance::VerilogElabInstance(const string& identifier_, const VerilogElabConns& conns_,
        const VerilogElabModule* module_) 
        : VerilogElabItem(ITEM_INSTANCE, identifier_), 
        m_conns_(conns_), 
        m_module_(module_)
    {}

    VerilogElabInstance::~VerilogElabInstance()
    {}
    
    VerilogElabItem* VerilogElabInstance::getFlattenedClone(const std::string& hier_, const std::string& hier_sep_) const
    {
        VerilogElabInstance* c = (VerilogElabInstance*) VerilogElabItem::getFlattenedClone(hier_, hier_sep_);
        // Need to change the connections to use flat naming
        const VerilogElabConns& conns = c->m_conns_;
        for (VerilogElabConns::const_iterator it = conns.begin(); it != conns.end(); ++it)
        {
            // Port name
            (*it)->first = hier_ + getIdentifier() + hier_sep_ + (*it)->first;
            // Rebase the expression as well
            (*it)->second = hier_ + (*it)->second;
        }
        
        // for (VerilogConns::const_iterator it = params->begin(); it != params->end(); ++it)
        // {
            // // Parameter name
            // (*it)->first = hier_ + c->m_module_name_ + hier_sep_ + (*it)->first;
            // // Rebase the expression as well
            // (*it)->second.useHierName(hier_);
        // }
        return c;
    }
    
    
} // namespace VerilogParser
