
#include "hw/dmi/DmiTools.h"


#include <cstring>


namespace xmrig {


/* Replace non-ASCII characters with dots */
static void ascii_filter(char *bp, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        if (bp[i] < 32 || bp[i] >= 127) {
            bp[i] = '.';
        }
    }
}


static char *_dmi_string(dmi_header *dm, uint8_t s, bool filter)
{
    char *bp = reinterpret_cast<char *>(dm->data);

    bp += dm->length;
    while (s > 1 && *bp)  {
        bp += strlen(bp);
        bp++;
        s--;
    }

    if (!*bp) {
        return nullptr;
    }

    if (filter) {
        ascii_filter(bp, strlen(bp));
    }

    return bp;
}


const char *dmi_string(dmi_header *dm, size_t offset)
{
    if (offset < 4) {
        return nullptr;
    }

    return _dmi_string(dm, dm->data[offset], true);
}


} // namespace xmrig
