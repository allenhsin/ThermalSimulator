#ifndef VERILOG_PARSER_BIT_H
#define VERILOG_PARSER_BIT_H

#include <string>
#include "VerilogMisc.h"

namespace VerilogParser
{
    class ElabNet;

    // A class base representing a 1-bit elaborated target
    class Bit
    {
        public:
            Bit(bool constant_);
            virtual ~Bit();
            
        public:
            // Accessor functions
            inline bool isConstant() const { return m_constant_; }
            // Clone this bit target
            virtual Bit* clone() const = 0;
            // Return a string representation
            virtual std::string toString() const = 0;

        protected:
            // Protected copy constructor, use clone instead    
            Bit(const Bit& bit_);
                        
        private:
            // Whether the bit target is a constant number
            const bool m_constant_;
    };
    
    // A clas representing a constant bit target
    class ConBit : public Bit
    {
        public:
            ConBit(bool bit_);
            ~ConBit();
        public:
            // Accessor functions
            inline bool getBit() const { return m_bit_; }
            // Clone this bit target
            inline ConBit* clone() const { return new ConBit(*this); }
            // Return a string representation
            std::string toString() const;
            
        protected:
            // Protected copy constructor, use clone instead    
            ConBit(const ConBit& bit_);
                        
        private:
            // The value of the constant bit
            const bool m_bit_;
    };
    
    // A class representing a bit target that is an elaborated net
    class NetBit : public Bit
    {
        public:
            NetBit(const ElabNet* net_);
            ~NetBit();
        
        public:
            // Accessor functions
            inline const ElabNet* getNet() const { return m_net_; }
            // Clone this bit target
            inline NetBit* clone() const { return new NetBit(*this); }
            // Return a string representation
            std::string toString() const;
            
        protected:
            // Protected copy constructor, use clone instead
            NetBit(const NetBit& bit_);
            
        private:
            // The net this bit target points to
            const ElabNet* m_net_;
    };
}
#endif
