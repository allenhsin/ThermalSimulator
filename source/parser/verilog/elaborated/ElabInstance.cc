
#include "ElabInstance.h"
#include "ElabModule.h"

namespace VerilogParser
{
    using std::string;

    ElabInstance::ElabInstance(const string& identifier_, const VerilogScope* scope_, const ElabModule* module_) 
        : ElabItem(ELAB_ITEM_INST, identifier_, scope_),
        m_module_(module_)
    {}
    
    ElabInstance::ElabInstance(const ElabInstance& instance_) 
        : ElabItem(instance_),
        m_module_(instance_.m_module_)
    {}

    ElabInstance::~ElabInstance()
    {}
    
    ElabItem* ElabInstance::getFlattenedClone(const std::string& hier_, const std::string& hier_sep_) const
    {
        return ElabItem::getFlattenedClone(hier_, hier_sep_);
    }
    
    string ElabInstance::toString() const
    { 
        return ElabItem::toString() + "(" + m_module_->getName() + ")";
    }
    
    
} // namespace VerilogParser
