// A fragment of capsule.cpp (0x800c746c): the CVolCapsule destructor (its virtual
// table pointer, then IVolume's through the inline base destructor, and the
// object freed when asked). The file name is this project's; the original
// record is capsule.cpp, and the destructor follows capsule_create.cpp. IVolume and
// CVolCapsule are named by the mangled symbols; only the virtuals the destructor
// needs are declared. CVolCapsule declares Create (defined elsewhere) before its
// destructor, so its own virtual table, which belongs with the destructor in
// the original file, is not emitted twice: the override keeps IVolume's slot
// order either way. IVolume's destructor is inline (weak in the original), so
// the compiler's copies of it and of IVolume's weak virtual table are weak
// duplicates, linked to the original copies.
class IVolume {
public:
    virtual ~IVolume() {}
};

class CVolCapsule : public IVolume {
public:
    virtual IVolume* Create() const;
    virtual ~CVolCapsule();
};

CVolCapsule::~CVolCapsule() {
}
