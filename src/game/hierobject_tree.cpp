// A fragment of hierobject.cpp (0x8009ba2c): CHierObject::RemoveObjectFromTree
// (unlinks an object from the child lists below this one, clearing its
// sub-object slot in each root it passes; the compiler inlines the recursion),
// GetAttachPointMatrix (copies the matrix of the attach point with the given
// id from the mesh's attach-point table), GetSubObject, SetRotation and Init
// (links the object under its root's sub-object at the parent index, taking
// the attach point's matrix, and resets the buffer at +1680 to its 32-entry
// inline storage), the destructor and the two constructors. The classes are
// named by the mangled symbols; the members, the attach-point record, the
// buffer class and the flag-byte bit-field view are inferred from offsets,
// and each class declares a non-inline virtual ahead of its destructor so
// that no virtual table is emitted here. The constructors' exception cleanup
// makes the compiler emit a weak copy of CAttachableObject's destructor; the
// original linker kept an earlier file's copy.
void DebugMsg(const char*, ...);
extern "C" void MEM_free(void*);

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    void Ident();
    CMatrix& operator=(const CMatrix&);

    float m[4][4];
    static bool s_ClassInit;
} __attribute__((aligned(16)));

// The offset is copied in doublewords, as for a vector holding an 8-byte
// aligned three-float record (inferred).
struct CVector3Data {
    float x;
    float y;
    float z;
} __attribute__((aligned(8)));

class CVector3 {
public:
    CVector3Data v;
};

class CHierObject;

class CCSGVolume {
public:
    CCSGVolume();
    ~CCSGVolume();
    void Init(int, CHierObject*);
};

class CQuaternion {
public:
    float x;
    float y;
    float z;
    float w;
};

struct AttachPoint {
    CMatrix matrix;
    int id;
    unsigned char unknown44[12];
};

struct AttachPointTable {
    unsigned char unknown00[40];
    AttachPoint* points;
    int count;
};

struct MeshView {
    unsigned char unknown00[4];
    AttachPointTable* attachPoints;
};

class CStaticMesh;

class CStaticObject {
public:
    struct MotionFrame;

    virtual void BeginUpdate(float);
    virtual ~CStaticObject();
    void Init();
};

class CAttachableObject : public CStaticObject {
public:
    CAttachableObject();
    CAttachableObject(CStaticMesh*, int, int, int, int, int, CStaticObject::MotionFrame*);
    virtual void BeginUpdate(float);
    virtual ~CAttachableObject() {}
};

// The buffer at +1680, with 32-entry inline storage (inferred).
class CHierObjectBuffer {
public:
    CHierObjectBuffer() : m_data(0), m_unknown04(0), m_unknown08(0), m_size(0), m_owned(true), m_unknown14(-1) {}
    ~CHierObjectBuffer() {
        if (m_owned && m_data)
            MEM_free(m_data);
    }

    unsigned char* m_data;
    int m_unknown04;
    int m_unknown08;
    int m_size;
    bool m_owned;
    int m_unknown14;
    unsigned char m_inline[128];
};

class CHierObject : public CAttachableObject {
public:
    CHierObject();
    CHierObject(CStaticMesh*, int, int, int, int, int, CStaticObject::MotionFrame*);
    virtual void BeginUpdate(float);
    virtual ~CHierObject();
    void Init(CHierObject*, int, int, int, CVector3&);
    bool RemoveObjectFromTree(CHierObject*, int);
    void GetAttachPointMatrix(int, CMatrix&);
    CHierObject* GetSubObject(int);
    void SetRotation(CQuaternion&);

    unsigned char unknown004[44];
    MeshView* m_mesh;
    unsigned char unknown034[332];
    CQuaternion m_rotation;
    unsigned char unknown190[1008];
    CHierObject* m_firstChild;
    CHierObject* m_nextSibling;
    bool m_paused : 1;
    unsigned char unknown588b6 : 1;
    bool m_isRoot : 1;
    bool m_unknown588b4 : 1;
    unsigned char unknown588b3 : 4;
    unsigned char unknown589[3];
    int m_index;
    CHierObject* m_root;
    CCSGVolume m_volume;
    unsigned char unknown595[11];
    CHierObject* m_subObjects[20];
    CMatrix m_matrix5f0;
    CMatrix m_attachMatrix;
    CVector3 m_offset;
    unsigned char unknown680[16];
    CHierObjectBuffer m_buffer;
    int m_unknown728;
    unsigned char unknown72c[8];
    int m_unknown734;
};

bool CHierObject::RemoveObjectFromTree(CHierObject* object, int index) {
    if (m_isRoot)
        m_subObjects[index] = 0;
    if (object == m_firstChild) {
        m_firstChild = object->m_nextSibling;
        return true;
    }
    CHierObject* previous = 0;
    for (CHierObject* child = m_firstChild; child; previous = child, child = child->m_nextSibling) {
        if (child == object) {
            previous->m_nextSibling = child->m_nextSibling;
            return true;
        }
        if (child->RemoveObjectFromTree(object, index))
            return true;
    }
    return false;
}

void CHierObject::GetAttachPointMatrix(int id, CMatrix& matrix) {
    AttachPointTable* table = m_mesh->attachPoints;
    AttachPoint* point = table->points;
    for (int i = 0; i < table->count; i++, point++) {
        if (point->id == id) {
            matrix = point->matrix;
            return;
        }
    }
    DebugMsg("The specified attachpoint %d was not found\n", id);
}

CHierObject* CHierObject::GetSubObject(int index) {
    return m_subObjects[index];
}

void CHierObject::SetRotation(CQuaternion& rotation) {
    m_rotation = rotation;
}

void CHierObject::Init(CHierObject* root, int index, int parentIndex, int attachPoint, CVector3& offset) {
    CStaticObject::Init();
    m_isRoot = this == root;
    m_root = root;
    m_firstChild = 0;
    m_nextSibling = 0;
    m_unknown588b4 = false;
    m_paused = false;
    if (m_isRoot) {
        for (int i = 0; i < 20; i++)
            m_subObjects[i] = 0;
    }
    m_matrix5f0.Ident();
    m_index = index;
    if (!m_isRoot) {
        CMatrix attach;
        root->m_subObjects[parentIndex]->GetAttachPointMatrix(attachPoint, attach);
        m_attachMatrix = attach;
        root->m_subObjects[index] = this;
        CHierObject* parent = root->m_subObjects[parentIndex];
        if (parent->m_firstChild)
            m_nextSibling = parent->m_firstChild;
        parent->m_firstChild = this;
    } else {
        m_attachMatrix.Ident();
    }
    m_offset = offset;
    if (m_isRoot) {
        m_subObjects[0] = this;
        m_volume.Init(20, this);
    }
    if (m_buffer.m_owned && m_buffer.m_data)
        MEM_free(m_buffer.m_data);
    m_buffer.m_data = m_buffer.m_inline;
    m_buffer.m_size = 32;
    m_buffer.m_owned = false;
    m_unknown728 = -1;
    m_unknown734 = -1;
}

CHierObject::~CHierObject() {
}

CHierObject::CHierObject(CStaticMesh* mesh, int a, int b, int c, int d, int e, CStaticObject::MotionFrame* frame)
    : CAttachableObject(mesh, a, b, c, d, e, frame) {
}

CHierObject::CHierObject() {
}
