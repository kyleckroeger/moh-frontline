// A fragment of avplayer.cpp (0x800fb2f4): the weak class operator delete of RCMP::CHUNK, which frees through the RCMP system's free hook. RCMP, RCMP_SYSTEM, rcmp_sys and CHUNK
// are named by the symbols; the hook slot is as in rcmp_mpc_codec.cpp. The
// delete is inline in the original (a weak symbol), so it is defined
// __declspec(weak). The rest of the file is not part of this unit.
namespace RCMP {
class RCMP_SYSTEM {
public:
    virtual ~RCMP_SYSTEM();

    void* (*m_alloc)(unsigned long);
    void (*m_free)(void*);
};

extern RCMP_SYSTEM rcmp_sys;

class CHUNK {
public:
    void operator delete(void*);
};
}

__declspec(weak) void RCMP::CHUNK::operator delete(void* p) {
    rcmp_sys.m_free(p);
}
