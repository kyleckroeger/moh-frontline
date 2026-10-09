// PATHX_endservice, PATHX_initservice, PATHX_status, PATHX_statusall and
// PATHX_timeremaining (pathxSND.c, 0x800e8e64): removes the path service's
// timer client and sync task and clears the service; clears the track table,
// records the service and its timer, registers them and installs the abort
// and print hooks; reports a track's two positions and its error or state (-8
// without a track); refreshes every track's state (or, with a non-zero flag,
// marks each track pending) - sound-bank tracks from the patch length and the
// time remaining, streamed tracks from the stream and each queued request's
// status - inside a critical section; reports a track's or a queued
// request's time. The status calls refresh the track states first when a
// request is pending. pathService, pathTimer, pathabortmsg, pathprintf and
// pathxinfo are named by their symbols; the 24-entry table size comes from
// the symbol layout; the track, queue and status record views and the
// parameter meanings are inferred (see pathxSND.c). This fragment is built
// with -use_lmw_stmw on, as PATHX_statusall's stmw shows; the explicit
// case 3 reproduces the request-state compare tree.
struct PathxQueueView {
    int length;
    int handle;
    int value08;
};

struct PathxTrackView {
    int stream;
    int patch;
    unsigned char unknown08[6];
    signed char isSound;
    unsigned char unknown0f[45];
    int position3c;
    int position40;
    int played;
    int total;
    int ready;
    int state50;
    int volume;
    int pending;
    int error5c;
    int queueCount;
    PathxQueueView* queue;
};

struct PathxFormatView {
    unsigned short rate;
    unsigned short unknown02;
};

struct PathxRequestStatusView {
    int state;
    int value04;
    int value08;
    int value0c;
};

struct PathxDescView {
    unsigned int samples;
    int unknown04[4];
};

struct PathxAttrView {
    int unknown00[21];
};

extern void (*pathService)(void);
extern void (*pathTimer)(void);
extern void (*pathabortmsg)(const char*, ...);
extern int (*pathprintf)(const char*, ...);
extern PathxTrackView* pathxinfo[24];

extern "C" {
void SNDSYS_add100hzclient(void (*)(void));
void SNDSYS_remove100hzclient(void (*)(void));
void SYNCTASK_add(int (*)(int, int), int, int);
void SYNCTASK_del(int (*)(int, int));
void REAL_abortmessage(const char*, ...);
int printf(const char*, ...);
void PATHX_statusall(int);
void SNDSYS_entercritical(void);
void SNDSYS_leavecritical(void);
int SNDSTRM_status(int, int*);
void SNDSTRM_requeststatus(int, PathxRequestStatusView*);
int SNDSTRM_getprogvol(int);
int SNDBANK_patchinfo(int, int, PathxFormatView*, PathxAttrView*, PathxDescView*);
int SNDtimeremaining(int);
int SNDCTRL_getprogvol(int);
}
int PATHX_isynctask(int, int);

extern "C" void PATHX_endservice(void) {
    if (pathService) {
        SNDSYS_remove100hzclient(pathTimer);
        SYNCTASK_del(PATHX_isynctask);
        pathService = 0;
        pathTimer = 0;
    }
}

extern "C" void PATHX_initservice(void (*service)(void), void (*timer)(void)) {
    int i;

    for (i = 0; i < 24; i++)
        pathxinfo[i] = 0;
    pathService = service;
    pathTimer = timer;
    SNDSYS_add100hzclient(timer);
    SYNCTASK_add(PATHX_isynctask, 5, 100);
    pathabortmsg = REAL_abortmessage;
    pathprintf = printf;
}

extern "C" int PATHX_status(int track, int* position1, int* position2) {
    PathxTrackView* info = pathxinfo[track];

    if (!info)
        return -8;
    if (info->pending)
        PATHX_statusall(0);
    *position1 = info->position3c;
    *position2 = info->position40;
    if (info->error5c)
        return info->error5c;
    return info->state50;
}

extern "C" void PATHX_statusall(int flag) {
    int j;
    int i;
    int error;
    int position2;
    int position1;
    int remaining;
    PathxTrackView* info;
    PathxFormatView format;
    int status[3];
    PathxRequestStatusView request;
    PathxDescView desc;
    PathxAttrView attr;

    SNDSYS_entercritical();
    for (i = 0; i < 24; i++) {
        info = pathxinfo[i];
        if (!info)
            continue;
        if (flag) {
            info->error5c = 0;
            info->pending = 1;
            continue;
        }
        error = -8;
        info->ready = 1;
        position1 = 0;
        position2 = 0;
        if (info->stream >= 0) {
            if (info->isSound) {
                if (info->patch >= 0 && info->queue->handle >= 0) {
                    error = SNDBANK_patchinfo(info->stream, info->patch, &format, &attr, &desc);
                    if (error >= 0) {
                        remaining = SNDtimeremaining(info->queue->handle);
                        if (remaining < 0) {
                            position2 = 0;
                            info->queue->handle = -1;
                            goto done;
                        }
                        position2 = remaining * 10;
                        position1 = (int)(1000.0f * ((float)desc.samples / (float)format.rate)) - position2;
                    }
                    info->queue->value08 = position2;
                    info->played = info->queue->length;
                    info->volume = SNDCTRL_getprogvol(info->queue->handle);
                    info->state50 = info->queue->handle;
                }
            } else {
                error = SNDSTRM_status(info->stream, status);
                info->played = 0;
                if (status[0] >= info->queueCount)
                    info->ready = 0;
                for (j = 0; j < info->queueCount; j++) {
                    info->queue[j].value08 = 0;
                    if (info->queue[j].handle < 0)
                        continue;
                    SNDSTRM_requeststatus(info->queue[j].handle, &request);
                    switch (request.state) {
                    case 1:
                        info->queue[j].value08 = info->queue[j].length;
                        info->total += info->queue[j].length;
                        info->played += request.value0c;
                        info->ready = 0;
                        info->state50 = info->queue[j].handle;
                        break;
                    case 0:
                    case 2:
                        position1 = request.value04;
                        position2 = request.value08;
                        if (!position1 && !position2)
                            position1 = position2 = info->queue[j].length;
                        info->queue[j].value08 = position2;
                        info->total += position2;
                        info->played += request.value0c;
                        if (request.value0c < request.value08)
                            info->ready = 0;
                        info->state50 = info->queue[j].handle;
                        break;
                    case 3:
                    default:
                        info->queue[j].handle = -1;
                        break;
                    }
                }
                info->volume = SNDSTRM_getprogvol(info->stream);
            }
        }
    done:
        info->position3c = position1;
        info->position40 = position2;
        info->error5c = error;
        info->pending = 0;
    }
    SNDSYS_leavecritical();
}

extern "C" int PATHX_timeremaining(int track, int index) {
    PathxTrackView* info = pathxinfo[track];

    if (info && info->pending)
        PATHX_statusall(0);
    if (index < 0)
        return info->total;
    if (index < info->queueCount && info->queue[index].handle >= 0)
        return info->queue[index].value08;
    return -8;
}
