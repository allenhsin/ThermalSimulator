#ifndef VERILOG_PARSER_ELAB_ASSIGN_H
#define VERILOG_PARSER_ELAB_ASSIGN_H

#include <string>
#include "../VerilogMisc.h"
#include "../BitVector.h"
#include "ElabItem.h"

namespace VerilogParser
{
    // An elaborated verilog net
    class ElabAssign : public ElabItem
    {
        public:
            // Private constructor, use the static elaborate function instead
            ElabAssign(const BitVector& left_, const BitVector& right_);
            virtual ~ElabAssign();
            
        protected:
            // Protected copy constructor, use clone instead
            ElabAssign(const ElabAssign& net_);
            
        public:
            // Accessor functions
            inline const BitVector& getLeft() const { return m_left_; }
            inline const BitVector& getRight() const { return m_right_; }
            
            // Clone
            inline ElabAssign* clone() const { return new ElabAssign(*this); }            
            // Get string representation            
            inline std::string toString() const { return ElabItem::toString(); }
            
        private:
            // Bit targets
            const BitVector m_left_;
            // Net type
            const BitVector m_right_;
    };
}
#endif
