
#include <cassert>
#include <stdio.h>

#include "../../../include/libutil/LibUtil.h"
#include "VerilogModule.h"
#include "VerilogNet.h"
#include "VerilogInstance.h"
#include "VerilogMisc.h"

namespace Thermal
{
    using namespace std;
    using namespace LibUtil;

    VerilogModule::VerilogModule(const string& name_, const VerilogItems* items_)
        : m_name_(name_)
    {        
        m_nets_ = new vector<VerilogNet*>();
        m_instances_ = new vector<VerilogInstance*>();
        
        initializeItems(items_);
    }

    VerilogModule::~VerilogModule()
    {
        deletePtrVector<VerilogNet>(m_nets_);
        deletePtrVector<VerilogInstance>(m_instances_);

        m_nets_ = NULL;
        m_instances_ = NULL;
    }
    
    void VerilogModule::initializeItems(const VerilogItems* module_items_)
    {
        VerilogItems::const_iterator it;
        for(it = module_items_->begin(); it != module_items_->end(); ++it)
        {
            VerilogItemType type = (*it)->getType();
            if (type == ITEM_NET)
                m_nets_->push_back((VerilogNet*) *it);
            else if (type == ITEM_INSTANCE)
                m_instances_->push_back((VerilogInstance*) *it);
        }
    }
    
} // namespace Thermal
