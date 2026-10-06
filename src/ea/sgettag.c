// SNDI_gettag: read the next tag of a sound header tag stream (252 = padding,
// 253/254 = markers without data, 255 = end). Small values (up to 4 bytes)
// are decoded; data points at the tag's payload. SNDTAGINFO is named by the
// mangled symbol; its members are inferred from offsets.
struct SNDTAGINFO {
    unsigned char* p;
    int tag;
    int value;
    unsigned char* data;
    unsigned int length;
};

int SNDI_getb(void*, int);

int SNDI_gettag(SNDTAGINFO* info) {
    while (*info->p == 252)
        info->p++;
    info->tag = *info->p;
    if (info->tag == 255)
        return 0;
    info->p++;
    if (info->tag == 253)
        return 1;
    if (info->tag == 254)
        return 1;
    info->length = *info->p;
    if (info->length == 255) {
        info->length = SNDI_getb(info->p + 1, 4);
        info->p += 4;
    }
    info->p++;
    info->data = info->p;
    if (info->length <= 4)
        info->value = SNDI_getb(info->p, info->length);
    info->p += info->length;
    return 1;
}
