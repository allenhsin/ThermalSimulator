
#include <cassert>
#include <stdio.h>

#include "ElabAssign.h"

namespace VerilogParser
{
    using namespace std;

    ElabAssign::ElabAssign(const BitVector& left_, const BitVector& right_)
        : ElabItem(ELAB_ITEM_ASSIGN, left_.toString() + "=" + right_.toString()),
        m_left_(left_),
        m_right_(right_)
    {}

    ElabAssign::~ElabAssign()
    {}
    
    ElabAssign::ElabAssign(const ElabAssign& assign_)
        : ElabItem(assign_), m_left_(assign_.m_left_), m_right_(assign_.m_right_)
    {}

    
} // namespace VerilogParser
