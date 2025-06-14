

#ifndef XMRIG_DMITOOLS_H
#define XMRIG_DMITOOLS_H


#include <cstddef>
#include <cstdint>


namespace xmrig {


struct dmi_header
{
    uint8_t type;
    uint8_t length;
    uint16_t handle;
    uint8_t *data;
};


struct u64 {
    uint32_t l;
    uint32_t h;
};


template<typename T>
inline T dmi_get(const uint8_t *data)                   { return *reinterpret_cast<const T *>(data); }

template<typename T>
inline T dmi_get(const dmi_header *h, size_t offset)    { return *reinterpret_cast<const T *>(h->data + offset); }


const char *dmi_string(dmi_header *dm, size_t offset);


} /* namespace xmrig */


#endif /* XMRIG_DMITOOLS_H */
