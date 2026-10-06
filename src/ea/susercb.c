// User-data clients of the sound system: a callback list in sndgs. sndgs's
// layout is not known; the members used here are an inferred view.
struct SNDGSVIEW {
    char field0[360];
    signed char running;
    char field169[5];
    signed char userdataclients;
    char field16F[57];
    void* userdataclient[1];
};

extern "C" {
extern SNDGSVIEW sndgs;
void SNDSYS_entercritical(void);
void SNDSYS_leavecritical(void);

void SNDSYS_adduserdataclient(void* client) {
    if (sndgs.running) {
        sndgs.userdataclient[sndgs.userdataclients] = client;
        sndgs.userdataclients++;
    }
}

void SNDSYS_removeuserdataclient(void* client) {
    int i;

    SNDSYS_entercritical();
    for (i = 0; i < sndgs.userdataclients; i++) {
        if (sndgs.userdataclient[i] == client) {
            sndgs.userdataclients--;
            for (; i < sndgs.userdataclients; i++)
                sndgs.userdataclient[i] = sndgs.userdataclient[i + 1];
            break;
        }
    }
    SNDSYS_leavecritical();
}
}
