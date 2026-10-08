// A fragment of sphere.cpp (0x800cbb24): the CVolSphere destructor (its virtual
// table pointer, then IVolume's through the inline base destructor, and the
// object freed when asked). The file name is this project's; the original
// record is sphere.cpp, and the destructor follows sphere_create.cpp. IVolume and
// CVolSphere are named by the mangled symbols; only the virtuals the destructor
// needs are declared. CVolSphere declares Create (defined elsewhere) before its
// destructor, so its own virtual table, which belongs with the destructor in
// the original file, is not emitted twice: the override keeps IVolume's slot
// order either way. IVolume's destructor is inline (weak in the original), so
// the compiler's copies of it and of IVolume's weak virtual table are weak
// duplicates, linked to the original copies.
class IVolume {
public:
    virtual ~IVolume() {}
};

class CVolSphere : public IVolume {
public:
    virtual IVolume* Create() const;
    virtual ~CVolSphere();
};

CVolSphere::~CVolSphere() {
}
