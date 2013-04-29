
#include <cassert>
#include <stdio.h>

#include "../../../include/libutil/LibUtil.h"
#include "VerilogModule.h"
#include "VerilogNet.h"
#include "VerilogInstance.h"
#include "VerilogMisc.h"
#include "VerilogScope.h"
#include "VerilogParameter.h"

namespace VerilogParser
{
    using namespace std;
    using namespace LibUtil;

    VerilogModule::VerilogModule(const string& name_, const VerilogItems& items_)
        : m_name_(name_), m_elaborated_(false)
    {        
        m_items_ = new VerilogItems(items_.begin(), items_.end());
        m_item_map_ = new VerilogItemMap();        
    }

    VerilogModule::~VerilogModule()
    {
        deletePtrVector<VerilogItem>(m_items_);
        delete m_item_map_;

        m_items_ = NULL;
        m_item_map_ = NULL;
    }
    
    void VerilogModule::copyModule(const VerilogModule* module_, const VerilogConns& parameter_values_)
    {
        // Add the parameter values into a map
        map<string, const VerilogExpression&> param_val_map;        
        // Check to see if there are parameters that need to be overwritten
        for(VerilogConns::const_iterator param_it = parameter_values_.begin(); 
                param_it != parameter_values_.end(); ++param_it)
            param_val_map.insert(**param_it);
            
        // Copy over and overwrite the parameter values
        VerilogItems::const_iterator it;
        for(it = module_->m_items_->begin(); it != module_->m_items_->end(); ++it)
        {
            if((*it)->getType() == ITEM_PARAMETER)
            {
                const VerilogParameter* cur_param = (VerilogParameter*) *it;
                VerilogParameter* new_param = NULL;
                // Check to see if this parameter needs to be overwriteen
                if (param_val_map.count(cur_param->getIdentifier()) != 0)
                {
                    new_param = new VerilogParameter(cur_param->getIdentifier(), param_val_map.at(cur_param->getIdentifier()));
                    // Append replaced parameter values to the end of the module name
                    m_name_ = m_name_ + "_" + new_param->getIdentifier() + "_" + new_param->getValue().toString();
                }
                // If there are no overwrites
                if (new_param == NULL) new_param = (VerilogParameter*) cur_param->clone();
                // Add the parameter
                m_items_->push_back(new_param);                
            }
            // Everything else can be trivially copied over
            else m_items_->push_back((*it)->clone());
        }
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
        
        // Add this module as an elaborated module
		scope_->addElabModule(this);
        // Change the module scope to the current module
        VerilogModule* old_scope = scope_->getModuleScope();
		scope_->setModuleScope(this);
        
        VerilogItems::const_iterator it;
        for(it = m_items_->begin(); it != m_items_->end(); ++it)
        {
            // Get the identifier
            const string& identifier = (*it)->getIdentifier();
            // Catch duplicate identifiers
            if (hasItem(identifier)) throw VerilogException("Duplicate module item: " + identifier);
            (*m_item_map_)[identifier] = (*it);
            // Elaborate the item
            (*it)->elaborate(scope_);
        }
        
        // Change the module scope back
        scope_->setModuleScope(old_scope);
        m_elaborated_ = true;		
	}
    
    VerilogModule::VerilogModule(const VerilogModule* module_, const VerilogConns& parameter_values_)
        : m_name_(module_->m_name_), m_elaborated_(module_->m_elaborated_)
    {
        m_items_ = new VerilogItems();
        m_item_map_ = new VerilogItemMap();
        
        copyModule(module_, parameter_values_);
    }
    
} // namespace VerilogParser
