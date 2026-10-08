// A fragment of anim_obj.cpp (0x800959a4): the CAnimObject default
// constructor (the scene-node base constructors: table pointers and eight
// cleared words; then the members in order: the skinned animation at +48,
// 80 transforms at +3904 through the CMatrix array wrapper, a transform at
// +9024, the collision list at +9272 (an empty dwi::fast_vec that owns its
// storage, growth -1; the list's constructor is inferred), a transform at
// +9312, the light-volume manager at +9400 and 128 collision records at
// +9432; then the animated volume pointer cleared) and the weak, empty
// CAnimObject::CClsnInfo constructor the record array uses. The file name is
// this project's; the original record is anim_obj.cpp. The classes and
// functions are named by the mangled symbols; the member positions come from
// the code (the transform at +9312 from the gap Init leaves), while their
// names, the record's contents and the space between members are inferred,
// and only the virtuals the constructor needs are declared. CAnimObject
// declares MarkForDestruction (defined elsewhere) before its destructor so
// its global virtual table is not emitted here. CMatrix's default constructor
// takes an unknown default argument (the compiler's __defctor__7CMatrixFv
// array wrapper). That wrapper, the collision list's weak destructor (kept in
// anim_obj_fastvec.cpp) and the scene-node bases' weak tables and inline
// destructors are weak duplicates, linked to the original copies.
extern "C" void MEM_free(void*);

class IDestructible {
public:
    IDestructible() {}
    virtual void MarkForDestruction(int);
    virtual ~IDestructible();
};

class ISubject : public IDestructible {
public:
    ISubject() {}
    virtual ~ISubject();
};

class IObserver : public ISubject {
public:
    IObserver() {}
    virtual ~IObserver();
};

class ISceneNode : public IObserver {
public:
    ISceneNode() : data04(0), data08(0), data0c(0), data10(0), data14(0), data18(0), data1c(0), data20(0) {}
    virtual ~ISceneNode() {}

    int data04;
    int data08;
    int data0c;
    int data10;
    int data14;
    int data18;
    int data1c;
    int data20;
};

class IMovingSceneNode : public ISceneNode {
public:
    IMovingSceneNode() {}
    virtual ~IMovingSceneNode() {}
};

namespace dwi {
template <class T>
class fast_vec {
public:
    fast_vec() : m_data(0), m_unknown4(0), m_size(0), m_capacity(0), m_owned(true), m_growth(-1) {}
    ~fast_vec() {
        if (m_owned && m_data)
            MEM_free(m_data);
    }

    T* m_data;
    int m_unknown4;
    int m_size;
    int m_capacity;
    bool m_owned;
    int m_growth;
};
}

class CVector3 {
public:
    float x, y, z, w;
} __attribute__((aligned(8)));

class CMatrix {
public:
    /* The default argument is not known; it does not change this code. */
    CMatrix(int unknown = 0) {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    static bool s_ClassInit;

    CVector3 right;
    CVector3 forward;
    CVector3 up;
    CVector3 position;
};

class CLightVolumeManager {
public:
    CLightVolumeManager();
    ~CLightVolumeManager();

    unsigned char unknown00[4];
};

class CAnimSkinned {
public:
    CAnimSkinned();
    virtual ~CAnimSkinned();

    unsigned char unknown04[44];
};

class CObjLocationCollisionData;
class CObjLocationCollisionMap;

class CAnimatedVolume {
public:
    virtual ~CAnimatedVolume();
    void Set(CObjLocationCollisionData*, CObjLocationCollisionMap*);
};

class CAnimObject : public IMovingSceneNode {
public:
    struct CClsnInfo {
        CClsnInfo() {}

        unsigned char unknown00[24];
    };
    virtual void MarkForDestruction(int);
    virtual ~CAnimObject();
    CAnimObject();

    unsigned char unknown24[12];
    CAnimSkinned m_skinned;
    unsigned char unknown0060[3808];
    CMatrix m_matrices[80];
    CMatrix m_matrix2340;
    unsigned char unknown2380[24];
    CAnimatedVolume* m_volume;
    unsigned char unknown239c[156];
    dwi::fast_vec<CClsnInfo> m_collisions;
    unsigned char unknown2450[16];
    CMatrix m_matrix2460;
    unsigned char unknown24a0[24];
    CLightVolumeManager m_lightVolumes;
    unsigned char unknown24bc[28];
    CClsnInfo m_collisionStorage[128];
};

CAnimObject::CAnimObject() {
    m_volume = 0;
}
