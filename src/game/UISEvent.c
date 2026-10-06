// UI rate functions: a per-UI table of 52-byte entries that animate a control
// value toward a target, keyed by function id and control. UISInfo_t,
// UISScreen_t and UISControlInfo_t are named by the mangled symbols; the
// members are inferred from offsets and are not original. The file is
// compiled with deferred inlining (the search in UISFindRateFnc, last in the
// image, is inlined into the functions before it), so functions are listed
// in reverse image order.
extern "C" void* memmove(void*, const void*, unsigned long);

struct UISScreen_t;
struct UISControlInfo_t;

struct UISRateFnc_t {
    unsigned long id;
    unsigned char* data;
    unsigned long field08;
    unsigned long field0C;
    unsigned long frames;
    unsigned long state;
    UISControlInfo_t* control;
    UISScreen_t* screen;
    unsigned long action;
    float target;
    float rate;
    unsigned long field2C;
    UISControlInfo_t* source;
};

struct UISInfo_t {
    char field00[8];
    unsigned long frameRate;
    char field0C[56];
    int rateCount;
    UISRateFnc_t* rates;
};

float* UISGetActionPtrValue(unsigned long, UISControlInfo_t*);

unsigned long UISFindRateFnc(UISInfo_t* info, UISControlInfo_t* control, unsigned long id) {
    unsigned long i;

    for (i = 0; i < info->rateCount; i++) {
        UISRateFnc_t* rate = &info->rates[i];

        if (rate->id == id && rate->control == control)
            break;
    }
    return i;
}

void UISLoadAdvRateFnc(UISInfo_t* info, UISScreen_t* screen, UISControlInfo_t* control, UISControlInfo_t* source,
                       unsigned long id, unsigned char* field2C, unsigned char* data, unsigned long frames,
                       float target, unsigned long action) {
    unsigned long i = UISFindRateFnc(info, control, id);
    UISRateFnc_t* rate;

    if (i == info->rateCount)
        info->rateCount++;
    rate = &info->rates[i];
    rate->control = control;
    rate->source = source;
    rate->screen = screen;
    rate->id = id;
    rate->data = data;
    rate->frames = info->frameRate;
    rate->field08 = 0;
    rate->field0C = 0;
    rate->state = 0;
    rate->action = action;
    rate->target = target;
    rate->field2C = (unsigned long)field2C;
    rate->rate = (target - *UISGetActionPtrValue(rate->action, rate->source)) /
                 ((float)frames / (float)info->frameRate);
}

void UISLoadRateFnc(UISInfo_t* info, UISScreen_t* screen, UISControlInfo_t* control, unsigned long id,
                    unsigned char* data, unsigned long frames) {
    unsigned long i = UISFindRateFnc(info, control, id);
    UISRateFnc_t* rate;

    if (i == info->rateCount)
        info->rateCount++;
    rate = &info->rates[i];
    rate->control = control;
    rate->id = id;
    rate->data = data;
    rate->frames = frames;
    rate->field08 = 0;
    rate->field0C = 0;
    rate->screen = screen;
    rate->state = 0;
    rate->action = 0;
    rate->target = 0.0f;
    rate->field2C = 0;
}

void UISUnloadRateFnc(UISInfo_t* info, UISControlInfo_t* control, unsigned long id) {
    unsigned long i = UISFindRateFnc(info, control, id);

    if (i < info->rateCount)
        info->rates[i].state = 1;
}

void UISRemoveUnNessaryRateFncs(UISInfo_t* info) {
    int i = info->rateCount;

    while (i--) {
        if (info->rates[i].state == 1) {
            info->rateCount--;
            for (int j = i; j < info->rateCount; j++)
                memmove(&info->rates[j], &info->rates[j + 1], sizeof(UISRateFnc_t));
        } else if (info->rates[i].state == 0) {
            info->rates[i].state = 2;
        }
    }
}
