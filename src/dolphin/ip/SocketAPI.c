/* Reviewed fragment of IPSocket.c adapted for the older target behavior.
 * Provenance, scope and ABI evidence are recorded in docs/Network.md.
 */
#include <dolphin/private/ip.h>
#include <dolphin/ip/IPArp.h>

#ifdef NULL
#undef NULL
#endif

#define NULL 0

static SOAllocFunc Alloc = NULL;
static SOFreeFunc Free = NULL;
static u32 Allocated = 0;

#define SO_TABLE_NUM 256
static SONode SocketTable[SO_TABLE_NUM];
static IFQueue LingerQueue;
static SOSockAddrIn SockAnyIn = { 8, 2, 0, { 0 } };
static u8* TimeWaitBuf = NULL;
static s32 TimeWaitBufSize = 0;
static u8* ReassemblyBuffer = NULL;
static s32 ReassemblyBufferSize = 0;
static s32 State = 0;
static u32 Flag = 0;
static s32 Mtu = 0;
static s32 Rwin = 0;
static OSTime R2 = 0;

static OSThreadQueue CleaningQueue;
static OSThreadQueue PollingQueue;
static BOOL LowInitialized;
static BOOL Initialized;

static BOOL OnReset(BOOL);
static OSResetFunctionInfo ResetFunctionInfo = { &OnReset, 110, NULL, NULL };

void SOFree(u32 name, void* ptr, s32 size) {
    BOOL enabled;

    ASSERTLINE(321, Free);

    if (ptr != NULL) {
        (*Free)(name, ptr, size);
        enabled = OSDisableInterrupts();
        Allocated -= size;

        if (Allocated == 0 && State == 2) {
            OSWakeupThread(&CleaningQueue);
        }

        OSRestoreInterrupts(enabled);
    }
}

char* SOInetNtoP(int af, void* src, char* dst, u32 len) {
    const u8* addr;

    addr = (const u8*)src;
    if (af == 2 && dst != NULL && len >= 16) {
        sprintf(dst, "%u.%u.%u.%u", addr[0], addr[1], addr[2], addr[3]);
        return dst;
    }

    return NULL;
}

static struct SONode* GetNode(int s, IPInfo** pinfo) {
    SONode* node;
    IPInfo* info;
    IPInfo* next;
    TCPInfo* tcp;
    BOOL enabled;
    IFQueue queue;

    queue.next = queue.prev = NULL;
    enabled = OSDisableInterrupts();

    /* Find any TCP packets which are unused */
    IFQueueIterator(IPInfo*, &LingerQueue, info, next) {
        tcp = (TCPInfo*)info;
        if (tcp->node == NULL || ((SONode*)tcp->node)->ref == 0) {
            IFQueueDequeueEntry(IPInfo*, &LingerQueue, info);
            IFQueueEnqueueTail(IPInfo*, &queue, info);
        }
    }

    OSRestoreInterrupts(enabled);

    /* Free all unused TCP packets */
    while (queue.next != NULL) {
        IFQueueDequeueHead(IPInfo*, &queue, info);

        tcp = (TCPInfo*)info;
        SOFree(2, tcp->recvData, tcp->recvBuff);
        SOFree(1, tcp->sendData, tcp->sendBuff);
        SOFree(0, tcp, sizeof(TCPInfo));
    }

    node = NULL;
    enabled = OSDisableInterrupts();
    if (s >= 0 && s < SO_TABLE_NUM) {
        node = &SocketTable[s];
        if (node->ref <= 0 || node->info == NULL) {
            node = NULL;
        } else {
            node->ref++;
            if (pinfo != NULL) {
                *pinfo = node->info;
            }
        }
    }
    OSRestoreInterrupts(enabled);
    return node;
}

static void PutNode(SONode* node) {
    BOOL enabled;
    IPInfo* info;
    TCPInfo* tcp;
    UDPInfo* udp;
    u8 proto;

    ASSERTLINE(538, node);

    proto = 0;
    info = NULL;
    enabled = OSDisableInterrupts();
    ASSERTLINE(542, 0 < node->ref);
    if (--node->ref == 0 && node->info != NULL) {
        info = node->info;
        node->info = NULL;
        proto = node->proto;
        node->proto = 0;
    }
    OSRestoreInterrupts(enabled);

    if (info != NULL) {
        switch (proto) {
            case IP_PROTO_UDP:
                udp = (UDPInfo*)info;
                SOFree(5, udp->recvRing, udp->recvBuff);
                SOFree(4, udp->sendData, udp->sendBuff);
                SOFree(3, udp, sizeof(UDPInfo));
                break;
            case IP_PROTO_TCP:
                tcp = (TCPInfo*)info;
                SOFree(2, tcp->recvData, tcp->recvBuff);
                SOFree(1, tcp->sendData, tcp->sendBuff);
                SOFree(0, tcp, sizeof(TCPInfo));
                break;
            default:
                OSPanic("IPSocket.c", 494, "PutNode: unknown proto");
                break;
        }
    }
}

int SOConnect(int s, void* sockAddr) {
    SONode* node;
    IPInfo* info;
    TCPInfo* tcp;
    UDPInfo* udp;
    s32 rc;
    s32 result;

    if (State != 1) {
        return -39;
    }

    ASSERTLINE(1825, sockAddr != NULL && sizeof(SOSockAddrIn) <= ((SOSockAddr*) sockAddr)->len);
    if (sockAddr == NULL || ((SOSockAddr*) sockAddr)->len < sizeof(SOSockAddrIn)) {
        return -28;
    }

    node = GetNode(s, &info);
    if (node == NULL || info == NULL) {
        return -8;
    }

    switch (info->proto) {
        case IP_PROTO_UDP:
            udp = (UDPInfo*)info;
            if (((SOSockAddr*) sockAddr)->family == 0) {
                sockAddr = &SockAnyIn;
            }
            rc = UDPConnect(udp, (IPSocket*)sockAddr);
            break;
        case IP_PROTO_TCP:
            tcp = (TCPInfo*)info;
            if ((node->flag & 0x4) == 0) {
                rc = TCPConnect(tcp, (IPSocket*)sockAddr);
            } else {
                rc = TCPConnectAsync(tcp, (IPSocket*)sockAddr, NULL, &result);
                if (rc == 0) {
                    rc = result;
                }
            }
            break;
        default:
            PutNode(node);
            return -8;
    }

    PutNode(node);
    switch (rc) {
        case 0:
            return 0;
        case -1:
            return -26;
        case -13:
            return -5;
        case -5:
            return -30;
        case -3:
            return -15;
        case -11:
            return -14;
        case -10:
            return -76;
        case -12:
            return -28;
        case -7:
            return -42;
        case -19:
            return -38;
        default:
            return -40;
    }
}

int SOShutdown(int s, int how) {
    SONode* node;
    IPInfo* info;
    TCPInfo* tcp;
    s32 rc;

    if (State != 1) {
        return -39;
    }

    switch (how) {
        case 0:
        case 1:
        case 2:
            break;
        default:
            return -28;
    }

    node = GetNode(s, &info);
    if (node == NULL || info == NULL) {
        return -8;
    }

    switch (info->proto) {
        case IP_PROTO_UDP:
            rc = 0;
            break;
        case IP_PROTO_TCP:
            tcp = (TCPInfo*)info;
            rc = TCPShutdown(tcp, how);
            break;
        default:
            PutNode(node);
            return -8;
    }

    PutNode(node);
    switch (rc) {
        case 0:
        case -8:
            return 0;
        case -4:
            return -56;
        case -12:
        default:
            return -28;
    }
}

int SORecvFrom(int s, void* buf, int len, int flags, void* sockFrom) {
    SONode* node;
    IPInfo* info;
    UDPInfo* udp;
    TCPInfo* tcp;
    s32 rc;

    if (State != 1) {
        return -39;
    }

    ASSERTLINE(2198, sockFrom == NULL || sizeof(SOSockAddrIn) <= ((SOSockAddr*) sockFrom)->len);
    if (sockFrom != NULL && ((SOSockAddr*) sockFrom)->len < sizeof(SOSockAddrIn)) {
        return -28;
    }

    node = GetNode(s, &info);
    if (node == NULL || info == NULL) {
        return -8;
    }

    switch (info->proto) {
        case IP_PROTO_UDP:
            if (flags & ~(0x2 | 0x4)) {
                PutNode(node);
                return -63;
            }

            OSLockMutex(&node->mutexRead);
            if (node->info == NULL) {
                rc = -8;
            } else {
                if (node->flag & 0x4) {
                    flags |= 0x4;
                }

                udp = (UDPInfo*)info;
                rc = UDPReceiveEx(udp, buf, len, NULL, (IPSocket*)sockFrom, flags);
            }
            OSUnlockMutex(&node->mutexRead);
            break;
        case IP_PROTO_TCP:
            if (flags & ~(0x1 | 0x2 | 0x4)) {
                PutNode(node);
                return -63;
            }

            tcp = (TCPInfo*)info;
            if (sockFrom != NULL) {
                rc = TCPGetRemoteSocket(tcp, (IPSocket*)sockFrom);
                if (rc < 0) {
                    PutNode(node);
                    return -8;
                }
            }

            OSLockMutex(&node->mutexRead);
            if (node->info == NULL) {
                rc = -8;
            } else {
                if (node->flag & 0x4) {
                    flags |= 0x4;
                }

                tcp = (TCPInfo*)info;
                if (!(flags & 0x1)) {
                    rc = TCPReceiveEx(tcp, buf, len, flags);
                } else {
                    rc = TCPReceiveUrgEx(tcp, buf, len, flags);
                }
            }
            OSUnlockMutex(&node->mutexRead);
            break;
        default:
            PutNode(node);
            return -8;
    }

    PutNode(node);
    if (rc < 0) {
        switch (rc) {
            case -1:
            case -9:
                rc = -6;
                break;
            case -4:
            case -8:
                rc = -56;
                break;
            case -16:
                rc = -27;
                break;
            case -10:
            case -19:
                rc = -76;
                break;
            case -3:
            case -11:
            case -18:
                rc = -15;
                break;
            default:
                rc = -28;
                break;
        }
    }

    return rc;
}

int SOSendTo(int s, void* buf, int len, int flags, void* sockTo) {
    SONode* node;
    IPInfo* info;
    UDPInfo* udp;
    TCPInfo* tcp;
    s32 rc;

    if (State != 1) {
        return -39;
    }

    ASSERTLINE(2404, sockTo == NULL || sizeof(SOSockAddrIn) <= ((SOSockAddr*) sockTo)->len);
    if (sockTo != NULL && ((SOSockAddr*) sockTo)->len < sizeof(SOSockAddrIn)) {
        return -28;
    }

    node = GetNode(s, &info);
    if (node == NULL || info == NULL) {
        return -8;
    }

    switch (info->proto) {
        case IP_PROTO_UDP:
            if (flags != 0) {
                PutNode(node);
                return -63;
            }

            OSLockMutex(&node->mutexWrite);
            if (node->info == NULL) {
                rc = -8;
            } else {
                udp = (UDPInfo*)info;
                switch (flags) {
                    case 0:
                        rc = UDPSend(udp, buf, len, (IPSocket*)sockTo);
                        break;
                }
            }
            OSUnlockMutex(&node->mutexWrite);
            break;
        case IP_PROTO_TCP:
            if (flags & ~(0x1 | 0x4)) {
                PutNode(node);
                return -63;
            }

            OSLockMutex(&node->mutexWrite);
            if (node->info == NULL) {
                rc = -8;
            } else {
                tcp = (TCPInfo*)info;
                if (node->flag & 0x4) {
                    flags |= 0x4;
                }

                if (!(flags & 4)) {
                    switch (flags) {
                        case 0:
                            rc = TCPSend(tcp, buf, len);
                            break;
                        case 1:
                            rc = TCPSendUrg(tcp, buf, len);
                            break;
                    }
                } else {
                    switch (flags) {
                        case 4:
                            rc = TCPSendNonblock(tcp, buf, len);
                            break;
                        case 5:
                            rc = TCPSendUrgNonblock(tcp, buf, len);
                            break;
                    }
                    if (rc == 0 && len > 0) {
                        rc = -9;
                    }
                }
            }
            OSUnlockMutex(&node->mutexWrite);
            break;
        default:
            PutNode(node);
            return -8;
    }

    PutNode(node);
    if (rc < 0) {
        switch (rc) {
            case -13:
                rc = -5;
                break;
            case -6:
                rc = -17;
                break;
            case -17:
                rc = -35;
                break;
            case -2:
                rc = -40;
                break;
            case -7:
                rc = -42;
                break;
            case -1:
            case -9:
                rc = -6;
                break;
            case -4:
            case -8:
                rc = -56;
                break;
            case -16:
                rc = -27;
                break;
            case -10:
                rc = -76;
                break;
            case -3:
            case -11:
            case -18:
                rc = -15;
                break;
            case -12:
                rc = -28;
                break;
            case -19:
                rc = -38;
                break;
            default:
                rc = -8;
                break;
        }
    }

    return rc;
}

void __IPWakeupPollingThreads(void) {
    OSWakeupThread(&PollingQueue);
}

static BOOL OnReset(BOOL) {
    State = 3;
    return TRUE;
}
