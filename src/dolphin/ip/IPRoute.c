#include <dolphin/ip.h>
#include <dolphin/ip/IPArp.h>
#include <dolphin/private/ip.h>

#define ROUTE_TABLE_SIZE 32

#define ROUTE_GATEWAY  0x1
#define ROUTE_DYNAMIC  0x2
#define ROUTE_MODIFIED 0x4

static BOOL Initialized; // size: 0x4, address: 0x0
static IFQueue Up; // size: 0x8, address: 0x4
static IFQueue Free; // size: 0x8, address: 0xC
static IPRoute RoutingTable[ROUTE_TABLE_SIZE]; // size: 0x500, address: 0x0

// Range: 0x0 -> 0x68
static int CountBits(const u8* addr /* r3 */) {
    // Local variables
    int count; // r29
    int i; // r31
    u8 mask; // r30

    count = 0;
    for (i = 0; i < IP_ALEN; i++) {
        for (mask = 0x80; mask; mask >>= 1) {
            if (addr[i] & mask) {
                count++;
            }
        }
    }
    return count;
}

// Range: 0x68 -> 0xC8
void IPGetMacAddr(IPInterface* interface /* r31 */, u8* macAddr /* r1+0xC */) {
    // Local variables
    BOOL enabled; // r30

    // References
    // -> struct IPInterface __IFDefault;
    interface = interface ? interface : &__IFDefault;
    enabled = OSDisableInterrupts();
    memcpy(macAddr, interface->mac, 6);
    OSRestoreInterrupts(enabled);
}

// Range: 0xC8 -> 0x128
void IPGetBroadcastAddr(IPInterface* interface /* r31 */, u8* broadcastAddr /* r1+0xC */) {
    // Local variables
    BOOL enabled; // r30

    // References
    // -> struct IPInterface __IFDefault;
    interface = interface ? interface : &__IFDefault;
    enabled = OSDisableInterrupts();
    memcpy(broadcastAddr, interface->broadcast, IP_ALEN);
    OSRestoreInterrupts(enabled);
}

// Range: 0x128 -> 0x188
void IPGetNetmask(IPInterface* interface /* r31 */, u8* netmask /* r1+0xC */) {
    // Local variables
    BOOL enabled; // r30

    // References
    // -> struct IPInterface __IFDefault;
    interface = interface ? interface : &__IFDefault;
    enabled = OSDisableInterrupts();
    memcpy(netmask, interface->netmask, IP_ALEN);
    OSRestoreInterrupts(enabled);
}

// Range: 0x188 -> 0x1E8
void IPGetAddr(IPInterface* interface /* r31 */, u8* addr /* r1+0xC */) {
    // Local variables
    BOOL enabled; // r30

    // References
    // -> struct IPInterface __IFDefault;
    interface = interface ? interface : &__IFDefault;
    enabled = OSDisableInterrupts();
    memcpy(addr, interface->addr, IP_ALEN);
    OSRestoreInterrupts(enabled);
}

// Range: 0x1E8 -> 0x248
void IPGetGateway(IPInterface* interface /* r31 */, u8* gateway /* r1+0xC */) {
    // Local variables
    BOOL enabled; // r30

    // References
    // -> struct IPInterface __IFDefault;
    interface = interface ? interface : &__IFDefault;
    enabled = OSDisableInterrupts();
    memcpy(gateway, interface->gateway, IP_ALEN);
    OSRestoreInterrupts(enabled);
}

// Range: 0x248 -> 0x2A8
void IPGetAlias(IPInterface* interface /* r31 */, u8* alias /* r1+0xC */) {
    // Local variables
    BOOL enabled; // r30

    // References
    // -> struct IPInterface __IFDefault;
    interface = interface ? interface : &__IFDefault;
    enabled = OSDisableInterrupts();
    memcpy(alias, interface->alias, IP_ALEN);
    OSRestoreInterrupts(enabled);
}

// Range: 0x2A8 -> 0x304
void IPGetMtu(IPInterface* interface /* r31 */, s32* mtu /* r1+0xC */) {
    // Local variables
    BOOL enabled; // r30

    // References
    // -> struct IPInterface __IFDefault;
    interface = interface ? interface : &__IFDefault;
    enabled = OSDisableInterrupts();
    *mtu = interface->mtu;
    OSRestoreInterrupts(enabled);
}

// Range: 0x304 -> 0x360
void IPGetLinkState(IPInterface* interface /* r31 */, BOOL* up /* r1+0xC */) {
    // Local variables
    BOOL enabled; // r30

    // References
    // -> struct IPInterface __IFDefault;
    interface = interface ? interface : &__IFDefault;
    enabled = OSDisableInterrupts();
    *up = interface->up;
    OSRestoreInterrupts(enabled);
}

// Range: 0x360 -> 0x3C8
void IPGetInterfaceStat(IPInterface* interface /* r31 */, IPInterfaceStat* stat /* r29 */) {
    // Local variables
    BOOL enabled; // r30

    // References
    // -> struct IPInterface __IFDefault;
    interface = interface ? interface : &__IFDefault;
    if (stat) {
        enabled = OSDisableInterrupts();
        memmove(stat, &interface->stat, sizeof(IPInterfaceStat));
        OSRestoreInterrupts(enabled);
    }
}

// Range: 0x3C8 -> 0x424
void IPClearInterfaceStat(IPInterface* interface /* r31 */) {
    // Local variables
    BOOL enabled; // r30

    // References
    // -> struct IPInterface __IFDefault;
    interface = interface ? interface : &__IFDefault;
    enabled = OSDisableInterrupts();
    memset(&interface->stat, 0, sizeof(IPInterfaceStat));
    OSRestoreInterrupts(enabled);
}

// Range: 0x424 -> 0x494
void IPSetBroadcastAddr(IPInterface* interface /* r31 */, const u8* broadcastAddr /* r29 */) {
    // Local variables
    BOOL enabled; // r30

    // References
    // -> unsigned char IPLimited[4];
    // -> struct IPInterface __IFDefault;
    interface = interface ? interface : &__IFDefault;
    enabled = OSDisableInterrupts();
    memcpy(interface->broadcast, broadcastAddr ? broadcastAddr : IPLimited, IP_ALEN);
    OSRestoreInterrupts(enabled);
}

// Range: 0x494 -> 0x504
void IPSetNetmask(IPInterface* interface /* r31 */, const u8* netmask /* r29 */) {
    // Local variables
    BOOL enabled; // r30

    // References
    // -> unsigned char IPAddrAny[4];
    // -> struct IPInterface __IFDefault;
    interface = interface ? interface : &__IFDefault;
    enabled = OSDisableInterrupts();
    memcpy(interface->netmask, netmask ? netmask : IPAddrAny, IP_ALEN);
    OSRestoreInterrupts(enabled);
}

// Range: 0x504 -> 0x574
void IPSetAddr(IPInterface* interface /* r31 */, const u8* addr /* r29 */) {
    // Local variables
    BOOL enabled; // r30

    // References
    // -> unsigned char IPAddrAny[4];
    // -> struct IPInterface __IFDefault;
    interface = interface ? interface : &__IFDefault;
    enabled = OSDisableInterrupts();
    memcpy(interface->addr, addr ? addr : IPAddrAny, IP_ALEN);
    OSRestoreInterrupts(enabled);
}

// Range: 0x574 -> 0x5E4
void IPSetGateway(IPInterface* interface /* r31 */, const u8* gateway /* r29 */) {
    // Local variables
    BOOL enabled; // r30

    // References
    // -> unsigned char IPAddrAny[4];
    // -> struct IPInterface __IFDefault;
    interface = interface ? interface : &__IFDefault;
    enabled = OSDisableInterrupts();
    memcpy(interface->gateway, gateway ? gateway : IPAddrAny, IP_ALEN);
    OSRestoreInterrupts(enabled);
}

// Range: 0x5E4 -> 0x6A4
void IPSetAlias(IPInterface* interface /* r30 */, const u8* alias /* r31 */) {
    // Local variables
    BOOL enabled; // r29

    // References
    // -> unsigned char IPAddrAny[4];
    // -> struct IPInterface __IFDefault;
    ASSERTLINE(273, alias == NULL || alias[0] == 169 && alias[1] == 254 || IP_ADDR_EQ(alias, IPAddrAny));
    interface = interface ? interface : &__IFDefault;
    enabled = OSDisableInterrupts();
    memcpy(interface->alias, alias ? alias : IPAddrAny, IP_ALEN);
    OSRestoreInterrupts(enabled);
}

// Range: 0x6A4 -> 0x728
void IPSetMtu(IPInterface* interface /* r31 */, s32 mtu /* r30 */) {
    // Local variables
    BOOL enabled; // r29

    // References
    // -> struct IPInterface __IFDefault;
    ASSERTLINE(284, 68 <= mtu);
    interface = interface ? interface : &__IFDefault;
    enabled = OSDisableInterrupts();
    interface->mtu = (mtu < 68) ? 68 : mtu;
    OSRestoreInterrupts(enabled);
}

// Range: 0x728 -> 0x858
void IPPrintRoutingTable() {
    // Local variables
    IPRoute* route; // r31
    IPRoute* next; // r29

    // References
    // -> static struct IFQueue Up;
    OSReport("IP Routing table\n");
    OSReport("Destination        Netmask            Gateway            Flags\n");
    IFQueueIterator(IPRoute*, &Up, route, next) {
        OSReport("%3d.%3d.%3d.%3d    %3d.%3d.%3d.%3d    %3d.%3d.%3d.%3d    ",
                 route->dst[0], route->dst[1], route->dst[2], route->dst[3],
                 route->netmask[0], route->netmask[1], route->netmask[2], route->netmask[3],
                 route->gateway[0], route->gateway[1], route->gateway[2], route->gateway[3]);
        if (route->flag & ROUTE_GATEWAY) {
            OSReport("G");
        }
        if (route->flag & ROUTE_DYNAMIC) {
            OSReport("D");
        }
        if (route->flag & ROUTE_MODIFIED) {
            OSReport("M");
        }
        OSReport("\n");
    }
}

// Range: 0x858 -> 0x8A4
static BOOL Hit(const u8* dst /* r3 */, const u8* addr /* r4 */, const u8* netmask /* r5 */) {
    // Local variables
    int i; // r31

    for (i = 0; i < IP_ALEN; i++) {
        if (dst[i] != (addr[i] & netmask[i])) {
            return FALSE;
        }
    }
    return TRUE;
}

// Range: 0x8A4 -> 0x9DC
BOOL IPIsBroadcastAddr(IPInterface* interface /* r30 */, const u8* addr /* r27 */) {
    // Local variables
    u32 netmask; // r31
    u32 netid; // r28
    u32 ipaddr; // r29

    // References
    // -> unsigned char IPAddrAny[4];
    // -> unsigned char IPLimited[4];
    // -> struct IPInterface __IFDefault;
    interface = interface ? interface : &__IFDefault;
    if (IP_CLASSD(addr) || IP_CLASSE(addr)) {
        return FALSE;
    }
    if (IPEQ(addr, IPLimited) || IPEQ(addr, IPAddrAny) || IPEQ(addr, interface->broadcast)) {
        return TRUE;
    }

    ipaddr = IPU32(addr);
    netmask = IPU32(interface->netmask);
    netid = IPU32(interface->addr) & netmask;
    if (netid != 0) {
        if (netid == (ipaddr & netmask) && ~netmask == (ipaddr & ~netmask)) {
            return TRUE;
        }
        if (IP_CLASSA(interface->addr)) {
            netmask = 0xff000000;
        } else if (IP_CLASSB(interface->addr)) {
            netmask = 0xffff0000;
        } else if (IP_CLASSC(interface->addr)) {
            netmask = 0xffffff00;
        }
        netid = IPU32(interface->addr) & netmask;
        if (netid == (ipaddr & netmask) && ~netmask == (ipaddr & ~netmask)) {
            return TRUE;
        }
    }

    netmask = 0xFFFF0000;
    netid = IPU32(interface->alias) & netmask;
    if (netid != 0 && netid == (ipaddr & netmask) && ~netmask == (ipaddr & ~netmask)) {
        return TRUE;
    }
    return FALSE;
}

// Range: 0x9DC -> 0x9F8
BOOL IPIsLoopbackAddr(IPInterface*, const u8* addr /* r4 */) {
    return (addr[0] == 127) ? TRUE : FALSE;
}

// Range: 0x9F8 -> 0xAC0
BOOL IPIsLocalAddr(IPInterface* interface /* r31 */, const u8* addr /* r28 */) {
    // Local variables
    u8 classB[4] = { 255, 255, 0, 0 }; // r1+0x10
    BOOL enabled; // r29
    BOOL rc; // r30

    // References
    // -> unsigned char IPAddrAny[4];
    // -> struct IPInterface __IFDefault;
    rc = FALSE;
    interface = interface ? interface : &__IFDefault;
    enabled = OSDisableInterrupts();
    if (IPNEQ(interface->addr, IPAddrAny) &&
        (IPU32(addr) & IPU32(interface->netmask)) == (IPU32(interface->addr) & IPU32(interface->netmask))) {
        rc = TRUE;
    }
    if (IPNEQ(interface->alias, IPAddrAny) && (IPU32(addr) & IPU32(classB)) == (IPU32(interface->alias) & IPU32(classB))) {
        rc = TRUE;
    }
    OSRestoreInterrupts(enabled);
    return rc;
}

// Range: 0xAC0 -> 0xB9C
IPInterface* IPGetRoute(const u8* addr /* r28 */, u8* dst /* r29 */) {
    // Local variables
    IPRoute* route; // r31
    IPRoute* next; // r30

    // References
    // -> static struct IFQueue Up;
    IFQueueIterator(IPRoute*, &Up, route, next) {
        if (Hit(route->dst, addr, route->netmask)) {
            if (dst) {
                if (route->flag & ROUTE_GATEWAY) {
                    memmove(dst, route->gateway, IP_ALEN);
                } else {
                    memmove(dst, addr, IP_ALEN);
                }
            }
            route->time = __OSGetSystemTime();
            return route->interface;
        }
    }
    return NULL;
}

// Range: 0xB9C -> 0xF6C
static BOOL AddRoute(const u8* dst /* r20 */, const u8* netmask /* r22 */, const u8* gateway /* r21 */, u32 flag /* r23 */) {
    // Local variables
    IPRoute* route; // r31
    IPRoute* next; // r27
    IPRoute* ent; // r30
    int i; // r24
    int bits; // r19
    // IFQueue* ___next; // r29 (IFQueueDequeueEntry)
    // IFQueue* ___prev; // r28 (IFQueueDequeueEntry)
    // IFQueue* ___next; // r26 (IFQueueDequeueHead)
    // IFQueue* ___prev; // r25 (IFQueueEnqueueTail)

    // References
    // -> static struct IFQueue Up;
    // -> struct IPInterface __IFDefault;
    // -> static struct IFQueue Free;
    // -> unsigned char IPAddrAny[4];
    if (!Hit(dst, dst, netmask)) {
        return FALSE;
    }

    if (IPEQ(gateway, IPAddrAny)) {
        flag &= ~ROUTE_GATEWAY;
    } else {
        flag |= ROUTE_GATEWAY;
    }

    IFQueueIterator(IPRoute*, &Up, route, next) {
        for (i = 0; i < IP_ALEN; i++) {
            if ((route->dst[i] & route->netmask[i]) != (dst[i] & netmask[i])) {
                break;
            }
        }
        if (i < IP_ALEN) {
            continue;
        }

        if ((flag & ROUTE_GATEWAY) && !(route->flag & (ROUTE_DYNAMIC | ROUTE_MODIFIED))) {
            if (IPEQ(gateway, route->gateway)) {
                return TRUE;
            }
        } else {
            memmove(route->gateway, gateway, IP_ALEN);
            memmove(route->netmask, netmask, IP_ALEN);
            if (!(route->flag & ROUTE_DYNAMIC)) {
                flag &= ~ROUTE_DYNAMIC;
            }
            route->flag = flag;
            route->time = __OSGetSystemTime();
            route->interface = &__IFDefault;
            return TRUE;
        }
    }

    if (Free.next == NULL) {
        ent = NULL;
        IFQueueIterator(IPRoute*, &Up, route, next) {
            if (route->flag & ROUTE_DYNAMIC) {
                if (ent == NULL) {
                    ent = route;
                } else if (route->time < ent->time) {
                    ent = route;
                }
            }
        }
        if (ent == NULL) {
            return FALSE;
        }

        do {
            register IFQueue* ___next;
            register IFQueue* ___prev;

            ___next = ent->link.next;
            ___prev = ent->link.prev;
            if (___next == NULL) {
                Up.prev = ___prev;
            } else {
                ((IPRoute*)___next)->link.prev = ___prev;
            }
            if (___prev == NULL) {
                Up.next = ___next;
            } else {
                ((IPRoute*)___prev)->link.next = ___next;
            }
        } while (0);
    } else {
        do {
            register IFQueue* ___next;

            ent = (IPRoute*)Free.next;
            ___next = ((IPRoute*)ent)->link.next;
            if (___next == NULL) {
                Free.prev = NULL;
            } else {
                ((IPRoute*)___next)->link.prev = NULL;
            }
            Free.next = ___next;
        } while (0);
    }

    memmove(ent->dst, dst, IP_ALEN);
    memmove(ent->netmask, netmask, IP_ALEN);
    memmove(ent->gateway, gateway, IP_ALEN);
    flag &= ~ROUTE_MODIFIED;
    ent->flag = flag;
    ent->time = __OSGetSystemTime();
    ent->interface = &__IFDefault;

    bits = CountBits(netmask);
    IFQueueIterator(IPRoute*, &Up, route, next) {
        if (CountBits(route->netmask) < bits) {
            ent->link.prev = route->link.prev;
            ent->link.next = (IFQueue*)route;
            route->link.prev = (IFQueue*)ent;
            if (ent->link.prev == NULL) {
                Up.next = (IFQueue*)ent;
            } else {
                ent->link.prev->next = (IFQueue*)ent;
            }
            return TRUE;
        }
    }

    do {
        register IFQueue* ___prev;

        ___prev = Up.prev;
        if (___prev == NULL) {
            Up.next = (IFQueue*)ent;
        } else {
            ((IPRoute*)___prev)->link.next = (IFQueue*)ent;
        }
        ent->link.prev = ___prev;
        ent->link.next = NULL;
        Up.prev = (IFQueue*)ent;
    } while (0);
    return TRUE;
}

// Range: 0xF6C -> 0x10A4
BOOL IPRedirect(const u8* dst /* r28 */, const u8* gateway /* r31 */, const u8* router /* r29 */) {
    // Local variables
    IPInterface* interface; // r30
    u8 addr[4]; // r1+0x14

    // References
    // -> unsigned char IPLimited[4];
    // -> struct IPInterface __IFDefault;
    // -> unsigned char IPAddrAny[4];
    if (IPEQ(gateway, IPAddrAny)) {
        return FALSE;
    }

    interface = IPGetRoute(dst, addr);
    if (interface == NULL) {
        return FALSE;
    }

    if ((IPU32(gateway) & IPU32(interface->netmask)) != (IPU32(interface->addr) & IPU32(interface->netmask)) ||
        IPIsBroadcastAddr(interface, gateway)) {
        return FALSE;
    }

    if (IPNEQ(addr, router)) {
        return FALSE;
    }

    if (IPGetRoute(gateway, NULL) != IPGetRoute(router, NULL)) {
        return FALSE;
    }

    if (IPEQ(gateway, __IFDefault.addr) || IPEQ(gateway, __IFDefault.alias)) {
        return FALSE;
    }

    return AddRoute(dst, IPLimited, gateway, ROUTE_GATEWAY | ROUTE_DYNAMIC | ROUTE_MODIFIED);
}

// Range: 0x10A4 -> 0x1100
BOOL IPAddRoute(const u8* dst /* r1+0x8 */, const u8* netmask /* r1+0xC */, const u8* gateway /* r1+0x10 */) {
    // Local variables
    BOOL enabled; // r31
    BOOL rc; // r30

    enabled = OSDisableInterrupts();
    rc = AddRoute(dst, netmask, gateway, 0);
    OSRestoreInterrupts(enabled);
    return rc;
}

// Range: 0x1100 -> 0x125C
BOOL IPRemoveRoute(const u8* dst /* r1+0x8 */, const u8* netmask /* r23 */, const u8* gateway /* r24 */) {
    // Local variables
    BOOL enabled; // r25
    BOOL rc; // r27
    IPRoute* route; // r31
    IPRoute* next; // r26
    // IFQueue* ___next; // r30 (IFQueueDequeueEntry)
    // IFQueue* ___prev; // r29 (IFQueueDequeueEntry)
    // IFQueue* ___next; // r28 (IFQueueEnqueueHead)

    // References
    // -> static struct IFQueue Free;
    // -> static struct IFQueue Up;
    // -> unsigned char IPAddrAny[4];
    enabled = OSDisableInterrupts();
    rc = FALSE;
    IFQueueReverseIterator(IPRoute*, &Up, route, next) {
        if (IPEQ(dst, route->dst) && (IPEQ(netmask, IPAddrAny) || IPEQ(netmask, route->netmask)) &&
            (IPEQ(gateway, IPAddrAny) || IPEQ(gateway, route->gateway))) {
            do {
                register IFQueue* ___next;
                register IFQueue* ___prev;

                ___next = route->link.next;
                ___prev = route->link.prev;
                if (___next == NULL) {
                    Up.prev = ___prev;
                } else {
                    ((IPRoute*)___next)->link.prev = ___prev;
                }
                if (___prev == NULL) {
                    Up.next = ___next;
                } else {
                    ((IPRoute*)___prev)->link.next = ___next;
                }
            } while (0);

            do {
                register IFQueue* ___next;

                ___next = Free.next;
                if (___next == NULL) {
                    Free.prev = (IFQueue*)route;
                } else {
                    ((IPRoute*)___next)->link.prev = (IFQueue*)route;
                }
                route->link.next = ___next;
                route->link.prev = NULL;
                Free.next = (IFQueue*)route;
            } while (0);

            rc = TRUE;
            break;
        }
    }
    OSRestoreInterrupts(enabled);
    return rc;
}

// Range: 0x125C -> 0x1478
BOOL IPInitRoute(const u8* addr /* r1+0x8 */, const u8* netmask /* r1+0xC */, const u8* gateway /* r1+0x10 */) {
    // Local variables
    BOOL enabled; // r25
    IPRoute* route; // r29
    IPRoute* end; // r24
    IPInterface* interface; // r31
    int prefix; // r26
    // IFQueue* ___prev; // r30 (IFQueueEnqueueTail)
    u8 broadcast[4]; // r1+0x14
    int i; // r28

    // References
    // -> unsigned char IPAddrAny[4];
    // -> static struct IFQueue Free;
    // -> static struct IPRoute RoutingTable[32];
    // -> static struct IFQueue Up;
    // -> static int Initialized;
    // -> struct IPInterface __IFDefault;
    interface = &__IFDefault;
    enabled = OSDisableInterrupts();
    if (!Initialized) {
        Initialized = TRUE;
        memset(RoutingTable, 0, sizeof(RoutingTable));
        IFQueueInit(&Up);
        IFQueueInit(&Free);
        end = &RoutingTable[ROUTE_TABLE_SIZE];
        for (route = RoutingTable; route < end; ++route) {
            do {
                register IFQueue* ___prev;

                ___prev = Free.prev;
                if (___prev == NULL) {
                    Free.next = (IFQueue*)route;
                } else {
                    ((IPRoute*)___prev)->link.next = (IFQueue*)route;
                }
                route->link.prev = ___prev;
                route->link.next = NULL;
                Free.prev = (IFQueue*)route;
            } while (0);
        }
    }
    OSRestoreInterrupts(enabled);

    IPSetAddr(interface, addr);
    IPSetNetmask(interface, netmask);
    IPSetGateway(interface, gateway);
    IPSetBroadcastAddr(interface, NULL);

    prefix = CountBits(interface->netmask);
    if (prefix < 31) {
        for (i = 0; i < IP_ALEN; i++) {
            broadcast[i] = interface->addr[i] & interface->netmask[i];
            broadcast[i] |= ~interface->netmask[i];
        }
        IPSetBroadcastAddr(interface, broadcast);
    }

    if (IPIsBroadcastAddr(interface, interface->addr) || IPEQ(interface->gateway, interface->addr) ||
        (IPNEQ(interface->gateway, IPAddrAny) &&
         (IPIsBroadcastAddr(interface, interface->gateway) ||
          (prefix != 32 && (IPU32(interface->gateway) & IPU32(interface->netmask)) != (IPU32(interface->addr) & IPU32(interface->netmask)))))) {
        IPSetAddr(interface, NULL);
        IPSetNetmask(interface, NULL);
        IPSetGateway(interface, NULL);
        IPSetBroadcastAddr(interface, NULL);
    }

    return IPRefreshRoute();
}

// Range: 0x1478 -> 0x16AC
BOOL IPRefreshRoute() {
    // Local variables
    BOOL enabled; // r21
    IPRoute* route; // r31
    IPRoute* next; // r24
    u8* addr; // r26
    u8* netmask; // r23
    u8* gateway; // r22
    u8* alias; // r25
    u8 loopback[4] = { 127, 0, 0, 0 }; // r1+0x14
    u8 classA[4] = { 255, 0, 0, 0 }; // r1+0x10
    u8 classB[4] = { 255, 255, 0, 0 }; // r1+0xC
    u8 netid[4]; // r1+0x8
    int i; // r30
    // IFQueue* ___next; // r29 (IFQueueDequeueEntry)
    // IFQueue* ___prev; // r28 (IFQueueDequeueEntry)
    // IFQueue* ___next; // r27 (IFQueueEnqueueHead)

    // References
    // -> struct IPInterface __IFDefault;
    // -> unsigned char IPAddrAny[4];
    // -> unsigned char IPLoopbackAddr[4];
    // -> unsigned char IPLimited[4];
    // -> static struct IFQueue Free;
    // -> static struct IFQueue Up;
    enabled = OSDisableInterrupts();
    addr = __IFDefault.addr;
    netmask = __IFDefault.netmask;
    gateway = __IFDefault.gateway;
    alias = __IFDefault.alias;

    IFQueueIterator(IPRoute*, &Up, route, next) {
        do {
            register IFQueue* ___next;
            register IFQueue* ___prev;

            ___next = route->link.next;
            ___prev = route->link.prev;
            if (___next == NULL) {
                Up.prev = ___prev;
            } else {
                ((IPRoute*)___next)->link.prev = ___prev;
            }
            if (___prev == NULL) {
                Up.next = ___next;
            } else {
                ((IPRoute*)___prev)->link.next = ___next;
            }
        } while (0);

        do {
            register IFQueue* ___next;

            ___next = Free.next;
            if (___next == NULL) {
                Free.prev = (IFQueue*)route;
            } else {
                ((IPRoute*)___next)->link.prev = (IFQueue*)route;
            }
            route->link.next = ___next;
            route->link.prev = NULL;
            Free.next = (IFQueue*)route;
        } while (0);
    }

    IPAddRoute(IPLimited, IPLimited, IPAddrAny);
    if (IPNEQ(addr, IPAddrAny)) {
        IPAddRoute(addr, IPLimited, IPLoopbackAddr);
        for (i = 0; i < IP_ALEN; i++) {
            netid[i] = addr[i] & netmask[i];
        }
        IPAddRoute(netid, netmask, IPAddrAny);
    }
    if (IPNEQ(alias, IPAddrAny)) {
        IPAddRoute(alias, IPLimited, IPLoopbackAddr);
        for (i = 0; i < IP_ALEN; i++) {
            netid[i] = classB[i] & alias[i];
        }
        IPAddRoute(netid, classB, IPAddrAny);
    }
    IPAddRoute(loopback, classA, IPLoopbackAddr);
    if (IPNEQ(gateway, IPAddrAny)) {
        IPAddRoute(IPAddrAny, IPAddrAny, gateway);
    }
    ARPGratuitous(&__IFDefault);
    OSRestoreInterrupts(enabled);
    return TRUE;
}

// Range: 0x16AC -> 0x17E0
static void DecayRoute(IPRoute* dead /* r31 */) {
    // Local variables
    int bits; // r25
    IPRoute* route; // r30
    IPRoute* next; // r26
    // IFQueue* ___next; // r29 (IFQueueDequeueEntry)
    // IFQueue* ___prev; // r28 (IFQueueDequeueEntry)
    // IFQueue* ___next; // r27 (IFQueueEnqueueHead)

    // References
    // -> static struct IFQueue Up;
    bits = CountBits(dead->netmask);
    do {
        register IFQueue* ___next;
        register IFQueue* ___prev;

        ___next = dead->link.next;
        ___prev = dead->link.prev;
        if (___next == NULL) {
            Up.prev = ___prev;
        } else {
            ((IPRoute*)___next)->link.prev = ___prev;
        }
        if (___prev == NULL) {
            Up.next = ___next;
        } else {
            ((IPRoute*)___prev)->link.next = ___next;
        }
    } while (0);

    IFQueueReverseIterator(IPRoute*, &Up, route, next) {
        if (bits <= CountBits(route->netmask)) {
            dead->link.prev = (IFQueue*)route;
            dead->link.next = route->link.next;
            route->link.next = (IFQueue*)dead;
            if (dead->link.next == NULL) {
                Up.prev = (IFQueue*)dead;
            } else {
                dead->link.next->prev = (IFQueue*)dead;
            }
            return;
        }
    }

    do {
        register IFQueue* ___next;

        ___next = Up.next;
        if (___next == NULL) {
            Up.prev = (IFQueue*)dead;
        } else {
            ((IPRoute*)___next)->link.prev = (IFQueue*)dead;
        }
        dead->link.next = ___next;
        dead->link.prev = NULL;
        Up.next = (IFQueue*)dead;
    } while (0);
}

// Range: 0x17E0 -> 0x193C
BOOL IPRecoverGateway(const u8* dst /* r29 */) {
    // Local variables
    IPRoute* route; // r31
    IPRoute* next; // r30

    // References
    // -> static struct IFQueue Up;
    // -> unsigned char IPAddrAny[4];
    if (IPEQ(dst, IPAddrAny) || dst[0] == 127) {
        return FALSE;
    }

    IFQueueReverseIterator(IPRoute*, &Up, route, next) {
        if ((route->flag & ROUTE_GATEWAY) && route->gateway[0] != 127 && IPEQ(route->gateway, dst)) {
            DecayRoute(route);
            return TRUE;
        }
    }

    IFQueueIterator(IPRoute*, &Up, route, next) {
        if (Hit(route->dst, dst, route->netmask)) {
            if ((route->flag & ROUTE_GATEWAY) && route->gateway[0] != 127) {
                DecayRoute(route);
                return TRUE;
            }
            break;
        }
    }
    return FALSE;
}

// Range: 0x193C -> 0x1958
s32 IPGetConfigError(IPInterface* interface /* r3 */) {
    // References
    // -> struct IPInterface __IFDefault;
    interface = interface ? interface : &__IFDefault;
    return interface->err;
}

// Range: 0x1958 -> 0x19C0
s32 IPSetConfigError(IPInterface* interface /* r31 */, s32 err /* r1+0xC */) {
    // Local variables
    BOOL enabled; // r30

    // References
    // -> struct IPInterface __IFDefault;
    interface = interface ? interface : &__IFDefault;
    enabled = OSDisableInterrupts();
    if (interface->err == 0) {
        interface->err = err;
    }
    OSRestoreInterrupts(enabled);
    return interface->err;
}

// Range: 0x19C0 -> 0x1A1C
s32 IPClearConfigError(IPInterface* interface /* r31 */) {
    // Local variables
    BOOL enabled; // r30
    s32 prev; // r29

    // References
    // -> struct IPInterface __IFDefault;
    interface = interface ? interface : &__IFDefault;
    enabled = OSDisableInterrupts();
    prev = interface->err;
    interface->err = 0;
    OSRestoreInterrupts(enabled);
    return prev;
}
