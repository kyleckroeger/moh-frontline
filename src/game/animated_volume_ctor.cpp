// A fragment of animated_volume.cpp (0x800c021c): the CAnimatedVolume
// destructor (its virtual table pointer, the array of 24 box volumes at +48
// destroyed, then CVolSphere's destructor, and the object freed when asked)
// and the default constructor (CVolSphere's inline constructor, the table
// pointer, the 24 boxes constructed, and a byte and three words cleared). The
// file name is this project's; the original record is animated_volume.cpp.
// IVolume, CVolSphere, CVolBox and CAnimatedVolume are named by the mangled
// symbols (CAnimatedVolume derives from CVolSphere, as in
// animated_volume_dispatch.cpp); the members and their names are inferred
// views, and only the virtuals these functions need are declared. CVolBox and
// CAnimatedVolume declare Create (defined elsewhere) before their
// destructors so their global virtual tables are not emitted here. IVolume's
// destructor and CVolBox's constructor are inline (weak in the original;
// the constructor's retained copy is compartment_volbox.cpp's), so the
// compiler's copies of them and of IVolume's weak virtual table are weak
// duplicates, linked to the original copies.
class IVolume {
public:
    IVolume() {}
    virtual ~IVolume() {}
};

class CVolSphere : public IVolume {
public:
    CVolSphere() {}
    virtual IVolume* Create() const;
    virtual ~CVolSphere();

    unsigned char unknown04[28];
};

class CVolBox : public IVolume {
public:
    CVolBox() {}
    virtual IVolume* Create() const;
    virtual ~CVolBox();

    unsigned char unknown04[76];
} __attribute__((aligned(8))); /* it holds 8-aligned vectors */

class CAnimatedVolume : public CVolSphere {
public:
    virtual IVolume* Create() const;
    virtual ~CAnimatedVolume();
    CAnimatedVolume();

    unsigned char data20;
    int data24;
    int data28;
    CVolBox m_boxes[24];
    int data7b0;
};

CAnimatedVolume::~CAnimatedVolume() {
}

CAnimatedVolume::CAnimatedVolume() {
    data24 = 0;
    data20 = 0;
    data28 = 0;
    data7b0 = 0;
}
