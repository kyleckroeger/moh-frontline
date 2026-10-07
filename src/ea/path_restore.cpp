/* A fragment of EA's music path player (path.c, 0x8010ed14):
   PATHI_eventrestore, which ends an event on every active track of the
   current player state that it holds: it restores the track's fade level
   (setting the volume scaled by the track's volume percentage), clears the
   event, and then either stops the track (restore flag 2) or re-enters its
   saved node (flag 1), setting the direction from whether a next node is
   linked, and queues it. PATHTRACK, pfstate and the functions are named by
   their symbols; the track and state layouts (and their member names) are
   inferred. The functions around this one are not part of the unit. */
struct PATHTRACK {
    unsigned char unknown00;
    signed char layer;
    unsigned char unknown02;
    signed char volume;
    unsigned char unknown04[8];
    unsigned short unknown0c : 4;
    signed short direction : 5;
    unsigned short unknown0c_9 : 7;
    unsigned char unknown0e[6];
    void* next;
    unsigned char unknown18[2];
    short event;
    void* active;
    unsigned char fade;
    unsigned char mode;
    short node;
    unsigned char level;
    unsigned char restore;
    short restoreNode;
    unsigned char unknown28[12];
    int queued;
    unsigned char unknown38[4];
    int handle;
    int beat;
};

struct PATHSTATEVIEW {
    unsigned char unknown000[328];
    PATHTRACK* tracks[24];
};

extern PATHSTATEVIEW* pfstate;

extern "C" void PATHX_volume(int, int);
short PATHI_enternode(int, int, int);
void PATHI_queuenode(PATHTRACK*, unsigned int);

void PATHI_eventrestore(short event) {
    PATHTRACK* track;
    int i;

    for (i = 0; i < 24; i++) {
        track = pfstate->tracks[i];
        if (track && track->active && event >= 0 && track->event == event) {
            int level = track->level & 0x7F;

            PATHX_volume(track->handle, level * track->volume / 100);
            track->fade = level;
            track->event = -1;
            if (track->restore & 2) {
                track->beat = -1;
                track->node = -1;
                track->queued = 0;
                track->active = 0;
            } else if (track->restore & 1) {
                track->node = PATHI_enternode(0, track->restoreNode, track->layer);
                track->mode = track->restore;
                track->direction = track->next ? 1 : -1;
                track->queued = 0;
                PATHI_queuenode(track, 0);
            }
        }
    }
}
