// Behaviour-script events: the functions at the start of the file, the
// process-state flag, the event memory blocks taken from g_pMemBlockAllocator
// and the declared-state search. The structure names come from the mangled
// symbols; the members and the memory-block view are inferred from offsets.
class BSUtilObjectInstanceMemoryAllocator {
public:
    bool DoWeOwnThisMemory(void*);
    void FreeElement(void*);
    void* GetFreeElement(bool);

private:
    unsigned char unknown00[28];
};

struct BSEventMemoryBlockView {
    unsigned char unknown00[32];
    int referenceCount;
};

struct BSCode_EventHandlerEntry_struct {
    unsigned int code;
    unsigned short eventNumber;
    unsigned char unknown06;
    unsigned char flags;
};

struct BSStateView {
    unsigned int code;
    BSCode_EventHandlerEntry_struct* events;
    void* messages;
    unsigned short parent;
    unsigned char depth;
    unsigned char messageCount;
    unsigned char eventCount;
};

struct BSCode_struct {
    int** codeBases;
    unsigned char unknown04[4];
    BSStateView** states;
};

struct BSMachineThread_struct {
    int* frame;
    int* stackTop;
    void* context;
    int* instruction;
    BSStateView* state;
    BSCode_struct* level;
    void* messages;
    unsigned short stateId;
    unsigned short flags;
};

extern BSUtilObjectInstanceMemoryAllocator g_pMemBlockAllocator;
extern int g_eProcessState;

void BSEventCurrentlyProcessingMessages(bool processing) {
    int state = 0;
    if (processing)
        state = 3;
    g_eProcessState = state;
}

bool BSEventIsEventMemoryBlock(void* block) {
    return g_pMemBlockAllocator.DoWeOwnThisMemory(block);
}

void BSEventCheckMemBlockReferenceCount(void* block) {
    BSEventMemoryBlockView* view = (BSEventMemoryBlockView*)block;

    if (view->referenceCount <= 0 && view->referenceCount != (int)0xFFFF0000) {
        view->referenceCount = 0xFFFF0000;
        g_pMemBlockAllocator.FreeElement(block);
    }
}

void* BSEventGetEventMemoryBlock(unsigned int) {
    BSEventMemoryBlockView* block = (BSEventMemoryBlockView*)g_pMemBlockAllocator.GetFreeElement(true);
    block->referenceCount = 0;
    return block;
}

unsigned short BSEventFindDeclaredState(unsigned short eventNumber, BSMachineThread_struct* thread,
                                        BSCode_EventHandlerEntry_struct** handler) {
    BSStateView* state;
    BSStateView** states = thread->level->states;
    unsigned short stateId = thread->stateId;

    while (stateId != 0xFFFF) {
        state = states[stateId];
        BSCode_EventHandlerEntry_struct* entry = state->events;

        for (int i = 0; i < state->eventCount; i++, entry++) {
            if (entry->flags & 4) {
                *handler = 0;
                return 0xFFFF;
            }
            if (entry->eventNumber == eventNumber) {
                if (entry->flags & 1) {
                    *handler = entry;
                    return stateId;
                }
                *handler = 0;
                return 0xFFFF;
            }
            if (entry->eventNumber > eventNumber)
                break;
        }
        stateId = state->parent;
    }
    *handler = 0;
    return 0xFFFF;
}
