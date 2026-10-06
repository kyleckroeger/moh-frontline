// SNDPKTPLAYI_voicetopackethandle: the packet player that owns a voice, or -1.
// sndgs and sndpps layouts are not known; their fields are read through
// inferred offsets.
extern "C" char sndgs[];
struct SNDPPSVIEW {
    char field0[772];
    int* players[1];
};

extern "C" SNDPPSVIEW sndpps;
int SNDVOICEI_get(int);

int SNDPKTPLAYI_voicetopackethandle(int voice) {
    int i;

    for (i = 0; i < *(unsigned char*)(sndgs + 71); i++) {
        int* player = sndpps.players[i];

        if (player && voice == SNDVOICEI_get(*player))
            return i;
    }
    return -1;
}
