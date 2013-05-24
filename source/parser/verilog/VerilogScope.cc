
#include <cassert>
#include <map>

#include "VerilogScope.h"
#include "raw/RawModule.h"
#include "elaborated/ElabModule.h"
#include "elaborated/ElabItem.h"

namespace VerilogParser
{
    using std::string;

    VerilogScope::VerilogScope() 
        : m_symbol_map_(), m_raw_module_map_(), m_module_scope_(NULL), m_elab_modules_()
    {}

    VerilogScope::~VerilogScope()
    {
        // Delete all raw modules
        for (RawModuleMap::const_iterator it = m_raw_module_map_.begin(); it != m_raw_module_map_.end(); ++it)
            delete it->second;        
            
        // Delete all containers holding elaborated items
        for (IndexedElabItemsMap::const_iterator it = m_symbol_map_.begin(); it != m_symbol_map_.end(); ++it)
            delete it->second;        

        // Delete all elaborated modules
        clearPtrVector<ElabModule>(&m_elab_modules_);
    }
    
    bool VerilogScope::hasRawModule(const string& name_) const
    {
        return (m_raw_module_map_.count(name_) != 0);
    }
    
    void VerilogScope::addRawModule(RawModule* module_)
    {
        const string& name = module_->getName();
        if (hasRawModule(name))
            throw VerilogException("Duplicate raw module: " + name);
        m_raw_module_map_[name] = module_;
    }
    
    RawModule* VerilogScope::getRawModule(const string& name_)
    {
        if(!hasRawModule(name_))
            throw VerilogException("Undefined raw module: " + name_);
        return m_raw_module_map_[name_];
    }

    void VerilogScope::elaborate(const string& top_name_)
    {
        // Elaborate all potential top-level modules
        getRawModule(top_name_);
    }

} // namespace VerilogParser
