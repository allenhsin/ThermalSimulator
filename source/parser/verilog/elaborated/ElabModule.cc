
#include <cassert>
#include <stdio.h>

#include "ElabModule.h"
#include "ElabItem.h"

namespace VerilogParser
{
    using namespace std;

    ElabModule::ElabModule(const string& name_)
        : m_name_(name_), m_items_()
    {}
    
    ElabModule::ElabModule(const ElabModule& module_)
        : m_name_(module_.m_name_), m_items_()
    {
        // Deep copy of all items
        ElabItems::const_iterator it;
        for (it = module_.m_items_.begin(); it != module_.m_items_.end(); ++it)
            m_items_.push_back((*it)->clone());
    }

    ElabModule::~ElabModule()
    {
        ElabItems::const_iterator it;
        for (it = m_items_.begin(); it != m_items_.end(); ++it)
            delete *it;
    }
    
    // bool ElabModule::hasItem(const string& identifier_) const
    // {
        // return (m_item_map_.count(identifier_) != 0);
    // }
    
    // void ElabModule::addItem(ElabItem* item_)
    // {
        // const string& id = item_->getIdentifier();
        // if (hasItem(id))
            // throw VerilogException("Duplicate item: " + id);            
        // m_item_map_[id] = item_;
    // }
    
    // void ElabModule::addItems(ElabItems* items_)
    // {
        // ElabItems::const_iterator it;
        // for (it = items_->begin(); it != items_->end(); ++it)
            // addItem(*it);
    // }
    
    // const ElabItem* ElabModule::getItem(const string& identifier_) const
    // {
        // if (!hasItem(identifier_))
            // throw VerilogException("Item does not exist in module: " + identifier_);

        // return m_item_map_.at(identifier_);
    // }

} // namespace VerilogParser
