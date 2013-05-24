
#include <cassert>
#include <stdio.h>

#include "ElabInstance.h"

namespace VerilogParser
{
    using std::string;

    ElabInstance::ElabInstance(const string& identifier_, const ElabModule* module_) 
        : ElabItem(ELAB_ITEM_INST, identifier_),
        m_module_(module_)
    {}

    ElabInstance::~ElabInstance()
    {}
    
    ElabItem* ElabInstance::getFlattenedClone(const std::string& hier_, const std::string& hier_sep_) const
    {
        return ElabItem::getFlattenedClone(hier_, hier_sep_);
    }
    
    
} // namespace VerilogParser
