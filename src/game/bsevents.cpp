// The static initialisation of bsevents.cpp (0x80035a34): it registers the
// destructor of the global event-memory allocator, whose constructor is
// empty. The rest of the file is in other units. g_pMemBlockAllocator and
// BSUtilObjectInstanceMemoryAllocator are named by the symbols; the object
// size comes from the symbol.
class BSUtilObjectInstanceMemoryAllocator {
public:
    BSUtilObjectInstanceMemoryAllocator() {}
    ~BSUtilObjectInstanceMemoryAllocator();

    unsigned char unknown00[28];
};

BSUtilObjectInstanceMemoryAllocator g_pMemBlockAllocator;
