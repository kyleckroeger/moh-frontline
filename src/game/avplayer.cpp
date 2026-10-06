// The class deletes of RCMP::AV_MS_TIMER and RCMP::DECODER, which free through
// the RCMP system's free hook. The names come from the mangled symbols; the
// hook slot of RCMP_SYSTEM is inferred (as in rcmpbase.cpp). They are inline
// in the original (weak symbols), so they are defined __declspec(weak). The
// rest of the file is not part of this unit.
namespace RCMP {

class RCMP_SYSTEM {
public:
    virtual ~RCMP_SYSTEM();

    void* (*m_alloc)(unsigned long);
    void (*m_free)(void*);
};

extern RCMP_SYSTEM rcmp_sys;

class AV_MS_TIMER {
public:
    void operator delete(void*);
};

class DECODER {
public:
    void operator delete(void*);
};

__declspec(weak) void AV_MS_TIMER::operator delete(void* p) {
    rcmp_sys.m_free(p);
}

__declspec(weak) void DECODER::operator delete(void* p) {
    rcmp_sys.m_free(p);
}

}
