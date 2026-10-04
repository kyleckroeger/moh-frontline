/* Reviewed fragment of IPPPP.c, adapted for the older target behavior.
 * Provenance, scope and ABI evidence are recorded in docs/Network.md.
 */
#include <dolphin/ip.h>
#include <dolphin/private/ip.h>

#ifdef NULL
#undef NULL
#endif
#define NULL 0

static void PPPTimeout(OSAlarm* alarm, OSContext* context);

int PPPLayerUp(PPPConf* conf) {
    return conf->up(conf);
}

int PPPLayerDown(PPPConf* conf) {
    OSCancelAlarm(&conf->alarm);
    return conf->down(conf);
}

int PPPLayerStarted(PPPConf* conf) {
    return conf->started(conf);
}

int PPPLayerFinished(PPPConf* conf) {
    OSCancelAlarm(&conf->alarm);
    return conf->finished(conf);
}

void PPPInitializeRestartCount(PPPConf* conf) {
    conf->rxmit = 0;
    conf->configure = 10;
    conf->terminate = 2;
    conf->failure = 5;
    conf->id++;
}

void PPPZeroRestartCount(PPPConf* conf) {
    conf->rxmit = 0;
    conf->configure = 0;
    conf->terminate = 0;
    conf->failure = 0;
}

void PPPSetState(PPPConf* conf, int state) {
    conf->state = state;
}

int PPPGetState(PPPConf* conf) {
    return conf->state;
}

static void LCPOut(PPPConf* conf, u8 code, u8 id, s32 len, void* data) {

    IPInterface* interface;
    IFDatagram* datagram;
    u16* proto;
    LCPHeader* lcp;
    BOOL enabled;
    PPPConf* lcpConf;

    enabled = OSDisableInterrupts();
    interface = conf->interface;
    lcpConf = (PPPConf*)interface->ppp.next;
    if (code == 1 || code == 5) {
        OSCancelAlarm(&conf->alarm);
        OSSetAlarm(&conf->alarm, OSSecondsToTicks((OSTime)3), PPPTimeout);
    }

    if (lcpConf->mru < len + 4) {
        len = lcpConf->mru - sizeof(LCPHeader);
    }

    datagram = (IFDatagram*)interface->alloc(interface, sizeof(IFDatagram) + sizeof(u16) + sizeof(LCPHeader) + len);
    if (datagram != NULL) {
        proto = (u16*)(datagram + 1);
        *proto = conf->protocol;
        lcp = (LCPHeader*)(proto + 1);
        lcp->code = code;
        lcp->id = id;
        lcp->len = len + sizeof(LCPHeader);
        memmove(lcp + 1, data, len);
        datagram->nVec = 1;
        datagram->vec[0].data = proto;
        datagram->vec[0].len = len + sizeof(u16) + sizeof(LCPHeader);
        datagram->callback = NULL;
        datagram->param = NULL;
        datagram->type = 0x8864;
        datagram->queue = NULL;
        interface->out(interface, datagram);
    }

    OSRestoreInterrupts(enabled);
}

static int SendConfigureRequest(PPPConf* conf) {
    if (0 < conf->configure) {
        conf->configure--;
        LCPOut(conf, 1, conf->id, conf->len, conf->data);
        return TRUE;
    }
    return FALSE;
}

static void SendConfigureAck(PPPConf* conf, LCPHeader* lcp) {
    conf->failure = 5;
    LCPOut(conf, 2, lcp->id, lcp->len - sizeof(LCPHeader), lcp + 1);
}

static void SendConfigureNak(PPPConf* conf, LCPHeader* lcp) {
    ASSERTLINE(370, 0 < conf->failure);
    conf->failure--;
    LCPOut(conf, 3, lcp->id, lcp->len - sizeof(LCPHeader), lcp + 1);
}

static void SendConfigureReject(PPPConf* conf, LCPHeader* lcp) {
    LCPOut(conf, 4, lcp->id, lcp->len - sizeof(LCPHeader), lcp + 1);
}

static int SendTerminateRequest(PPPConf* conf) {
    if (0 < conf->terminate) {
        conf->terminate--;
        LCPOut(conf, 5, conf->id, 0, NULL);
        return TRUE;
    }
    return FALSE;
}

static void SendTerminateAck(PPPConf* conf) {
    LCPOut(conf, 6, conf->idTerminate, 0, NULL);
}

static void SendCodeReject(PPPConf* conf, LCPHeader* lcp) {
    conf->idReject++;
    LCPOut(conf, 7, conf->idReject, lcp->len, lcp);
}

static void SendProtocolReject(PPPConf* conf, LCPHeader* lcp) {
    conf->idReject++;
    LCPOut(conf, 8, conf->idReject, lcp->len, lcp);
}

static void SendEchoReply(PPPConf* conf, LCPHeader* lcp) {
    LCPOut(conf, 10, conf->idEcho, lcp->len - sizeof(LCPHeader), lcp + 1);
}

static void ReceiveConfigureRequest(PPPConf* conf, LCPHeader* lcp) {

    int rc;
    u8* data;

    data = (u8*)(lcp + 1);
    rc = conf->receiveConfigureRequest(conf, lcp);
    switch (rc) {
        case 0:
        switch (conf->state) {
            case PPP_STATE_CLOSED:
            SendTerminateAck(conf);
            break;
            case PPP_STATE_STOPPED:
            PPPInitializeRestartCount(conf);
            SendConfigureRequest(conf);
            SendConfigureAck(conf, lcp);
            PPPSetState(conf, PPP_STATE_ACK_SENT);
            break;
            case PPP_STATE_REQ_SENT:
            SendConfigureAck(conf, lcp);
            PPPSetState(conf, PPP_STATE_ACK_SENT);
            break;
            case PPP_STATE_ACK_RCVD:
            SendConfigureAck(conf, lcp);
            PPPLayerUp(conf);
            PPPSetState(conf, PPP_STATE_OPENED);
            break;
            case PPP_STATE_ACK_SENT:
            SendConfigureAck(conf, lcp);
            break;
            case PPP_STATE_OPENED:
            PPPLayerDown(conf);
            SendConfigureRequest(conf);
            SendConfigureAck(conf, lcp);
            PPPSetState(conf, PPP_STATE_ACK_SENT);
            break;
        }
        break;
        case -1:
        switch (conf->state) {
            case PPP_STATE_CLOSED:
            SendTerminateAck(conf);
            break;
            case PPP_STATE_STOPPED:
            PPPInitializeRestartCount(conf);
            SendConfigureRequest(conf);
            SendConfigureReject(conf, lcp);
            PPPSetState(conf, PPP_STATE_REQ_SENT);
            break;
            case PPP_STATE_REQ_SENT:
            SendConfigureReject(conf, lcp);
            break;
            case PPP_STATE_ACK_RCVD:
            SendConfigureReject(conf, lcp);
            break;
            case PPP_STATE_ACK_SENT:
            SendConfigureReject(conf, lcp);
            PPPSetState(conf, PPP_STATE_REQ_SENT);
            break;
            case PPP_STATE_OPENED:
            PPPLayerDown(conf);
            SendConfigureRequest(conf);
            SendConfigureReject(conf, lcp);
            PPPSetState(conf, PPP_STATE_REQ_SENT);
            break;
        }
        break;
        case -2:
        switch (conf->state) {
            case PPP_STATE_CLOSED:
            SendTerminateAck(conf);
            break;
            case PPP_STATE_STOPPED:
            PPPInitializeRestartCount(conf);
            SendConfigureRequest(conf);
            SendConfigureNak(conf, lcp);
            PPPSetState(conf, PPP_STATE_REQ_SENT);
            break;
            case PPP_STATE_REQ_SENT:
            SendConfigureNak(conf, lcp);
            break;
            case PPP_STATE_ACK_RCVD:
            SendConfigureNak(conf, lcp);
            break;
            case PPP_STATE_ACK_SENT:
            SendConfigureNak(conf, lcp);
            PPPSetState(conf, PPP_STATE_REQ_SENT);
            break;
            case PPP_STATE_OPENED:
            PPPLayerDown(conf);
            SendConfigureRequest(conf);
            SendConfigureNak(conf, lcp);
            PPPSetState(conf, PPP_STATE_REQ_SENT);
            break;
        }
        break;
        case -3:
        return;
    }
}

static void ReceiveConfigureAck(PPPConf* conf, LCPHeader* lcp) {

    int rc;

    rc = conf->receiveConfigureAck(conf, lcp);
    if (rc) {
        switch (conf->state) {
            case PPP_STATE_CLOSED:
            SendTerminateAck(conf);
            break;
            case PPP_STATE_STOPPED:
            SendTerminateAck(conf);
            break;
            case PPP_STATE_REQ_SENT:
            PPPInitializeRestartCount(conf);
            PPPSetState(conf, PPP_STATE_ACK_RCVD);
            break;
            case PPP_STATE_ACK_RCVD:
            SendConfigureRequest(conf);
            break;
            case PPP_STATE_ACK_SENT:
            PPPInitializeRestartCount(conf);
            PPPLayerUp(conf);
            PPPSetState(conf, PPP_STATE_OPENED);
            break;
            case PPP_STATE_OPENED:
            SendConfigureRequest(conf);
            PPPSetState(conf, PPP_STATE_REQ_SENT);
            break;
        }
    }
}

static void ReceiveConfigureNak(PPPConf* conf, LCPHeader* lcp) {

    int rc;

    rc = conf->receiveConfigureNak(conf, lcp);
    if (rc) {
        switch (conf->state) {
            case PPP_STATE_CLOSED:
            SendTerminateAck(conf);
            break;
            case PPP_STATE_STOPPED:
            SendTerminateAck(conf);
            break;
            case PPP_STATE_REQ_SENT:
            PPPInitializeRestartCount(conf);
            SendConfigureRequest(conf);
            break;
            case PPP_STATE_ACK_RCVD:
            SendConfigureRequest(conf);
            break;
            case PPP_STATE_ACK_SENT:
            PPPInitializeRestartCount(conf);
            SendConfigureRequest(conf);
            break;
            case PPP_STATE_OPENED:
            PPPLayerDown(conf);
            SendConfigureRequest(conf);
            PPPSetState(conf, PPP_STATE_REQ_SENT);
            break;
        }
    }
}

static void ReceiveConfigureReject(PPPConf* conf, LCPHeader* lcp) {

    int rc;

    rc = conf->receiveConfigureReject(conf, lcp);
    if (rc) {
        switch (conf->state) {
            case PPP_STATE_CLOSED:
            SendTerminateAck(conf);
            break;
            case PPP_STATE_STOPPED:
            SendTerminateAck(conf);
            break;
            case PPP_STATE_REQ_SENT:
            PPPInitializeRestartCount(conf);
            SendConfigureRequest(conf);
            break;
            case PPP_STATE_ACK_RCVD:
            SendConfigureRequest(conf);
            break;
            case PPP_STATE_ACK_SENT:
            PPPInitializeRestartCount(conf);
            SendConfigureRequest(conf);
            break;
            case PPP_STATE_OPENED:
            PPPLayerDown(conf);
            SendConfigureRequest(conf);
            PPPSetState(conf, PPP_STATE_REQ_SENT);
            break;
        }
    }
}

static void ReceiveTerminateRequest(PPPConf* conf, LCPHeader* lcp) {
    conf->idTerminate = lcp->id;
    switch (conf->state) {
        case PPP_STATE_CLOSED:
        SendTerminateAck(conf);
        break;
        case PPP_STATE_STOPPED:
        SendTerminateAck(conf);
        break;
        case PPP_STATE_CLOSING:
        SendTerminateAck(conf);
        break;
        case PPP_STATE_STOPPING:
        SendTerminateAck(conf);
        break;
        case PPP_STATE_REQ_SENT:
        SendTerminateAck(conf);
        break;
        case PPP_STATE_ACK_RCVD:
        SendTerminateAck(conf);
        PPPSetState(conf, PPP_STATE_REQ_SENT);
        break;
        case PPP_STATE_ACK_SENT:
        SendTerminateAck(conf);
        PPPSetState(conf, PPP_STATE_REQ_SENT);
        break;
        case PPP_STATE_OPENED:
        PPPLayerDown(conf);
        PPPZeroRestartCount(conf);
        SendTerminateAck(conf);
        PPPSetState(conf, PPP_STATE_STOPPING);
        break;
    }
}

static void ReceiveTerminateAck(PPPConf* conf, LCPHeader* lcp) {
    switch (conf->state) {
        case PPP_STATE_CLOSING:
        PPPLayerFinished(conf);
        PPPSetState(conf, PPP_STATE_CLOSED);
        break;
        case PPP_STATE_STOPPING:
        PPPLayerFinished(conf);
        PPPSetState(conf, PPP_STATE_STOPPED);
        break;
        case PPP_STATE_ACK_RCVD:
        PPPSetState(conf, PPP_STATE_REQ_SENT);
        break;
        case PPP_STATE_OPENED:
        PPPLayerDown(conf);
        SendConfigureRequest(conf);
        PPPSetState(conf, PPP_STATE_REQ_SENT);
        break;
        case PPP_STATE_REQ_SENT:
        (void)0;
        break;
    }
}

static void ReceiveUnknownCode(PPPConf* conf, LCPHeader* lcp) {
    switch (conf->state) {
        case PPP_STATE_CLOSED:
        SendCodeReject(conf, lcp);
        break;
        case PPP_STATE_STOPPED:
        SendCodeReject(conf, lcp);
        break;
        case PPP_STATE_CLOSING:
        SendCodeReject(conf, lcp);
        break;
        case PPP_STATE_STOPPING:
        SendCodeReject(conf, lcp);
        break;
        case PPP_STATE_REQ_SENT:
        SendCodeReject(conf, lcp);
        break;
        case PPP_STATE_ACK_RCVD:
        SendCodeReject(conf, lcp);
        break;
        case PPP_STATE_ACK_SENT:
        SendCodeReject(conf, lcp);
        break;
        case PPP_STATE_OPENED:
        SendCodeReject(conf, lcp);
        break;
    }
}

static void ReceiveCodeReject(PPPConf* conf, LCPHeader* lcp) {

    int plus;

    plus = FALSE;
    if (plus) {
        switch (conf->state) {
            case PPP_STATE_ACK_RCVD:
            PPPSetState(conf, PPP_STATE_REQ_SENT);
            break;
        }
    } else {
        switch (conf->state) {
            case PPP_STATE_CLOSED:
            PPPLayerFinished(conf);
            break;
            case PPP_STATE_STOPPED:
            PPPLayerFinished(conf);
            break;
            case PPP_STATE_CLOSING:
            PPPLayerFinished(conf);
            PPPSetState(conf, PPP_STATE_CLOSED);
            break;
            case PPP_STATE_STOPPING:
            PPPLayerFinished(conf);
            PPPSetState(conf, PPP_STATE_STOPPED);
            break;
            case PPP_STATE_REQ_SENT:
            PPPLayerFinished(conf);
            PPPSetState(conf, PPP_STATE_STOPPED);
            break;
            case PPP_STATE_ACK_RCVD:
            PPPLayerFinished(conf);
            PPPSetState(conf, PPP_STATE_STOPPED);
            break;
            case PPP_STATE_ACK_SENT:
            PPPLayerFinished(conf);
            PPPSetState(conf, PPP_STATE_STOPPED);
            break;
            case PPP_STATE_OPENED:
            PPPLayerDown(conf);
            PPPInitializeRestartCount(conf);
            PPPSetState(conf, PPP_STATE_STOPPING);
            break;
        }
    }
}

static void ReceiveProtocolReject(PPPConf* conf, LCPHeader* lcp) {

    int plus;

    plus = FALSE;
    if (plus) {
        switch (conf->state) {
            case PPP_STATE_ACK_RCVD:
            PPPSetState(conf, PPP_STATE_REQ_SENT);
            break;
        }
    } else {
        switch (conf->state) {
            case PPP_STATE_CLOSED:
            PPPLayerFinished(conf);
            break;
            case PPP_STATE_STOPPED:
            PPPLayerFinished(conf);
            break;
            case PPP_STATE_CLOSING:
            PPPLayerFinished(conf);
            PPPSetState(conf, PPP_STATE_CLOSED);
            break;
            case PPP_STATE_STOPPING:
            PPPLayerFinished(conf);
            PPPSetState(conf, PPP_STATE_STOPPED);
            break;
            case PPP_STATE_REQ_SENT:
            PPPLayerFinished(conf);
            PPPSetState(conf, PPP_STATE_STOPPED);
            break;
            case PPP_STATE_ACK_RCVD:
            PPPLayerFinished(conf);
            PPPSetState(conf, PPP_STATE_STOPPED);
            break;
            case PPP_STATE_ACK_SENT:
            PPPLayerFinished(conf);
            PPPSetState(conf, PPP_STATE_STOPPED);
            break;
            case PPP_STATE_OPENED:
            PPPLayerDown(conf);
            PPPInitializeRestartCount(conf);
            PPPSetState(conf, PPP_STATE_STOPPING);
            break;
        }
    }
}

static void ReceiveEchoRequest(PPPConf* conf, LCPHeader* lcp) {
    conf->idEcho = lcp->id;
    switch (conf->state) {
        case PPP_STATE_OPENED:
        SendEchoReply(conf, lcp);
        break;
    }
}

static void ControlIn(PPPConf* conf, LCPHeader* lcp, s32 len, u32 flag) {
    if (conf == NULL || len < 4 || len < lcp->len || lcp->len < sizeof(LCPHeader)) {
        return;
    }

    switch (lcp->code) {
        case 1:
        ReceiveConfigureRequest(conf, lcp);
        break;
        case 2:
        if (lcp->id == conf->id) {
            OSCancelAlarm(&conf->alarm);
            ReceiveConfigureAck(conf, lcp);
        }
        break;
        case 3:
        if (lcp->id == conf->id) {
            OSCancelAlarm(&conf->alarm);
            ReceiveConfigureNak(conf, lcp);
        }
        break;
        case 4:
        if (lcp->id == conf->id) {
            OSCancelAlarm(&conf->alarm);
            ReceiveConfigureReject(conf, lcp);
        }
        break;
        case 5:
        ReceiveTerminateRequest(conf, lcp);
        break;
        case 6:
        if (lcp->id == conf->id) {
            OSCancelAlarm(&conf->alarm);
            ReceiveTerminateAck(conf, lcp);
        }
        break;
        case 7:
        ReceiveCodeReject(conf, lcp);
        break;
        case 8:
        ReceiveProtocolReject(conf, lcp);
        break;
        case 9:
        ReceiveEchoRequest(conf, lcp);
        break;
        case 10:
        case 11:
        break;
        default:
        ReceiveUnknownCode(conf, lcp);
        break;
    }
}

void PPPDown(PPPConf* conf) {

    switch (conf->state) {
        case PPP_STATE_CLOSED:
        PPPSetState(conf, PPP_STATE_INITIAL);
        break;
        case PPP_STATE_STOPPED:
        PPPLayerStarted(conf);
        PPPSetState(conf, PPP_STATE_STARTING);
        break;
        case PPP_STATE_CLOSING:
        PPPSetState(conf, PPP_STATE_INITIAL);
        break;
        case PPP_STATE_STOPPING:
        PPPSetState(conf, PPP_STATE_STARTING);
        break;
        case PPP_STATE_REQ_SENT:
        PPPSetState(conf, PPP_STATE_STARTING);
        break;
        case PPP_STATE_ACK_RCVD:
        PPPSetState(conf, PPP_STATE_STARTING);
        break;
        case PPP_STATE_ACK_SENT:
        PPPSetState(conf, PPP_STATE_STARTING);
        break;
        case PPP_STATE_OPENED:
        PPPLayerDown(conf);
        PPPSetState(conf, PPP_STATE_STARTING);
        break;
    }

}

void PPPClose(PPPConf* conf) {

    switch (conf->state) {
        case PPP_STATE_STARTING:
        PPPLayerFinished(conf);
        PPPSetState(conf, PPP_STATE_INITIAL);
        break;
        case PPP_STATE_STOPPED:
        PPPSetState(conf, PPP_STATE_CLOSED);
        break;
        case PPP_STATE_STOPPING:
        PPPSetState(conf, PPP_STATE_CLOSING);
        break;
        case PPP_STATE_REQ_SENT:
        PPPInitializeRestartCount(conf);
        SendTerminateRequest(conf);
        PPPSetState(conf, PPP_STATE_CLOSING);
        break;
        case PPP_STATE_ACK_RCVD:
        PPPInitializeRestartCount(conf);
        SendTerminateRequest(conf);
        PPPSetState(conf, PPP_STATE_CLOSING);
        break;
        case PPP_STATE_ACK_SENT:
        PPPInitializeRestartCount(conf);
        SendTerminateRequest(conf);
        PPPSetState(conf, PPP_STATE_CLOSING);
        break;
        case PPP_STATE_OPENED:
        PPPLayerDown(conf);
        PPPInitializeRestartCount(conf);
        SendTerminateRequest(conf);
        PPPSetState(conf, PPP_STATE_CLOSING);
        break;
    }

}

static void PPPTimeout(OSAlarm* alarm, OSContext* context) {

    PPPConf* conf;

    conf = (PPPConf*)((u8*)alarm - offsetof(PPPConf, alarm));
    conf->rxmit++;
    switch (conf->state) {
        case PPP_STATE_CLOSING:
        if (!SendTerminateRequest(conf)) {
            PPPLayerFinished(conf);
            PPPSetState(conf, PPP_STATE_CLOSED);
        }
        break;
        case PPP_STATE_STOPPING:
        if (!SendTerminateRequest(conf)) {
            PPPLayerFinished(conf);
            PPPSetState(conf, PPP_STATE_STOPPED);
        }
        break;
        case PPP_STATE_REQ_SENT:
        if (!SendConfigureRequest(conf)) {
            PPPLayerFinished(conf);
            PPPSetState(conf, PPP_STATE_STOPPED);
        }
        break;
        case PPP_STATE_ACK_RCVD:
        if (SendConfigureRequest(conf)) {
            PPPSetState(conf, PPP_STATE_REQ_SENT);
        } else {
            PPPLayerFinished(conf);
            PPPSetState(conf, PPP_STATE_STOPPED);
        }
        break;
        case PPP_STATE_ACK_SENT:
        if (!SendConfigureRequest(conf)) {
            PPPLayerFinished(conf);
            PPPSetState(conf, PPP_STATE_STOPPED);
        }
        break;
    }

}
