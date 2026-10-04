/* Reviewed fragment of IPTcpUser.c adapted for the older target behavior.
 * Provenance, scope and ABI evidence are recorded in docs/Network.md.
 */
#include <dolphin/ip.h>
#include <dolphin/private/ip.h>

#ifdef NULL
#undef NULL
#endif

#define NULL 0

static u16 Port = 1024; // size: 0x2, address: 0x0
IFQueue TCPInfoQueue; // size: 0x8, address: 0x0

static void NullCallback() {}

static void SyncCallback(TCPInfo* info /* r1+0x8 */, s32) {
    OSWakeupThread(&info->queueThread);
}

s32 TCPGetRemoteSocket(TCPInfo* info /* r29 */, IPSocket* socket /* r1+0xC */) {
    // Local variables
    BOOL enabled; // r30
    s32 rc; // r31

    enabled = OSDisableInterrupts();
    if (info->pair.proto != IP_PROTO_TCP) {
        rc = -12;
    } else {
        rc = IPGetRemoteSocket(&info->pair, socket);
    }
    OSRestoreInterrupts(enabled);
    return rc;
}

BOOL TCPAbort(TCPInfo* info /* r31 */) {
    // Local variables
    TCPCallback callback; // r24
    s32 state; // r21
    TCPInfo* log; // r23

    // References
    // -> struct IFQueue TCPInfoQueue;

    ASSERTLINE(304, info->pair.proto == IP_PROTO_TCP);
    if (info->pair.proto != IP_PROTO_TCP) {
        return FALSE;
    }

    state = info->state;
    info->state = 0;
    IPCancel(&info->datagram);
    TCPCancelRxmitTimer(info);
    OSCancelAlarm(&info->lingerAlarm);

    while (info->queueListen.next) {
        log = (TCPInfo*)info->queueListen.next;
        ASSERTLINE(330, log->listening == info);
        ASSERTLINE(331, log->state == TCP_STATE_LISTEN);
        log->err = info->err;
        TCPAbort(log);
    }

    if (info->listening) {
        IFQueueDequeueEntryLINK(TCPInfo*, &info->listening->queueListen, linkListen, info);
        info->listening = NULL;
        ASSERTLINE(342, state == TCP_STATE_LISTEN);
        IFQueueEnqueueHead(IPInfo*, &TCPInfoQueue, &info->pair);
    } else if (state == TCP_STATE_LISTEN) {
        info->openCallback = NullCallback;
    }

    if (info->flag & 1) {
        IFQueueDequeueEntry(IPInfo*, &TCPInfoQueue, &info->pair);
        info->pair.proto = 0;
    }

    callback = info->openCallback;
    if (callback) {
        if (info->openResult) {
            *info->openResult = info->err;
            info->openResult = NULL;
        }
        info->openCallback = NULL;
        callback(info, info->err);
    }
    callback = info->sendCallback;
    if (callback) {
        if (info->sendResult) {
            *info->sendResult = info->err;
            info->sendResult = NULL;
        }
        info->sendCallback = NULL;
        callback(info, info->err);
    }
    callback = info->recvCallback;
    if (callback) {
        if (info->recvResult) {
            *info->recvResult = info->err;
            info->recvResult = NULL;
        }
        info->recvCallback = NULL;
        callback(info, info->err);
    }
    callback = info->urgCallback;
    if (callback) {
        if (info->urgResult) {
            *info->urgResult = info->err;
            info->urgResult = NULL;
        }
        info->urgCallback = NULL;
        callback(info, info->err);
    }
    callback = info->closeCallback;
    if (callback) {
        if (info->closeResult) {
            *info->closeResult = 0;
            info->closeResult = NULL;
        }
        info->closeCallback = NULL;
        callback(info, 0);
    }
    return TRUE;
}

s32 TCPConnectAsync(TCPInfo* info /* r31 */, const IPSocket* socket /* r24 */, TCPCallback callback /* r28 */, s32* result /* r29 */) {
    // Local variables
    BOOL enabled; // r25
    IPHeader* header; // r27
    s32 rc; // r30
    IPInterface* interface; // r26

    // References
    // -> static unsigned short Port;
    // -> struct IFQueue TCPInfoQueue;

    enabled = OSDisableInterrupts();
    if (info->pair.proto != IP_PROTO_TCP || info->sendBuff == 0 || info->recvBuff == 0) {
        rc = -12;
    } else if (info->state != 0) {
        rc = -5;
    } else if ((rc = IPConnect(&TCPInfoQueue, &info->pair, socket, &Port)) == 0) {
        callback = callback ? callback : NullCallback;
        header = (IPHeader*)info->header;
        memmove(header->dst, info->pair.remote.addr, 4);
        memmove(header->src, info->pair.local.addr, 4);
        interface = IPGetRoute(socket->addr, NULL);
        ASSERTLINE(776, interface);
        info->mss = interface->mtu - 40;
        info->openCallback = callback;
        info->openResult = result;
        info->iss = TCPIsn(&info->pair);
        info->sendUna = info->iss;
        info->sendNext = info->iss;
        info->sendMax = info->iss;
        info->sendUp = info->sendUna;
        info->state = 2;
        if (result) {
            *result = IP_ERR_BUSY;
        }
        TCPOutput(info, 0);
    }
    if (result && rc < 0) {
        *result = rc;
    }
    OSRestoreInterrupts(enabled);
    return rc;
}

s32 TCPConnect(TCPInfo* info /* r29 */, const IPSocket* socket /* r1+0xC */) {
    // Local variables
    BOOL enabled; // r30
    s32 result; // r1+0x10
    s32 rc; // r31

    rc = TCPConnectAsync(info, socket, SyncCallback, &result);
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

static s32 SendAsync(TCPInfo* info /* r31 */, void* data /* r1+0xC */, s32 len /* r26 */, u32 flag /* r27 */, TCPCallback callback /* r28 */, s32* result /* r29 */) {
    // Local variables
    BOOL enabled; // r25
    s32 rc; // r30

    enabled = OSDisableInterrupts();
    if (info->pair.proto != IP_PROTO_TCP || len < 0) {
        rc = -12;
    } else if (info->sendCallback) {
        rc = IP_ERR_BUSY;
    } else if ((flag & 1) && len == 0) {
        rc = -12;
    } else {
        rc = 0;
        callback = callback ? callback : NullCallback;
        switch (info->state) {
            case 0:
                rc = -4;
                break;
            case 1:
                rc = -6;
                break;
            case 2:
            case 3:
            case 4:
            case 7:
                if (info->flag & 0x8) {
                    rc = -8;
                    break;
                }
                info->userAcked = 0;
                info->userSendData = data;
                info->userSendLen = len;
                info->sendCallback = callback;
                info->sendResult = result;
                if (result) {
                    *result = IP_ERR_BUSY;
                }
                TCPSendIn(info, flag & 4);
                if (flag & 1) {
                    info->sendUp = info->sendUna + info->sendLen + info->userSendLen;
                    if (info->state < 4 && info->iss == info->sendUna) {
                        info->sendUp++;
                    }
                }
                if (info->userSendLen <= 0) {
                    if (info->sendResult) {
                        ASSERTLINE(912, rc == info->userAcked);
                        *info->sendResult = info->userAcked;
                        info->sendResult = NULL;
                    }
                    info->sendCallback = NULL;
                    callback(info, info->userAcked);
                }
                if (info->state == 4 || info->state == 7) {
                    TCPOutput(info, 0);
                }
                break;
            case 5:
            case 6:
            case 8:
            case 9:
            case 10:
            default:
                rc = -8;
                break;
        }
    }
    if (result && rc < 0) {
        *result = rc;
    }
    OSRestoreInterrupts(enabled);
    return rc;
}

s32 TCPSendNonblock(TCPInfo* info /* r1+0x8 */, void* data /* r1+0xC */, s32 len /* r30 */) {
    // Local variables
    s32 result; // r1+0x14
    s32 rc; // r31

    rc = SendAsync(info, data, len, 4, NULL, &result);
    if (rc == 0) {
        rc = result;
    }
    return rc;
}

s32 TCPSendUrgNonblock(TCPInfo* info /* r1+0x8 */, void* data /* r1+0xC */, s32 len /* r30 */) {
    // Local variables
    s32 result; // r1+0x14
    s32 rc; // r31

    rc = SendAsync(info, data, len, 5, NULL, &result);
    if (rc == 0) {
        rc = result;
    }
    return rc;
}

static s32 Send(TCPInfo* info /* r29 */, void* data /* r1+0xC */, s32 len /* r1+0x10 */, u32 flag /* r1+0x14 */) {
    // Local variables
    BOOL enabled; // r30
    s32 result; // r1+0x18
    s32 rc; // r31

    rc = SendAsync(info, data, len, flag, SyncCallback, &result);
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

s32 TCPSend(TCPInfo* info /* r1+0x8 */, void* data /* r1+0xC */, s32 len /* r1+0x10 */) {
    return Send(info, data, len, 0);
}

s32 TCPSendUrg(TCPInfo* info /* r1+0x8 */, void* data /* r1+0xC */, s32 len /* r1+0x10 */) {
    return Send(info, data, len, 1);
}

s32 TCPReceiveExAsync(TCPInfo* info /* r30 */, void* data /* r24 */, s32 len /* r26 */, u32 flag /* r27 */, TCPCallback callback /* r28 */, s32* result /* r29 */) {
    // Local variables
    BOOL enabled; // r25
    int rc; // r31

    enabled = OSDisableInterrupts();
    if (info->pair.proto != IP_PROTO_TCP || len < 0) {
        rc = -12;
    } else if (info->recvCallback && !(flag & 4)) {
        rc = IP_ERR_BUSY;
    } else if (info->flag & 0x11) {
        rc = -8;
    } else {
        rc = 0;
        callback = callback ? callback : NullCallback;
        switch (info->state) {
            case 0:
                if (info->flag & 0x8) {
                    rc = 0;
                    if (result) {
                        *result = rc;
                    }
                    callback(info, rc);
                } else {
                    rc = -4;
                }
                break;
            case 1:
            case 2:
            case 3:
                if (len == 0) {
                    rc = 0;
                    if (result) {
                        *result = rc;
                    }
                    callback(info, rc);
                    break;
                }
                if (flag & 4) {
                    rc = -9;
                    break;
                }
                if (flag & 2) {
                    info->flag |= 0x400;
                } else {
                    info->flag &= ~0x400;
                }
                info->userData = data;
                info->userBuff = len;
                info->userLen = 0;
                info->recvCallback = callback;
                info->recvResult = result;
                if (result) {
                    *result = IP_ERR_BUSY;
                }
                break;
            case 4:
            case 5:
            case 6:
            case 7:
                if (len == 0) {
                    rc = 0;
                    if (result) {
                        *result = rc;
                    }
                    callback(info, rc);
                    break;
                }
                if (flag & 2) {
                    info->flag |= 0x400;
                } else {
                    info->flag &= ~0x400;
                }
                info->userData = data;
                info->userBuff = len;
                info->userLen = 0;
                rc = TCPRecvOut(info);
                if (0 < rc) {
                    info->userLen = 0;
                    if (result) {
                        *result = rc;
                    }
                    callback(info, rc);
                    TCPOutput(info, 0);
                    break;
                }
                if (info->state == 7 && info->recvUser == 0) {
                    rc = 0;
                    if (result) {
                        *result = rc;
                    }
                    callback(info, rc);
                    break;
                }
                if (flag & 4) {
                    rc = -9;
                    break;
                }
                rc = 0;
                info->recvCallback = callback;
                info->recvResult = result;
                if (result) {
                    *result = IP_ERR_BUSY;
                }
                break;
            case 8:
            case 9:
            case 10:
            default:
                rc = 0;
                if (result) {
                    *result = rc;
                }
                callback(info, rc);
                break;
        }
    }
    if (result && rc < 0) {
        *result = rc;
    }
    OSRestoreInterrupts(enabled);
    return rc;
}

s32 TCPReceiveEx(TCPInfo* info /* r29 */, void* data /* r1+0xC */, s32 len /* r1+0x10 */, u32 flag /* r1+0x14 */) {
    // Local variables
    BOOL enabled; // r30
    s32 result; // r1+0x18
    s32 rc; // r31

    rc = TCPReceiveExAsync(info, data, len, flag, SyncCallback, &result);
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

s32 TCPCloseAsync(TCPInfo* info /* r31 */, TCPCallback callback /* r28 */, s32* result /* r29 */) {
    // Local variables
    BOOL enabled; // r27
    s32 rc; // r30

    enabled = OSDisableInterrupts();
    callback = callback ? callback : NullCallback;
    if (info->pair.proto != IP_PROTO_TCP) {
        rc = -12;
    } else if (info->flag & 1) {
        rc = -8;
    } else if (info->recvUser > 0) {
        info->closeCallback = callback;
        info->closeResult = result;
        if (result) {
            *result = IP_ERR_BUSY;
        }
        rc = TCPCancel(info);
    } else {
        ASSERTLINE(1337, info->closeCallback == NULL);
        rc = 0;
        info->flag |= 0x19;
        info->closeCallback = callback;
        info->closeResult = result;
        if (result) {
            *result = IP_ERR_BUSY;
        }
        switch (info->state) {
            case 0:
            case 1:
            case 2:
                info->err = -8;
                TCPAbort(info);
                break;
            case 3:
                break;
            case 4:
                info->state = 5;
                TCPOutput(info, 0);
                break;
            case 5:
            case 6:
                break;
            case 7:
                info->state = 9;
                TCPOutput(info, 0);
                break;
            case 8:
            case 9:
                break;
            case 10:
            default:
                rc = -8;
                break;
        }
    }
    if (result && rc < 0) {
        *result = rc;
    }
    OSRestoreInterrupts(enabled);
    return rc;
}

s32 TCPShutdown(TCPInfo* info /* r31 */, u32 flag /* r25 */) {
    // Local variables
    BOOL enabled; // r26
    BOOL output; // r28
    s32 rcSend; // r29
    s32 rcRecv; // r27
    TCPCallback callback; // r30

    enabled = OSDisableInterrupts();
    if (info->pair.proto != IP_PROTO_TCP) {
        OSRestoreInterrupts(enabled);
        return -12;
    }

    output = FALSE;
    rcRecv = 0;
    rcSend = 0;
    if (flag == 0 || flag == 2) {
        if (info->recvUser > 0) {
            TCPCancel(info);
            OSRestoreInterrupts(enabled);
            return -8;
        }
        info->flag |= 0x10;
        switch (info->state) {
            case 0:
                rcRecv = -4;
                break;
            case 1:
            case 2:
            case 3:
                callback = info->recvCallback;
                if (callback) {
                    if (info->recvResult) {
                        *info->recvResult = -8;
                        info->recvResult = NULL;
                    }
                    info->recvCallback = NULL;
                    callback(info, -8);
                }
                callback = info->urgCallback;
                if (callback) {
                    if (info->urgResult) {
                        *info->urgResult = -8;
                        info->urgResult = NULL;
                    }
                    info->urgCallback = NULL;
                    callback(info, -8);
                }
                info->recvUser = 0;
                break;
            case 4:
            case 5:
            case 6:
            case 7:
                callback = info->recvCallback;
                if (callback) {
                    if (info->recvResult) {
                        *info->recvResult = -8;
                        info->recvResult = NULL;
                    }
                    info->recvCallback = NULL;
                    callback(info, -8);
                }
                callback = info->urgCallback;
                if (callback) {
                    if (info->urgResult) {
                        *info->urgResult = -8;
                        info->urgResult = NULL;
                    }
                    info->urgCallback = NULL;
                    callback(info, -8);
                }
                if (info->recvUser > 0) {
                    info->recvUser = 0;
                    output = TRUE;
                }
                break;
            case 8:
            case 9:
            case 10:
            default:
                rcRecv = -8;
                break;
        }
    }

    if (flag == 1 || flag == 2) {
        info->flag |= 0x8;
        switch (info->state) {
            case 0:
                rcSend = -4;
                break;
            case 1:
            case 2:
                info->err = -8;
                TCPAbort(info);
                break;
            case 3:
                break;
            case 4:
                info->state = 5;
                output = TRUE;
                break;
            case 5:
            case 6:
                rcSend = -8;
                break;
            case 7:
                info->state = 9;
                output = TRUE;
                break;
            case 8:
            case 9:
            case 10:
            default:
                rcSend = -8;
                break;
        }
    }

    if (output) {
        TCPOutput(info, 0);
    }
    OSRestoreInterrupts(enabled);
    return MIN(rcSend, rcRecv);
}

s32 TCPCancel(TCPInfo* info /* r31 */) {
    // Local variables
    BOOL enabled; // r29
    s32 rc; // r30

    enabled = OSDisableInterrupts();
    if (info->pair.proto != IP_PROTO_TCP) {
        rc = -12;
    } else {
        rc = 0;
        info->flag |= 0x19;
        info->err = -3;
        switch (info->state) {
            case 0:
            case 1:
            case 2:
            case 8:
            case 9:
            case 10:
                break;
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
                TCPRespond(info->interface, info->pair.remote.addr, info->pair.remote.port, info->pair.local.addr, info->pair.local.port, info->sendMax, 0, TCP_FLAG_RST, 0);
                break;
        }
        TCPAbort(info);
    }
    OSRestoreInterrupts(enabled);
    return rc;
}

s32 TCPReceiveUrgExAsync(TCPInfo* info /* r31 */, void* data /* r26 */, s32 len /* r1+0x10 */, u32 flag /* r27 */, TCPCallback callback /* r28 */, s32* result /* r29 */) {
    // Local variables
    BOOL enabled; // r25
    int rc; // r30

    enabled = OSDisableInterrupts();
    if (info->pair.proto != IP_PROTO_TCP || len < 1) {
        rc = -12;
    } else if (info->urgCallback) {
        rc = IP_ERR_BUSY;
    } else if (info->flag & 0x11) {
        rc = -8;
    } else {
        rc = 0;
        callback = callback ? callback : NullCallback;
        switch (info->state) {
            case 0:
                if (info->flag & 0x8) {
                    rc = 0;
                } else {
                    rc = -4;
                }
                break;
            case 1:
            case 2:
            case 3:
                if (flag & 4) {
                    rc = -12;
                    break;
                }
                if (flag & 2) {
                    info->flag |= 0x800;
                } else {
                    info->flag &= ~0x800;
                }
                info->urgData = data;
                info->urgCallback = callback;
                info->urgResult = result;
                if (result) {
                    *result = IP_ERR_BUSY;
                }
                break;
            case 4:
            case 5:
            case 6:
            case 7:
                if (flag & 2) {
                    info->flag |= 0x800;
                } else {
                    info->flag &= ~0x800;
                }
                info->urgData = data;
                if ((info->recvUrg > 0 || (info->flag & 0x20)) && !(info->flag & 0x40)) {
                    if (info->flag & 0x20) {
                        rc = 1;
                        if (data) {
                            *info->urgData = info->oob;
                        }
                        if (info->flag & 0x800) {
                            info->flag ^= 0x60;
                        }
                        if (result) {
                            *result = rc;
                        }
                        callback(info, rc);
                    } else {
                        rc = -9;
                    }
                    break;
                }
                if (info->state == 7) {
                    rc = 0;
                    if (result) {
                        *result = rc;
                    }
                    callback(info, rc);
                    break;
                }
                if (flag & 4) {
                    rc = -12;
                    break;
                }
                rc = 0;
                info->urgCallback = callback;
                info->urgResult = result;
                if (result) {
                    *result = IP_ERR_BUSY;
                }
                break;
            case 8:
            case 9:
            case 10:
            default:
                rc = 0;
                if (result) {
                    *result = rc;
                }
                callback(info, rc);
                break;
        }
    }
    if (result && rc < 0) {
        *result = rc;
    }
    OSRestoreInterrupts(enabled);
    return rc;
}

s32 TCPReceiveUrgEx(TCPInfo* info /* r29 */, void* data /* r1+0xC */, s32 len /* r1+0x10 */, u32 flags /* r1+0x14 */) {
    // Local variables
    BOOL enabled; // r30
    s32 result; // r1+0x18
    s32 rc; // r31

    rc = TCPReceiveUrgExAsync(info, data, len, flags, SyncCallback, &result);
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

BOOL TCPOnReset(BOOL) {
    // Local variables
    TCPInfo* info; // r31

    // References
    // -> struct IFQueue TCPInfoQueue;

    if (IFIsEmptyQueue(&TCPInfoQueue)) {
        return TRUE;
    }
    while (!IFIsEmptyQueue(&TCPInfoQueue)) {
        info = (TCPInfo*)TCPInfoQueue.next;
        TCPCancel(info);
    }
    return FALSE;
}
