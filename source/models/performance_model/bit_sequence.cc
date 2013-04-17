
#include <cassert>
#include <stdio.h>

#include "source/data/data.h"
#include "source/models/performance_model/bit_sequence.h"
#include "libutil/LibUtil.h"

namespace Thermal
{
    BitSequence::BitSequence(BitSequenceType type)
        : _ratio_of_ones    (0.5)
        , _activity         (0)
        , _cur_bit          (false)
        , _type             (type)
    {}

    BitSequence::~BitSequence()
    {}

    BitSequenceType BitSequence::getType()
    {
        return _type;
    }
    
} // namespace Thermal
