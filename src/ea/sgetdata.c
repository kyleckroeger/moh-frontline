// SNDI_getb: read a big-endian signed integer of 1 to 3 bytes (wider values
// are returned unextended).
int SNDI_getb(void* data, int bytes) {
    unsigned char* p = (unsigned char*)data;
    int value = 0;
    int i;

    for (i = bytes; i; i--)
        value = (value << 8) + *p++;
    if (bytes == 1 && value > 127)
        value -= 0x100;
    else if (bytes == 2 && value > 32767)
        value -= 0x10000;
    else if (bytes == 3 && value > 0x7fffff)
        value -= 0x1000000;
    return value;
}
