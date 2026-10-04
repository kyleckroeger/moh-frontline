#include <dolphin/ip.h>
#include <dolphin/private/ip.h>

#define TCP_TIMEWAIT_EXPIRE 240

static TCPTimeWaitControl Control; // size: 0x14, address: 0x0

// Range: 0x0 -> 0xAC
static TCPTimeWaitInfo* CreateTimeWaitInfo() {
    // Local variables
    TCPTimeWaitInfo* twInfo; // r30

    // References
    // -> static struct TCPTimeWaitControl Control;
    if (Control.used < Control.max) {
        twInfo = Control.head + Control.used;
        Control.used++;
        if (Control.end <= twInfo) {
            twInfo -= Control.max;
        }
    } else if (Control.array != NULL) {
        twInfo = Control.head;
        Control.head = (Control.head + 1 == Control.end) ? Control.array : Control.head + 1;
    } else {
        twInfo = NULL;
    }
    return twInfo;
}

// Range: 0xAC -> 0x124
s32 TCPSetTimeWaitBuffer(void* buffer /* r3 */, s32 len /* r4 */) {
    // References
    // -> static struct TCPTimeWaitControl Control;
    if (Control.array != NULL || (buffer && (u32)len < sizeof(TCPTimeWaitInfo))) {
        return -12;
    }

    Control.array = buffer;
    Control.head = buffer;
    Control.used = 0;
    Control.max = (u32)len / sizeof(TCPTimeWaitInfo);
    Control.end = Control.head + Control.max;
    return 0;
}

// Range: 0x124 -> 0x1D4
static void DiscardExpiredTimeWaitInfo(u32 now /* r3 */) {
    // Local variables
    s32 i; // r28
    s32 expired; // r30
    TCPTimeWaitInfo* twInfo; // r29

    // References
    // -> static struct TCPTimeWaitControl Control;
    expired = 0;
    twInfo = Control.head;
    for (i = 0; i < Control.used; i++) {
        if (twInfo->expire <= now) {
            expired++;
        }
        twInfo = (twInfo + 1 == Control.end) ? Control.array : twInfo + 1;
    }

    if (expired > 0) {
        Control.head += expired;
        if (Control.end <= Control.head) {
            Control.head -= Control.max;
        }
        Control.used -= expired;
    }
}

// Range: 0x1D4 -> 0x274
static TCPTimeWaitInfo* LookupTimeWaitInfo(const u8* src /* r3 */, u16 srcPort /* r4 */, const u8* dst /* r5 */, u16 dstPort /* r6 */) {
    // Local variables
    s32 i; // r29
    TCPTimeWaitInfo* twInfo; // r31

    // References
    // -> static struct TCPTimeWaitControl Control;
    twInfo = Control.head;
    for (i = 0; i < Control.used; i++) {
        if (twInfo->remotePort == srcPort && twInfo->localPort == dstPort && IPEQ(twInfo->remoteAddr, src) && IPEQ(twInfo->localAddr, dst)) {
            return twInfo;
        }
        twInfo = (twInfo + 1 == Control.end) ? Control.array : twInfo + 1;
    }
    return NULL;
}

// Range: 0x274 -> 0x2FC
BOOL TCPLookupTimeWaitInfo(const u8* src /* r1+0x8 */, u16 srcPort /* r1+0xC */, const u8* dst /* r1+0x10 */, u16 dstPort /* r1+0x14 */) {
    // Local variables
    u32 now; // r31

    now = (u32)OSTicksToSeconds(__OSGetSystemTime());
    DiscardExpiredTimeWaitInfo(now);
    return LookupTimeWaitInfo(src, srcPort, dst, dstPort) ? TRUE : FALSE;
}

// Range: 0x2FC -> 0x3F0
BOOL TCPTestTimeWait(IPInterface* interface /* r1+0x10 */, IPHeader* ip /* r28 */, TCPHeader* tcp /* r30 */) {
    // Local variables
    u32 now; // r27
    TCPTimeWaitInfo* twInfo; // r29
    TCPTimeWaitInfo* twExtended; // r31

    now = (u32)OSTicksToSeconds(__OSGetSystemTime());
    DiscardExpiredTimeWaitInfo(now);
    twInfo = LookupTimeWaitInfo(ip->src, tcp->src, ip->dst, tcp->dst);
    if (twInfo == NULL) {
        return FALSE;
    }

    twExtended = CreateTimeWaitInfo();
    memmove(twExtended, twInfo, sizeof(TCPTimeWaitInfo));
    twExtended->expire = now + TCP_TIMEWAIT_EXPIRE;
    twInfo->expire = 0;

    if (!(tcp->flag & TCP_FLAG_RST)) {
        TCPRespond(interface, NULL, ip->src, tcp->src, ip->dst, tcp->dst, twExtended->sendMax, twExtended->recvNext, 0x5000 | TCP_FLAG_ACK, 0);
    }
    return TRUE;
}

// Range: 0x3F0 -> 0x4EC
void TCPStartTimeWait(IPInterface* interface /* r1+0x10 */, TCPInfo* info /* r31 */) {
    // Local variables
    TCPTimeWaitInfo* twInfo; // r30
    u32 expire; // r29

    expire = (u32)OSTicksToSeconds(__OSGetSystemTime());
    expire += TCP_TIMEWAIT_EXPIRE;
    TCPCancelRxmitTimer(info);
    twInfo = CreateTimeWaitInfo();
    if (twInfo) {
        memmove(twInfo->remoteAddr, info->pair.remote.addr, 4);
        memmove(twInfo->localAddr, info->pair.local.addr, 4);
        twInfo->remotePort = info->pair.remote.port;
        twInfo->localPort = info->pair.local.port;
        twInfo->sendMax = info->sendMax;
        twInfo->recvNext = info->recvNext;
        twInfo->expire = expire;
    }

    if (info->recvNext - info->recvAcked > 0) {
        TCPRespond(interface, NULL, info->pair.remote.addr, info->pair.remote.port, info->pair.local.addr, info->pair.local.port, info->sendMax, info->recvNext, 0x5000 | TCP_FLAG_ACK, 0);
    }
    TCPAbort(info);
}
