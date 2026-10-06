// Animation sequences: per-channel playback state over an opcode stream. This
// unit covers the functions at the start of the file. AnimSeqAnimInfo_t and
// AnimSeqFrameList_t are named by the mangled symbols; their members and the
// inline rate helper are inferred and are not original.
struct AnimSeqFrameList_t {
    unsigned char framerate;
    unsigned char field01[7];
    unsigned short opcodes[1];
};

struct AnimSeqAnimInfo_t {
    AnimSeqFrameList_t* list;
    float rate;
    float life;
    float time;
    unsigned char field10[8];
    unsigned short position;
    unsigned short frames;
    unsigned char field1C[20];
};

extern "C" AnimSeqAnimInfo_t* _AnimSeq_CurrentInfo;

// File-local in the original; declared without static here because this
// fragment does not define it (the manifest lists it as a local external).
int _AnimSeqParseToEnd(AnimSeqAnimInfo_t*, AnimSeqFrameList_t*, unsigned short);

// Inferred helper: the body of AnimSeqSetRate, inlined into
// AnimSeqScaleDuration.
static inline void SetRate(AnimSeqAnimInfo_t* info, float rate) {
    info->time = info->time * rate / info->rate;
    info->life = info->life * rate / info->rate;
    info->rate = rate;
}

extern "C" int AnimSeqGetOpcodeParameter(void) {
    AnimSeqAnimInfo_t* info = _AnimSeq_CurrentInfo;
    unsigned int word = info->list->opcodes[info->position];
    unsigned int value;

    info->position++;
    value = word & 0xFFF;
    if (word & 0x800)
        value |= 0xF000;
    if ((word & 0x1000) == 0x1000) {
        value <<= 16;
        value += info->list->opcodes[info->position];
        info->position++;
    }
    return value;
}

extern "C" void AnimSeqScaleDuration(AnimSeqAnimInfo_t* info, float scale) {
    if (info->frames == 0xFFFF) {
        AnimSeqAnimInfo_t end;

        _AnimSeqParseToEnd(&end, info->list, 0xFFFF);
        info->frames = end.frames;
    }
    float duration = 0.0625f * ((float)info->frames * (float)info->list->framerate);

    SetRate(info, duration * scale / (float)info->frames);
}

extern "C" void AnimSeqSetRate(AnimSeqAnimInfo_t* info, float rate) {
    SetRate(info, rate);
}

extern "C" unsigned short AnimSeqGetFrameCount(AnimSeqFrameList_t* list) {
    AnimSeqAnimInfo_t end;

    _AnimSeqParseToEnd(&end, list, 0xFFFF);
    return end.frames;
}

extern "C" float AnimSeqGetLife(AnimSeqFrameList_t* list) {
    AnimSeqAnimInfo_t end;

    _AnimSeqParseToEnd(&end, list, 0xFFFF);
    return end.life;
}

// AnimSeqGrow, AnimSeqStart, _AnimSeqParseToEnd and _ParseForFrame follow in
// the original file and are not reconstructed.
