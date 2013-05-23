
#include <cassert>
#include <stdio.h>

#include "RawModule.h"
#include "RawItem.h"

namespace VerilogParser
{
    using std::string;

    RawModule::RawModule(const string& name_, const RawItems& items_)
        : m_name_(name_), m_items_()
    {        
        for(RawItems::const_iterator it = items_.begin(); it != items_.end(); ++it)
            m_items_.push_back((*it)->clone()); 
    }

    RawModule::~RawModule()
    {
        clearPtrVector<RawItem>(&m_items_);
    }

    RawModule::RawModule(const RawModule& module_)
        : m_name_(module_.m_name_), m_items_()
    {
        for(RawItems::const_iterator it = module_.m_items_.begin(); it != module_.m_items_.end(); ++it)
            m_items_.push_back((*it)->clone()); 
    }
    
} // namespace VerilogParser
