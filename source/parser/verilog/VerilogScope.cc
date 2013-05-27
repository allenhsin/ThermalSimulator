
#include <iostream>
#include <map>

#include "VerilogScope.h"
#include "BitVector.h"
#include "raw/RawModule.h"
#include "elaborated/ElabModule.h"
#include "elaborated/ElabItem.h"
#include "elaborated/ElabInstance.h"

namespace VerilogParser
{
    using std::string;

    VerilogScope::VerilogScope() 
        : m_stack_(), m_symbol_map_(), m_raw_module_map_(), m_elab_modules_()
    {}

    VerilogScope::~VerilogScope()
    {
        // Delete all raw modules
        for (RawModuleMap::const_iterator it = m_raw_module_map_.begin(); it != m_raw_module_map_.end(); ++it)
            delete it->second;        
            
        // Delete all bit vectors in the symbol map
        for (SymbolMap::const_iterator it = m_symbol_map_.begin(); it != m_symbol_map_.end(); ++it)
            delete it->second;        

        // Delete all elaborated modules
        clearPtrVector<const ElabModule>(&m_elab_modules_);
    }
    
    bool VerilogScope::hasRawModule(const string& name_) const
    {
        return (m_raw_module_map_.count(name_) != 0);
    }
    
    void VerilogScope::addRawModule(const RawModule* module_)
    {
        const string& name = module_->getName();
        if (hasRawModule(name))
            throw VerilogException("Duplicate raw module: " + name);
        m_raw_module_map_[name] = module_;
    }
    
    const RawModule* VerilogScope::getRawModule(const string& name_)
    {
        if(!hasRawModule(name_))
            throw VerilogException("Undefined raw module: " + name_);
        return m_raw_module_map_[name_];
    }
    
    bool VerilogScope::has(const string& symbol_) const
    {
        // Create the lookup string based on the current elaboration stack
        std::string lookup = "";
        for (ElabStack::const_iterator it = m_stack_.begin(); it != m_stack_.end(); ++it)
            lookup += (*it)->getIdentifier() + ".";
        lookup += symbol_;
        
        return hasSymbol(lookup);
    }
    
    void VerilogScope::add(const string& symbol_, const BitVector* stuff_)
    {
        // Create the lookup string based on the current elaboration stack
        std::string lookup = "";
        for (ElabStack::const_iterator it = m_stack_.begin(); it != m_stack_.end(); ++it)
            lookup += (*it)->getIdentifier() + ".";
        lookup += symbol_;
        
        if (hasSymbol(lookup))
            throw VerilogException("Duplicate symbol: " + symbol_);
        m_symbol_map_[symbol_] = stuff_;
    }    
    
    const BitVector* VerilogScope::get(const string& symbol_)
    {
        // Create the lookup string based on the current elaboration stack
        std::string lookup = "";
        for (ElabStack::const_iterator it = m_stack_.begin(); it != m_stack_.end(); ++it)
            lookup += (*it)->getIdentifier() + ".";
        lookup += symbol_;
        
        if (!hasSymbol(lookup))
            throw VerilogException("Symbol does not exist: " + symbol_);
        return m_symbol_map_[lookup];
    }

    void VerilogScope::elaborate(const string& top_name_)
    {
        // Clear the symbol map
        for (SymbolMap::const_iterator it = m_symbol_map_.begin(); it != m_symbol_map_.end(); ++it)
            delete it->second; 
        m_symbol_map_.clear();

        // Elaborate the top-level module
        getRawModule(top_name_)->elaborate(this);
    }
    
    bool VerilogScope::hasSymbol(const string& full_symbol_) const
    {    
        return (m_symbol_map_.count(full_symbol_) != 0);
    }

} // namespace VerilogParser
