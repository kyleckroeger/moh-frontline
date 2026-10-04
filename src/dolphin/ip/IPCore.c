/* Reviewed fragment of IP.c adapted for the older target behavior.
 * Provenance, scope and ABI evidence are recorded in docs/Network.md.
 */
#include <dolphin/private/ip.h>

static u16 Id = 1;
const u8 IPAddrAny[4] = { 0, 0, 0, 0 }; // 0.0.0.0
const u8 IPLoopbackAddr[4] = { 127, 0, 0, 1 }; // 127.0.0.1
const u8 IPLimited[4] = { 255, 255, 255, 255 }; // 255.255.255.255

IPInfo* IPLookupInfo(IFQueue* queue, u8* srcAddr, u8* dstAddr, u16 src, u16 dst) {
    IPInfo* info;
    IPInfo* next;
    int wildcard;
    int minimum;
    IPInfo* match;

    minimum = 3;
    match = NULL;

    IFQueueIterator(IPInfo*, queue, info, next) {
        if (info->local.port != 0 && info->local.port == dst) {
            wildcard = 0;
            if (IPNEQ(dstAddr, IPAddrAny)) {
                if (IPEQ(info->local.addr, IPAddrAny)) {
                    wildcard++;
                } else if (IPNEQ(info->local.addr, dstAddr)) {
                    continue;
                }
            } else if (IPNEQ(info->local.addr, IPAddrAny)) {
                wildcard++;
            }

            if (IPNEQ(srcAddr, IPAddrAny)) {
                if (IPEQ(info->remote.addr, IPAddrAny)) {
                    wildcard++;
                } else if (info->remote.port != src || IPNEQ(info->remote.addr, srcAddr)) {
                    continue;
                }
            } else if (IPNEQ(info->remote.addr, IPAddrAny)) {
                wildcard++;
            }

            if (wildcard < minimum) {
                match = info;
                minimum = wildcard;

                if (minimum == 0) {
                    break;
                }
            }
        }
    }

    return match;
}

u16 IPGetAnonPort(IFQueue* queue, u16* last) {
    u16 port;
    IPInfo* info;
    IPInfo* next;
    int skip;

    skip = 0;
    if (*last < 1024 || 5000 < *last) {
        *last = 1024;
    }

loop:
    port = *last;
    *last = port + 1;
    if (5000 < *last) {
        *last = 1024;
    }

    IFQueueIterator(IPInfo*, queue, info, next) {
        if (info->local.port == port) {
            if (++skip <= 5000 - 1024) {
                goto loop;
            }

            return 0;
        }
    }

    return port;
}

s32 IPConnect(IFQueue* queue, IPInfo* info, const IPSocket* socket, u16* last) {
    IPInfo* iter;
    IPInfo* next;
    IPInterface* interface;
    const u8* localAddr;

    if (socket == NULL || socket->len != 8 || socket->family != 2 || socket->port == 0 || IP_CLASSE(socket->addr)) {
        return -12;
    }

    if (IPEQ(socket->addr, IPAddrAny)) {
        return -13;
    }

    interface = IPGetRoute(socket->addr, NULL);
    if (interface == NULL) {
        return -2;
    }

    if (info->proto == 6 && (IP_CLASSD(socket->addr) || IPIsBroadcastAddr(interface, socket->addr))) {
        return -13;
    }


    if (IPEQ(info->local.addr, IPAddrAny)) {
        if (socket->addr[0] == 127) {
            localAddr = IPLoopbackAddr;
        } else if (memcmp(socket->addr, interface->alias, 2) == 0 || IPEQ(interface->addr, IPAddrAny)) {
            localAddr = interface->alias;
        } else {
            localAddr = interface->addr;
        }
    } else {
        localAddr = info->local.addr;
    }

    if (info->local.port == 0) {
        do {
            info->local.port = IPGetAnonPort(queue, last);
            if (info->local.port == 0) {
                return -7;
            }
        } while (info->proto == IP_PROTO_TCP && TCPLookupTimeWaitInfo(socket->addr, socket->port, localAddr, info->local.port));
    } else if (info->proto == IP_PROTO_TCP) {
        IFQueueIterator(IPInfo*, queue, iter, next) {
            if (iter != info && iter->local.port == info->local.port && iter->remote.port == info->remote.port &&
                IPEQ(iter->local.addr, localAddr) && IPEQ(iter->remote.addr, info->remote.addr)) {
                return -5;
            }
        }

        if (TCPLookupTimeWaitInfo(socket->addr, socket->port, localAddr, info->local.port)) {
            return -5;
        }
    }

    memmove(&info->remote, socket, sizeof(info->remote));
    if (IPEQ(info->local.addr, IPAddrAny)) {
        memmove(info->local.addr, localAddr, sizeof(info->local.addr));
    }

    return 0;
}

s32 IPGetRemoteSocket(IPInfo* info, IPSocket* socket) {
    ASSERTLINE(668, info->remote.len == IP_SOCKLEN);
    ASSERTLINE(669, info->remote.family == IP_INET);
    memcpy(socket, &info->remote, sizeof(info->remote));
    return 0;
}

u16 IPCheckSum(IPHeader* ip) {
    int len;
    u32 sum;
    u16* p;

    sum = 0;
    len = IP_HLEN(ip);
    p = (u16*)ip;

    for (; len > 0; len -= sizeof(u16)) {
        sum += *p++;
    }

    /* Add the 16-bit carry values twice and return the inverse */
    sum = (sum & 0xFFFF) + ((sum >> 16) & 0xFFFF);
    sum = (sum & 0xFFFF) + ((sum >> 16) & 0xFFFF);
    return sum ^ 0xFFFF;
}

s32 IPOut(IFDatagram* datagram) {
    IPHeader* ip;
    IPInterface* interface;
    TCPHeader* tcp;
    UDPHeader* udp;

    ASSERTLINE(1035, 0 < datagram->nVec && datagram->nVec <= IF_MAX_VEC);
    ip = (IPHeader*)datagram->vec[0].data;
    ASSERTLINE(1037, IP_HLEN(ip) <= datagram->vec[0].len);

    ip->id = Id++;
    interface = IPGetRoute(ip->dst, datagram->dst);
    if (interface == NULL) {
        return -2;
    }

    if (interface->mtu < ip->len) {
        return -17;
    }

    if (ip->src[0] != 127 && IPNEQ(interface->addr, ip->src) && IPNEQ(interface->alias, ip->src)) {
        return -19;
    }


    ip->sum = 0;
    ip->sum = IPCheckSum(ip);

    switch (ip->proto) {
        case IP_PROTO_UDP:
            udp = (UDPHeader*)(((u8*)ip) + IP_HLEN(ip));
            udp->sum = 0;
            udp->sum = UDPCheckSum(datagram->vec, datagram->nVec);
            if (udp->sum == 0) {
                udp->sum = 0xFFFF;
            }
            break;
        case IP_PROTO_TCP:
            tcp = (TCPHeader*)(((u8*)ip) + IP_HLEN(ip));
            tcp->sum = 0;
            tcp->sum = TCPCheckSum(datagram->vec, datagram->nVec);
            ASSERTLINE(1098, (tcp->flag & (TCP_FLAG_SYN | TCP_FLAG_FIN)) != (TCP_FLAG_SYN | TCP_FLAG_FIN));
            break;
    }

    datagram->type = ETH_IP;
    (*interface->out)(interface, datagram);
    return 0;
}

void IPCancel(IFDatagram* datagram) {
    IPInterface* interface;

    interface = datagram->interface;
    if (interface) {
        (*interface->cancel)(interface, datagram);
        ASSERTLINE(1119, datagram->interface == NULL);
    }
}
