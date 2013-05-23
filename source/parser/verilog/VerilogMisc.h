#ifndef VERILOG_PARSER_VERILOG_MISC_H
#define VERILOG_PARSER_VERILOG_MISC_H

#include <exception>
#include <string>
#include <vector>
#include <map>

// Various other data structures for verilog parsing
namespace VerilogParser
{
    class VerilogElabItem;
    class VerilogElabModule;

    class VerilogModule;
    class VerilogItem;

    class VerilogExpression;
    class VerilogParameter;
    class VerilogNet;
    class VerilogInstance;
    class VerilogScope;
    
    // Type of items inside modules
    enum VerilogItemType
    {
        ITEM_PARAMETER,
        ITEM_NET,
        ITEM_INSTANCE
    };
    
    // Port type
    enum VerilogPortType
    {
        PORT_INPUT,
        PORT_OUTPUT,
        PORT_INOUT,
        PORT_NONE
    };
    
    // We don't support anything besides regular wires, but we will leave this in for
    // generality
    enum VerilogNetType
    {
        NET_WIRE
    };

    typedef std::pair<VerilogExpression, VerilogExpression> VerilogRange;
    typedef std::vector<std::string> VerilogVariables;
    
    typedef std::pair<std::string, VerilogExpression> VerilogConn;
    
    typedef std::vector<VerilogModule*> VerilogModules;
    typedef std::vector<VerilogItem*> VerilogItems;
    typedef std::vector<VerilogConn*> VerilogConns;
    typedef std::vector<VerilogExpression*> VerilogExpressions;
    
    typedef std::vector<VerilogParameter*> VerilogParameters;
    typedef std::vector<VerilogNet*> VerilogNets;
    typedef std::vector<VerilogInstance*> VerilogInstances;
        
    typedef std::map<std::string, VerilogModule*> VerilogModuleMap;
    typedef std::map<std::string, VerilogItem*> VerilogItemMap;
    
    typedef std::map<std::string, std::string> VerilogEquivNetMap;
    
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
    
    typedef std::pair<std::string, std::string> VerilogElabConn;
    typedef std::vector<VerilogElabConn*> VerilogElabConns;
    typedef std::vector<VerilogElabItem*> VerilogElabItems;
    
    typedef std::map<std::string, VerilogElabItem*> VerilogElabItemMap;
    typedef std::map<std::string, VerilogElabModule*> VerilogElabModuleMap;
    
    class VerilogElabRange
    {
        public:
            VerilogElabRange(int high_, int low_);
            ~VerilogElabRange();

        public:
            int high;
            int low;
    };
    
    //=========================================================================
    
    class Expression;
    class LHExpression;
    class RHExpression;
    class RawItem;        
    
    // Port type
    enum PortType
    {
        HAHA_PORT_INPUT,
        HAHA_PORT_OUTPUT,
        HAHA_PORT_INOUT,
        HAHA_PORT_NONE
    };
    
    // We don't support anything besides regular wires, but we will leave this in for
    // generality
    enum NetType
    {
        HAHA_NET_WIRE
    };

    // Bit range class
    class BitRange
    {
        public:
            BitRange(const Expression& high_, const Expression& low_);
            BitRange(const BitRange& range_);
            ~BitRange();
            
            std::string toString() const;
        
        public:
            const Expression* high;
            const Expression* low;
    };
    
    // Value set class
    class SetValue
    {
        public:
            SetValue(const std::string& identifier_, const Expression* value_);
            ~SetValue();
            
            std::string toString() const;
            
        public:
            const std::string identifier;
            const Expression* value;
    };
    
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
    
    typedef std::string BinOperator;

    typedef std::vector<std::string> Identifiers;
    typedef std::vector<SetValue*> SetValues;

    // Typedefs for expressions
    typedef std::vector<Expression*> Expressions;
    typedef std::vector<LHExpression*> LHExpressions;
    typedef std::vector<RHExpression*> RHExpressions;
    
    // Typdef for items
    typedef std::vector<RawItem*> RawItems;
    
}
#endif
