// A fragment of the packet player (0x8015ac50): the free packet slots and
// outstanding frames, stopping a player (stopping its voice, flushing the
// deferred callbacks and freeing the channel buffers) and destroying it,
// fetching a channel's next packet (frame count and first flag out, the
// primary channel also releasing the oldest delivered packet and moving its
// frames from queued to pending), returning played frames, and running the
// deferred frames-done and release callbacks. SNDPKTPLAY_start and
// SNDPKTPLAY_submit before it are not reconstructed (submit differs in the
// load order of its fullness test; draft in scratch/path/spkt_sub5.cpp).
// Player records and sndpps are inferred views; the released-packet index
// is volatile (the original rereads it after comparing it).
/* inferred: a packet as queued in a player */
struct SNDPKTPLAYPACKET {
    int sequence;
    int frames : 31;
    int first : 1;
    void* data[4];
};

struct SNDPKTPLAYER {
    int handle;
    volatile int sequence;
    short read[4];
    short queued[4];
    short maxpackets;
    signed char primary;
    signed char active;
    volatile short released;
    short write;
    unsigned int frames;
    unsigned int pending;
    void* memory;
    void (*release)(void*, void*);
    void (*framesdone)(int, int, void*);
    void* data;
    char field38[2];
    unsigned char channels;
    char field3B;
    void* buffers[4];
    SNDPKTPLAYPACKET packets[1];
};

/* inferred: a deferred callback (type 0: frames done, 1: packet released) */
struct SNDPKTCALLBACK {
    unsigned short type;
    unsigned short player;
    int data;
};

struct SNDPPSVIEW {
    int count;
    SNDPKTCALLBACK callbacks[96];
    SNDPKTPLAYER* players[1];
};

extern "C" {
extern SNDPPSVIEW sndpps;
void SNDstop(int);
}
void SNDPKTPLAYI_flushcallbackdata(void);
void SNDMEMI_free(void*);
void SNDPLATFORM_packetplaydestroy(int);

extern "C" int SNDPKTPLAY_submitspace(int index) {
    SNDPKTPLAYER* player = sndpps.players[index];

    return player->maxpackets - player->queued[0] - 1;
}

extern "C" int SNDPKTPLAY_framesoutstanding(int index) {
    SNDPKTPLAYER* player = sndpps.players[index];

    return player->frames + player->pending;
}

extern "C" int SNDPKTPLAY_stop(int index) {
    SNDPKTPLAYER* player = sndpps.players[index];
    int i;

    SNDstop(player->handle);
    SNDPKTPLAYI_flushcallbackdata();
    player->handle = -1;
    for (i = 0; i < player->channels; i++) {
        if (player->buffers[i])
            SNDMEMI_free(player->buffers[i]);
    }
    return 0;
}

extern "C" int SNDPKTPLAY_destroy(int index) {
    SNDPLATFORM_packetplaydestroy(index);
    sndpps.players[index] = 0;
    return 0;
}

/* inferred: defers a callback until the next flush */
static inline void addcallback(int type, int index, int data) {
    SNDPKTCALLBACK* callback = &sndpps.callbacks[sndpps.count];

    callback->type = type;
    callback->player = index;
    callback->data = data;
    sndpps.count++;
}

/* inferred: releases the oldest packet not yet released */
static inline void releasepacket(SNDPKTPLAYER* player, int index) {
    SNDPKTPLAYPACKET* packet = &player->packets[player->released];

    if (player->release) {
        SNDPKTCALLBACK* callback = &sndpps.callbacks[sndpps.count];

        callback->type = 1;
        callback->player = index;
        callback->data = (int)packet->data[0];
        sndpps.count++;
    }
    player->released++;
    if (player->released >= player->maxpackets)
        player->released = 0;
}

void* SNDPKTPLAYI_get(int index, int channel, int* frames, int* first) {
    SNDPKTPLAYER* player = sndpps.players[index];
    SNDPKTPLAYPACKET* packet;

    if (player->queued[channel] == 0) {
        if (channel == player->primary && !player->active && player->read[channel] != player->released)
            releasepacket(player, index);
        return 0;
    }
    packet = &player->packets[player->read[channel]];
    *frames = packet->frames;
    *first = packet->first;
    if (channel == player->primary) {
        if (player->read[channel] != player->released)
            releasepacket(player, index);
        player->frames -= packet->frames;
        player->pending += packet->frames;
    }
    player->read[channel]++;
    if (player->read[channel] >= player->maxpackets)
        player->read[channel] = 0;
    player->queued[channel]--;
    if (player->queued[channel] == 0 && channel == player->primary)
        player->active = 0;
    if (packet->data[channel])
        return packet->data[channel];
    return (void*)-1;
}

void SNDPKTPLAYI_freeframes(int index, int channel, int frames) {
    SNDPKTPLAYER* player = sndpps.players[index];

    if (channel != player->primary)
        return;
    player->pending -= frames;
    if (player->framesdone)
        addcallback(0, index, frames);
}

void SNDPKTPLAYI_flushcallbackdata(void) {
    int i;

    for (i = 0; i < sndpps.count; i++) {
        SNDPKTPLAYER* player;
        SNDPKTCALLBACK* callback = &sndpps.callbacks[i];

        player = sndpps.players[callback->player];

        if (callback->type == 0)
            player->framesdone(callback->player, callback->data, player->data);
        else
            player->release((void*)callback->data, player->data);
    }
    sndpps.count = 0;
}
