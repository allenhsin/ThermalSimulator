
#include <cassert>
#include <map>

#include "../../../include/libutil/LibUtil.h"
#include "VerilogExpression.h"
#include "VerilogScope.h"
#include "VerilogModule.h"

namespace VerilogParser
{
    using namespace LibUtil;

    VerilogScope::VerilogScope() 
        : m_raw_module_map_(), m_elab_module_map_(), m_module_scope_(NULL)
    {}

    VerilogScope::~VerilogScope()
    {
        VerilogModuleMap::const_iterator it;
        for (it = m_elab_module_map_.begin(); it != m_elab_module_map_.end(); ++it)
            delete it->second;
    }
    
    bool VerilogScope::hasRawModule(const string& name_) const
    {
        return (m_raw_module_map_.count(name_) != 0);
    }
    
    void VerilogScope::addRawModule(VerilogModule* module_)
    {
        const string& name = module_->getName();
        if (hasRawModule(name))
            throw VerilogException("Duplicate raw module: " + name);
        m_raw_module_map_[name] = module_;
    }
    
    VerilogModule* VerilogScope::getRawModule(const string& name_)
    {
        if(!hasRawModule(name_))
            throw VerilogException("Undefined raw module: " + name_);
        return m_raw_module_map_[name_];
    }

    
    bool VerilogScope::hasElabModule(const string& name_) const
    {
        return (m_elab_module_map_.count(name_) != 0);
    }
    
    void VerilogScope::addElabModule(VerilogModule* module_)
    {
        const string& name = module_->getName();
        if (hasElabModule(name))
            throw VerilogException("Duplicate elaborated module: " + name);
        m_elab_module_map_[name] = module_;
    }
    
    VerilogModule* VerilogScope::getElabModule(const string& name_)
    {
        if(!hasElabModule(name_))
            throw VerilogException("Undefined elaborated module: " + name_);
        return m_elab_module_map_[name_];
    }

} // namespace VerilogParser
