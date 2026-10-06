// MEMCLASS_create's parameter types are not established; the arguments are
// passed exactly as the original call passes them.
extern "C" void MEMCLASS_create(int, const char*, void*, int, int, int, int, int, int, int);

extern "C" void MEM_initadr(void* address, int size) {
    MEMCLASS_create(0, "RAM", address, size, 32, 32, 0, 0, 0, 1);
}
