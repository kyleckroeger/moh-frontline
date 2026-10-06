// The static mesh's fogging flag (weak) and the animated mesh's frame timing
// read from its animation header. CStaticMesh and CStaticAnimMesh are named by
// the mangled symbols; the flag byte and the header are inferred views. The
// rest of the file is not part of this unit.
class CStaticMesh {
public:
    bool IsFoggingEnabled();

    unsigned char unknown00[8];
    unsigned char flag0 : 1;
    bool fogging : 1;
    unsigned char flag2 : 6;
};

struct StaticAnimHeaderView {
    unsigned char unknown00[12];
    int numFrames;
    int ticksPerFrame;
};

class CStaticAnimMesh {
public:
    int ticksPerFrame() const;
    int numFrames() const;

    unsigned char unknown00[12];
    StaticAnimHeaderView* m_header;
};

__declspec(weak) bool CStaticMesh::IsFoggingEnabled() {
    return fogging;
}

int CStaticAnimMesh::ticksPerFrame() const {
    return m_header->ticksPerFrame;
}

int CStaticAnimMesh::numFrames() const {
    return m_header->numFrames;
}
