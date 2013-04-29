
#include <cassert>
#include <stdio.h>

#include "../../../include/libutil/LibUtil.h"
#include "VerilogItem.h"
#include "VerilogScope.h"
#include "VerilogModule.h"
#include "VerilogParameter.h"
#include "VerilogNet.h"
#include "VerilogInstance.h"

namespace VerilogParser
{
    VerilogItem::VerilogItem(VerilogItemType type_, const std::string& identifier_)
        : m_type_(type_), m_identifier_(identifier_), m_elaborated_(false)
    {}
    
    VerilogItem::~VerilogItem()
    {}
    
    void VerilogItem::elaborate(VerilogScope* scope_)
    {
        // Check for elaboration
        if (m_elaborated_)
            throw VerilogException("Item already elaborated: " + m_identifier_ +
                " in module " + scope_->getModuleScope()->getName());

        try
        {        
            elaborateItem(scope_);
        }
        catch (VerilogException ex)
        {
            std::string module = "MODULE " + scope_->getModuleScope()->getName();
            std::string item_type = "";

            if (getType() == ITEM_PARAMETER) item_type = "PARAMETER";
            else if (getType() == ITEM_NET) item_type = "NET";
            else if (getType() == ITEM_INSTANCE) item_type = "INSTANCE";

            throw VerilogException(module + ": " + item_type + " " + getIdentifier() + ": " + ex.what());
        }
            
        m_elaborated_ = true;        
    }
    
    VerilogItem::VerilogItem(const VerilogItem& item_)
        : m_type_(item_.m_type_), m_identifier_(item_.m_identifier_), m_elaborated_(item_.m_elaborated_)
    {}

    VerilogItem* VerilogItem::getFlattenedClone(const std::string& hier_, const std::string& /* hier_sep_ */) const
    {
        VerilogItem* c = clone();
        c->m_identifier_ = hier_ + c->m_identifier_;        
        return c;
    }
    
} // namespace VerilogParser
