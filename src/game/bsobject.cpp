// Behaviour-script objects: creation from a script class (one machine thread
// per code block, with the threads' context words after them), event
// triggering across the threads, destruction, and the 24-byte object
// allocator. The structure names come from the mangled symbols; the members
// are inferred from offsets.
struct BSCode_struct {
    unsigned char unknown00[20];
    unsigned short contextSize;
    unsigned char unknown16[6];
};

struct BSClass_struct {
    BSCode_struct* codes;
    unsigned char unknown04[84];
    unsigned short codeCount;
};

struct BSMachineThread_struct {
    unsigned char unknown00[32];
};

struct BSCode_EventHandlerEntry_struct;
struct BSGO_Basic;
struct TriggerObject_struct;

struct BSObject {
    BSClass_struct* scriptClass;
    BSMachineThread_struct* threads;
    TriggerObject_struct* triggerObject;
    BSGO_Basic* basic;
    int uniqueID;
    bool initialised;
};

class BSUtilObjectInstanceMemoryAllocator {
public:
    ~BSUtilObjectInstanceMemoryAllocator();
    void FreeElement(void*);
    void* GetFreeElement(bool);
    int ComputeNeededSize(int, int, int);
    void Init(void*, int, int, int);

private:
    unsigned char unknown00[28];
};

unsigned short BSEventFindDeclaredState(unsigned short, BSMachineThread_struct*, BSCode_EventHandlerEntry_struct**);
void BSEventQueueImmediateEvent(BSObject*, BSMachineThread_struct*, BSCode_EventHandlerEntry_struct*, unsigned short,
                                void*, BSObject*);
void BSEventTriggerEvent(BSObject*, BSMachineThread_struct*, BSCode_EventHandlerEntry_struct*, unsigned short, void*,
                         BSObject*);
void BSEventProcessImmediateEvents();
bool BSEventIsEventMemoryBlock(void*);
void BSEventCheckMemBlockReferenceCount(void*);
int BSDestroyThread(BSMachineThread_struct*);
int BSCreateThread(BSObject*, BSCode_struct*, BSMachineThread_struct*, void*);
void BSUnregisterTimerEvents(BSObject*);
void BSUtilFreeObjectInstanceMemory(void*);
void* BSUtilGetObjectInstanceMemory(BSClass_struct*);
void* BSUtilGetMemory(int, int);
int BSFileGetClass(const char*, BSClass_struct**);

static int g_uniqueIDCount;
static BSUtilObjectInstanceMemoryAllocator g_pBSObjectAllocator;

bool BSObjectTriggerEvent(BSObject* object, unsigned short event, void* data, BSObject* sender, bool immediate) {
    bool handled = false;
    unsigned short count = object->scriptClass->codeCount;
    BSMachineThread_struct* thread = object->threads;

    if (object->uniqueID != -1 && (object->initialised || !immediate || event == 44)) {
        for (int i = 0; i < count; i++, thread++) {
            BSCode_EventHandlerEntry_struct* handler;
            unsigned short state = BSEventFindDeclaredState(event, thread, &handler);
            if (handler) {
                handled = true;
                if (immediate)
                    BSEventQueueImmediateEvent(object, thread, handler, state, data, sender);
                else
                    BSEventTriggerEvent(object, thread, handler, state, data, sender);
            }
        }
        if (immediate)
            BSEventProcessImmediateEvents();
    }
    if (BSEventIsEventMemoryBlock(data) && !handled)
        BSEventCheckMemBlockReferenceCount(data);
    if (event == 44 && !handled)
        object->initialised = true;
    return handled;
}

int DestroyBSObject(BSObject* object) {
    object->uniqueID = -1;
    for (unsigned int i = 0; i < object->scriptClass->codeCount; i++)
        BSDestroyThread(&object->threads[i]);
    BSUnregisterTimerEvents(object);
    BSUtilFreeObjectInstanceMemory(object->threads);
    g_pBSObjectAllocator.FreeElement(object);
    return 0;
}

int CreateBSObject(const char* name, BSObject** object, BSGO_Basic* basic, TriggerObject_struct* triggerObject,
                   void** handle) {
    *object = (BSObject*)g_pBSObjectAllocator.GetFreeElement(true);
    if (handle)
        *handle = *object;
    (*object)->scriptClass = 0;
    BSFileGetClass(name, &(*object)->scriptClass);
    if (!(*object)->scriptClass)
        return 13;
    (*object)->threads = (BSMachineThread_struct*)BSUtilGetObjectInstanceMemory((*object)->scriptClass);
    int context = 0;
    (*object)->uniqueID = g_uniqueIDCount++;
    (*object)->triggerObject = triggerObject;
    (*object)->basic = basic;
    (*object)->initialised = false;
    int* contexts = (int*)&(*object)->threads[(*object)->scriptClass->codeCount];
    for (unsigned int i = 0; i < (*object)->scriptClass->codeCount; i++) {
        BSCode_struct* code = &(*object)->scriptClass->codes[i];
        int result = BSCreateThread(*object, code, &(*object)->threads[i], contexts + context);
        if (result)
            return result;
        context += code->contextSize;
    }
    return 0;
}

int BSInitBSObject() {
    void* memory = BSUtilGetMemory(g_pBSObjectAllocator.ComputeNeededSize(24, 880, 0), 0);
    g_pBSObjectAllocator.Init(memory, 24, 880, 0);
    return 0;
}

int BSObjectGetMemoryRequirements() {
    return g_pBSObjectAllocator.ComputeNeededSize(24, 880, 0);
}
