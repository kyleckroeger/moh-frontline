// PATHX_endservice, PATHX_initservice and PATHX_status (pathxSND.c,
// 0x800e8e64): removes the path service's timer client and sync task and
// clears the service; clears the track table, records the service and its
// timer, registers them and installs the abort and print hooks; reports a
// track's two positions and its error or state (-8 without a track),
// refreshing the track states first when a request is pending. pathService,
// pathTimer, pathabortmsg, pathprintf and pathxinfo are named by their
// symbols; the 24-entry table size comes from the symbol layout; the track
// record view and the parameter meanings are inferred (see pathxSND.c).
struct PathxTrackView {
    unsigned char unknown00[60];
    int position3c;
    int position40;
    unsigned char unknown44[12];
    int state50;
    unsigned char unknown54[4];
    int pending;
    int error5c;
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
int PATHX_statusall(int);
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
