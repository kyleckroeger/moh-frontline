// Behaviour-script timers. Structure and names follow the Rising Sun
// reconstruction (EA's later revision of this file); field names are
// descriptive, and only the accessed fields and strides are established.

// Only the accessed member is established; the rest of BSObject is unknown.
struct BSObject {
    unsigned char unknown00[16];
    int queueIdentity;
};

struct BSTimerEvent_struct {
    int time;
    unsigned short eventNumber;
    BSObject* object;
    void* context;
    bool ownsEventMemory;
    BSTimerEvent_struct* next;
};

struct BSTimerBucketView {
    BSTimerEvent_struct* head;
    BSTimerEvent_struct* tail;
};

enum ETimerReplaceMethod {
    TIMER_REPLACE_DUPLICATES = 0,
    TIMER_KEEP_EARLIER = 1,
    TIMER_APPEND = 2
};

// Only the size (28 bytes, from the g_pMemBlockAllocator symbol) and this
// method are established.
class BSUtilObjectInstanceMemoryAllocator {
public:
    bool DoWeOwnThisMemory(void*);

private:
    unsigned char unknown00[28];
};

extern BSUtilObjectInstanceMemoryAllocator g_pMemBlockAllocator;
void BSEventDecrimentReferenceCount(void*);
void BSEventIncrimentReferenceCount(void*);
void* BSUtilGetMemory(int, int);
int BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

static BSTimerBucketView* g_pTimerHashTable;
static BSTimerEvent_struct* g_pTimerEventArray;
static BSTimerEvent_struct* g_pFreeTimerEventHead;
static int g_LastHashIndex;
static float g_CurrentTime;
static unsigned char* g_pBSTimerMemory;
static int g_pBSTimerMemoryOffset;

static void BSTimerRemoveTimerEvent(BSTimerEvent_struct* event, BSTimerEvent_struct** link) {
    if (event->ownsEventMemory) {
        BSEventDecrimentReferenceCount(event->context);
    }
    *link = event->next;
    event->next = g_pFreeTimerEventHead;
    g_pFreeTimerEventHead = event;
}

static BSTimerEvent_struct* BSGetFreeTimerEvent() {
    BSTimerEvent_struct* event = g_pFreeTimerEventHead;
    g_pFreeTimerEventHead = event->next;
    return event;
}

static void BSTimerRemoveTimerInstances(BSObject* object) {
    BSTimerEvent_struct** link;
    BSTimerEvent_struct* event;
    BSTimerEvent_struct* next;
    BSTimerEvent_struct* tail;
    int i;
    int last;
    int bucket;

    i = (int)g_CurrentTime + 1;
    last = i + 120;
    bucket = g_LastHashIndex;
    for (; i <= last; ++i) {
        if (++bucket == 120) bucket = 0;
        event = g_pTimerHashTable[bucket].head;
        link = &g_pTimerHashTable[bucket].head;
        tail = 0;
        while (event) {
            next = event->next;
            if (event->object == object) {
                BSTimerRemoveTimerEvent(event, link);
            } else {
                tail = event;
                link = &tail->next;
            }
            event = next;
        }
        g_pTimerHashTable[bucket].tail = tail;
    }
}

static void BSTimerRemoveDuplicateTimerInstances(BSObject* object, unsigned short number) {
    BSTimerEvent_struct** link;
    BSTimerEvent_struct* event;
    BSTimerEvent_struct* next;
    BSTimerEvent_struct* tail;
    int i;
    int last;
    int bucket;

    i = (int)g_CurrentTime + 1;
    last = i + 120;
    bucket = g_LastHashIndex;
    for (; i <= last; ++i) {
        if (++bucket == 120) bucket = 0;
        event = g_pTimerHashTable[bucket].head;
        link = &g_pTimerHashTable[bucket].head;
        tail = 0;
        while (event) {
            next = event->next;
            if (event->object == object && event->eventNumber == number) {
                BSTimerRemoveTimerEvent(event, link);
            } else {
                tail = event;
                link = &tail->next;
            }
            event = next;
        }
        g_pTimerHashTable[bucket].tail = tail;
    }
}

static bool BSTimerRemoveLatterTimerInstances(BSObject* object, unsigned short number, int delay) {
    BSTimerEvent_struct** link;
    BSTimerEvent_struct* event;
    BSTimerEvent_struct* next;
    BSTimerEvent_struct* tail;
    int targetTime;
    int i;
    int last;
    int bucket;
    bool result;

    targetTime = g_CurrentTime + delay;
    result = true;
    i = (int)g_CurrentTime + 1;
    last = i + 120;
    bucket = g_LastHashIndex;
    for (; i <= last; ++i) {
        if (++bucket == 120) bucket = 0;
        event = g_pTimerHashTable[bucket].head;
        link = &g_pTimerHashTable[bucket].head;
        tail = 0;
        while (event) {
            next = event->next;
            if (event->object == object && event->eventNumber == number) {
                if (event->time > targetTime) {
                    BSTimerRemoveTimerEvent(event, link);
                } else {
                    result = false;
                }
            } else {
                tail = event;
                link = &tail->next;
            }
            event = next;
        }
        g_pTimerHashTable[bucket].tail = tail;
    }
    return result;
}

void BSUnregisterTimerEvents(BSObject* object) {
    BSTimerRemoveTimerInstances(object);
}

void BSRegisterTimerEvent(int delay, unsigned short number, BSObject* object, void* context,
                          ETimerReplaceMethod replace) {
    BSTimerEvent_struct* event;
    int bucket;
    bool ownsEventMemory;

    switch (replace) {
    case TIMER_REPLACE_DUPLICATES:
        BSTimerRemoveDuplicateTimerInstances(object, number);
        break;
    case TIMER_KEEP_EARLIER:
        if (!BSTimerRemoveLatterTimerInstances(object, number, delay)) {
            if (g_pMemBlockAllocator.DoWeOwnThisMemory(context)) {
                BSEventDecrimentReferenceCount(context);
            }
            return;
        }
        break;
    }

    ownsEventMemory = false;
    if (g_pMemBlockAllocator.DoWeOwnThisMemory(context)) {
        ownsEventMemory = true;
    }
    if (ownsEventMemory) {
        BSEventIncrimentReferenceCount(context);
    }
    if (object->queueIdentity == -1) {
        if (ownsEventMemory) {
            BSEventDecrimentReferenceCount(context);
        }
        return;
    }

    event = BSGetFreeTimerEvent();
    event->eventNumber = number;
    event->object = object;
    event->time = g_CurrentTime + delay;
    event->context = context;
    event->next = 0;
    event->ownsEventMemory = ownsEventMemory;
    bucket = event->time % 120;
    if (g_pTimerHashTable[bucket].tail) {
        g_pTimerHashTable[bucket].tail->next = event;
    }
    g_pTimerHashTable[bucket].tail = event;
    if (!g_pTimerHashTable[bucket].head) {
        g_pTimerHashTable[bucket].head = event;
    }
}

void BSUpdateTimer(float dt) {
    BSTimerEvent_struct* event;
    BSTimerEvent_struct* next;
    BSTimerEvent_struct** link;
    BSTimerEvent_struct* tail;
    int previous;
    int i;
    int current;

    previous = (int)g_CurrentTime;
    g_CurrentTime += dt;
    current = (int)g_CurrentTime;
    for (i = previous + 1; i <= current; ++i) {
        if (++g_LastHashIndex == 120) g_LastHashIndex = 0;
        event = g_pTimerHashTable[g_LastHashIndex].head;
        link = &g_pTimerHashTable[g_LastHashIndex].head;
        tail = 0;
        while (event) {
            next = event->next;
            if (event->time <= g_CurrentTime) {
                BSObjectTriggerEvent(event->object, event->eventNumber, event->context, 0, false);
                BSTimerRemoveTimerEvent(event, link);
            } else {
                tail = event;
                link = &tail->next;
            }
            event = next;
        }
        g_pTimerHashTable[g_LastHashIndex].tail = tail;
    }
}

void BSEndTimer() {
    g_pTimerHashTable = 0;
    g_pTimerEventArray = 0;
    g_pFreeTimerEventHead = 0;
}

void BSInitTimer() {
    int i;

    g_pBSTimerMemory = (unsigned char*)BSUtilGetMemory(
        120 * sizeof(BSTimerBucketView) + 256 * sizeof(BSTimerEvent_struct), 0);
    g_pBSTimerMemoryOffset = 0;
    g_pTimerHashTable = (BSTimerBucketView*)(g_pBSTimerMemory + g_pBSTimerMemoryOffset);
    g_pBSTimerMemoryOffset += 120 * sizeof(BSTimerBucketView);
    for (i = 0; i < 120; i++) {
        g_pTimerHashTable[i].head = 0;
        g_pTimerHashTable[i].tail = 0;
    }
    g_pTimerEventArray = (BSTimerEvent_struct*)(g_pBSTimerMemory + g_pBSTimerMemoryOffset);
    g_pBSTimerMemoryOffset += 256 * sizeof(BSTimerEvent_struct);
    g_pFreeTimerEventHead = g_pTimerEventArray;
    for (i = 0; i < 255; i++) {
        g_pTimerEventArray[i].next = &g_pTimerEventArray[i + 1];
    }
    g_pTimerEventArray[255].next = 0;
    g_LastHashIndex = 0;
    g_CurrentTime = 0.0f;
}

int BSTimerGetMemoryRequirements() {
    return 120 * sizeof(BSTimerBucketView) + 256 * sizeof(BSTimerEvent_struct);
}
