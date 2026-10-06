// SND::CEAXABLKDecf, the EA-XA block decoder to float samples: allocation
// through the CODA hooks, construction and feeding a block of input.
// CEAXABLKDecf and XAFSTATE are named by the mangled symbols; the members are
// inferred from offsets and are not original.
namespace SND {
extern void* (*CODANew)(unsigned long);
extern void (*CODADelete)(void*);

struct XAFSTATE {
    float previous0;
    float previous1;
};

class CEAXABLKDecf {
public:
    CEAXABLKDecf();
    void* operator new(unsigned long);
    void operator delete(void*);
    int Feed(void*, int, int);

    int field0;
    int field4;
    int m_frames;
    int m_bytes;
    char field10[136];
    XAFSTATE m_state;
    void* m_data;
    int fieldA4;
};

// decodexac comes first in the original file, and Decode, GetState and
// SetState follow Feed; they are not reconstructed, so this unit covers the
// allocation, constructor and Feed only.

void* CEAXABLKDecf::operator new(unsigned long size) {
    return CODANew(size);
}

void CEAXABLKDecf::operator delete(void* p) {
    CODADelete(p);
}

CEAXABLKDecf::CEAXABLKDecf() {
    m_bytes = 0;
    m_frames = 0;
    field4 = 0;
    m_state.previous0 = 0.0f;
    m_state.previous1 = 0.0f;
}

int CEAXABLKDecf::Feed(void* data, int bytes, int frames) {
    if (!data)
        return -1;
    if (m_frames)
        return -1;
    m_frames = frames;
    m_bytes = bytes;
    m_data = data;
    return 0;
}
}
