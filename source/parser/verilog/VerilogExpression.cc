
#include <cassert>
#include <stdio.h>
#include <cstdlib>

#include "../../../include/libutil/LibUtil.h"
#include "VerilogExpression.h"

namespace Thermal
{
    using namespace std;
    using namespace LibUtil;

    VerilogExpression::VerilogExpression()
        : m_type_(NONE)
    {
        m_val_num_ = NULL;
        m_val_concat_ = NULL;
        m_val_range_ = NULL;
    }
    
    VerilogExpression::VerilogExpression(const VerilogNumber& num_)
        : m_type_(NUMBER)
    {
        m_val_num_ = new VerilogNumber(num_);
        m_val_concat_ = NULL;
        m_val_range_ = NULL;
    }

    VerilogExpression::VerilogExpression(const VerilogExpressions& concat_)
        : m_type_(CONCAT)
    {
        m_val_num_ = NULL;
        m_val_concat_ = new VerilogExpressions(concat_.begin(), concat_.end());
        m_val_range_ = NULL;
    }
    
    VerilogExpression::VerilogExpression(const string& identifier_)
        : m_type_(IDENTIFIER)
    {
        m_val_num_ = NULL;
        m_val_concat_ = NULL;
        m_val_identifier_ = identifier_;
        m_val_range_ = NULL;
    }
    
    VerilogExpression::VerilogExpression(const string& identifier_, const VerilogRange& range_)
        : m_type_(IDENTIFIER_RANGE)
    {
        m_val_num_ = NULL;
        m_val_concat_ = NULL;
        m_val_identifier_ = identifier_;
        m_val_range_ = new VerilogRange(range_);
    }


    VerilogExpression::~VerilogExpression()
    {
        if (m_val_num_ != NULL)
            delete m_val_num_;
        if (m_val_concat_ != NULL)
            deletePtrVector<VerilogExpression>(m_val_concat_);
        if (m_val_range_ != NULL)
            delete m_val_range_;
    }
    
    // Copy constructor
    VerilogExpression::VerilogExpression(const VerilogExpression& expr_)
        : m_type_(expr_.m_type_), m_val_identifier_(expr_.m_val_identifier_)
    {
        if (expr_.m_val_num_ == NULL) m_val_num_ = NULL;
        else m_val_num_ = new VerilogNumber(*expr_.m_val_num_);
        
        if(expr_.m_val_concat_ == NULL) m_val_concat_ = NULL;
        else
        {
            m_val_concat_ = new VerilogExpressions();
            VerilogExpressions::const_iterator it;
            for (it = expr_.m_val_concat_->begin(); it != expr_.m_val_concat_->end(); ++it)
                m_val_concat_->push_back(new VerilogExpression(*(*it)));
        }
        
        if(expr_.m_val_range_ == NULL) m_val_range_ = NULL;
        else m_val_range_ = new VerilogRange(*expr_.m_val_range_);
    }
    
} // namespace Thermal
