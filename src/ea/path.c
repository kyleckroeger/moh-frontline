/* The music path player's latency setter and per-track memory overhead:
   PATH_latency finds the track a handle names (the handle's top nibble
   selects the project, the next nibble the song and the low 24 bits a track
   mask) among the four player states and stores its latency, and
   PATH_overheadtrack sizes a track for a number of nodes, rounded up to a
   16-byte multiple. pfstates and the functions are named by their symbols;
   the state and track layouts and the inline lookup are inferred. The
   functions around these are not part of the unit. */
struct PATHTRACKVIEW {
    unsigned char unknown00[16];
    int latency;
};

struct PATHSTATEVIEW {
    unsigned char unknown000[32];
    unsigned int id;
    void* active;
    unsigned char unknown028[288];
    PATHTRACKVIEW* tracks[24];
};

extern PATHSTATEVIEW pfstates[4];

extern "C" int PATHX_overheadtrack(int);

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
