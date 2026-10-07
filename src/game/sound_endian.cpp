// A fragment of sound.cpp (0x800edc3c): EndianSwap(SOUND_MULTISUBTITLEDATA&),
// converting the record's ten 32-bit fields in order. The record is named by
// the mangled symbol; its layout is inferred. The byte-order helpers are the
// inlined ones described in propdat.cpp (inferred).
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

struct SOUND_MULTISUBTITLEDATA {
    int fields[10];
};

void EndianSwap(SOUND_MULTISUBTITLEDATA& data) {
    ChangeEndian(data.fields[0]);
    ChangeEndian(data.fields[1]);
    ChangeEndian(data.fields[2]);
    ChangeEndian(data.fields[3]);
    ChangeEndian(data.fields[4]);
    ChangeEndian(data.fields[5]);
    ChangeEndian(data.fields[6]);
    ChangeEndian(data.fields[7]);
    ChangeEndian(data.fields[8]);
    ChangeEndian(data.fields[9]);
}
