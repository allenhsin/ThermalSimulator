
#include <cassert>
#include <stdio.h>

#include "../../../include/libutil/LibUtil.h"
#include "VerilogFile.h"

namespace VerilogParser
{
    using namespace std;
    using namespace LibUtil;

    VerilogFile::VerilogFile(std::string file_name_)
        : m_file_name_(file_name_)
    {}
    
    VerilogFile::~VerilogFile()
    {}

} // namespace VerilogParser
