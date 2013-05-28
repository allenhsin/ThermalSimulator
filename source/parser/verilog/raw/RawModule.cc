
#include "RawModule.h"
#include "RawItem.h"
#include "../VerilogScope.h"
#include "../elaborated/ElabModule.h"

namespace VerilogParser
{
    using std::string;

    RawModule::RawModule(const string& name_, const Identifiers& port_list_, const RawItems& items_)
        : m_name_(name_), m_port_list_(), m_items_()
    {
        for(Identifiers::const_iterator it = port_list_.begin(); it != port_list_.end(); ++it)
            m_port_list_.push_back(*it);
        for(RawItems::const_iterator it = items_.begin(); it != items_.end(); ++it)
            m_items_.push_back((*it)->clone()); 
    }

    RawModule::RawModule(const RawModule& module_)
        : m_name_(module_.m_name_), m_items_()
    {
        for(Identifiers::const_iterator it = module_.m_port_list_.begin(); it != module_.m_port_list_.end(); ++it)
            m_port_list_.push_back(*it);
        for(RawItems::const_iterator it = module_.m_items_.begin(); it != module_.m_items_.end(); ++it)
            m_items_.push_back((*it)->clone()); 
    }
    
    RawModule::~RawModule()
    {
        clearPtrVector<RawItem>(&m_items_);
    }
    
    void RawModule::elaborate(VerilogScope* scope_, const SymbolMap& params_) const
    {
        // Create new elaborated module
        ElabModule* new_mod = new ElabModule(m_name_, params_);

        // Set the module as the current elaborating module
        scope_->setElabModule(new_mod);

        // Elaborate each raw item
        for(RawItems::const_iterator it = m_items_.begin(); it != m_items_.end(); ++it)
            (*it)->elaborate(scope_);

        // Add the new elaborated module
        scope_->addElabModule(new_mod);
    }
    
    string RawModule::toString() const
    {
        string out = "Module: " + m_name_ + "\n";
        for (RawItems::const_iterator it = m_items_.begin(); it != m_items_.end(); ++it)
        {
            out += "    " + (*it)->toString() + "\n";
        }
        return out;
    }

} // namespace VerilogParser
