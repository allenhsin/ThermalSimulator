
#include <iostream>

#include "RawAssign.h"
#include "../VerilogScope.h"
#include "../Bit.h"
#include "../BitVector.h"
#include "../elaborated/ElabAssign.h"
#include "../elaborated/ElabModule.h"

namespace VerilogParser
{
    using std::string;

    RawAssign::RawAssign(const LHExpression& lh_expr_, const RHExpression& rh_expr_)
        : RawItem(RAW_ITEM_ASSIGN, lh_expr_.toString() + "=" + rh_expr_.toString()),
        m_lh_expr_(lh_expr_.clone()),
        m_rh_expr_(rh_expr_.clone())
    {}

    RawAssign::~RawAssign()
    {}
    
    void RawAssign::elaborate(VerilogScope* scope_) const
    {
        // Elaborate the left- and right-hand side expressions
        const BitVector* left = m_lh_expr_->elaborate(scope_);
        const BitVector* right = m_rh_expr_->elaborate(scope_);
        
        // Create the elaborated assign
        ElabAssign* assign = new ElabAssign(scope_, *left, *right);
        scope_->getElabModule()->addItem(assign);

        // Cleanup
        delete left;
        delete right;
    }    

    RawItem* RawAssign::clone() const
    {
        return new RawAssign(*this);
    }
        
    RawAssign::RawAssign(const RawAssign& assign_)
        : RawItem(assign_), m_lh_expr_(assign_.m_lh_expr_->clone()), m_rh_expr_(assign_.m_rh_expr_->clone())
    {}

} // namespace VerilogParser
