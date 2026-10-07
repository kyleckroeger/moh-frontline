// A fragment of compartment.cpp (0x80076e14): weak copies of the render bins'
// header inline defaults emitted in this file: CUcodeRenderBin::Link and
// CRenderBin::Link do nothing and CRenderBin::Render returns 0. The classes
// are named by the mangled symbols and are non-virtual views; the result type
// is inferred. The bins' destructors after these and the rest of the file are
// not part of this unit.
class CDmaTag;
class CDmaPacket;

class CRenderBin {
public:
    int Render(CDmaPacket&, void*);
    void Link(CDmaTag*, CDmaPacket&);
};

class CUcodeRenderBin {
public:
    void Link(CDmaTag*, CDmaPacket&);
};

__declspec(weak) void CUcodeRenderBin::Link(CDmaTag*, CDmaPacket&) {
}

__declspec(weak) int CRenderBin::Render(CDmaPacket&, void*) {
    return 0;
}

__declspec(weak) void CRenderBin::Link(CDmaTag*, CDmaPacket&) {
}
