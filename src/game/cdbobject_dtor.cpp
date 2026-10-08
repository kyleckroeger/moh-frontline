// A fragment of cdbobject.cpp (0x800712e0): the CCDBObject destructor (its virtual
// table pointer, then IVolume's through the inline base destructor, and the
// object freed when asked). The file name is this project's; the original
// record is cdbobject.cpp, and the destructor follows cdbobject_csg.cpp. IVolume and
// CCDBObject are named by the mangled symbols; only the virtuals the destructor
// needs are declared. CCDBObject declares its TestCollision override for IVolume
// (defined elsewhere) before its destructor, so its own virtual table, which
// belongs with the destructor in the original file, is not emitted twice; the
// overrides keep IVolume's slot order either way. IVolume's destructor is
// inline (weak in the original), so the compiler's copies of it and of IVolume's weak virtual table are weak
// duplicates, linked to the original copies.
class CCollision;

class IVolume {
public:
    virtual ~IVolume() {}
};

class CCDBObject : public IVolume {
public:
    virtual bool TestCollision(const IVolume&, CCollision&, bool) const;
    virtual ~CCDBObject();
};

CCDBObject::~CCDBObject() {
}
