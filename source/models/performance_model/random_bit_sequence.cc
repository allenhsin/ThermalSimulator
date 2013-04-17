
#include <cassert>
#include <stdio.h>

#include <cstdlib>

#include "source/data/data.h"
#include "source/models/performance_model/random_bit_sequence.h"
#include "libutil/LibUtil.h"

namespace Thermal
{
    const double RandomBitSequence::RAND_MAX_INV = 1.0 / (double) RAND_MAX;

    RandomBitSequence::RandomBitSequence()
        : BitSequence(RANDOM)
    {}

    RandomBitSequence::~RandomBitSequence()
    {}

    bool RandomBitSequence::getNextBit()
    {
        // roll for activity
        double act_roll = ((double) rand()) * RAND_MAX_INV;
        if (act_roll <= _activity)
        {
            // roll for bit
            double bit_roll = ((double) rand()) * RAND_MAX_INV;
            _cur_bit = (bit_roll <= _ratio_of_ones);
        }
        return _cur_bit;
    }
    
} // namespace Thermal
