
#include <cassert>
#include <stdio.h>
#include <cstdlib>

#include "../../../include/libutil/LibUtil.h"
#include "VerilogExpression.h"
#include "VerilogScope.h"
#include "VerilogModule.h"
#include "VerilogItem.h"
#include "VerilogParameter.h"
#include "VerilogNet.h"

namespace Thermal
{
    using namespace std;
    using namespace LibUtil;

    VerilogExpression::VerilogExpression(const VerilogNumber& num_)
        : m_type_(NUMBER), m_elaborated_(false)
    {
        m_val_num_ = new VerilogNumber(num_);
        m_val_concat_ = NULL;
        m_val_range_ = NULL;
    }

    VerilogExpression::VerilogExpression(const VerilogExpressions& concat_)
        : m_type_(CONCAT), m_elaborated_(false)
    {
        m_val_num_ = NULL;
        m_val_concat_ = new VerilogExpressions(concat_.begin(), concat_.end());
        m_val_range_ = NULL;
    }
    
    VerilogExpression::VerilogExpression(const string& identifier_)
        : m_type_(IDENTIFIER), m_elaborated_(false)
    {
        m_val_num_ = NULL;
        m_val_concat_ = NULL;
        m_val_identifier_ = identifier_;
        m_val_range_ = NULL;
    }
    
    VerilogExpression::VerilogExpression(const string& identifier_, const VerilogRange& range_)
        : m_type_(IDENTIFIER_RANGE), m_elaborated_(false)
    {
        m_val_num_ = NULL;
        m_val_concat_ = NULL;
        m_val_identifier_ = identifier_;
        m_val_range_ = new VerilogRange(range_);
    }


    VerilogExpression::~VerilogExpression()
    {
        deleteSelf();
    }
    
    // Copy constructor
     VerilogExpression::VerilogExpression(const VerilogExpression& expr_)
    {
        copy(expr_);
    }

    // VerilogExpression::VerilogExpression(const VerilogExpression& expr_)
        // : m_type_(expr_.m_type_), m_elaborated_(expr_.m_elaborated_), m_val_identifier_(expr_.m_val_identifier_)
    // {
        // if (expr_.m_val_num_ == NULL) m_val_num_ = NULL;
        // else m_val_num_ = new VerilogNumber(*expr_.m_val_num_);
        
        // if(expr_.m_val_concat_ == NULL) m_val_concat_ = NULL;
        // else
        // {
            // m_val_concat_ = new VerilogExpressions();
            // VerilogExpressions::const_iterator it;
            // for (it = expr_.m_val_concat_->begin(); it != expr_.m_val_concat_->end(); ++it)
                // m_val_concat_->push_back(new VerilogExpression(*(*it)));
        // }
        
        // if(expr_.m_val_range_ == NULL) m_val_range_ = NULL;
        // else m_val_range_ = new VerilogRange(*expr_.m_val_range_);
    // }
    
    void VerilogExpression::deleteSelf()
    {
        if (m_val_num_ != NULL)
            delete m_val_num_;
        if (m_val_concat_ != NULL)
            deletePtrVector<VerilogExpression>(m_val_concat_);
        if (m_val_range_ != NULL)
            delete m_val_range_;
    }
    
    void VerilogExpression::copy(const VerilogExpression& expr_)
    {
        m_type_ = expr_.m_type_;
        m_elaborated_ = expr_.m_elaborated_;
        m_val_identifier_ = expr_.m_val_identifier_;
        m_const_expr_ = expr_.m_const_expr_;
        
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
    
    void VerilogExpression::elaborate(VerilogScope* scope_)
    {        
        // Mostly will just simplify the expression as best as possible
        if (m_elaborated_)
            throw VerilogException("expression already elaborated: " + toString());
        
        // If I am a concat, I am currently not supported (TODO)
        if (m_type_ == CONCAT) throw VerilogException("Concatenations currently not supported!");                
        // If I am a number, I am considered a constant expression
        else if (m_type_ == NUMBER) m_const_expr_ = true;        
        // Error-check IDENTIFIERs
        else if (m_type_ == IDENTIFIER_RANGE || m_type_ == IDENTIFIER)
        {
            const VerilogItem* item = scope_->getModuleScope()->getItem(m_val_identifier_);            
            if (item->getType() == ITEM_INSTANCE)
                throw VerilogException("Expression identifier cannot be an instance!");
            // Resolve parameters
            else if (item->getType() == ITEM_PARAMETER)
            {
                // The value of the parameter is an expression, so we should just delete ourself and copy it
                deleteSelf();
                copy(((VerilogParameter*) item)->getValue());
                if (!m_elaborated_)
                    throw VerilogException("Internal error: parameter " + item->getIdentifier() +
                        " should have contained an elaborated expression!");
                
            }
            // Turn a plain IDENTIFIER to an IDENTIFIER_RANGE by finding the wire and getting its range
            else if ((item->getType() == ITEM_NET) && (m_type_ == IDENTIFIER))
            {
                m_val_range_ = new VerilogRange(((VerilogNet*) item)->getRange());                            
                m_type_ = IDENTIFIER_RANGE;
            }
        }        
        // After an expression has been elaborated, it must be either a NUMBER or
        // an IDENTIFIER_RANGE or a CONCAT
        if ((m_type_ != NUMBER) && (m_type_ != IDENTIFIER_RANGE) && (m_type_ != CONCAT))
            throw VerilogException("bad expression: " + toString());
        m_elaborated_ = true;
    }
    
    string VerilogExpression::toString() const
    {
        if (m_type_ == NUMBER)
            return m_val_num_->toString();
        else if (m_type_ == IDENTIFIER)
            return m_val_identifier_;
        else if (m_type_ == IDENTIFIER_RANGE)
            return m_val_identifier_ + "[" + m_val_range_->first.toString() + ":" + 
                m_val_range_->second.toString() + "]";
        
        throw VerilogException("No string support for current expression type."); 
    }
    
} // namespace Thermal
