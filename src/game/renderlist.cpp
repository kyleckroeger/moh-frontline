// CRenderList's bin list and destructor: render bins are kept in a list
// ordered by priority (Register inserts a bin before the first bin of higher
// priority, or appends it; UnRegister unlinks it), and the destructor frees
// the display-list buffer. CRenderList and CRenderBin are named by the
// symbols; the list and bin layouts are inferred from offsets. The
// constructor after these functions is not part of the unit (it keeps the
// entry count in an extra register; draft in scratch/lib/renderlist_wip.cpp). Register walks the list to its end
// before and after inserting without using the result; both walks are kept.
// CRenderBin is an inferred view: its links and priority, then its virtual
// table pointer after 28 bytes, with the virtual destructor in the first slot.
extern "C" void MEM_free(void*);

class CRenderBin {
public:
    CRenderBin* m_prev;
    CRenderBin* m_next;
    unsigned char unknown08[12];
    int m_priority;
    unsigned char unknown18[4];

    virtual ~CRenderBin();
};

class CRenderList {
public:
    ~CRenderList();
    void UnRegister(CRenderBin&);
    void Register(CRenderBin&);

    void* m_buffer;
    void* m_end;
    void* m_current;
    void* m_last;
    unsigned long m_count;
    int unknown14;
    CRenderBin* m_bins;
};

void CRenderList::UnRegister(CRenderBin& bin) {
    if (m_bins == &bin)
        m_bins = bin.m_next;
    if (bin.m_prev)
        bin.m_prev->m_next = bin.m_next;
    if (bin.m_next)
        bin.m_next->m_prev = bin.m_prev;
    bin.m_next = 0;
    bin.m_prev = 0;
}

void CRenderList::Register(CRenderBin& bin) {
    CRenderBin* head = m_bins;
    CRenderBin* last;
    CRenderBin* current;
    if (!head) {
        m_bins = &bin;
        return;
    }
    for (last = head; last; last = last->m_next)
        ;
    for (current = head; current; current = current->m_next) {
        if (bin.m_priority < current->m_priority) {
            if (current == head)
                m_bins = &bin;
            bin.m_prev = current->m_prev;
            if (current->m_prev)
                current->m_prev->m_next = &bin;
            bin.m_next = current;
            current->m_prev = &bin;
            break;
        }
        if (!current->m_next) {
            bin.m_next = current->m_next;
            if (current->m_next)
                current->m_next->m_prev = &bin;
            bin.m_prev = current;
            current->m_next = &bin;
            break;
        }
    }
    for (last = m_bins; last->m_next; last = last->m_next)
        ;
}

CRenderList::~CRenderList() {
    MEM_free(m_buffer);
}
