#include <dolphin/ip.h>
#include <dolphin/private/ip.h>

// Range: 0x0 -> 0x5C
void IPSwapAddr(u8* a /* r30 */, u8* b /* r31 */) {
    // Local variables
    u8 c[4]; // r1+0x10

    memmove(c, a, 4);
    memmove(a, b, 4);
    memmove(b, c, 4);
}

// Range: 0x5C -> 0x194
s32 IPProcessSourceRoute(IPHeader* ip /* r27 */) {
    // Local variables
    u8* opt; // r30
    int optlen; // r28
    int i; // r31
    s32 len; // r29
    s32 ptr; // r26

    ASSERTLINE(51, ip);
    optlen = IP_HLEN(ip) - (int)sizeof(IPHeader);
    ASSERTLINE(53, 0 <= optlen);
    opt = (u8*)(ip + 1);
    for (i = 0; i < optlen && opt[i] != 0; i += len) {
        switch (opt[i]) {
            case 0:
            case 1:
                len = 1;
                break;
            default:
                if (optlen <= i + 1) {
                    return -12;
                }
                len = opt[i + 1];
                if (optlen < i + len) {
                    return -12;
                }
                break;
        }
        if (opt[i] == 0x83 || opt[i] == 0x89) {
            if (len < 7) {
                return -12;
            }
            ptr = opt[i + 2];
            if (ptr + 3 <= len) {
                return -12;
            }
            return 0;
        }
    }
    return 0;
}

// Range: 0x194 -> 0x308
s32 IPReverseSourceRoute(IPHeader* ip /* r27 */) {
    // Local variables
    u8* opt; // r30
    int optlen; // r28
    int i; // r31
    s32 len; // r29
    s32 ptr; // r26

    if (ip == NULL) {
        return -12;
    }
    optlen = IP_HLEN(ip) - (int)sizeof(IPHeader);
    if (optlen <= 0) {
        return 0;
    }
    opt = (u8*)(ip + 1);
    for (i = 0; i < optlen && opt[i] != 0; i += len) {
        switch (opt[i]) {
            case 0:
            case 1:
                len = 1;
                break;
            default:
                if (optlen <= i + 1) {
                    return -12;
                }
                len = opt[i + 1];
                if (optlen < i + len) {
                    return -12;
                }
                break;
        }
        if (opt[i] == 0x83 || opt[i] == 0x89) {
            if (len < 7) {
                return -12;
            }
            ptr = opt[i + 2];
            if (ptr + 3 <= len) {
                return -12;
            }
            len -= 3;
            len &= ~3;
            opt += i;
            opt[2] = 4;
            opt += 3;
            IPSwapAddr(ip->dst, opt + len - 4);
            len -= 4;
            for (i = len / 2; i >= 4; i -= 4) {
                IPSwapAddr(opt + i, opt + len - 4 - i);
            }
            return 0;
        }
    }
    return 0;
}

// Range: 0x308 -> 0x444
s32 IPUpdateRecordRoute(IPHeader* ip /* r26 */, u8* addr /* r25 */) {
    // Local variables
    u8* opt; // r30
    int optlen; // r28
    int i; // r31
    s32 len; // r29
    s32 ptr; // r27

    if (ip == NULL || addr == NULL) {
        return -12;
    }
    optlen = IP_HLEN(ip) - (int)sizeof(IPHeader);
    if (optlen <= 0) {
        return 0;
    }
    for (i = 0, opt = (u8*)(ip + 1); i < optlen && opt[i] != 0; i += len) {
        switch (opt[i]) {
            case 0:
            case 1:
                len = 1;
                break;
            default:
                if (optlen <= i + 1) {
                    return -12;
                }
                len = opt[i + 1];
                if (optlen < i + len) {
                    return -12;
                }
                break;
        }
        if (opt[i] == 7) {
            if (len < 7) {
                return -12;
            }
            ptr = opt[i + 2];
            if (ptr + 3 <= len) {
                memmove(opt + i + ptr - 1, addr, 4);
                opt[i + 2] = ptr + 4;
            }
            return 0;
        }
    }
    return 0;
}
