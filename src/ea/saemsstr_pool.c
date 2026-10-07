/* A fragment of EA's AEMS streaming file saemsstr.c (0x801578e0):
   SNDAEMS_streamrestore removes the stream task and destroys every stream
   and its status record; SNDAEMSI_streamgethandle gives a player a free
   stream, or takes the lowest priority one when the request outranks it;
   SNDAEMSI_streamdying detaches a stream's player; and
   SNDAEMSI_streamupdatestatus copies the stream's request status (and the
   queued request's) into the player's outputs, releasing the stream when its
   request has finished. SNDAEMS_streaminit before
   them, which divides caller memory between the streams, is not part of the
   unit. AEMSSTREAMSTATUS and AEMSCOMPDYNAMICPLAYER are named by the mangled
   symbols; their members and the stream pool view are inferred. */
struct AEMSCOMPDYNAMICPLAYER {
    void* def;
    unsigned char unknown04[12];
    int out;
    int unknown14;
    int unknown18;
    int handle;
    int queued;
    int stream;
    unsigned int unknown28;
};

/* inferred: a stream request's status as SNDSTRM_requeststatus reports it */
struct STREAMREQUESTVIEW {
    int state;
    int unknown04;
    int unknown08;
    int unknown0c;
};

struct AEMSSTREAMSTATUS {
    char* buffer;
    int handle;
    int priority;
    AEMSCOMPDYNAMICPLAYER* player;
    int unknown10;
    int unknown14;
    int unknown18;
    int unknown1c;
    int unknown20;
    unsigned char dying;
};

/* inferred: the stream pool */
struct AEMSSTREAMPOOLVIEW {
    char* memory;
    int size;
    int count;
    int rate;
    AEMSSTREAMSTATUS** statuses;
};

struct SNDAEMSVIEW {
    unsigned char unknown00[16];
    AEMSSTREAMPOOLVIEW* streams;
};

/* inferred: the default play settings SNDplaysetdef fills */
extern SNDAEMSVIEW sndaems;

extern "C" {
void SNDSYS_entercritical();
void SNDSYS_leavecritical();
void SNDSTRM_destroy(int);
void SNDSTRM_purge(int);
void SNDSTRM_requeststatus(int, STREAMREQUESTVIEW*);
}
void SNDMEMI_free(void*);
void iSNDserverremoveclient(void (*)());
void SNDAEMSI_streamsystemtask();
void SNDAEMSI_streamdying(AEMSSTREAMSTATUS*);

extern "C" int SNDAEMS_streamrestore() {
    int i;

    if (!sndaems.streams)
        return 0;
    iSNDserverremoveclient(SNDAEMSI_streamsystemtask);
    for (i = 0; i < sndaems.streams->count; i++) {
        AEMSSTREAMSTATUS* status = sndaems.streams->statuses[i];

        if (status->player)
            SNDAEMSI_streamdying(status);
        SNDSTRM_destroy(status->handle);
        SNDSYS_entercritical();
        SNDMEMI_free(status);
        SNDSYS_leavecritical();
    }
    SNDSYS_entercritical();
    SNDMEMI_free(sndaems.streams->statuses);
    SNDMEMI_free(sndaems.streams);
    SNDSYS_leavecritical();
    sndaems.streams = 0;
    return 0;
}

int SNDAEMSI_streamgethandle(AEMSCOMPDYNAMICPLAYER* player, int priority) {
    int lowest = 100;
    int index = -1;
    int i;

    if (!sndaems.streams)
        return -14;
    for (i = 0; i < sndaems.streams->count; i++) {
        AEMSSTREAMSTATUS* status = sndaems.streams->statuses[i];

        if (!status->player && !status->dying) {
            status->player = player;
            status->priority = priority;
            return status->handle;
        }
    }
    for (i = 0; i < sndaems.streams->count; i++) {
        if (sndaems.streams->statuses[i]->priority < lowest) {
            lowest = sndaems.streams->statuses[i]->priority;
            index = i;
        }
    }
    if (priority > lowest && index >= 0) {
        AEMSSTREAMSTATUS* status = sndaems.streams->statuses[index];

        if (status->player)
            SNDAEMSI_streamdying(status);
        SNDSTRM_purge(status->handle);
        status->player = player;
        status->priority = priority;
        return status->handle;
    }
    return -1;
}

void SNDAEMSI_streamdying(AEMSSTREAMSTATUS* status) {
    SNDSYS_entercritical();
    if (!status->player) {
        SNDSYS_leavecritical();
        return;
    }
    status->dying = 0;
    status->player->out = 0;
    status->player->unknown14 = 0;
    status->player->unknown18 = 0;
    status->player->handle = -1;
    status->player->stream = -1;
    status->player->unknown28 = 0;
    SNDSYS_leavecritical();
}

void SNDAEMSI_streamupdatestatus(AEMSCOMPDYNAMICPLAYER* player) {
    STREAMREQUESTVIEW current;
    STREAMREQUESTVIEW queued = {0};
    AEMSSTREAMSTATUS* status = 0;
    int i;

    for (i = 0; i < sndaems.streams->count; i++) {
        status = sndaems.streams->statuses[i];
        if (status->handle == player->stream)
            break;
    }
    if (i >= sndaems.streams->count || !status->player)
        return;
    if (player->handle == 0x7FFFFFFF) {
        player->out = 0;
        player->unknown14 = 0;
        player->unknown18 = 0;
        player->handle = -1;
        player->stream = -1;
        status->player = 0;
        status->unknown10 = 0;
        status->dying = 0;
        player->unknown28 = 0;
        return;
    }
    if (status->dying == 2 || status->dying == 1)
        return;
    SNDSTRM_requeststatus(player->handle, &current);
    if (player->queued >= 0)
        SNDSTRM_requeststatus(player->queued, &queued);
    if (current.state == 3) {
        player->out = 0;
        player->unknown14 = 0;
        player->unknown18 = 0;
        player->handle = -1;
        player->queued = -1;
        status->player = 0;
        status->dying = 0;
        player->unknown28 = 0;
        SNDSTRM_purge(player->stream);
        player->stream = -1;
    } else if (current.state == 2) {
        player->out = 1;
        if (player->unknown28 && status->dying == 3) {
            status->dying = 2;
        } else {
            player->unknown14 = current.unknown08;
            player->unknown18 = current.unknown04;
        }
    } else if (current.state == 0 || current.state == 1) {
        player->out = 1;
        player->unknown14 = 0x7FFFFFFF;
        if (player->unknown28 && player->queued >= 0 && queued.state == 2)
            player->unknown18 = queued.unknown04;
        else
            player->unknown18 = 0;
    }
}
