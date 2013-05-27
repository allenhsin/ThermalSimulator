
#include "ElabItem.h"
#include "../VerilogScope.h"

namespace VerilogParser
{
    using std::string;

    ElabItem::ElabItem(ElabItemType type_, const string& identifier_, const VerilogScope* scope_)
        : m_type_(type_), m_identifier_(identifier_), m_path_(scope_->getHierString() + identifier_)
    {}
    
    ElabItem::~ElabItem()
    {}
        
    ElabItem::ElabItem(const ElabItem& item_)
        : m_type_(item_.m_type_), m_identifier_(item_.m_identifier_), m_path_(item_.m_path_)
    {}

    ElabItem* ElabItem::getFlattenedClone(const std::string& hier_, const std::string& /* hier_sep_ */) const
    {
        ElabItem* c = clone();
        c->m_identifier_ = hier_ + c->m_identifier_;        
        return c;
    }
    
    string ElabItem::toString() const
    {
        switch (m_type_)
        {
            case ELAB_ITEM_PARAM: return "ELAB:PARAM>" + m_path_;
            case ELAB_ITEM_NET: return "ELAB:NET>" + m_path_;
            case ELAB_ITEM_INST: return "ELAB:INST>" + m_path_;
            case ELAB_ITEM_ASSIGN: return "ELAB:ASSIGN>" + m_path_;
            default: return "ELAB:BADTYPE>" + m_path_;
        }
    }
    
} // namespace VerilogParser
