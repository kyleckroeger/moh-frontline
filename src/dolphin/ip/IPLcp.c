#include <dolphin/ip.h>
#include <dolphin/private/ip.h>

#ifdef NULL
#undef NULL
#endif
#define NULL 0

static const LCPOptPAP OptPAP = { 3, sizeof(LCPOptPAP), PPP_PAP }; // size: 0x4, address: 0x0
static const LCPOptCHAP OptCHAP = { 3, 5, PPP_CHAP, 5 }; // size: 0x6, address: 0x4

// Range: 0x0 -> 0x160
void PPPDumpLCP(LCPHeader* lcp /* r29 */) {
    // Local variables
    u8* data; // r28
    u16 len; // r27
    LCPOpt* opt; // r31

    data = (u8*)lcp + sizeof(LCPHeader);
    len = lcp->len - sizeof(LCPHeader);
    OSReport("LCP: code=%d id=%d len=%d\n", lcp->code, lcp->id, lcp->len);
    switch (lcp->code) {
        case 1:
        case 2:
        case 3:
        case 4:
            for (opt = (LCPOpt*)data; (u8*)opt < data + len; opt = (LCPOpt*)((u8*)opt + opt->len)) {
                switch (opt->type) {
                    case 1:
                        OSReport(" MaximumReceiveUnit(%d)", opt->type);
                        break;
                    case 2:
                        OSReport(" AsyncronousControlCharacterMap(%d)", opt->type);
                        break;
                    case 3:
                        OSReport(" AuthenticationProtocol(%d)", opt->type);
                        break;
                    case 4:
                        OSReport(" QualityProtocol(%d)", opt->type);
                        break;
                    case 5:
                        OSReport(" MagicNumber(%d)", opt->type);
                        break;
                    case 7:
                        OSReport(" ProtocolFieldCompression(%d)", opt->type);
                        break;
                    case 8:
                        OSReport(" AddressandControlFieldCompression(%d)", opt->type);
                        break;
                    case 9:
                        OSReport(" FCSAlternative(%d)", opt->type);
                        break;
                    default:
                        OSReport(" Unknown(%d)", opt->type);
                        break;
                }
            }
            break;
    }
}

// Range: 0x160 -> 0x53C
static int ReceiveConfigureRequest(PPPConf* conf /* r22 */, LCPHeader* lcp /* r25 */) {
    // Local variables
    u8* data; // r29
    u16 len; // r30
    LCPOpt* req; // r31
    int reject; // r24
    int nack; // r28
    u16 mru; // r27
    u16 auth; // r26
    u8 algorithm; // r23

    // References
    // -> static const LCPOptPAP OptPAP;

    data = (u8*)lcp + sizeof(LCPHeader);
    len = lcp->len - sizeof(LCPHeader);
    reject = FALSE;
    nack = FALSE;
    mru = conf->mru;
    auth = 0;

    for (req = (LCPOpt*)data; (u8*)req < data + len && !reject; req = (LCPOpt*)((u8*)req + req->len)) {
        switch (req->type) {
            case 1:
                if (req->len != 4) {
                    return -3;
                }
                mru = *(u16*)(req + 1);
                if (mru < 128) {
                    nack = TRUE;
                }
                break;
            case 3:
                if (req->len < 4) {
                    return -3;
                }
                auth = ((LCPOptPAP*)req)->auth;
                switch (auth) {
                    case PPP_PAP:
                        if (req->len != 4) {
                            return -3;
                        }
                        break;
                    case PPP_CHAP:
                        if (req->len != 5) {
                            return -3;
                        }
                        algorithm = ((LCPOptCHAP*)req)->algorithm;
                        if (algorithm != 5) {
                            nack = TRUE;
                        }
                        break;
                    default:
                        nack = TRUE;
                        break;
                }
                break;
            case 5:
            default:
                reject = TRUE;
                break;
        }
    }

    if (reject) {
        req = (LCPOpt*)data;
        while ((u8*)req < data + len) {
            switch (req->type) {
                case 1:
                case 3:
                    len = PPPDeleteOpt(data, len, req);
                    break;
                default:
                    req = (LCPOpt*)((u8*)req + req->len);
                    break;
            }
        }
        lcp->len = len + 4;
        return -1;
    }

    if (nack && conf->failure <= 0) {
        req = (LCPOpt*)data;
        while ((u8*)req < data + len) {
            switch (req->type) {
                case 1:
                    if (128 <= mru) {
                        len = PPPDeleteOpt(data, len, req);
                        continue;
                    }
                    break;
                case 3:
                    switch (auth) {
                        case PPP_PAP:
                            len = PPPDeleteOpt(data, len, req);
                            continue;
                        case PPP_CHAP:
                            if (algorithm == 5) {
                                len = PPPDeleteOpt(data, len, req);
                                continue;
                            }
                            break;
                    }
                    break;
            }
            req = (LCPOpt*)((u8*)req + req->len);
        }
        lcp->len = len + 4;
        return -1;
    }

    if (nack) {
        req = (LCPOpt*)data;
        while ((u8*)req < data + len) {
            switch (req->type) {
                case 1:
                    if (mru < 128) {
                        *(u16*)(req + 1) = 128;
                        req = (LCPOpt*)((u8*)req + req->len);
                        continue;
                    }
                    break;
                case 3:
                    switch (auth) {
                        case PPP_PAP:
                            break;
                        case PPP_CHAP:
                            if (algorithm != 5) {
                                ((LCPOptCHAP*)req)->algorithm = 5;
                                req = (LCPOpt*)((u8*)req + req->len);
                                continue;
                            }
                            break;
                        default:
                            len = PPPDeleteOpt(data, len, req);
                            len = PPPInsertOpt(data, len, req, (LCPOpt*)&OptPAP);
                            req = (LCPOpt*)((u8*)req + req->len);
                            continue;
                    }
                    break;
            }
            len = PPPDeleteOpt(data, len, req);
        }
        lcp->len = len + 4;
        return -2;
    }

    conf->mru = mru;
    conf->auth = auth;
    return 0;
}

// Range: 0x53C -> 0x5A0
static int ReceiveConfigureAck(PPPConf* conf /* r31 */, LCPHeader* lcp /* r30 */) {
    if (lcp->len != conf->len + 4 || memcmp(lcp + 1, conf->data, conf->len) != 0) {
        return FALSE;
    }
    return TRUE;
}

// Range: 0x5A0 -> 0x5EC
static int ReceiveConfigureNak(PPPConf* conf /* r3 */, LCPHeader* lcp /* r4 */) {
    // Local variables
    u8* data; // r30
    u16 len; // r29
    LCPOpt* cur; // r1+0x14
    LCPOpt* nak; // r31
    LCPOpt* end; // r1+0x10 (guessed)

    data = (u8*)lcp + sizeof(LCPHeader);
    len = lcp->len - sizeof(LCPHeader);
    cur = (LCPOpt*)conf->data;
    for (nak = (LCPOpt*)data; (u8*)nak < data + len; nak = (LCPOpt*)((u8*)nak + nak->len)) {
    }
    return TRUE;
}

// Range: 0x5EC -> 0x698
static int ReceiveConfigureReject(PPPConf* conf /* r30 */, LCPHeader* lcp /* r25 */) {
    // Local variables
    u8* data; // r28
    u16 len; // r27
    LCPOpt* cur; // r31
    LCPOpt* rej; // r29
    LCPOpt* end; // r26

    data = (u8*)lcp + sizeof(LCPHeader);
    len = lcp->len - sizeof(LCPHeader);
    cur = (LCPOpt*)conf->data;
    for (rej = (LCPOpt*)data; (u8*)rej < data + len; (u8*)rej += rej->len) {
        end = (LCPOpt*)(conf->data + conf->len);
        while (rej->type != cur->type) {
            if (end <= cur) {
                return FALSE;
            }
            (u8*)cur += cur->len;
        }
        conf->len = PPPDeleteOpt(conf->data, conf->len, cur);
    }
    return TRUE;
}

// Range: 0x698 -> 0x77C
static int Up(PPPConf* conf /* r30 */) {
    // Local variables
    IPInterface* interface; // r29
    PPPConf* auth; // r31

    // References
    // -> PPPConf PPPAuthConf;

    interface = conf->interface;
    if (conf->mru < interface->mtu) {
        interface->mtu = conf->mru;
    }

    switch (conf->auth) {
        case PPP_PAP:
            auth = &PPPAuthConf;
            PAPInit(auth, interface);
            break;
        case PPP_CHAP:
            auth = &PPPAuthConf;
            CHAPInit(auth, interface);
            break;
        default:
            auth = NULL;
            break;
    }

    if (auth != NULL) {
        auth->link.prev = (IFLink*)conf;
        auth->link.next = conf->link.next;
        conf->link.next = (IFLink*)auth;
        if (auth->link.next == NULL) {
            interface->ppp.prev = (IFQueue*)auth;
        } else {
            ((PPPConf*)auth->link.next)->link.prev = (IFLink*)auth;
        }
        PPPOpen(auth);
    }
    return TRUE;
}

// Range: 0x77C -> 0x7DC
static int Down(PPPConf* conf /* r3 */) {
    // Local variables
    PPPConf* auth; // r29
    IPInterface* interface; // r28

    // References
    // -> PPPConf PPPAuthConf;

    auth = (PPPConf*)conf->link.next;
    if (auth == &PPPAuthConf) {
        interface = conf->interface;
        IFQueueDequeueEntry(PPPConf*, &interface->ppp, auth);
    }
    return TRUE;
}

// Range: 0x7DC -> 0x808
static int Started(PPPConf* conf /* r1+0x8 */) {
    return PPPoEOpen(conf->interface);
}

// Range: 0x808 -> 0x838
static int Finished(PPPConf* conf /* r1+0x8 */) {
    PPPoETerminate(conf->interface);
    return TRUE;
}

// Range: 0x838 -> 0x8FC
int PPPInitLCP(PPPConf* conf /* r31 */, IPInterface* interface /* r1+0xC */) {
    memset(conf, 0, sizeof(PPPConf));
    conf->protocol = PPP_LCP;
    OSCreateAlarm(&conf->alarm);
    conf->interface = interface;
    conf->receiveConfigureRequest = ReceiveConfigureRequest;
    conf->receiveConfigureAck = ReceiveConfigureAck;
    conf->receiveConfigureNak = ReceiveConfigureNak;
    conf->receiveConfigureReject = ReceiveConfigureReject;
    conf->up = Up;
    conf->down = Down;
    conf->started = Started;
    conf->finished = Finished;
    conf->mru = 1500;
    return TRUE;
}
