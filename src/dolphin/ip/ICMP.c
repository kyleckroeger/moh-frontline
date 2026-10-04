/* Reviewed fragment of IPIcmp.c, adapted for the older target behavior.
 * Provenance, scope and ABI evidence are recorded in docs/Network.md.
 */
#include <dolphin/ip.h>
#include <dolphin/private/ip.h>

static void DoNotify(IPHeader* org, u8* gateway, s32 err) {
    switch (org->proto) {
        case IP_PROTO_UDP:
        UDPNotify(org, gateway, err);
        break;
        case IP_PROTO_TCP:
        TCPNotify(org, gateway, err);
        break;
    }
}

static int ICMPIsErrorMessage(u8 type) {
    switch (type) {
        case 3:
        return sizeof(ICMPUnreachable);
        case 4:
        return sizeof(ICMPUnreachable);
        case 5:
        return sizeof(ICMPRedirect);
        case 11:
        return sizeof(ICMPUnreachable);
        case 12:
        return sizeof(ICMPUnreachable);
        default:
        return 0;
    }
}

static u16 ICMPCheckSum(ICMPHeader* ip, int len) {

    u32 sum;
    u16* p;

    sum = 0;
    for (p = (u16*)ip; len > 0; len -= 2) {
        sum += *p++;
    }
    sum = (sum & 0xFFFF) + (sum >> 16);
    sum = (sum & 0xFFFF) + (sum >> 16);
    return sum ^ 0xFFFF;
}

static void DoEchoRequest(IPInterface* interface, IPHeader* ip, u32 flag) {

    IFDatagram* datagram;
    IPHeader* res;
    ICMPHeader* icmp;
    void* data;

    if (ip->dst[0] == 127 || IPEQ(ip->dst, interface->addr) || IPEQ(ip->dst, interface->alias)) {
        if ((flag & 3) || IPIsBroadcastAddr(interface, ip->src) || IP_CLASSD(ip->src) || IP_CLASSE(ip->src)) {
            return;
        }

        datagram = interface->alloc(interface, ip->len + sizeof(IFDatagram));
        if (datagram == NULL) {
            return;
        }

        datagram->vec[0].data = data = datagram + 1;
        datagram->vec[0].len = ip->len;
        res = datagram->vec[0].data;
        memmove(res, ip, IP_HLEN(ip));
        memmove(res->src, ip->dst, IP_ALEN);
        memmove(res->dst, ip->src, IP_ALEN);
        res->tos = 0;
        res->ttl = 255;
        if (IP_HLEN(res) > 20) {
            IPUpdateRecordRoute(res, res->src);
            IPReverseSourceRoute(res);
        }

        icmp = (ICMPHeader*)((u8*)res + IP_HLEN(res));
        memmove(icmp, (u8*)ip + IP_HLEN(ip), ip->len - IP_HLEN(ip));
        icmp->type = 0;
        icmp->sum = 0;
        icmp->sum = ICMPCheckSum(icmp, res->len - IP_HLEN(res));
        datagram->nVec = 1;
        datagram->callback = NULL;
        datagram->param = NULL;
        datagram->queue = NULL;
        if (IPOut(datagram) < 0) {
            interface->free(interface, datagram, ip->len + sizeof(IFDatagram));
        }
    }
}

static u16 ReduceMtu(s32 mtu) {
    if (mtu > 1492) {
        return 1006;
    }
    if (mtu > 1006) {
        return 508;
    }
    return 68;
}

static void DoUnreachable(IPInterface* interface, IPHeader* ip, u32) {

    ICMPUnreachable* icmp;
    IPHeader* org;
    u16 mtu;
    IPInfo* info;
    IPInfo* next;
    TCPInfo* tcpInfo;

    icmp = (ICMPUnreachable*)((u8*)ip + IP_HLEN(ip));
    org = (IPHeader*)(icmp + 1);
    if (ip->len < IP_HLEN(ip) + sizeof(ICMPUnreachable) || IP_HLEN(org) < 20) {
        return;
    }
    switch (icmp->code) {
        case 4:
        if (icmp->mtu != 0) {
            mtu = (icmp->mtu < 68) ? 68 : icmp->mtu;
        } else {
            mtu = ReduceMtu((interface->mtu < org->len) ? interface->mtu : org->len);
        }
        again:
        mtu = (interface->mtu < mtu) ? interface->mtu : mtu;
        IFQueueIterator(IPInfo*, &TCPInfoQueue, info, next) {
            tcpInfo = (TCPInfo*)info;
            if (IPEQ(info->remote.addr, org->dst)) {
                if (tcpInfo->mss < mtu - 40) {
                    mtu = ReduceMtu(tcpInfo->mss + 40);
                    goto again;
                }
                tcpInfo->mss = mtu - 40;
                tcpInfo->cWin = tcpInfo->mss;
            }
        }
        break;
        case 0:
        case 1:
        case 2:
        case 3:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        DoNotify(org, ip->src, -2);
        break;
    }
}

static int DoRedirect(IPInterface*, IPHeader* ip, u32) {

    ICMPRedirect* icmp;
    IPHeader* org;

    icmp = (ICMPRedirect*)((u8*)ip + IP_HLEN(ip));
    org = (IPHeader*)(icmp + 1);
    if (icmp->code > 3) {
        return FALSE;
    }
    if (ip->len < IP_HLEN(ip) + sizeof(ICMPRedirect) || IP_HLEN(org) < 20) {
        return FALSE;
    }
    return IPRedirect(org->dst, icmp->gateway, ip->src);
}

static void DoSourceQuench(IPInterface* interface, IPHeader* ip, u32 flag) {
    ICMPUnreachable* icmp;
    IPHeader* org;
    icmp = (ICMPUnreachable*)((u8*)ip + IP_HLEN(ip));
    org = (IPHeader*)(icmp + 1);
    if (ip->len < IP_HLEN(ip) + sizeof(ICMPUnreachable) || IP_HLEN(org) < 20) {
        return;
    }
    switch (org->proto) {
        case IP_PROTO_UDP:
        UDPNotify(org, ip->src, -18);
        break;
        case IP_PROTO_TCP:
        TCPSourceQuench(org, ip->src);
        break;
    }
}

static void DoTimeExceeded(IPInterface* interface, IPHeader* ip, u32 flag) {
    ICMPHeader* icmp = (ICMPHeader*)((u8*)ip + IP_HLEN(ip));
    IPHeader* org = (IPHeader*)((u8*)icmp + 8);
    if (ip->len < IP_HLEN(ip) + 8 || IP_HLEN(org) < 20) {
        return;
    }
    DoNotify(org, ip->src, -10);
}

static void DoParameterProblem(IPInterface* interface, IPHeader* ip, u32 flag) {
    ICMPHeader* icmp = (ICMPHeader*)((u8*)ip + IP_HLEN(ip));
    IPHeader* org = (IPHeader*)((u8*)icmp + 8);
    if (ip->len < IP_HLEN(ip) + 8 || IP_HLEN(org) < 20) {
        return;
    }
    DoNotify(org, ip->src, -12);
}

void ICMPIn(IPInterface* interface, IPHeader* ip, u32 flag) {

    ICMPHeader* icmp;
    IPHeader* org;
    int hlen;

    ASSERTLINE(355, ip->proto == IP_PROTO_ICMP);
    icmp = (ICMPHeader*)((u8*)ip + IP_HLEN(ip));
    if (ip->len < IP_HLEN(ip) + 4 || ICMPCheckSum(icmp, ip->len - IP_HLEN(ip))) {
        return;
    }

    switch (icmp->type) {
        case 0:
        break;
        case 3:
        DoUnreachable(interface, ip, flag);
        break;
        case 4:
        DoSourceQuench(interface, ip, flag);
        break;
        case 8:
        DoEchoRequest(interface, ip, flag);
        break;
        case 5:
        DoRedirect(interface, ip, flag);
        break;
        case 11:
        DoTimeExceeded(interface, ip, flag);
        break;
        case 12:
        DoParameterProblem(interface, ip, flag);
        break;
    }
}
