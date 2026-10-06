// Animation weights: a weight moving toward a target each frame, either held
// constant, linearly over a number of frames, faded in and out, or dropped
// when stopping. AnimWgtInfo_t is named by the mangled symbols; its members,
// the per-type parameter records and the inline set-up helper are inferred.
extern "C" void* memset(void*, int, unsigned long);
extern "C" void* memcpy(void*, const void*, unsigned long);

struct AnimWgtInfo_t {
    float weight;
    float target;
    int type;
    union {
        struct {
            float start;
            unsigned int frames;
        } linear;
        struct {
            float start;
            float end;
            int hold;
            signed char steps;
            signed char remaining;
            signed char state;
        } inout;
    } params;
};

struct AnimWgtLinearParams_t {
    float start;
    unsigned int frames;
};

struct AnimWgtLinearInAndOutParams_t {
    float start;
    float end;
    int hold;
    signed char steps;
    signed char remaining;
    signed char state;
};

typedef void (*AnimWgtFunction)(AnimWgtInfo_t*);

static void _AnimWgtLinearInAndOutFunction(AnimWgtInfo_t*);
static void _AnimWgtWaitToStopFunction(AnimWgtInfo_t*);
static void _AnimWgtLinearFunction(AnimWgtInfo_t*);
static void _AnimWgtConstantFunction(AnimWgtInfo_t*);

extern "C" {
unsigned char _AnimWgt_TypeStructSize[4] = {0, sizeof(AnimWgtLinearParams_t), 4, sizeof(AnimWgtLinearInAndOutParams_t)};
AnimWgtFunction _AnimWgt_pFunctions[4] = {
    _AnimWgtConstantFunction,
    _AnimWgtLinearFunction,
    _AnimWgtWaitToStopFunction,
    _AnimWgtLinearInAndOutFunction,
};
}

// Inferred helper: the body of AnimWgtSet, inlined into the typed setters.
static inline void SetWeight(AnimWgtInfo_t* info, float weight, float target, int type, void* params) {
    if (weight != -1.0f)
        info->weight = weight;
    if (target != -1.0f)
        info->target = target;
    info->type = type;
    if (params && _AnimWgt_TypeStructSize[info->type])
        memcpy(&info->params, params, _AnimWgt_TypeStructSize[info->type]);
}

extern "C" void AnimWgtSetLinearInAndOut(AnimWgtInfo_t* info, float weight, float target, signed char steps, unsigned char hold) {
    AnimWgtLinearInAndOutParams_t params;

    if (_AnimWgt_TypeStructSize[3])
        memset(&params, 0, _AnimWgt_TypeStructSize[3]);
    params.start = weight != -1.0f ? weight : (weight = info->weight);
    params.steps = steps;
    params.remaining = steps;
    params.hold = hold;
    params.end = target;
    params.state = 0;
    SetWeight(info, weight, target, 3, &params);
}

extern "C" void AnimWgtSetLinear(AnimWgtInfo_t* info, float weight, float target, unsigned int frames) {
    AnimWgtLinearParams_t params;

    if (_AnimWgt_TypeStructSize[1])
        memset(&params, 0, _AnimWgt_TypeStructSize[1]);
    if (weight == -1.0f)
        weight = info->weight;
    params.frames = frames;
    params.start = weight;
    SetWeight(info, weight, target, 1, &params);
}

extern "C" void AnimWgtProcess(AnimWgtInfo_t* info, float frames) {
    for (unsigned int i = 0; i < frames; i++)
        _AnimWgt_pFunctions[info->type](info);
}

extern "C" void AnimWgtSet(AnimWgtInfo_t* info, float weight, float target, int type, void* params) {
    SetWeight(info, weight, target, type, params);
}

extern "C" int AnimWgtShutdown(void) {
    return 0;
}

extern "C" int AnimWgtInit(void) {
    return 0;
}

static void _AnimWgtLinearInAndOutFunction(AnimWgtInfo_t* info) {
    bool step = false;

    switch (info->params.inout.state) {
    case 0:
        step = true;
        if (--info->params.inout.remaining >= 0)
            break;
        info->params.inout.state = 1;
    case 1:
        if (--info->params.inout.hold >= 0)
            break;
        info->params.inout.state = 2;
        info->params.inout.remaining = info->params.inout.steps;
    case 2:
        step = true;
        break;
    }
    if (step == true) {
        float from;
        float to;

        if (info->params.inout.state != 2) {
            from = info->params.inout.start;
            to = info->params.inout.end;
        } else {
            from = info->params.inout.end;
            to = info->params.inout.start;
        }
        info->weight += (to - from) / info->params.inout.steps;
        if (from < to) {
            if (info->weight >= to)
                info->weight = to;
        } else {
            if (info->weight <= to)
                info->weight = to;
        }
    }
}

static void _AnimWgtWaitToStopFunction(AnimWgtInfo_t* info) {
    // Tests the address of the type field, as compiled.
    if (!&info->type)
        info->weight = 0.0f;
}

static void _AnimWgtLinearFunction(AnimWgtInfo_t* info) {
    if (info->weight != info->target) {
        info->weight += (info->target - info->params.linear.start) / info->params.linear.frames;
        if (info->params.linear.start < info->target) {
            if (info->weight > info->target)
                info->weight = info->target;
        } else {
            if (info->weight < info->target)
                info->weight = info->target;
        }
    }
}

static void _AnimWgtConstantFunction(AnimWgtInfo_t*) {
}
