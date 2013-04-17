
#ifndef __THERMAL_BIT_SEQUENCE_H__
#define __THERMAL_BIT_SEQUENCE_H__

namespace Thermal
{
    enum BitSequenceType
    {
        RANDOM,
        PRBS7,
        NUMBER_BIT_SEQUENCE_TYPES
    };
    
    class BitSequence
    {

    protected:
        BitSequence(BitSequenceType type);

    public:
        virtual ~BitSequence();

        // Get the type of bit sequence
        BitSequenceType getType();
        
        // Get next bit in the sequence
        virtual bool getNextBit() = 0;

        // set the activity factor of the sequence
        void setActivityFactor(double activity)
        { _activity = activity; }
        
        // set the ratio of 1s in the sequence
        void setBitRatio(double ratio_of_ones)
        { _ratio_of_ones = ratio_of_ones; }

    protected:
        // ratio of ones
        double  _ratio_of_ones;
        // activity
        double  _activity;
        // cur bit
        bool    _cur_bit;

    private:
        // type
        BitSequenceType _type;
        
    };

} // namespace Thermal

#endif // __THERMAL_BIT_SEQUENCE_H__

