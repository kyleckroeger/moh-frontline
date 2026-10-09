// The end of bsutil.cpp (0x8003bedc): the static initialisation of the
// three object allocators (built through their constructor by
// __construct_array, destroyed through the generated array destructor that
// follows), then the weak, empty allocator constructor (the copy the other
// units' duplicates link to). The rest of the file is in other units.
// g_pMemAllocators and BSUtilObjectInstanceMemoryAllocator are named by the
// symbols; the array size comes from the symbol.
class BSUtilObjectInstanceMemoryAllocator {
public:
    BSUtilObjectInstanceMemoryAllocator() {}
    ~BSUtilObjectInstanceMemoryAllocator();

    unsigned char unknown00[28];
};

static BSUtilObjectInstanceMemoryAllocator g_pMemAllocators[3];
