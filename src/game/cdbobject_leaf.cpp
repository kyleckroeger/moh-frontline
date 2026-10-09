// A fragment of cdbobject.cpp (0x80072ab0): the weak instances of
// CCDBObject::TestCDBLeafNodeCollision<T> for CVolBox, CVolSphere,
// CVolCapsule and CAnimatedVolume, the last code of the file. Each builds a
// CCDBVolBox from the leaf's box, tests the volume against it and, unless
// the result is 1, gathers each of the leaf's tri groups not yet gathered
// (a positive mark, then negated) into the caller's vector; the two
// OSGetTick calls are unused, as in the image. The file name is this
// project's; the original record is cdbobject.cpp. The classes, functions
// and templates are named by the mangled symbols; the layouts and member
// names are inferred views, and the tests return int-sized results
// (inferred). CCDBVolBox's override for CVolBox is inline (weak in the
// original: it calls CVolBox's test), and it declares an override defined
// elsewhere first so its table is not emitted here; the compiler's copy of
// its inline destructor is a weak duplicate. The explicit instantiations of
// the weak template stand in for the implicit ones made by the branch-node
// tests (the compiler emits them in the reverse order).
class CMatrix;
class CPlane;
class CTriangle;
class CLine3;
class CVolSphere;
class CVolCapsule;
class CCDBObject;
class CWorldVolume;
class CAnimatedVolume;
class CVolFiber;
class CCollision;

/* Inferred: CVector3 as four floats overlaid with two doubles; 16-aligned. */
union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3Data d;
} __attribute__((aligned(16)));

/* IVolume's virtual functions in the order of __vt__7IVolume. */
class IVolume {
public:
    virtual ~IVolume();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
    virtual int TestCollision(const IVolume&, CCollision&, bool) const;
    virtual int TestCollision(const class CVolBox&, CCollision&, bool) const;
    virtual int TestCollision(const CVolSphere&, CCollision&, bool) const;
    virtual int TestCollision(const CVolCapsule&, CCollision&, bool) const;
    virtual int TestCollision(const CCDBObject&, CCollision&, bool) const;
    virtual int TestCollision(const CVolFiber&, CCollision&, bool) const;
    virtual int TestCollision(const CAnimatedVolume&, CCollision&, bool) const;
    virtual int TestCollision(const CWorldVolume&, CCollision&, bool) const;
    virtual int TestCollision(CVector3, CCollision&, bool) const;
    virtual int TestCollision(const CLine3&, CCollision&, bool) const;
    virtual int TestCollision(const CPlane&, CCollision&, bool) const;
    virtual int TestCollision(const CTriangle&, CCollision&, bool) const;
};

/* The box's extents and its four rows (the vectors make it 16-aligned). */
class CVolBox : public IVolume {
public:
    virtual ~CVolBox();
    int TestCollision(const CVolBox&, CCollision&, bool) const;

    float m_values[3];
    CVector3 m_vectors[4];
};

class CVolSphere : public IVolume {};
class CVolCapsule : public IVolume {};
class CAnimatedVolume : public CVolSphere {};

struct CDBBox;

class CCDBVolBox : public CVolBox {
public:
    virtual int TestCollision(const CTriangle&, CCollision&, bool) const;
    CCDBVolBox(const CDBBox&);
    virtual ~CCDBVolBox() {}
    virtual int TestCollision(const CVolBox& other, CCollision& collision, bool flag) const {
        return CVolBox::TestCollision(other, collision, flag);
    }
    virtual int TestCollision(const CVolSphere&, CCollision&, bool) const;
    virtual int TestCollision(const CVolCapsule&, CCollision&, bool) const;
    virtual int TestCollision(const CAnimatedVolume&, CCollision&, bool) const;
};

extern "C" unsigned int OSGetTick();

namespace dwi {
// Inferred: the vector's storage and size; push_back appends in place.
template <class T>
class fast_vec {
public:
    void push_back(const T& x) { m_data[m_size++] = x; }

    T* m_data;
    int m_unknown4;
    int m_size;
};
}

// Inferred: a tri group's mark at +16 (negated once gathered).
struct CDBTriGroup {
    unsigned char unknown00[16];
    int m_mark;
};

// Inferred: a leaf node's box at +4, then its tri groups.
struct CDBLeafNode {
    int m_type;
    unsigned char m_box[60];
    int m_groupCount;
    CDBTriGroup* m_groups[1];
};

/* CCDBObject's destructor (defined elsewhere) is declared first so its table
   is not emitted here. */
class CCDBObject : public IVolume {
public:
    virtual ~CCDBObject();
    template <class T>
    int TestCDBLeafNodeCollision(const CDBLeafNode&, const T&, CCollision&, bool, dwi::fast_vec<CDBTriGroup*>&) const;
};

template <class T>
__declspec(weak) int CCDBObject::TestCDBLeafNodeCollision(const CDBLeafNode& leaf, const T& volume, CCollision& collision,
                                                         bool flag, dwi::fast_vec<CDBTriGroup*>& groups) const {
    OSGetTick();
    CCDBVolBox box(*(const CDBBox*)leaf.m_box);
    int result = box.TestCollision(volume, collision, false);
    if (result != 1) {
        for (int i = 0; i < leaf.m_groupCount; i++) {
            CDBTriGroup* group = leaf.m_groups[i];
            if (group->m_mark > 0) {
                group->m_mark = -group->m_mark;
                groups.push_back(group);
            }
        }
    }
    OSGetTick();
    return result;
}

template int CCDBObject::TestCDBLeafNodeCollision<CAnimatedVolume>(const CDBLeafNode&, const CAnimatedVolume&, CCollision&, bool,
                                                                   dwi::fast_vec<CDBTriGroup*>&) const;
template int CCDBObject::TestCDBLeafNodeCollision<CVolCapsule>(const CDBLeafNode&, const CVolCapsule&, CCollision&, bool,
                                                               dwi::fast_vec<CDBTriGroup*>&) const;
template int CCDBObject::TestCDBLeafNodeCollision<CVolSphere>(const CDBLeafNode&, const CVolSphere&, CCollision&, bool,
                                                              dwi::fast_vec<CDBTriGroup*>&) const;
template int CCDBObject::TestCDBLeafNodeCollision<CVolBox>(const CDBLeafNode&, const CVolBox&, CCollision&, bool,
                                                           dwi::fast_vec<CDBTriGroup*>&) const;
