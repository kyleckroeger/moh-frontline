// CDestructorQueue: objects marked for destruction wait a number of
// Execute calls before their Destroy virtual runs. The queue is a singleton
// holding an array of (object, delay) entries. The class and member function
// names come from the symbols; the member names, the entry view and the
// inline constructor and destructor are inferred. The vtable order of
// IDestructible (MarkForDestruction, the destructor, Destroy) follows
// __vt__13IDestructible.
extern "C" void MEM_free(void*);
void* DWI_allocalign(const char*, int, int, int);

class IDestructible {
public:
    virtual void MarkForDestruction(int);
    virtual ~IDestructible();
    virtual void Destroy();
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
