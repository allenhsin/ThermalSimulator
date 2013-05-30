#ifndef VERILOG_PARSER_VERILOG_MISC_H
#define VERILOG_PARSER_VERILOG_MISC_H

#include <exception>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <limits>

// Various other data structures for verilog parsing
namespace VerilogParser
{
    class VerilogScope;

    class BitVector;
    class Expression;
    class LHExpression;
    class RHExpression;
    class RawItem;
    class RawNet;
    class RawInstance;
    class RawParameter;
    class RawModule;
    
    class ElabItem;
    class ElabNet;
    class ElabInstance;
    class ElabParameter;
    class ElabModule;
    
    // Global constants
    static const unsigned long MAX_WIDTH = std::numeric_limits<unsigned long>::digits;;
    
    // Bit range class
    class BitRange
    {
        public:
            BitRange(const Expression& high_, const Expression& low_);
            BitRange(const BitRange& range_);
            ~BitRange();
            
            // Get high and low as numbers
            int high(VerilogScope* scope_) const;
            int low(VerilogScope* scope_) const;
            
            // To string
            std::string toString() const;
        
        private:
            const Expression* m_high_;
            const Expression* m_low_;
    };
    
    // Value set class
    class SetValue
    {
        public:
            // Set value where both the identifier (variable being set) and its value are defined
            SetValue(const std::string& identifier_, const Expression& value_);
            // Set value where only the value is defined, the identifier is implicit with some ordering
            SetValue(const Expression& value_);
            // Copy constructor
            SetValue(const SetValue& val_);
            ~SetValue();
            
            // Accessors
            inline bool hasImplicitID() const { return m_implicit_; }
            inline const std::string& getIdentifier() const { return m_identifier_; }
            inline const Expression* getValue() const { return m_value_; }
            
            // To string
            std::string toString() const;
            
        private:
            const bool m_implicit_;
            const std::string m_identifier_;
            const Expression* m_value_;
    };
    
    class VerilogException : public std::exception
    {
        public:
            VerilogException(const std::string& message_);
            virtual ~VerilogException() throw();
            
            inline virtual const char* what() const throw() { return m_message_.c_str(); }
        
        private:
            // String containing the exception
            std::string m_message_;
    };
    
    template<class T> std::string makeString(const T& value_)
    {
        std::ostringstream ost;
        ost << value_;
        return ost.str();
    }

    template<class T> void clearPtrVector(std::vector<T*>* vec_)
    {
        for(typename std::vector<T*>::iterator it = vec_->begin(); it != vec_->end(); ++it)
        {
            T* temp_T = (*it);
            delete temp_T;
        }
        vec_->clear();
        return;
    }

    template<class T> void deletePtrVector(std::vector<T*>* vec_)
    {
        clearPtrVector<T>(vec_);
        delete vec_;
        return;
    }
    
    // Port type
    enum PortType
    {
        PORT_INPUT,
        PORT_OUTPUT,
        PORT_INOUT,
        PORT_NONE
    };
    
    // We don't support anything besides regular wires, but we will leave this in for
    // generality
    enum NetType
    {
        NET_WIRE
    };
    
    // Type of items inside raw modules
    enum RawItemType
    {
        RAW_ITEM_PARAM,
        RAW_ITEM_NET,
        RAW_ITEM_INST,
        RAW_ITEM_ASSIGN
    };
    
    // Type of items inside elaborated modules
    enum ElabItemType
    {
        ELAB_ITEM_PARAM,
        ELAB_ITEM_NET,
        ELAB_ITEM_INST,
        ELAB_ITEM_ASSIGN
    };
    
    typedef std::string BinOperator;

    typedef std::vector<std::string> Identifiers;
    typedef std::vector<SetValue*> SetValues;

    // Typedefs for expressions
    typedef std::vector<Expression*> Expressions;
    typedef std::vector<LHExpression*> LHExpressions;
    typedef std::vector<RHExpression*> RHExpressions;
    
    // Typdefs for raw items
    typedef std::vector<RawItem*> RawItems;
    typedef std::vector<RawNet*> RawNets;
    typedef std::vector<RawInstance*> RawInstances;
    typedef std::vector<RawParameter*> RawParameters;
    
    // Typedefs for raw modules
    typedef std::vector<const RawModule*> RawModules;
    typedef std::map<std::string, const RawModule*> RawModuleMap;
    
    // Typedefs for elaborated items
    typedef std::vector<const ElabItem*> ElabItems;
    typedef std::map<std::string, const ElabItem*> ElabItemMap;
    typedef std::vector<ElabNet*> ElabNets;
    
    // Typedef for elaborated modules
    typedef std::vector<const ElabModule*> ElabModules;
    
    // Scope variables
    typedef std::list<const RawItem*> ScopeStack;
    typedef std::map<std::string, const BitVector*> SymbolMap;
    
}
#endif
