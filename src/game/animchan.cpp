// Animation channels: per-object arrays of 136-byte channel records driven by
// state. This unit covers the accessors at the start of the file. The record
// views (channel and owner) are inferred from offsets; their names are not
// original.
struct AnimChannelView {
    unsigned char field00;
    unsigned char status;
    unsigned char field02[12];
    unsigned short state;
    unsigned char field10[64];
    float delay;
    float speed;
    unsigned char field58[48];
};

struct AnimChannelOwnerView {
    unsigned char field00[4];
    unsigned short count;
};

extern "C" void AnimChanSetSpeed(AnimChannelView* channel, float speed) {
    channel->speed = speed;
}

extern "C" void AnimChanSetDelayByState(AnimChannelOwnerView* owner, AnimChannelView* channel, unsigned short state, float delay) {
    for (int i = 0; i < owner->count; channel++, i++) {
        if (channel->status == 2 && state == channel->state)
            channel->delay = delay;
    }
}

extern "C" unsigned short AnimChanGetStateChannelByIndex(AnimChannelOwnerView* owner, AnimChannelView* channel,
                                                          unsigned short state, unsigned short index) {
    int found = 0;

    for (int i = 0; i < owner->count; channel++, i++) {
        if ((channel->status == 2 || channel->status == 1) && state == channel->state) {
            if ((unsigned short)found == index)
                return i;
            found++;
        }
    }
    return 0xFFFF;
}

extern "C" int AnimChanGetNumRunningOrDoneByState(AnimChannelOwnerView* owner, AnimChannelView* channel, unsigned short state) {
    int count = 0;

    for (int i = 0; i < owner->count; channel++, i++) {
        if ((channel->status == 2 || channel->status == 1) && state == channel->state)
            count++;
    }
    return count;
}

extern "C" int AnimChanGetNumActiveByState(AnimChannelOwnerView* owner, AnimChannelView* channel, unsigned short state) {
    int count = 0;

    for (int i = 0; i < owner->count; channel++, i++) {
        if (channel->status == 2 && state == channel->state)
            count++;
    }
    return count;
}

// AnimChanSwitchAnimByDuration, AnimChanStopByStateIndex, AnimChanStartAnim,
// AnimChanProcess and AnimChanInitChannels follow in the original file and
// are not reconstructed.
