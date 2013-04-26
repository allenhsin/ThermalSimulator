
#include <cassert>
#include <stdio.h>

#include "../../../include/libutil/LibUtil.h"
#include "VerilogMisc.h"

namespace Thermal
{
    using namespace std;
    using namespace LibUtil;

    VerilogException::VerilogException(const std::string& message_)
        : exception(), m_message_(message_)
    {}

    VerilogException::~VerilogException() throw()
    {}
    
    // VerilogElaborate::VerilogElaborate(const string& error_msg_)
        // : m_elaborated_(false)
    // {}
    
    // VerilogElaborate::~VerilogElaborate()
    // {}
    
    // void VerilogElaborate::elaborate(VerilogScope* scope_)
    // {        
        // elaborateItem(scope_);
    // }
    
} // namespace Thermal
