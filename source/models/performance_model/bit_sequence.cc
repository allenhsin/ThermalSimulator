
#include <cassert>
#include <stdio.h>

#include "source/data/data.h"
#include "source/models/performance_model/bit_sequence.h"
#include "libutil/LibUtil.h"

namespace Thermal
{
    BitSequence::BitSequence(BitSequenceType type)
        : _type                     (type)
    {}

    BitSequence::~BitSequence()
    {}

    BitSequenceType BitSequence::getType()
    {
        return _type;
    }
    
} // namespace Thermal
