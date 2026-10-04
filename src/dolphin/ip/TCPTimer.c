/* Reviewed fragment of IPTcpTimer.c, adapted for the older target behavior.
 * Provenance, scope and ABI evidence are recorded in docs/Network.md.
 */
#include <dolphin/ip.h>
#include <dolphin/private/ip.h>

static void TCPRxmitTimeOut(OSAlarm* alarm, OSContext*) {

    TCPInfo* info;
    IPHeader* header;
    OSTime r2;

    TCPStat.rxmitTimeout++;
    info = (TCPInfo*)((u8*)alarm - offsetof(TCPInfo, rxmitAlarm));
    info->rxmitCount++;
    if (!(info->flag & 0x200)) {
        if (info->rxmitCount == 3) {
            IPRecoverGateway(info->datagram.dst);
        }
        if (info->rxmitCount == 4 && info->state >= 4) {
            header = (IPHeader*)info->header;
            header->frag &= ~0x4000;
            info->mss = (info->mss > 536) ? 536 : info->mss;
        }

    }
    if (info->rxmitCount >= 16) {
        info->rxmitCount = 16;
    }
    if (info->rxmitCount > 3) {
        switch (info->state) {
            case 2:
            case 3:
            r2 = OSSecondsToTicks((OSTime)180);
            if (info->iss == info->sendUna && info->r2 < r2) {
                break;
            }
            default:
            r2 = info->r2;
            break;
        }

        if (OSGetTime() - info->r0 >= r2) {
            if (info->err == 0) {
                info->err = -10;
            }
            TCPAbort(info);
            return;
        }
    }

    info->sendNext = info->sendUna;
    info->rttTiming = FALSE;
    info->ssThresh = ((info->cWin < info->sendWin) ? info->cWin : info->sendWin) / 2;
    info->ssThresh = (info->ssThresh > 2 * info->mss) ? info->ssThresh : 2 * info->mss;
    info->cWin = info->mss;
    TCPOutput(info, TCP_FLAG_ACK);
}

void TCPStartRxmitTimer(TCPInfo* info) {
    OSTime rto;

    if (info->rxmitAlarm.handler == (OSAlarmHandler)NULL) {
        rto = (1 << info->rxmitCount) * info->rto;
        if (OSSecondsToTicks((OSTime)240) < rto) {
            rto = OSSecondsToTicks((OSTime)240);
        }
        if (info->rxmitCount == 0) {
            info->r0 = OSGetTime();
        }
        OSSetAlarm(&info->rxmitAlarm, rto, TCPRxmitTimeOut);
    }
}

void TCPCancelRxmitTimer(TCPInfo* info) {
    info->flag &= ~0x200;
    info->rxmitCount = 0;
    OSCancelAlarm(&info->rxmitAlarm);
}
