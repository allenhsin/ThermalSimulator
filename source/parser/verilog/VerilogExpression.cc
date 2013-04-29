
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

namespace VerilogParser
{
    using namespace std;
    using namespace LibUtil;

    VerilogExpression::VerilogExpression(const VerilogNumber& num_)
        : m_type_(NUMBER), m_elaborated_(false), m_const_expr_(true)
    {
        m_val_num_ = new VerilogNumber(num_);
        m_val_concat_ = NULL;
        m_val_range_ = NULL;
        m_val_expr0_ = NULL;
        m_val_expr1_ = NULL;
    }

    VerilogExpression::VerilogExpression(const VerilogExpressions& concat_)
        : m_type_(CONCAT), m_elaborated_(false), m_const_expr_(false)
    {
        m_val_num_ = NULL;
        m_val_concat_ = new VerilogExpressions(concat_.begin(), concat_.end());
        m_val_range_ = NULL;
        m_val_expr0_ = NULL;
        m_val_expr1_ = NULL;
    }
    
    VerilogExpression::VerilogExpression(const string& identifier_)
        : m_type_(IDENTIFIER), m_elaborated_(false), m_const_expr_(false)
    {
        m_val_num_ = NULL;
        m_val_concat_ = NULL;
        m_val_identifier_ = identifier_;
        m_val_range_ = NULL;
        m_val_expr0_ = NULL;
        m_val_expr1_ = NULL;
    }
    
    VerilogExpression::VerilogExpression(const string& identifier_, const VerilogRange& range_)
        : m_type_(IDENTIFIER_RANGE), m_elaborated_(false), m_const_expr_(false)
    {
        m_val_num_ = NULL;
        m_val_concat_ = NULL;
        m_val_identifier_ = identifier_;
        m_val_range_ = new VerilogRange(range_);
        m_val_expr0_ = NULL;
        m_val_expr1_ = NULL;
    }
    
    VerilogExpression::VerilogExpression(const VerilogExpression& expr0_, VerilogBinOp bin_op, const VerilogExpression& expr1_)
        : m_type_(BINARY_EXPRESSION), m_elaborated_(false), m_const_expr_(false)
    {
        m_val_num_ = NULL;
        m_val_concat_ = NULL;
        m_val_range_ = NULL;
        m_val_bin_op_ = bin_op;
        m_val_expr0_ = new VerilogExpression(expr0_);
        m_val_expr1_ = new VerilogExpression(expr1_);
        
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

    void VerilogExpression::deleteSelf()
    {
        if (m_val_num_ != NULL)
            delete m_val_num_;
        if (m_val_concat_ != NULL)
            deletePtrVector<VerilogExpression>(m_val_concat_);
        if (m_val_range_ != NULL)
            delete m_val_range_;
        if (m_val_expr0_ != NULL)
            delete m_val_expr0_;
        if (m_val_expr1_ != NULL)
            delete m_val_expr1_;
        
        m_val_num_ = NULL;
        m_val_concat_ = NULL;
        m_val_range_ = NULL;
        m_val_expr0_ = NULL;
        m_val_expr1_ = NULL;
    }
    
    void VerilogExpression::copy(const VerilogExpression& expr_)
    {
        m_type_ = expr_.m_type_;
        m_elaborated_ = expr_.m_elaborated_;
        m_val_identifier_ = expr_.m_val_identifier_;
        m_val_bin_op_ = expr_.m_val_bin_op_;
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
        
        if(expr_.m_val_expr0_ == NULL) m_val_expr0_ = NULL;
        else m_val_expr0_ = new VerilogExpression(*expr_.m_val_expr0_);

        if(expr_.m_val_expr1_ == NULL) m_val_expr1_ = NULL;
        else m_val_expr1_ = new VerilogExpression(*expr_.m_val_expr1_);
    }
    
    void VerilogExpression::elaborate(VerilogScope* scope_)
    {        
        // Mostly will just simplify the expression as best as possible
        // if (m_elaborated_)
            // throw VerilogException("expression already elaborated: " + toString());
        
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
            else if (item->getType() == ITEM_NET)
            {
                // Turn a plain IDENTIFIER to an IDENTIFIER_RANGE by finding the wire and getting its range
                if (m_type_ == IDENTIFIER)
                {
                    m_val_range_ = new VerilogRange(((VerilogNet*) item)->getRange());                            
                    m_type_ = IDENTIFIER_RANGE;
                }
                // Check that the range is valid
                if (!VerilogNet::isValidSubRange(((VerilogNet*) item)->getRange(), *m_val_range_))
                    throw VerilogException("Invalid range: [" + m_val_range_->first.toString() + ":" + m_val_range_->second.toString() + "]");
            }
        }
        else if (m_type_ == BINARY_EXPRESSION)
        {            
            m_val_expr0_->elaborate(scope_);
            m_val_expr1_->elaborate(scope_);
            evaluate();
        }
        // After an expression has been elaborated, it must be either a NUMBER or
        // an IDENTIFIER_RANGE or a CONCAT
        if ((m_type_ != NUMBER) && (m_type_ != IDENTIFIER_RANGE) && (m_type_ != CONCAT))
            throw VerilogException("bad expression: " + toString());
        m_elaborated_ = true;
    }
    
    void VerilogExpression::evaluate()
    {
        if (m_type_ == BINARY_EXPRESSION)
        {
            if ((m_val_expr0_->getType() != NUMBER) || (m_val_expr1_->getType() != NUMBER))
                throw VerilogException("Expression did not resolve to constant number: " + toString());

            switch(m_val_bin_op_)
            {
                case BOP_PLUS:
                    m_val_num_ = new VerilogNumber(m_val_expr0_->getNumber().getUInt() + m_val_expr1_->getNumber().getUInt());
                    break;
                case BOP_MINUS:
                    m_val_num_ = new VerilogNumber(m_val_expr0_->getNumber().getUInt() - m_val_expr1_->getNumber().getUInt());                    
                    break;
                case BOP_TIMES:
                    m_val_num_ = new VerilogNumber(m_val_expr0_->getNumber().getUInt() * m_val_expr1_->getNumber().getUInt());
                    break;
                case BOP_DIV:
                    m_val_num_ = new VerilogNumber(m_val_expr0_->getNumber().getUInt() / m_val_expr1_->getNumber().getUInt());
                    break;
                case BOP_MOD:
                    m_val_num_ = new VerilogNumber(m_val_expr0_->getNumber().getUInt() % m_val_expr1_->getNumber().getUInt());
                    break;
                default:
                    throw VerilogException("Bad binary operator " + m_val_bin_op_);
            }
            m_type_ = NUMBER;
            m_const_expr_ = true;
            
            delete m_val_expr0_;
            delete m_val_expr1_;
            
            m_val_expr0_ = NULL;
            m_val_expr1_ = NULL;
        }
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
        else if (m_type_ == BINARY_EXPRESSION)
        {
            switch(m_val_bin_op_)
            {
                case BOP_PLUS:
                    return m_val_expr0_->toString() + " + " + m_val_expr1_->toString();
                case BOP_MINUS:
                    return m_val_expr0_->toString() + " - " + m_val_expr1_->toString();
                case BOP_TIMES:
                    return m_val_expr0_->toString() + " * " + m_val_expr1_->toString();
                case BOP_DIV:
                    return m_val_expr0_->toString() + " / " + m_val_expr1_->toString();
                case BOP_MOD:
                    return m_val_expr0_->toString() + " % " + m_val_expr1_->toString();
                default:
                    throw VerilogException("Bad binary operator " + m_val_bin_op_);
            }
        }
        
        throw VerilogException("No string support for current expression type."); 
    }
    
    void VerilogExpression::useHierName(const string& hier_name_)
    {
        if (m_val_identifier_.size() != 0)
            m_val_identifier_ = hier_name_ + m_val_identifier_;
    }
    
    unsigned int VerilogExpression::getConstExpr() const
    {
        if (!m_const_expr_) throw VerilogException("Expected constant expression, got " + toString());        
        return m_val_num_->getUInt();
    }
    
} // namespace VerilogParser
