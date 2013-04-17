
#ifndef __THERMAL_RANDOM_BIT_SEQUENCE_H__
#define __THERMAL_RANDOM_BIT_SEQUENCE_H__

#include "source/models/performance_model/bit_sequence.h"

namespace Thermal
{
    class RandomBitSequence : public BitSequence
    {
    
    public:
        RandomBitSequence();
        virtual ~RandomBitSequence();

        // Get next bit in the sequence
        bool getNextBit();

    private:
        // inverse of max random number, in double
        static const double RAND_MAX_INV;
        
    };

} // namespace Thermal

#endif // __THERMAL_RANDOM_BIT_SEQUENCE_H__

