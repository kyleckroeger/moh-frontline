// PATHX_endservice (pathxSND.c, 0x800e8e64): removes the path service's
// timer client and sync task and clears the service. pathService and
// pathTimer are named by their symbols.
extern void (*pathService)(void);
extern void (*pathTimer)(void);

extern "C" {
void SNDSYS_remove100hzclient(void (*)(void));
void SYNCTASK_del(int (*)(int, int));
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
