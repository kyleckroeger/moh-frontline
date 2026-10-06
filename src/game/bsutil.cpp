// Behaviour-script object memory: fixed-size element pools with an embedded
// free-list link after each element, three of which back object instances.
// BSUtilObjectInstanceMemoryAllocator is named by the mangled symbols; its
// members are inferred from offsets and are not original. The file is
// compiled with deferred inlining (Init inlines ComputeNeededSize, which
// follows it in the image), so functions are listed in reverse image order.
// This unit covers GetFreeElement and FreeElement only: Init and
// DoWeOwnThisMemory around them are drafted in scratch but differ in register
// numbering, and the static initialisation of g_pMemAllocators cannot be
// verified because its array destructor's compiler-numbered name
// (__arraydtor$623) depends on the rest of the file.
class BSUtilObjectInstanceMemoryAllocator {
public:
    BSUtilObjectInstanceMemoryAllocator() {}
    ~BSUtilObjectInstanceMemoryAllocator();
    int ComputeNeededSize(int, int, int);
    void Init(void*, int, int, int);
    void* GetFreeElement(bool);
    void FreeElement(void*);
    bool DoWeOwnThisMemory(void*);

    int m_elementWords;
    int m_strideWords;
    int m_maxCount;
    int m_count;
    int m_totalWords;
    unsigned long* m_base;
    unsigned long* m_free;
};

void* BSUtilObjectInstanceMemoryAllocator::GetFreeElement(bool) {
    if (m_count < m_maxCount) {
        unsigned long* element = m_free;

        m_free = (unsigned long*)element[m_elementWords];
        m_count++;
        return element;
    }
    return 0;
}

void BSUtilObjectInstanceMemoryAllocator::FreeElement(void* element) {
    ((unsigned long*)element)[m_elementWords] = (unsigned long)m_free;
    m_free = (unsigned long*)element;
    m_count--;
}

