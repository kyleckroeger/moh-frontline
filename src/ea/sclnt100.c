// 100 Hz clients of the sound system: a callback list in sndgs. sndgs's
// layout is not known; the members used here are an inferred view.
struct SNDGSVIEW {
    char field0[360];
    signed char running;
    char field169[3];
    signed char hzclients;
    char field16D[11];
    void* hzclient[1];
};

extern "C" {
extern SNDGSVIEW sndgs;
void SNDSYS_entercritical(void);
void SNDSYS_leavecritical(void);

void SNDSYS_add100hzclient(void* client) {
    if (sndgs.running) {
        sndgs.hzclient[sndgs.hzclients] = client;
        sndgs.hzclients++;
    }
}

void SNDSYS_remove100hzclient(void* client) {
    int i;

    if (sndgs.running) {
        SNDSYS_entercritical();
        for (i = 0; i < sndgs.hzclients; i++) {
            if (sndgs.hzclient[i] == client) {
                sndgs.hzclients--;
                for (; i < sndgs.hzclients; i++)
                    sndgs.hzclient[i] = sndgs.hzclient[i + 1];
                break;
            }
        }
        SNDSYS_leavecritical();
    }
}
}
