
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
        : m_module_map_(), m_module_scope_(NULL)
    {}

    VerilogScope::~VerilogScope()
    {}
    
    bool VerilogScope::hasModule(string name_) const
    {
        return (m_module_map_.count(name_) != 0);
    }
    
    void VerilogScope::addModule(VerilogModule* module_)
    {
        const string& name = module_->getName();
        if (hasModule(name))
            throw VerilogException("Duplicate module: " + name);
        m_module_map_[name] = module_;
    }
    
    VerilogModule* VerilogScope::getModule(string name_)
    {
        if(!hasModule(name_))
            throw VerilogException("Undefined module: " + name_);
        return m_module_map_[name_];
    }
    
    
    void VerilogScope::setModuleScope(VerilogModule* module_)
    {
        m_module_scope_ = module_;
    }
    
} // namespace VerilogParser
