#include <dolphin/ip.h>
#include <dolphin/private/ip.h>
#include <ctype.h>
#include <stdlib.h>

#ifdef NULL
#undef NULL
#endif
#define NULL 0

static char* TypeStrings[17] = {
    "",
    "A",
    "NS",
    "MD",
    "MF",
    "CNAME",
    "SOA",
    "MB",
    "MG",
    "MR",
    "NULL",
    "WKS",
    "PTR",
    "HINFO",
    "MINFO",
    "MX",
    "TXT",
}; // size: 0xAC, address: 0xC

static char* ClassStrings[5] = {
    "",
    "IN",
    "CS",
    "CH",
    "HS",
}; // size: 0x14, address: 0xB8

static u8* DumpName(DNSHeader* dns /* r1+0x8 */, u8* ptr /* r31 */) {
    // Local variables
    int count; // r29
    u8* ret; // r30

    ret = NULL;
    while (*ptr != 0) {
        count = *ptr;
        if (count & 0xC0) {
            if (ret == NULL) {
                ret = ptr + 2;
            }
            ptr = (u8*)dns + (*(u16*)ptr & ~0xC000);
        } else {
            OSReport("%.*s", count, ++ptr);
            ptr += count;
            if (*ptr != 0) {
                OSReport(".");
            }
        }
    }
    OSReport(".");
    if (ret == NULL) {
        ret = ptr + 1;
    }
    return ret;
}

static u8* DumpQuestion(DNSHeader* dns /* r1+0x8 */, u8* ptr /* r31 */) {
    // Local variables
    u16 type; // r30

    // References
    // -> static char * TypeStrings[43];

    ptr = DumpName(dns, ptr);
    type = *(u16*)ptr;
    if (type < 17) {
        OSReport("\t%s\t", TypeStrings[type]);
    }
    ptr += 2;
    ptr += 2;
    OSReport("\n");
    return ptr;
}

static u8* DumpResource(DNSHeader* dns /* r29 */, u8* ptr /* r31 */) {
    // Local variables
    int count; // r28
    u16 type; // r27

    // References
    // -> static char * TypeStrings[43];

    ptr = DumpName(dns, ptr);
    type = *(u16*)ptr;
    if (type < 17) {
        OSReport("\t%s\t", TypeStrings[type]);
    }
    ptr += 2;
    ptr += 2;
    ptr += 4;
    count = *(u16*)ptr;
    ptr += 2;

    switch (type) {
        case 2:
        case 3:
        case 4:
        case 5:
        case 7:
        case 8:
        case 9:
        case 12:
            ptr = DumpName(dns, ptr);
            break;
        case 13:
        case 16:
            OSReport("%*.s", count, ptr);
            ptr += count;
            break;
        case 14:
            ptr = DumpName(dns, ptr);
            OSReport(" ");
            ptr = DumpName(dns, ptr);
            break;
        case 15:
            OSReport("%u ", *(u16*)ptr);
            ptr += 2;
            ptr = DumpName(dns, ptr);
            break;
        case 6:
            ptr = DumpName(dns, ptr);
            OSReport(" ");
            ptr = DumpName(dns, ptr);
            OSReport(" (\n\t\t\t");
            OSReport("%u ;serial\n\t\t\t", *(u32*)ptr);
            ptr += 4;
            OSReport("%u ;refresh\n\t\t\t", *(u32*)ptr);
            ptr += 4;
            OSReport("%u ;retry\n\t\t\t", *(u32*)ptr);
            ptr += 4;
            OSReport("%u ;expire\n\t\t\t", *(u32*)ptr);
            ptr += 4;
            OSReport("%u ;minimum\n\t\t\t)", *(u32*)ptr);
            ptr += 4;
            break;
        case 1:
            OSReport("%d.%d.%d.%d", ptr[0], ptr[1], ptr[2], ptr[3]);
            ptr += 4;
            break;
        case 11:
            OSReport("%d.%d.%d.%d", ptr[0], ptr[1], ptr[2], ptr[3]);
            ptr += 4;
            OSReport(":%d", *(u16*)ptr);
            ptr += 2;
            count -= 6;
            IFDump(ptr, count);
            ptr += count;
            break;
        default:
            IFDump(ptr, count);
            ptr += count;
            break;
    }
    OSReport("\n");
    return ptr;
}

void DNSDumpPacket(DNSHeader* dns /* r31 */) {
    // Local variables
    u8* opt; // r29
    int i; // r30

    opt = (u8*)(dns + 1);
    OSReport("qdcount: %d\n", dns->qdcount);
    for (i = 0; i < dns->qdcount; i++) {
        opt = DumpQuestion(dns, opt);
    }
    OSReport("ancount: %d\n", dns->ancount);
    for (i = 0; i < dns->ancount; i++) {
        opt = DumpResource(dns, opt);
    }
    OSReport("nscount: %d\n", dns->nscount);
    for (i = 0; i < dns->nscount; i++) {
        opt = DumpResource(dns, opt);
    }
    OSReport("arcount: %d\n", dns->arcount);
    for (i = 0; i < dns->arcount; i++) {
        opt = DumpResource(dns, opt);
    }
}

static u8* SkipName(u8* ptr /* r3 */, u8* end /* r4 */) {
    // Local variables
    int count; // r31

    for (;;) {
        if (end <= ptr) {
            return NULL;
        }
        if (*ptr == 0) {
            ptr++;
            return (ptr <= end) ? ptr : NULL;
        }
        count = *ptr;
        if (count & 0xC0) {
            if ((count & 0xC0) != 0xC0) {
                return NULL;
            }
            ptr += 2;
            return (ptr <= end) ? ptr : NULL;
        }
        ptr += count + 1;
    }
}

static u8* CheckResource(DNSHeader* dns, u8* end /* r30 */, u8* ptr /* r31 */) {
    // Local variables
    u16 type; // r28
    u16 class; // r27
    int count; // r29

    ptr = SkipName(ptr, end);
    if (ptr == NULL || end < ptr + 10) {
        return NULL;
    }
    type = *(u16*)ptr;
    ptr += 2;
    class = *(u16*)ptr;
    ptr += 2;
    ptr += 4;
    count = *(u16*)ptr;
    ptr += 2;
    if (class != 1 || type < 1) {
        return NULL;
    }

    end = ptr + count;
    switch (type) {
        case 2:
        case 3:
        case 4:
        case 5:
        case 7:
        case 8:
        case 9:
        case 12:
            ptr = SkipName(ptr, end);
            break;
        case 13:
        case 16:
            ptr += count;
            break;
        case 14:
            ptr = SkipName(ptr, end);
            if (ptr) {
                ptr = SkipName(ptr, end);
            }
            break;
        case 15:
            ptr += 2;
            ptr = SkipName(ptr, end);
            break;
        case 6:
            ptr = SkipName(ptr, end);
            if (ptr) {
                ptr = SkipName(ptr, end);
                if (ptr) {
                    ptr += 20;
                }
            }
            break;
        case 1:
            ptr += 4;
            break;
        case 11:
            ptr += count;
            break;
        default:
            ptr += count;
            break;
    }
    return (ptr == end) ? ptr : NULL;
}
