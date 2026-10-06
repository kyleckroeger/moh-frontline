// Endian conversion of the animated-object collision data: the collision data
// (box count and a second word, then each 128-byte box), each box (its volume,
// then one word at +112) and CVolBox (width, depth and height, the three basis
// rows and the centre, 16-byte rows). The names come from the mangled symbols;
// the members are inferred from offsets and the classes are non-virtual views.
// The conversion helpers are the inlined ones described in propdat.cpp
// (inferred). These functions are inline in the original (weak symbols), so
// they are defined __declspec(weak). The rest of the file is not part of this
// unit.
inline void ChangeEndian(int& value) {
    unsigned char bytes[4];
    *reinterpret_cast<int*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[3];
    bytes[3] = c;
    c = bytes[1];
    bytes[1] = bytes[2];
    bytes[2] = c;
    value = *reinterpret_cast<int*>(bytes);
}

inline void EndianSwap(float& value, bool) {
    ChangeEndian(*reinterpret_cast<int*>(&value));
}

// The game's 16-byte vector; its EndianSwap is always inlined (no symbol), so
// the method here is inferred.
struct CVector3 {
    void EndianSwap() {
        ::EndianSwap(x, true);
        ::EndianSwap(y, true);
        ::EndianSwap(z, true);
    }

    float x;
    float y;
    float z;
    float w;
};

class CVolBox {
public:
    void EndianSwap();

    unsigned char unknown00[4];
    float m_width;
    float m_depth;
    float m_height;
    CVector3 m_rows[3];
    CVector3 m_center;
    unsigned char unknown50[32];
};

struct CObjLocationCollisionBox {
    CVolBox box;
    int field70;
    unsigned char unknown74[12];
};

struct CObjLocationCollisionData {
    CObjLocationCollisionBox boxes[24];
    int count;
    int field0c04;
};

void EndianSwap(CObjLocationCollisionBox&);

__declspec(weak) void EndianSwap(CObjLocationCollisionData& data) {
    ChangeEndian(data.count);
    ChangeEndian(data.field0c04);
    for (int i = 0; i < data.count; i++)
        EndianSwap(data.boxes[i]);
}

__declspec(weak) void EndianSwap(CObjLocationCollisionBox& box) {
    box.box.EndianSwap();
    ChangeEndian(box.field70);
}

__declspec(weak) void CVolBox::EndianSwap() {
    ::EndianSwap(m_width, true);
    ::EndianSwap(m_depth, true);
    ::EndianSwap(m_height, true);
    m_rows[0].EndianSwap();
    m_rows[1].EndianSwap();
    m_rows[2].EndianSwap();
    m_center.EndianSwap();
}
