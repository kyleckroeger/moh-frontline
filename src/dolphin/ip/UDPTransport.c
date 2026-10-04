/* Reviewed fragment of IPUdp.c adapted for the older target behavior.
 * Provenance, scope and ABI evidence are recorded in docs/Network.md.
 */
#include <dolphin/ip.h>
#include <dolphin/private/ip.h>

static u16 Port = 1024; // size: 0x2, address: 0x0
IFQueue UDPInfoQueue; // size: 0x8, address: 0x0

static s32 PeekInput(UDPInfo* info /* r31 */, void* ptr /* r1+0xC */, s32 len /* r28 */, s32 offset /* r27 */) {
    // Local variables
    u8* head; // r30
    IFVec vec[2]; // r1+0x18
    int i; // r29
    int n; // r26

    if (info->recvUsed < offset + len) {
        len = info->recvUsed - offset;
    }
    if (len <= 0) {
        return 0;
    }

    head = info->recvPtr + offset;
    if (info->recvPtr + info->recvBuff <= head) {
        head -= info->recvBuff;
    }
    n = IFRingGet(info->recvRing, info->recvBuff, head, info->recvUsed - offset, vec, len);
    for (i = 0, head = ptr; i < n; i++) {
        memmove(head, vec[i].data, vec[i].len);
        head += vec[i].len;
    }
    return len;
}

static void DiscardInput(UDPInfo* info /* r31 */, IPHeader* ip /* r30 */) {
    ASSERTLINE(206, ip->len + sizeof(u32) <= info->recvUsed);
    info->recvPtr = IFRingPut(info->recvRing, info->recvBuff, info->recvPtr, info->recvUsed, ip->len + sizeof(u32));
    info->recvUsed -= ip->len + sizeof(u32);
}

static void CopySockets(IPHeader* ip /* r28 */, UDPHeader* udp /* r29 */, IPSocket* local /* r30 */, IPSocket* remote /* r31 */) {
    if (local) {
        local->len = IP_SOCKLEN;
        local->family = IP_INET;
        memmove(local->addr, ip->dst, IP_ALEN);
        local->port = udp->dst;
    }
    if (remote) {
        remote->len = IP_SOCKLEN;
        remote->family = IP_INET;
        memmove(remote->addr, ip->src, IP_ALEN);
        remote->port = udp->src;
    }
}

static void NullCallback() {}

static void SyncCallback(UDPInfo* info /* r1+0x8 */, s32) {
    OSWakeupThread(&info->queueThread);
}

u16 UDPCheckSum(IFVec* vec /* r29 */, s32 nVec /* r24 */) {
    // Local variables
    IPHeader* ip; // r30
    s32 hlen; // r26
    u16* p; // r25
    s32 len; // r27
    u32 sum; // r31

    sum = 0;
    ASSERTLINE(280, 0 < nVec);
    ASSERTLINE(281, IP_MIN_HLEN + UDP_HLEN <= vec->len);
    ip = (IPHeader*)vec->data;
    ASSERTLINE(285, ip->proto == IP_PROTO_UDP);
    hlen = IP_HLEN(ip);

    sum += *(u16*)&ip->src[0];
    sum += *(u16*)&ip->src[2];
    sum += *(u16*)&ip->dst[0];
    sum += *(u16*)&ip->dst[2];
    sum += IP_PROTO_UDP;
    sum += ip->len - hlen;

    p = (u16*)((u8*)ip + hlen);
    len = vec->len - hlen;
    for (;;) {
        while (1 < len) {
            sum += *p++;
            len -= 2;
        }
        if (len == 1) {
            sum += *(u8*)p << 8;
        }
        if (--nVec <= 0) {
            break;
        }
        ++vec;
        p = (u16*)vec->data;
        len = vec->len;
    }

    sum = (sum & 0xFFFF) + (sum >> 16);
    sum = (sum & 0xFFFF) + (sum >> 16);
    return sum ^ 0xFFFF;
}

s32 UDPConnect(UDPInfo* info /* r29 */, const IPSocket* socket /* r1+0xC */) {
    // Local variables
    BOOL enabled; // r30
    s32 rc; // r31

    // References
    // -> static unsigned short Port;
    // -> struct IFQueue UDPInfoQueue;
    enabled = OSDisableInterrupts();
    if (info->pair.proto != IP_PROTO_UDP) {
        rc = -12;
    } else {
        rc = IPConnect(&UDPInfoQueue, &info->pair, socket, &Port);
    }
    OSRestoreInterrupts(enabled);
    return rc;
}

static void SendCallback(UDPInfo* info /* r31 */, s32 result /* r29 */) {
    // Local variables
    UDPCallback callback; // r28

    ASSERTLINE(569, info->datagram.interface == NULL);
    ASSERTLINE(570, info->datagram.queue == NULL);
    result = (result < 0) ? result : info->datagram.vec[1].len;
    if (info->sendResult) {
        *info->sendResult = result;
        info->sendResult = NULL;
    }

    if (info->sendData) {
        ASSERTLINE(581, info->sendCallback == NULL);
        ASSERTLINE(582, 0 < info->sendUsed);
        info->sendUsed = 0;
    } else {
        ASSERTLINE(587, info->sendCallback != NULL);
        callback = info->sendCallback;
        info->sendCallback = NULL;
        callback(info, result);
    }

    if (0 < info->pair.poll && info->sendData == NULL) {
        __IPWakeupPollingThreads();
    }
}

s32 UDPSendAsync(UDPInfo* info /* r31 */, void* data /* r20 */, s32 len /* r27 */, const IPSocket* remote /* r23 */, UDPCallback callback /* r22 */, s32* result /* r25 */) {
    // Local variables
    BOOL enabled; // r21
    IPHeader* ip; // r30
    UDPHeader* udp; // r26
    s32 rc; // r28
    IFDatagram* datagram; // r29
    IPInterface* interface; // r24

    // References
    // -> struct IPInterface __IFDefault;
    // -> unsigned char IPAddrAny[4];
    // -> unsigned char IPLoopbackAddr[4];
    // -> static unsigned short Port;
    // -> struct IFQueue UDPInfoQueue;
    interface = NULL;
    enabled = OSDisableInterrupts();
    if (info->pair.proto != IP_PROTO_UDP) {
        rc = -12;
        goto error;
    }

    if (info->pair.local.port == 0) {
        info->pair.local.port = IPGetAnonPort(&UDPInfoQueue, &Port);
        if (info->pair.local.port == 0) {
            rc = -7;
            goto error;
        }
    }

    if (len < 0 || 65535 - UDP_HLEN < len) {
        rc = -17;
        goto error;
    }

    if (info->sendCallback && info->sendData == NULL) {
        rc = IP_ERR_BUSY;
        goto error;
    }

    if (info->pair.remote.port == 0) {
        if (remote == NULL) {
            rc = -6;
            goto error;
        }
        if (remote->len != IP_SOCKLEN || remote->family != IP_INET || remote->port == 0 || IP_CLASSE(remote->addr)) {
            rc = -12;
            goto error;
        }
        if (IPEQ(remote->addr, IPAddrAny)) {
            rc = -13;
            goto error;
        }
    }

    if (info->sendData && info->sendBuff < len) {
        rc = -17;
        goto error;
    }

    if (info->sendData == NULL) {
        ip = (IPHeader*)info->header;
        udp = (UDPHeader*)(info->header + IP_MIN_HLEN);
        datagram = &info->datagram;
        info->sendCallback = callback ? callback : NullCallback;
        info->sendResult = result;
        if (result) {
            *result = IP_ERR_BUSY;
        }
        datagram->vec[1].data = data;
        datagram->callback = (void (*)(void*, s32))SendCallback;
    } else if (info->sendUsed <= 0) {
        ip = (IPHeader*)info->header;
        udp = (UDPHeader*)(info->header + IP_MIN_HLEN);
        datagram = &info->datagram;
        memmove(info->sendData, data, len);
        info->sendUsed = len;
        datagram->vec[1].data = info->sendData;
        datagram->callback = (void (*)(void*, s32))SendCallback;
    } else {
        interface = &__IFDefault;
        datagram = (IFDatagram*)interface->alloc(interface, sizeof(IFDatagram) + sizeof(IFVec) + IP_MIN_HLEN + UDP_HLEN + len);
        if (datagram == NULL) {
            rc = -7;
            goto error;
        }
        ip = (IPHeader*)((u8*)datagram + sizeof(IFDatagram) + sizeof(IFVec));
        udp = (UDPHeader*)((u8*)ip + IP_MIN_HLEN);
        memmove((u8*)udp + UDP_HLEN, data, len);
        datagram->vec[1].data = (u8*)udp + UDP_HLEN;
        datagram->callback = NULL;
        datagram->queue = NULL;
        datagram->interface = NULL;
    }

    ip->verlen = 0x45;
    ip->tos = info->pair.tos;
    ip->len = IP_HLEN(ip) + UDP_HLEN + len;
    ip->ttl = info->pair.ttl;
    ip->proto = IP_PROTO_UDP;
    ip->frag = 0;
    udp->src = info->pair.local.port;
    udp->len = len + UDP_HLEN;
    udp->sum = 0;
    if (info->pair.remote.port != 0) {
        udp->dst = info->pair.remote.port;
        memmove(ip->dst, info->pair.remote.addr, IP_ALEN);
    } else {
        udp->dst = remote->port;
        memmove(ip->dst, remote->addr, IP_ALEN);
    }

    if (IPNEQ(info->pair.local.addr, IPAddrAny)) {
        memmove(ip->src, info->pair.local.addr, IP_ALEN);
    } else if (ip->dst[0] == 127) {
        memmove(ip->src, IPLoopbackAddr, IP_ALEN);
    } else if (memcmp(ip->dst, __IFDefault.alias, 2) == 0 || IPEQ(__IFDefault.addr, IPAddrAny)) {
        memmove(ip->src, __IFDefault.alias, IP_ALEN);
    } else {
        memmove(ip->src, __IFDefault.addr, IP_ALEN);
    }

    datagram->nVec = 2;
    datagram->vec[0].data = ip;
    datagram->vec[0].len = IP_HLEN(ip) + UDP_HLEN;
    datagram->vec[1].len = len;
    datagram->param = info;
    rc = IPOut(datagram);
    if (rc < 0) {
        if (result) {
            *result = rc;
        }
        info->sendCallback = NULL;
        if (0 < info->pair.poll && info->sendData == NULL) {
            __IPWakeupPollingThreads();
        }
        if (interface) {
            interface->free(interface, datagram, sizeof(IFDatagram) + sizeof(IFVec) + IP_MIN_HLEN + UDP_HLEN + len);
        }
    } else if (info->sendData) {
        if (result) {
            *result = len;
        }
        if (callback) {
            callback(info, len);
        }
    }
    OSRestoreInterrupts(enabled);
    return rc;

error:
    if (result) {
        *result = rc;
    }
    OSRestoreInterrupts(enabled);
    return rc;
}

s32 UDPSend(UDPInfo* info /* r29 */, void* data /* r1+0xC */, s32 len /* r1+0x10 */, const IPSocket* remote /* r1+0x14 */) {
    // Local variables
    BOOL enabled; // r30
    s32 result; // r1+0x18
    s32 rc; // r31

    rc = UDPSendAsync(info, data, len, remote, SyncCallback, &result);
    if (rc < 0) {
        return rc;
    }

    enabled = OSDisableInterrupts();
    while (result == IP_ERR_BUSY) {
        OSSleepThread(&info->queueThread);
    }
    OSRestoreInterrupts(enabled);
    return result;
}

s32 UDPReceiveExAsync(UDPInfo* info /* r31 */, void* data /* r22 */, s32 len /* r28 */, IPSocket* local /* r23 */, IPSocket* remote /* r24 */, u32 flag /* r26 */, UDPCallback callback /* r27 */, s32* result /* r29 */) {
    // Local variables
    BOOL enabled; // r25
    s32 rc; // r30
    IPHeader ip; // r1+0x30
    UDPHeader udp; // r1+0x28

    rc = 0;
    enabled = OSDisableInterrupts();
    if (info->pair.proto != IP_PROTO_UDP) {
        rc = -12;
    } else if (len < 0) {
        rc = -17;
    } else if (info->pair.local.port == 0) {
        rc = -4;
    } else if (info->recvCallback && !(flag & 4)) {
        rc = IP_ERR_BUSY;
    }

    if (rc != 0) {
        if (result) {
            *result = rc;
        }
        OSRestoreInterrupts(enabled);
        return rc;
    }

    callback = callback ? callback : NullCallback;
    if (0 < info->recvUsed) {
        rc = PeekInput(info, &ip, sizeof(IPHeader), 0);
        ASSERTLINE(886, rc == sizeof(IPHeader));
        rc = PeekInput(info, &udp, sizeof(UDPHeader), IP_HLEN(&ip));
        ASSERTLINE(888, rc == sizeof(UDPHeader));
        len = (udp.len - UDP_HLEN < len) ? udp.len - UDP_HLEN : len;
        rc = PeekInput(info, data, len, IP_HLEN(&ip) + UDP_HLEN);
        CopySockets(&ip, &udp, local, remote);
        if (!(flag & 2)) {
            DiscardInput(info, &ip);
        }
        if (result) {
            *result = udp.len - UDP_HLEN;
        }
        callback(info, udp.len - UDP_HLEN);
    } else if (flag & 4) {
        rc = -9;
        if (result) {
            *result = rc;
        }
    } else {
        if (flag & 2) {
            info->flag |= 0x400;
        } else {
            info->flag &= ~0x400;
        }
        info->recvCallback = callback;
        info->recvResult = result;
        if (result) {
            *result = IP_ERR_BUSY;
        }
        info->data = data;
        info->len = len;
        info->remote = remote;
        info->local = local;
    }
    OSRestoreInterrupts(enabled);
    return rc;
}

s32 UDPReceiveEx(UDPInfo* info /* r29 */, void* data /* r1+0xC */, s32 len /* r1+0x10 */, IPSocket* local /* r1+0x14 */, IPSocket* remote /* r1+0x18 */, u32 flag /* r1+0x1C */) {
    // Local variables
    BOOL enabled; // r30
    s32 result; // r1+0x20
    s32 rc; // r31

    rc = UDPReceiveExAsync(info, data, len, local, remote, flag, SyncCallback, &result);
    if (rc < 0) {
        return rc;
    }

    enabled = OSDisableInterrupts();
    while (result == IP_ERR_BUSY) {
        OSSleepThread(&info->queueThread);
    }
    OSRestoreInterrupts(enabled);
    return result;
}

static void Cancel(UDPInfo* info, s32 result) {
    UDPCallback callback;
    IPCancel(&info->datagram);
    callback = info->sendCallback;
    if (callback) {
        info->sendCallback = NullCallback;
        if (info->sendResult) {
            if (*info->sendResult == IP_ERR_BUSY) {
                *info->sendResult = result;
            }
            result = *info->sendResult;
            info->sendResult = NULL;
        }
        callback(info, result);
    }
    callback = info->recvCallback;
    if (callback) {
        info->recvCallback = NullCallback;
        if (info->recvResult) {
            if (*info->recvResult == IP_ERR_BUSY) {
                *info->recvResult = result;
            }
            result = *info->recvResult;
            info->recvResult = NULL;
        }
        callback(info, result);
    }
    info->sendCallback = NULL;
    info->recvCallback = NULL;
    if (0 < info->pair.poll) {
        __IPWakeupPollingThreads();
    }
}

s32 UDPClose(UDPInfo* info /* r29 */) {
    // Local variables
    BOOL enabled; // r27
    s32 rc; // r28
    // IFQueue* ___next; // r31 (IFQueueDequeueEntry)
    // IFQueue* ___prev; // r30 (IFQueueDequeueEntry)

    // References
    // -> struct IFQueue UDPInfoQueue;
    rc = 0;
    enabled = OSDisableInterrupts();
    if (info->pair.proto != IP_PROTO_UDP) {
        rc = -12;
    }
    if (rc != 0) {
        OSRestoreInterrupts(enabled);
        return rc;
    }

    Cancel(info, -8);

    do {
        register IFQueue* ___next;
        register IFQueue* ___prev;

        ___next = info->pair.link.next;
        ___prev = info->pair.link.prev;
        if (___next == NULL) {
            UDPInfoQueue.prev = ___prev;
        } else {
            ((IPInfo*)___next)->link.prev = ___prev;
        }
        if (___prev == NULL) {
            UDPInfoQueue.next = ___next;
        } else {
            ((IPInfo*)___prev)->link.next = ___next;
        }
    } while (0);

    info->pair.proto = 0;
    OSRestoreInterrupts(enabled);
    return rc;
}

void UDPNotify(IPHeader* ip /* r29 */, const u8*, s32 err /* r1+0x10 */) {
    // Local variables
    UDPHeader* udp; // r30
    IPInfo* info; // r31
    IPInfo* next; // r28
    UDPInfo* match; // r27

    // References
    // -> unsigned char IPAddrAny[4];
    // -> struct IFQueue UDPInfoQueue;
    udp = (UDPHeader*)((u8*)ip + IP_HLEN(ip));
    if (IPEQ(ip->dst, IPAddrAny) || udp->dst == 0) {
        return;
    }

    IFQueueIterator(IPInfo*, &UDPInfoQueue, info, next) {
        if (info->local.port != 0 && info->local.port == udp->src &&
            (IPEQ(info->local.addr, IPAddrAny) || IPEQ(info->local.addr, ip->src)) && info->remote.port == udp->dst &&
            IPEQ(info->remote.addr, ip->dst)) {
            match = (UDPInfo*)info;
            Cancel(match, err);
        }
    }
}

BOOL UDPOnReset(BOOL) {
    // Local variables
    UDPInfo* info; // r31

    // References
    // -> struct IFQueue UDPInfoQueue;
    if (UDPInfoQueue.next == NULL) {
        return TRUE;
    }

    while (UDPInfoQueue.next != NULL) {
        info = (UDPInfo*)UDPInfoQueue.next;
        UDPClose(info);
    }
    return FALSE;
}
