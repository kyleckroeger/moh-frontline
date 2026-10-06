// Fragment of the packet player: creation of a player in caller-supplied
// memory. SNDPKTPLAY_overhead before it (4 instructions differ) and the rest
// of the file (start, submit, ...) are not reconstructed. Player records and
// the sndgs/sndpps globals are accessed through inferred members.
struct SNDPKTPLAYER {
    int handle;
    char field4[20];
    short maxpackets;
    char field1A[14];
    void* memory;
    int arg0;
    int arg1;
    int arg2;
};

struct SNDPPSVIEW {
    char field0[772];
    SNDPKTPLAYER* players[1];
};

extern "C" {
extern char sndgs[];
extern SNDPPSVIEW sndpps;
void SNDSYS_entercritical(void);
void SNDSYS_leavecritical(void);
}
int SNDPLATFORM_packetoverhead(void);
int SNDPLATFORM_packetplaycreate(int, void*);

extern "C" int SNDPKTPLAY_create(int arg0, int arg1, int arg2, char* memory, unsigned int size) {
    int i;
    char* base;
    SNDPKTPLAYER* player;

    SNDSYS_entercritical();
    for (i = 0; i < *(unsigned char*)(sndgs + 71); i++) {
        if (!sndpps.players[i])
            goto found;
    }
    SNDSYS_leavecritical();
    return -9;
found:
    if (SNDPLATFORM_packetplaycreate(i, memory) < 0) {
        SNDSYS_leavecritical();
        return -6;
    }
    base = memory;
    memory += SNDPLATFORM_packetoverhead();
    size -= SNDPLATFORM_packetoverhead();
    player = (SNDPKTPLAYER*)memory;
    player->maxpackets = (size - 100) / 24;
    player->memory = base;
    player->arg0 = arg0;
    player->arg1 = arg1;
    player->arg2 = arg2;
    player->handle = -1;
    sndpps.players[i] = player;
    SNDSYS_leavecritical();
    return i;
}
