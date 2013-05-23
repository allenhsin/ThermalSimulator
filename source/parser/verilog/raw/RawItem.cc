
#include "RawItem.h"

namespace VerilogParser
{
    using std::string;

    RawItem::RawItem(RawItemType type_, const std::string& identifier_)
        : m_type_(type_), m_identifier_(identifier_)
    {}
    
    RawItem::~RawItem()
    {}
    
    RawItem::RawItem(const RawItem& item_)
        : m_type_(item_.m_type_), m_identifier_(item_.m_identifier_)
    {}

    string RawItem::toString() const
    {
        switch (m_type_)
        {
            case RAW_ITEM_PARAM: return "RAW:PARAM>" + m_identifier_;
            case RAW_ITEM_NET: return "RAW:NET>" + m_identifier_;
            case RAW_ITEM_INST: return "RAW:INST>" + m_identifier_;        
            default: return "RAW:BADTYPE>" + m_identifier_;
        }
    }
    
} // namespace VerilogParser
