#ifndef THERMAL_PARSER_VERILOG_MISC_H
#define THERMAL_PARSER_VERILOG_MISC_H

#include <exception>
#include <string>
#include <vector>
#include <map>

// Various other data structures for verilog parsing
namespace VerilogParser
{
    class VerilogModule;
    class VerilogItem;

    class VerilogExpression;
    class VerilogParameter;
    class VerilogNet;
    class VerilogInstance;
    class VerilogScope;
    
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
    
    // // A base class for a elaborate-able verilog class
    // class VerilogElaborate
    // {
        // public:
            // VerilogElaborate();
            // virtual ~VerilogElaborate();
            
        // public:
            // void elaborate(VerilogScope* scope_);
        
        // protected:
            // virtual void elaborateItem(VerilogScope* scope_) == 0;
            
        // private:
            // // Boolean containing whether this module has been elaborated
            // bool m_elaborated_;
            // // Error message to display on elaboration failure
            
    // }
}
#endif
