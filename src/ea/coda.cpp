// Allocation hooks for the sound library's CODA (codec) layer.
namespace SND {
extern void* (*CODANew)(unsigned long);
extern void (*CODADelete)(void*);

void CODASetNew(void* (*allocate)(unsigned long)) {
    CODANew = allocate;
}

void CODASetDelete(void (*release)(void*)) {
    CODADelete = release;
}
}
