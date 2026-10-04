/* Reviewed fragment of IPTcp.c adapted for the older target behavior.
 * Provenance, scope and ABI evidence are recorded in docs/Network.md.
 */
#include <dolphin/ip.h>
#include <dolphin/private/ip.h>

static u32 Rotate(u32 n /* r3 */, u32 s /* r4 */) {
    return (n << s) | (n >> (32 - s));
}

s32 TCPIsn(IPInfo* info /* r30 */) {
    // Local variables
    u32 m; // r31
    u32 unused; // Assumed, required for stack in release

    m = *__OSSystemTime;
    m = Rotate(m, m % 32);
    if (info) {
        m ^= IPU32(info->local.addr);
        m = Rotate(m, m % 32);
        m ^= Rotate(info->local.port, info->remote.port & 31);
        m ^= IPU32(info->remote.addr);
        m = Rotate(m, m % 32);
        m ^= Rotate(info->remote.port, info->local.port & 31);
    }
    m += (u32)(OSGetTime() / (OSTime)(OS_TIMER_CLOCK / 250000));
    return m;
}

u16 TCPCheckSum(IFVec* vec /* r29 */, s32 nVec /* r23 */) {
    // Local variables
    IPHeader* ip; // r30
    s32 hlen; // r24
    u16* p; // r27
    s32 len; // r28
    u32 sum; // r31
    u32 odd; // r25

    sum = 0;
    ASSERTLINE(263, 0 < nVec);
    ASSERTLINE(264, IP_MIN_HLEN + TCP_MIN_HLEN <= vec->len);
    ip = (IPHeader*)vec->data;
    ASSERTLINE(268, ip->proto == IP_PROTO_TCP);
    hlen = IP_HLEN(ip);

    sum += ((u16*)ip->src)[0];
    sum += ((u16*)ip->src)[1];
    sum += ((u16*)ip->dst)[0];
    sum += ((u16*)ip->dst)[1];
    sum += IP_PROTO_TCP;
    sum += ip->len - hlen;

    p = (u16*)((u8*)ip + hlen);
    len = vec->len - hlen;
    for (;;) {
        while (1 < len) {
            sum += *p++;
            len -= 2;
        }
        if (len == 1) {
            odd = *(u8*)p << 8;
        } else {
            odd = 0;
        }
        do {
            if (--nVec <= 0) {
                sum += odd;
                goto done;
            }
            vec++;
        } while (vec->len == 0);
        p = (u16*)vec->data;
        if (len == 1) {
            odd |= *(u8*)p;
            sum += odd;
            ((u8*)p)++;
            len = vec->len - 1;
        } else {
            len = vec->len;
        }
    }
done:
    sum = (sum & 0xFFFF) + (sum >> 16);
    sum = (sum & 0xFFFF) + (sum >> 16);
    return sum ^ 0xFFFF;
}

void TCPRespond(IPInterface* interface /* r25 */, u8* dstAddr /* r1+0x10 */, u16 dst /* r1+0x14 */, u8* srcAddr /* r1+0x18 */, u16 src /* r1+0x1C */, s32 seq /* r1+0x20 */, s32 ack /* r1+0x24 */, u16 flag /* r22 */, u16 win /* r1+0x5E */) {
    // Local variables
    IPHeader* ip; // r31
    TCPHeader* tcp; // r30
    IFDatagram* datagram; // r28
    s32 len; // r23

    len = sizeof(IFDatagram) + sizeof(IPHeader) + sizeof(TCPHeader);
    datagram = interface->alloc(interface, len);
    if (datagram != NULL) {
        ip = (IPHeader*)(datagram + 1);
        memmove(ip->dst, dstAddr, IP_ALEN);
        memmove(ip->src, srcAddr, IP_ALEN);
        ip->verlen = 0x45;
        ip->tos = 0;
        ip->len = IP_HLEN(ip) + TCP_MIN_HLEN;
        ip->ttl = 255;
        ip->proto = IP_PROTO_TCP;
        ip->frag = 0;

        tcp = (TCPHeader*)((u8*)ip + IP_HLEN(ip));
        tcp->src = src;
        tcp->dst = dst;
        tcp->seq = seq;
        tcp->ack = ack;
        tcp->flag = 0x5000 | (flag & 0x3F);
        tcp->win = win;
        tcp->sum = 0;
        tcp->urg = 0;

        datagram->nVec = 1;
        datagram->vec[0].data = ip;
        datagram->vec[0].len = ip->len;
        datagram->callback = NULL;
        datagram->param = NULL;
        datagram->queue = NULL;
        if (IPOut(datagram) < 0) {
            interface->free(interface, datagram, len);
        }
    }
}

s32 TCPSendIn(TCPInfo* info /* r31 */, BOOL nonblock /* r28 */) {
    // Local variables
    s32 useable; // r29
    s32 len; // r30

    if (info->sendCallback == NULL || info->userSendLen <= 0) {
        return 0;
    }

    if (!nonblock && info->sendLen == 0 && info->sendBuff < info->userSendLen) {
        len = 0;
    } else {
        useable = info->sendBuff - info->sendLen;
        len = MIN(useable, info->userSendLen);
    }

    if (0 < len) {
        info->sendPtr = IFRingIn(info->sendData, info->sendBuff, info->sendPtr, info->sendLen, info->userSendData, len);
        info->sendLen += len;
        info->userSendData += len;
        info->userSendLen -= len;
        info->userAcked += len;
    }

    if (nonblock) {
        info->userSendData = NULL;
        info->userSendLen = 0;
    }
    return len;
}

s32 TCPPeekOut(TCPInfo* info /* r31 */, void* ptr /* r1+0xC */, s32 len /* r30 */, BOOL peek /* r1+0x14 */) {
    // Local variables
    void* nextPtr; // r28
    u8 oob; // r1+0x1C

    len = MIN(len, info->recvUser);
    if (len <= 0) {
        return 0;
    }

    if (0 < info->recvUrg) {
        if (info->recvUrg == 1) {
            if (info->flag & 0x80) {
                info->recvPtr = IFRingOut(info->recvData, info->recvBuff, info->recvPtr, info->recvUser, &oob, 1);
                info->recvUser--;
                info->recvUrg--;
                len = MIN(len, info->recvUser);
            } else {
                len = 1;

            }
        } else if (info->recvUrg <= len) {
            len = info->recvUrg - 1;

        }
    }

    if (0 < len) {
        nextPtr = IFRingOut(info->recvData, info->recvBuff, info->recvPtr, info->recvUser, ptr, len);
        if (!peek) {
            info->recvPtr = nextPtr;
            info->recvUser -= len;
            if (0 < info->recvUrg) {
                info->recvUrg -= len;
            }
        }
    }
    return len;
}

s32 TCPRecvOut(TCPInfo* info /* r31 */) {
    // Local variables
    u8* ptr; // r28
    s32 len; // r30
    BOOL peek; // r29

    peek = (info->flag & 0x400) ? TRUE : FALSE;
    ASSERTLINE(2017, info->userData);
    ptr = info->userData + info->userLen;
    len = info->userBuff - info->userLen;
    len = TCPPeekOut(info, ptr, len, peek);
    if (peek) {
        return len;
    }
    return info->userLen += len;
}

void TCPNotify(IPHeader* ip /* r31 */, const u8*, s32 err /* r1+0x10 */) {
    // Local variables
    TCPInfo* info; // r30
    TCPHeader* tcp; // r29

    // References
    // -> struct IFQueue TCPInfoQueue;
    tcp = (TCPHeader*)((u8*)ip + IP_HLEN(ip));
    info = (TCPInfo*)IPLookupInfo(&TCPInfoQueue, ip->dst, ip->src, tcp->dst, tcp->src);
    if (info) {
        info->err = err;
    }
}

void TCPSourceQuench(IPHeader* ip /* r30 */, const u8*) {
    // Local variables
    TCPInfo* info; // r31
    TCPHeader* tcp; // r29

    // References
    // -> struct IFQueue TCPInfoQueue;
    tcp = (TCPHeader*)((u8*)ip + IP_HLEN(ip));
    info = (TCPInfo*)IPLookupInfo(&TCPInfoQueue, ip->dst, ip->src, tcp->dst, tcp->src);
    if (info) {
        info->cWin = info->mss;
    }
}
