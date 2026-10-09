// The whole of debug_picker.cpp as linked: only the weak CLine3 destructor
// survives (nothing to destroy; the object freed when asked). CLine3 is named
// by the mangled symbol; its members are not part of this view.
class CLine3 {
public:
    ~CLine3();

    unsigned char unknown00[48];
};

__declspec(weak) CLine3::~CLine3() {
}
