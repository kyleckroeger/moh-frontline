// BIG archive headers: format detection (0xC0FB, "BIGF", "BIG\0"), header
// size, and the optional trailing debug tag (a letter and a three-digit
// version, then a 32-bit value). The rest of the file (entry lookup) is not
// reconstructed here.
extern "C" int BIG_typeofheader(const void* header) {
    int type = 0;

    if (*(unsigned short*)header == 0xc0fb)
        type = 1;
    else if (*(unsigned int*)header == 0x42494746)
        type = 2;
    else if ((*(unsigned int*)header & ~0xff) == 0x42494700)
        type = 3;
    return type;
}

extern "C" int BIG_sizeofheader(const void* header) {
    int size = 0;

    switch (BIG_typeofheader(header)) {
    case 1:
        size = ((unsigned short*)header)[1] + 4;
        break;
    case 2:
    case 3:
        size = ((int*)header)[3];
        break;
    }
    return size;
}

static int BIG_debuginfo(const void* header, int* version, unsigned int* value) {
    int result = 0;
    const char* tag = (const char*)header + BIG_sizeofheader(header) - 8;

    if (((tag[0] >= 'A' && tag[0] <= 'Z') || (tag[0] >= 'a' && tag[0] <= 'z')) && tag[1] >= '0' && tag[1] <= '9' &&
        tag[2] >= '0' && tag[2] <= '9' && tag[3] >= '0' && tag[3] <= '9') {
        result = 8;
        if (version)
            *version = (tag[1] - '0') * 100 + (tag[2] - '0') * 10 + (tag[3] - '0');
        if (value)
            *value = *(unsigned int*)(tag + 4);
    }
    return result;
}
