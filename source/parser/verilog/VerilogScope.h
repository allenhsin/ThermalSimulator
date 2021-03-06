#ifndef VERILOG_PARSER_VERILOG_SCOPE_H
#define VERILOG_PARSER_VERILOG_SCOPE_H

#include <string>
#include <vector>
#include <list>
#include <map>
#include "VerilogMisc.h"

// Class responsible for taking care of scoping and symbol lookup during
// module elaboration
namespace VerilogParser
{
    class ElabModule;

    class VerilogScope
    {
        public:
            VerilogScope();
            virtual ~VerilogScope();
            
            // Managing raw modules
            bool hasRawModule(const std::string& name_) const;
            void addRawModule(const RawModule* module_);                        
            const RawModule* getRawModule(const std::string& name_);
            const RawModuleMap& getRawModuleMap() const { return m_raw_module_map_; }
            
            // Managing elaborated modules
            inline void addElabModule(const ElabModule* module_) { m_elab_modules_.push_back(module_); }

            // Get and set current elaborating module
            inline void setElabModule(ElabModule* module_) { m_elab_module_ = module_; }
            inline ElabModule* getElabModule() { return m_elab_module_; }
            inline const ElabModules& getElabModules() const { return m_elab_modules_; }
            
            // Get the value of the symbol in the current stack
            bool has(const std::string& symbol_) const;
            void add(const std::string& symbol_, const BitVector* stuff_);
            const BitVector* get(const std::string& symbol_);

            // Push/pop the elaboration stack
            inline void push(const RawItem* item_) { m_stack_.push_back(item_); }
            inline void pop() { m_stack_.pop_back(); }
            // Get current hierarchy string
            std::string getHierString() const;
            
            // Elaborate the verilog for some top-level module name
            void elaborate(const std::string& top_name_);
            // Get the top post elaboration verilog module, can just return
            // the current module being elaborated as the top module is the
            // last module to finish elaboration
            const ElabModule* getTopModule() const { return m_elab_module_; }
            
        private:
            // Helper function for looking up a symbol
            bool hasSymbol(const std::string& full_symbol_) const;
            
        private:
            // Elaboration hierarchy stack
            ScopeStack m_stack_;
            // Current module being elaborated
            ElabModule* m_elab_module_;
            // Global symbol lookup map
            SymbolMap m_symbol_map_;            

            // Map of all raw modules
            RawModuleMap m_raw_module_map_;            
            // Vector of all elaborated modules
            ElabModules m_elab_modules_;
    };
}
#endif
