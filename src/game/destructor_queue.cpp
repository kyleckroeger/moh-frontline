// CDestructorQueue: objects marked for destruction wait a number of
// Execute calls before their Destroy virtual runs. The queue is a singleton
// holding an array of (object, delay) entries; IDestructible::MarkForDestruction
// adds an object with its delay unless it is already queued (then it reports
// the duplicate). The whole file: MarkForDestruction is IDestructible's key
// function, so the file holds its virtual table, type information and class
// name (compiled with RTTI on); its inline destructor and Destroy are weak
// duplicates, and the strings link to the file's .rodata pool. The class and
// member function names come from the symbols; the member names, the entry
// view and the inline constructor, destructor, begin/end and push_back
// helpers are inferred (push_back indexes through the singleton, as the code
// reloads it). The vtable order of IDestructible (MarkForDestruction, the
// destructor, Destroy) follows __vt__13IDestructible.
extern "C" void MEM_free(void*);
void DebugMsg(const char*, ...);
void* DWI_allocalign(const char*, int, int, int);

class IDestructible {
public:
    virtual void MarkForDestruction(int);
    virtual ~IDestructible() {}
    virtual void Destroy() {}
};

struct DestructorQueueEntryView {
    IDestructible* object;
    int delay;
};

class CDestructorQueue {
public:
    CDestructorQueue(int capacity) {
        m_entries = (DestructorQueueEntryView*)DWI_allocalign(0, capacity * sizeof(DestructorQueueEntryView), 16, 1024);
        m_capacity = capacity;
        m_count = 0;
    }
    ~CDestructorQueue() {
        MEM_free(m_entries);
        m_entries = 0;
        m_capacity = 0;
        m_count = 0;
    }

    DestructorQueueEntryView* begin() { return m_entries; }
    DestructorQueueEntryView* end() { return m_entries + m_count; }
    void push_back(IDestructible* object, int delay) {
        DestructorQueueEntryView* entry = &sm_pSingleton->m_entries[m_count++];
        entry->object = object;
        entry->delay = delay;
    }

    static void Execute();
    static void Shutdown();
    static void Reset();
    static void Init(int);

    DestructorQueueEntryView* m_entries;
    int m_capacity;
    int m_count;

    static CDestructorQueue* sm_pSingleton;
};

void CDestructorQueue::Execute() {
    DestructorQueueEntryView* dst = sm_pSingleton->m_entries;
    DestructorQueueEntryView* src = sm_pSingleton->m_entries;
    DestructorQueueEntryView* end = sm_pSingleton->m_entries + sm_pSingleton->m_count;

    for (; src != end; src++) {
        if (src->delay-- <= 0) {
            src->object->Destroy();
            sm_pSingleton->m_count--;
        } else {
            *dst++ = *src;
        }
    }
}

void CDestructorQueue::Shutdown() {
    delete sm_pSingleton;
    sm_pSingleton = 0;
}

void CDestructorQueue::Reset() {
    sm_pSingleton->m_count = 0;
}

void CDestructorQueue::Init(int capacity) {
    sm_pSingleton = new CDestructorQueue(capacity);
}

void IDestructible::MarkForDestruction(int delay) {
    CDestructorQueue* queue = CDestructorQueue::sm_pSingleton;
    for (DestructorQueueEntryView* it = queue->begin(); it != queue->end(); it++) {
        if (it->object == this) {
            DebugMsg("Object already added to destructor queue. Ignoring duplicate request\n");
            return;
        }
    }
    queue->push_back(this, delay);
}
