// MPC codec helpers: the frame's class delete through the RCMP system's free
// hook, the stream frame rate (the decoder's double MPDframe_rate) and the
// current frame number. MPC_FRAME, MPC_CODEC_INTERNAL, RCMP_SYSTEM and
// MPDframe_rate are named by the symbols; the frame-number member and the hook
// slot are inferred. The delete is inline in the original (a weak symbol), so
// it is defined __declspec(weak). The rest of the file is not part of this
// unit.
namespace RCMP {
class RCMP_SYSTEM {
public:
    virtual ~RCMP_SYSTEM();

    void* (*m_alloc)(unsigned long);
    void (*m_free)(void*);
};

extern RCMP_SYSTEM rcmp_sys;
}

extern double MPDframe_rate;

class MPC_FRAME {
public:
    void operator delete(void*);
};

class MPC_CODEC_INTERNAL {
public:
    float GetFrameRate();
    unsigned int GetCurrentFrameNumber();

    unsigned char unknown00[32];
    unsigned int m_frameNumber;
};

__declspec(weak) void MPC_FRAME::operator delete(void* p) {
    RCMP::rcmp_sys.m_free(p);
}

float MPC_CODEC_INTERNAL::GetFrameRate() {
    return MPDframe_rate;
}

unsigned int MPC_CODEC_INTERNAL::GetCurrentFrameNumber() {
    return m_frameNumber;
}
