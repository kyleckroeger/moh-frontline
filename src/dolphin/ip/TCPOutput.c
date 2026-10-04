/* Reviewed fragment of IPTcpOutput.c, adapted for the older target behavior.
 * Provenance, scope and ABI evidence are recorded in docs/Network.md.
 */
#include <dolphin/ip.h>
#include <dolphin/private/ip.h>

#ifdef NULL
#undef NULL
#endif

#define NULL 0

#define TCP_RXMIT_THRESH 3
#define TCP_FLAG_793 (TCP_FLAG_FIN | TCP_FLAG_SYN | TCP_FLAG_RST | TCP_FLAG_PSH | TCP_FLAG_ACK | TCP_FLAG_URG)

#define SEQ_LT(a, b) ((s32)((a) - (b)) < 0)
#define SEQ_LEQ(a, b) ((s32)((a) - (b)) <= 0)
#define SEQ_GT(a, b) ((s32)((a) - (b)) > 0)
#define SEQ_GEQ(a, b) ((s32)((a) - (b)) >= 0)

static void TCPOutputCallback(TCPInfo* info, s32 result);

int TCPMakeOption(TCPHeader* tcp, TCPInfo* info, u16 flag) {
    int optlen = 0;
    u8* opt;
    opt = (u8*)tcp + TCP_MIN_HLEN;
    if (flag & TCP_FLAG_SYN) {
        opt[0] = TCP_OPT_MSS;
        opt[1] = 4;
        *(u16*)(opt + 2) = (u16)info->mss;
        optlen = 4;
    }
    tcp->flag &= ~0xF000;
    tcp->flag |= (TCP_MIN_HLEN + optlen) << 10;
    return optlen;
}

static s32 TCPCalcSendSize(TCPInfo* info, s32 effSendMss, u16* pflag) {

    u16 flag;
    s32 win;
    s32 useable;
    s32 offset;
    s32 dataLen;
    s32 end;
    s32 sendSize;
    s32 len;

    flag = *pflag;
    ASSERTLINE(238, 0 <= info->sendLen);

    dataLen = 0;
    switch (info->state) {
        case 2:
        case 3:
        if (info->iss == info->sendNext) {
            ASSERTLINE(249, info->iss == info->sendUna);
            flag |= TCP_FLAG_SYN;
            dataLen = 1;
        } else if (info->sendUna == info->iss) {
            offset = info->sendNext - info->iss - 1;
            break;
        }
        default:
        offset = info->sendNext - info->sendUna;
        break;
    }

    dataLen += info->sendLen;
    if (info->sendCallback && info->userSendLen > 0) {
        dataLen += info->userSendLen;
    }
    end = info->sendUna + dataLen;
    dataLen -= offset;

    switch (info->state) {
        case 5:
        case 8:
        case 9:
        if (0 <= dataLen) {
            flag |= TCP_FLAG_FIN;
            dataLen++;
            end++;
        }
        break;
    }

    if (flag & TCP_FLAG_SYN) {
        flag &= ~TCP_FLAG_FIN;
        offset = 0;
        dataLen = 1;
        end = info->sendUna + dataLen;
    }

    win = MIN(info->sendWin, info->cWin);
    if (win == 0 && (SEQ_GT(end, info->sendUna) || (flag & (TCP_FLAG_FIN | TCP_FLAG_SYN))) && info->rxmitAlarm.handler == NULL) {
        flag |= TCP_FLAG_ACK;
        info->flag |= 0x200;
        win = 1;
        info->sendNext = info->sendUna;
    }

    useable = info->sendUna + win - info->sendNext;
    info->datagram.nVec = 1;

    sendSize = MIN(effSendMss, MIN(dataLen, useable));
    if (sendSize == dataLen && sendSize > 0 && !(flag & (TCP_FLAG_FIN | TCP_FLAG_SYN | TCP_FLAG_RST))) {
        flag |= TCP_FLAG_PSH;
    }
    if ((flag & TCP_FLAG_FIN) && SEQ_GT(end, info->sendNext + sendSize)) {
        flag &= ~TCP_FLAG_FIN;
    }

    if (sendSize <= 0) {
        *pflag &= ~(TCP_FLAG_FIN | TCP_FLAG_SYN);
        return 0;
    }

    dataLen = sendSize;
    if (flag & TCP_FLAG_SYN) {
        dataLen--;
        ASSERTLINE(416, offset == 0);
    }
    if (flag & TCP_FLAG_FIN) {
        dataLen--;
    }

    if (dataLen > 0) {
        if (info->sendLen > 0 && offset < info->sendLen) {
            len = info->sendLen - offset;
            len = MIN(len, dataLen);
            info->datagram.nVec += IFRingGet(info->sendData, info->sendBuff, info->sendPtr + offset, info->sendLen, &info->datagram.vec[1], len);
            dataLen -= len;
            offset += len;
        }

        if (dataLen > 0) {
            ASSERTLINE(438, info->sendCallback && 0 < info->userSendLen);
            offset -= info->sendLen;
            ASSERTLINE(440, offset + dataLen <= info->userSendLen);
            info->datagram.vec[info->datagram.nVec].data = info->userSendData + offset;
            info->datagram.vec[info->datagram.nVec].len = dataLen;
            info->datagram.nVec++;
        }
    }

    *pflag = flag;
    return sendSize;
}

static BOOL DoSwsAvoidance(TCPInfo* info, s32 sendSize, s32 effSendMss, u16 flag) {

    BOOL acked;
    s32 win;
    s32 reduction;

    reduction = MIN(info->recvBuff / 2, info->mss);
    if (reduction <= info->recvBuff - info->recvUser - info->recvWin) {
        info->recvWin = info->recvBuff - info->recvUser;
        return FALSE;
    }

    if (flag & (TCP_FLAG_FIN | TCP_FLAG_RST | TCP_FLAG_ACK)) {
        return FALSE;
    }
    if (sendSize <= 0) {
        return TRUE;
    }
    if (effSendMss <= sendSize) {
        return FALSE;
    }

    if (info->flag & 0x2) {
        acked = (info->sendNext == info->sendUna);
    } else {
        acked = TRUE;
    }
    if (acked && (flag & TCP_FLAG_PSH)) {
        return FALSE;
    }
    if (acked && info->sendMaxWin / 2 <= sendSize) {
        return FALSE;
    }

    if (SEQ_GT(info->sendMax, info->sendNext)) {
        return FALSE;
    }
    if (SEQ_GT(info->sendUp, info->sendUna)) {
        return FALSE;
    }
    return TRUE;
}

static void TCPOutputCallback(TCPInfo* info, s32 result) {
    ASSERTLINE(568, info->datagram.interface == NULL);
    ASSERTLINE(569, info->datagram.queue == NULL);
    info->sendBusy = FALSE;
    if (result < 0) {
        info->err = result;
    }
    TCPOutput(info, 0);
}

void TCPOutput(TCPInfo* info, u16 flag) {
    IPHeader* ip;
    TCPHeader* tcp;
    int optlen;
    s32 sendSize;
    s32 result;
    IFVec* vec;
    s32 offset;

    if (info->state == 0) {
        return;
    }
    if (info->sendBusy == TRUE) {
        TCPStartRxmitTimer(info);
        return;
    }

    switch (info->state) {
        case 2:
        flag &= ~TCP_FLAG_ACK;
        break;
        case 3:
        if (SEQ_GT(info->irs + 1, info->recvAcked)) {
            flag |= TCP_FLAG_ACK;
        }
        break;
        default:
        if (SEQ_GT(info->recvNext, info->recvAcked)) {
            flag |= TCP_FLAG_ACK;
        }
        break;
    }

    switch (info->state) {
        case 2:
        case 3:
        if (info->iss == info->sendNext) {
            flag |= TCP_FLAG_SYN;
        }
        break;
    }

    ip = (IPHeader*)info->header;
    ip->len = IP_HLEN(ip);
    tcp = (TCPHeader*)((u32)info->header + IP_HLEN(ip));
    tcp->flag = 0;
    optlen = TCPMakeOption(tcp, info, flag);
    sendSize = TCPCalcSendSize(info, info->mss - optlen, &flag);

    if (DoSwsAvoidance(info, sendSize, info->mss - optlen, flag)) {
        if (sendSize > 0) {
            TCPStartRxmitTimer(info);
        }
        return;
    }

    if (info->state != 3) {
        tcp->ack = info->recvAcked = info->recvNext;
    } else {
        tcp->ack = info->recvAcked = info->irs + 1;
    }
    tcp->src = info->pair.local.port;
    tcp->dst = info->pair.remote.port;
    if (sendSize > 0) {
        tcp->seq = info->sendNext;
    } else {
        tcp->seq = info->sendMax;
    }

    if (SEQ_GT(info->sendUp, tcp->seq)) {
        flag |= TCP_FLAG_URG;
        offset = info->sendUp - tcp->seq;
        tcp->urg = (u16)((65535 - IP_MIN_HLEN - TCP_MIN_HLEN < offset) ? 65535 : offset);
    } else {
        tcp->urg = 0;
        info->sendUp = info->sendUna;
    }

    if (info->state != 2) {
        flag |= TCP_FLAG_ACK;
    }
    tcp->flag &= ~TCP_FLAG_793;
    tcp->flag |= flag & TCP_FLAG_793;
    tcp->win = (u16)info->recvWin;
    tcp->sum = 0;

    ip->len += TCP_HLEN(tcp);
    for (vec = &info->datagram.vec[1]; vec < &info->datagram.vec[info->datagram.nVec]; vec++) {
        ip->len += (u16)vec->len;
    }
    ASSERTLINE(789, IP_MIN_HLEN + TCP_HLEN(tcp) <= ip->len);
    ASSERTLINE(790, IP_HLEN(ip) + TCP_HLEN(tcp) <= ip->len);

    info->sendNext += sendSize;
    if (SEQ_GT(info->sendNext, info->sendMax)) {
        info->sendMax = info->sendNext;
        if (!info->rttTiming) {
            info->rttTiming = TRUE;
            info->rttSeq = tcp->seq;
            info->rtt = OSGetTime();
        }
    } else if (sendSize > 0) {
        TCPStat.rxmitPackets++;
        TCPStat.rxmitBytes += ip->len - (IP_HLEN(ip) + TCP_HLEN(tcp));
    }

    if (info->sendNext != info->sendUna) {
        TCPStartRxmitTimer(info);
    }

    info->sendBusy = TRUE;
    info->datagram.vec[0].data = ip;
    info->datagram.vec[0].len = IP_HLEN(ip) + TCP_HLEN(tcp);
    info->datagram.callback = (void (*)(void*, s32))TCPOutputCallback;
    info->datagram.param = info;
    result = IPOut(&info->datagram);
    if (result < 0) {
        TCPOutputCallback(info, result);
        return;
    }
    TCPStat.sendTotal++;
    info->flag &= ~0x4;
}
