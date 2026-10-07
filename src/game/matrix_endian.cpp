// A fragment of matrix.cpp (0x8008ebf0): CMatrix::EndianSwap (each element,
// column by column), GetGAMECUBEMatrix34 (the transposed upper 3x4) and
// TibToMOHFL (negates the first row and swaps the second and third), then
// ToEulerXZY and ToEulerXYZ (asin/atan2 of the rotation entries, with the
// +-pi/2 cases when the sine entry reaches +-1; the constants are entries of
// the file's .sdata2 pool). CMatrix
// is named by the mangled symbols; the 4x4 float layout is inferred. The
// byte-order helpers are the inlined ones described in propdat.cpp (inferred).
extern "C" double atan2(double, double);
extern "C" double asin(double);

inline void ChangeEndian(short& value) {
    unsigned char bytes[2];
    *reinterpret_cast<short*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[1];
    bytes[1] = c;
    value = *reinterpret_cast<short*>(bytes);
}

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

inline void ChangeEndian(unsigned short& value) {
    ChangeEndian(*reinterpret_cast<short*>(&value));
}

template <class T> inline void ChangeEndian(T& value) {
    ChangeEndian(*reinterpret_cast<int*>(&value));
}

class CMatrix {
public:
    void EndianSwap();
    void GetGAMECUBEMatrix34(float (&)[3][4]) const;
    void TibToMOHFL();
    void ToEulerXZY(float&, float&, float&) const;
    void ToEulerXYZ(float&, float&, float&) const;

    float m[4][4];
} __attribute__((aligned(16)));

void CMatrix::EndianSwap() {
    ChangeEndian(m[0][0]);
    ChangeEndian(m[1][0]);
    ChangeEndian(m[2][0]);
    ChangeEndian(m[3][0]);
    ChangeEndian(m[0][1]);
    ChangeEndian(m[1][1]);
    ChangeEndian(m[2][1]);
    ChangeEndian(m[3][1]);
    ChangeEndian(m[0][2]);
    ChangeEndian(m[1][2]);
    ChangeEndian(m[2][2]);
    ChangeEndian(m[3][2]);
    ChangeEndian(m[0][3]);
    ChangeEndian(m[1][3]);
    ChangeEndian(m[2][3]);
    ChangeEndian(m[3][3]);
}

void CMatrix::GetGAMECUBEMatrix34(float (&out)[3][4]) const {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++)
            out[i][j] = m[j][i];
    }
}

void CMatrix::TibToMOHFL() {
    m[0][0] = -m[0][0];
    m[0][1] = -m[0][1];
    m[0][2] = -m[0][2];
    for (int i = 0; i < 3; i++) {
        float t = m[1][i];
        m[1][i] = m[2][i];
        m[2][i] = t;
    }
}

void CMatrix::ToEulerXZY(float& x, float& y, float& z) const {
    if (m[0][1] < 1.0f) {
        if (m[0][1] > -1.0f) {
            y = atan2(-m[0][2], m[0][0]);
            z = asin(m[0][1]);
            x = atan2(-m[2][1], m[1][1]);
        } else {
            x = -(float)atan2(m[1][2], m[2][2]);
            z = -1.5707964f;
            y = 0.0f;
        }
    } else {
        x = atan2(m[1][2], m[2][2]);
        z = 1.5707964f;
        y = 0.0f;
    }
}

void CMatrix::ToEulerXYZ(float& x, float& y, float& z) const {
    if (m[0][2] < 1.0f) {
        if (m[0][2] > -1.0f) {
            x = asin(-m[1][2]);
            y = atan2(-m[0][2], m[2][2]);
            z = atan2(m[0][1], m[0][0]);
        } else {
            x = -1.5707964f;
            y = -(float)atan2(m[1][0], m[1][1]);
            z = 0.0f;
        }
    } else {
        x = 1.5707964f;
        y = atan2(m[1][0], m[1][1]);
        z = 0.0f;
    }
}
