// A fragment of compartment.cpp (0x80074bec): the weak CVolBox default
// constructor emitted in this file (IVolume's table pointer through the
// inline base constructor, then CVolBox's). The file name is this project's;
// the original record is compartment.cpp, and the rest of the file is not
// part of this unit. IVolume and CVolBox are named by the mangled symbols;
// only the virtuals the constructor needs are declared. CVolBox declares
// Create (defined in box.cpp) before its destructor, so its global virtual
// table is not emitted here; IVolume's destructor is inline (weak in the
// original), so the compiler's copies of it and of IVolume's weak virtual
// table are weak duplicates, linked to the original copies.
class IVolume {
public:
    IVolume() {}
    virtual ~IVolume() {}
};

class CVolBox : public IVolume {
public:
    virtual IVolume* Create() const;
    virtual ~CVolBox();
    CVolBox();
};

__declspec(weak) CVolBox::CVolBox() {
}
