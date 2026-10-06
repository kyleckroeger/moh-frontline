// Sound server clients: callbacks run by SNDSYS_service. sndgs's layout is
// not known; the members used here are an inferred view.
struct SNDGSVIEW {
    char field0[360];
    signed char running;
    char field169[4];
    signed char serverclients;
    char field16E[34];
    void (*serverclient[1])(void);
};

extern "C" SNDGSVIEW sndgs;

void iSNDserveraddclient(void (*client)(void)) {
    sndgs.serverclient[sndgs.serverclients] = client;
    sndgs.serverclients++;
}

void iSNDserverremoveclient(void (*client)(void)) {
    int i;

    for (i = 0; i < sndgs.serverclients; i++) {
        if (sndgs.serverclient[i] == client) {
            sndgs.serverclients--;
            for (; i < sndgs.serverclients; i++)
                sndgs.serverclient[i] = sndgs.serverclient[i + 1];
            return;
        }
    }
}

extern "C" void SNDSYS_service(void) {
    int i;

    if (sndgs.running) {
        for (i = 0; i < sndgs.serverclients; i++)
            sndgs.serverclient[i]();
    }
}
