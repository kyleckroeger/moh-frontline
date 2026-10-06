// CQuaternion::EndianSwap, the first function of the file: byte-swaps the
// vector part and then the scalar part. CQuaternion is named by the mangled
// symbols; the member layout (scalar first) is inferred from the order of the
// swaps. The conversion helpers are the inlined ones described in propdat.cpp
// (inferred). The rotation, interpolation and matrix functions after it are
// not part of this unit.
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

class CQuaternion {
public:
    void EndianSwap();

    float w;
    float x;
    float y;
    float z;
};

void CQuaternion::EndianSwap() {
    ::EndianSwap(x, true);
    ::EndianSwap(y, true);
    ::EndianSwap(z, true);
    ::EndianSwap(w, true);
}
