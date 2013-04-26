
#include <cassert>
#include <stdio.h>

#include "../../../include/libutil/LibUtil.h"
#include "VerilogModule.h"
#include "VerilogNet.h"
#include "VerilogInstance.h"
#include "VerilogMisc.h"
#include "VerilogScope.h"

namespace VerilogParser
{
    using namespace std;
    using namespace LibUtil;

    VerilogModule::VerilogModule(const string& name_, const VerilogItems& items_)
        : m_name_(name_), m_elaborated_(false)
    {        
        m_items_ = new VerilogItems();
        m_item_map_ = new VerilogItemMap();
        initializeItems(items_);
    }

    VerilogModule::~VerilogModule()
    {
        deletePtrVector<VerilogItem>(m_items_);
        delete m_item_map_;

        m_items_ = NULL;
        m_item_map_ = NULL;
    }
    
    void VerilogModule::initializeItems(const VerilogItems& module_items_)
    {
        VerilogItems::const_iterator it;
        for(it = module_items_.begin(); it != module_items_.end(); ++it)
            m_items_->push_back(*it);
    }
    
    bool VerilogModule::hasItem(const string& identifier_) const
    {
        return (m_item_map_->count(identifier_) != 0);
    }
    
    const VerilogItem* VerilogModule::getItem(const string& identifier_) const
    {
        if (!hasItem(identifier_))
            throw VerilogException("Item does not exist in module: " + identifier_);

        return (*m_item_map_)[identifier_];
    }
	
	void VerilogModule::elaborate(VerilogScope* scope_)
	{
        if (m_elaborated_)
            throw VerilogException("Module already elaborated: " + m_name_);
            
		scope_->addModule(this);
		scope_->setModuleScope(this);
        
        VerilogItems::const_iterator it;
        for(it = m_items_->begin(); it != m_items_->end(); ++it)
        {
            // Get the identifier
            const string& identifier = (*it)->getIdentifier();
            // Catch duplicate identifiers
            if (hasItem(identifier))
                throw VerilogException("Duplicate module item: " + identifier);
            (*m_item_map_)[identifier] = (*it);
            // Elaborate the item
            (*it)->elaborate(scope_);
        }
        m_elaborated_ = true;		
	}
    
} // namespace VerilogParser
