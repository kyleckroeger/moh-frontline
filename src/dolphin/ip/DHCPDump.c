#include <dolphin/ip.h>
#include <dolphin/ip/IPArp.h>
#include <dolphin/private/ip.h>

#ifdef NULL
#undef NULL
#endif

#define NULL 0

static u8 MagicCookie[4] = { 99, 130, 83, 99 }; // size: 0x4, address: 0x0
static char* TypeNames[8] = {
    "(None)",
    "DISCOVER",
    "OFFER",
    "REQUEST",
    "DECLINE",
    "ACK",
    "NAK",
    "RELEASE",
}; // size: 0x20, address: 0xC

void DHCPDump(DHCPHeader* dhcp /* r30 */, s32 optlen /* r27 */) {
    // Local variables
    u8 type; // r24
    u8* opt; // r31
    s32 len; // r28
    u8* sname; // r26
    u8* file; // r25

    // References
    // -> static char * TypeNames[8];
    // -> static unsigned char MagicCookie[4];

    sname = NULL;
    file = NULL;
    opt = (u8*)(dhcp + 1);
    if (optlen < 5 || IPNEQ(opt, MagicCookie)) {
        return;
    }
    opt += 4;
    optlen -= 4;

    OSReport("op %d, htype %d, hlen %d, hops %d, xid %u, secs %u, flags %u\n", dhcp->op, dhcp->htype, dhcp->hlen, dhcp->hops, dhcp->xid, dhcp->secs, dhcp->flags);
    OSReport("ciaddr: %d.%d.%d.%d\n", dhcp->ciaddr[0], dhcp->ciaddr[1], dhcp->ciaddr[2], dhcp->ciaddr[3]);
    OSReport("yiaddr: %d.%d.%d.%d\n", dhcp->yiaddr[0], dhcp->yiaddr[1], dhcp->yiaddr[2], dhcp->yiaddr[3]);
    OSReport("siaddr: %d.%d.%d.%d\n", dhcp->siaddr[0], dhcp->siaddr[1], dhcp->siaddr[2], dhcp->siaddr[3]);
    OSReport("giaddr: %d.%d.%d.%d\n", dhcp->giaddr[0], dhcp->giaddr[1], dhcp->giaddr[2], dhcp->giaddr[3]);
    OSReport("chaddr: %02x:%02x:%02x:%02x:%02x:%02x\n", dhcp->chaddr[0], dhcp->chaddr[1], dhcp->chaddr[2], dhcp->chaddr[3], dhcp->chaddr[4], dhcp->chaddr[5]);

    for (;;) {
        while (0 < optlen && *opt != 255) {
            type = *opt++;
            --optlen;
            if (type == 0) {
                (void)0;
            }
            if (optlen < 1) {
                return;
            }
            len = *opt++;
            --optlen;
            if (optlen < len) {
                return;
            }

            switch (type) {
                case 1:
                    if (len != 4) {
                        return;
                    }
                    OSReport("DHCP: SUBNETMASK: %d.%d.%d.%d\n", opt[0], opt[1], opt[2], opt[3]);
                    break;
                case 3:
                    OSReport("DHCP: ROUTER: %d.%d.%d.%d\n", opt[0], opt[1], opt[2], opt[3]);
                    break;
                case 6:
                    if (len <= 0 || len % 4) {
                        return;
                    }
                    OSReport("DHCP: DNS1: %d.%d.%d.%d\n", opt[0], opt[1], opt[2], opt[3]);
                    if (8 <= len) {
                        OSReport("DHCP: DNS2: %d.%d.%d.%d\n", opt[4], opt[5], opt[6], opt[7]);
                    }
                    break;
                case 12:
                    OSReport("DHCP: HOST_NAME: %.*s\n", len, opt);
                    break;
                case 15:
                    OSReport("DHCP: DOMAIN_NAME: %.*s\n", len, opt);
                    break;
                case 26:
                    OSReport("DHCP: MTU: %d\n", *(u16*)opt);
                    break;
                case 28:
                    if (len != 4) {
                        return;
                    }
                    OSReport("DHCP: BROADCAST_ADDR: %d.%d.%d.%d\n", opt[0], opt[1], opt[2], opt[3]);
                    break;
                case 51:
                    if (len != 4) {
                        return;
                    }
                    OSReport("DHCP: LEASE_TIME: %u\n", *(u32*)opt);
                    break;
                case 52:
                    if (len != 1) {
                        return;
                    }
                    switch (*opt) {
                        case 1:
                            file = dhcp->file;
                            break;
                        case 2:
                            sname = dhcp->sname;
                            break;
                        case 3:
                            file = dhcp->file;
                            sname = dhcp->sname;
                            break;
                    }
                    break;
                case 53:
                    if (len != 1) {
                        return;
                    }
                    OSReport("DHCP: Type: %d (%s)\n", *opt, TypeNames[*opt]);
                    break;
                case 54:
                    if (len != 4) {
                        return;
                    }
                    OSReport("DHCP: SERVER_ID: %d.%d.%d.%d\n", opt[0], opt[1], opt[2], opt[3]);
                    break;
                case 58:
                    if (len != 4) {
                        return;
                    }
                    OSReport("DHCP: RENEWAL_TIME: %u\n", *(u32*)opt);
                    break;
                case 59:
                    if (len != 4) {
                        return;
                    }
                    OSReport("DHCP: REBINDING_TIME: %u\n", *(u32*)opt);
                    break;
            }
            opt += len;
            optlen -= len;
        }

        if (sname) {
            opt = sname;
            optlen = 64;
            sname = NULL;
        } else if (file) {
            opt = file;
            optlen = 128;
            file = NULL;
        } else {
            return;
        }
    }
}
