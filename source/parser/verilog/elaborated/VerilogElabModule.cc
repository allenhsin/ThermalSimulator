
#include <cassert>
#include <stdio.h>

#include "VerilogElabModule.h"
#include "VerilogElabItem.h"

namespace VerilogParser
{
    using namespace std;

    VerilogElabModule::VerilogElabModule(const string& name_)
        : m_name_(name_), m_item_map_()
    {}

    VerilogElabModule::~VerilogElabModule()
    {
        VerilogElabItemMap::const_iterator it;
        for (it = m_item_map_.begin(); it != m_item_map_.end(); ++it)
            delete it->second;
    }
    
    VerilogElabModule* VerilogElabModule::clone() const
    {
        // Create a new VerilogElabModule with the same name
        VerilogElabModule* copy = new VerilogElabModule(m_name_);
                
        // Copy over all items
        VerilogElabItemMap::const_iterator it;
        for (it = m_item_map_.begin(); it != m_item_map_.end(); ++it)
            copy->m_item_map_[it->first] = it->second->clone();

        // Return copy
        return copy;
    }

    bool VerilogElabModule::hasItem(const string& identifier_) const
    {
        return (m_item_map_.count(identifier_) != 0);
    }
    
    const VerilogElabItem* VerilogElabModule::getItem(const string& identifier_) const
    {
        if (!hasItem(identifier_))
            throw VerilogException("Item does not exist in module: " + identifier_);

        return m_item_map_.at(identifier_);
    }

} // namespace VerilogParser
