
#include <iostream>
#include <sstream>
#include <vector>

#include "VerilogFileReader.h"
#include "VerilogFile.h"

#include "raw/RawModule.h"
#include "elaborated/ElabModule.h"
#include "VerilogScope.h"

namespace VerilogParser
{
    using std::string;
    using std::ostream;

    VerilogFileReader::VerilogFileReader()
    {
        m_scope_ = new VerilogScope();        
    }
    
    VerilogFileReader::~VerilogFileReader()
    {
        delete m_scope_;
        m_scope_ = NULL;
    }

    void VerilogFileReader::elaborate(const string& top_name_)
    {
        m_scope_->elaborate(top_name_);
    }    
    
    void VerilogFileReader::addModule(RawModule* module_)
    {
        m_scope_->addRawModule(module_);
    }    
    
    void VerilogFileReader::dumpRawModules(ostream& ostr_) const
    {
        RawModuleMap::const_iterator it;
        for (it = m_scope_->getRawModuleMap().begin(); it != m_scope_->getRawModuleMap().end(); ++it)
            ostr_ << it->second->toString();
    }
    
    void VerilogFileReader::dumpElabModules(ostream& ostr_) const
    {
        ElabModules::const_iterator it;
        for (it = m_scope_->getElabModules().begin(); it != m_scope_->getElabModules().end(); ++it)
            ostr_ << (*it)->toString();
    }
}
