
#include <cassert>
#include <stdio.h>

#include "../../../include/libutil/LibUtil.h"
#include "VerilogFile.h"
#include "VerilogModule.h"

namespace Thermal
{
    using namespace std;
    using namespace LibUtil;

    VerilogFile::VerilogFile(std::string file_name_)
        : m_file_name_(file_name_)
    {
        m_modules_ = new vector<VerilogModule*>();
    }

    VerilogFile::~VerilogFile()
    {
        deletePtrVector<VerilogModule>(m_modules_);
        m_modules_ = NULL;
    }

    void VerilogFile::addModule(VerilogModule* module_)
    {
        m_modules_->push_back(module_);
    }
    
} // namespace Thermal
