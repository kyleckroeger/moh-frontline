// A fragment of csg_volume.cpp (0x800c920c): the CCSGVolume destructor (its
// virtual table pointer, then IVolume's through the inline base destructor,
// and the object freed when asked) and the default constructor (IVolume's and
// CCSGVolume's table pointers, two cleared words). The file name is this
// project's; the original record is csg_volume.cpp, and the rest of the file
// is not part of this unit. IVolume and CCSGVolume are named by the mangled
// symbols; the two members are an inferred view (their meaning is not known),
// and only the virtuals these functions need are declared. CCSGVolume
// declares Create (defined elsewhere) before its destructor, so its own
// virtual table, which belongs with the destructor in the original file, is
// not emitted twice; the override keeps IVolume's slot order either way.
// IVolume's destructor is inline (weak in the original), so the compiler's
// copies of it and of IVolume's weak virtual table are weak duplicates,
// linked to the original copies.
class IVolume {
public:
    IVolume() {}
    virtual ~IVolume() {}
};

class CCSGVolume : public IVolume {
public:
    virtual IVolume* Create() const;
    virtual ~CCSGVolume();
    CCSGVolume();

    int data04;
    int data08;
};

CCSGVolume::~CCSGVolume() {
}

CCSGVolume::CCSGVolume() {
    data04 = 0;
    data08 = 0;
}
