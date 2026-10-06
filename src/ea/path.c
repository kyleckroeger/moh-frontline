/* The music path player's control setter, latency setter and per-track
   memory overhead: PATH_control locks the player (failing with -18 when it
   is not initialised or already busy), and for a value of 0 to 127 sets the
   control byte of every track in the matching player states; PATH_latency
   finds the track a handle names (the handle's top nibble
   selects the project, the next nibble the song and the low 24 bits a track
   mask) among the four player states and stores its latency, and
   PATH_overheadtrack sizes a track for a number of nodes, rounded up to a
   16-byte multiple. pfstates and the functions are named by their symbols;
   the state and track layouts and the inline helpers (state selection and
   track lookup) are inferred. The
   functions around these are not part of the unit. */
struct PATHTRACKVIEW {
    unsigned char unknown00;
    unsigned char control;
    unsigned char unknown02[14];
    int latency;
};

struct PATHSTATEVIEW {
    unsigned char unknown000[32];
    unsigned int id;
    void* active;
    unsigned char unknown028[288];
    PATHTRACKVIEW* tracks[24];
};

extern signed char inited;
extern unsigned int pfactive;
extern PATHSTATEVIEW* pfstate;
extern PATHSTATEVIEW pfstates[4];

extern "C" int PATHX_overheadtrack(int);

static inline int selectstate(int index) {
    if (!pfactive)
        pfactive = -1;
    if (index >= 0 && index < 4 && pfstates[index].active) {
        pfstate = &pfstates[index];
        return pfactive = pfstates[index].id;
    } else if (index != -1) {
        pfactive = 0;
    }
    return pfactive;
}

extern "C" int PATH_control(unsigned int handle, int value) {
    int j;
    unsigned int project;
    unsigned int song;
    int i;
    if (!inited || pfactive || !selectstate(-1))
        return -18;
    if (value >= 0 && value <= 127) {
        project = handle & 0xF0000000;
        song = handle & 0x0F000000;
        for (i = 0; i < 4; i++) {
            if (pfstates[i].active && (project & pfstates[i].id) && (song & pfstates[i].id)) {
                if (selectstate((unsigned char)i)) {
                    for (j = 0; j < 24; j++) {
                        if (pfstate->tracks[j])
                            pfstate->tracks[j]->control = value;
                    }
                }
            }
        }
    }
    pfactive = 0;
    pfstate = 0;
    return 0;
}

static inline PATHTRACKVIEW* findtrack(unsigned int handle) {
    unsigned int i;
    int j;
    unsigned int id;
    unsigned int project;
    unsigned int song;
    unsigned int mask;
    project = handle & 0xF0000000;
    song = handle & 0x0F000000;
    mask = handle & 0x00FFFFFF;
    for (i = 0; i < 4; i++) {
        id = pfstates[i].id;
        if (pfstates[i].active && (id & project) && (id & song)) {
            for (j = 0; j < 24; j++) {
                if ((1 << j) & mask)
                    return pfstates[i].tracks[j];
            }
        }
    }
    return 0;
}

extern "C" int PATH_latency(unsigned int handle, int latency) {
    PATHTRACKVIEW* track = findtrack(handle);
    if (!track)
        return -8;
    track->latency = (unsigned short)latency;
    return 0;
}

extern "C" int PATH_overheadtrack(int nodes) {
    int size = PATHX_overheadtrack(nodes) + 232;
    size += nodes * 16;
    size += 16 - size % 16;
    return size;
}
