// A fragment of bsutil.cpp (0x8003bea8):
// BSUtilObjectInstanceMemoryAllocator::ComputeNeededSize, the pool size for a
// count of objects (each with a 4-byte header, padded to the alignment),
// rounded up past the next 16-byte boundary. The file name is this project's;
// the original record is bsutil.cpp and the functions around it are not
// reconstructed. The class is named by the mangled symbols.
// Behaviour-script object memory: fixed-size element pools with an embedded
// free-list link after each element, three of which back object instances.
// BSUtilObjectInstanceMemoryAllocator is named by the mangled symbols; its
// members are inferred from offsets and are not original. The file is
// compiled with deferred inlining (Init inlines ComputeNeededSize, which
// follows it in the image), so functions are listed in reverse image order.
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

extern BSUtilObjectInstanceMemoryAllocator g_pMemAllocators[3];

int BSUtilObjectInstanceMemoryAllocator::ComputeNeededSize(int size, int count, int align) {
    int stride = size + 4;

    if (align && (stride & (align - 1)))
        stride += align - (stride & (align - 1));
    int total = count * stride;
    return total + (16 - (total & 15));
}

void BSUtilObjectInstanceMemoryAllocator::Init(void* memory, int size, int count, int align);

void* BSUtilObjectInstanceMemoryAllocator::GetFreeElement(bool);

void BSUtilObjectInstanceMemoryAllocator::FreeElement(void* element);

bool BSUtilObjectInstanceMemoryAllocator::DoWeOwnThisMemory(void* memory);


