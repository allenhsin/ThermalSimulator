
#include "ElabInstance.h"
#include "ElabModule.h"

namespace VerilogParser
{
    using std::string;

    ElabInstance::ElabInstance(const string& identifier_, const ElabModule* module_, const SymbolMap& params_) 
        : ElabItem(ELAB_ITEM_INST, identifier_),
        m_module_(module_),
        m_params_(params_)
    {}
    
    ElabInstance::ElabInstance(const ElabInstance& instance_) 
        : ElabItem(instance_),
        m_module_(instance_.m_module_),
        m_params_(instance_.m_params_)
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
