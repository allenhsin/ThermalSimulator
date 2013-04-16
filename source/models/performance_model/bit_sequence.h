
#ifndef __THERMAL_BIT_SEQUENCE_H__
#define __THERMAL_BIT_SEQUENCE_H__

namespace Thermal
{
    enum BitSequenceType
    {
        RANDOM,
        PRBS7
    };
    
    class BitSequence
    {

    protected:
        BitSequence(BitSequenceType type);

    public:
        ~BitSequence();

        // Get the type of bit sequence
        BitSequenceType getType();
        
        // Get next bit in the sequence
        virtual bool getNextBit() = 0;

    private:
        // type
        BitSequenceType _type;
        
    };

} // namespace Thermal

#endif // __THERMAL_BIT_SEQUENCE_H__

