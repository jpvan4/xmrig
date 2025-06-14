
#include "hw/dmi/DmiBoard.h"
#include "3rdparty/rapidjson/document.h"
#include "hw/dmi/DmiTools.h"


void xmrig::DmiBoard::decode(dmi_header *h)
{
    if (h->length < 0x08) {
        return;
    }

    m_vendor  = dmi_string(h, 0x04);
    m_product = dmi_string(h, 0x05);
}


#ifdef XMRIG_FEATURE_API
rapidjson::Value xmrig::DmiBoard::toJSON(rapidjson::Document &doc) const
{
    using namespace rapidjson;

    auto &allocator = doc.GetAllocator();
    Value out(kObjectType);
    out.AddMember("vendor",     m_vendor.toJSON(doc), allocator);
    out.AddMember("product",    m_product.toJSON(doc), allocator);

    return out;
}
#endif
