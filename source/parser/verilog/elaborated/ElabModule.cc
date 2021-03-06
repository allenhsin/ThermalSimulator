
#include <cassert>
#include <stdio.h>

#include "ElabModule.h"
#include "ElabItem.h"

namespace VerilogParser
{
    using std::string;
    
    ElabModule::ElabModule(const string& name_, const SymbolMap& params_)
        : m_name_(name_), m_params_(params_), m_items_()
    {}
    
    ElabModule::ElabModule(const ElabModule& module_)
        : m_name_(module_.m_name_), m_params_(module_.m_params_), m_items_()
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
    
    string ElabModule::toString() const
    {
        string out = "Module: " + m_name_ + "\n";
        for (ElabItems::const_iterator it = m_items_.begin(); it != m_items_.end(); ++it)
        {
            out += "    " + (*it)->toString() + "\n";
        }
        return out;
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
