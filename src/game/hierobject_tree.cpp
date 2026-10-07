// A fragment of hierobject.cpp (0x8009ba2c): CHierObject::RemoveObjectFromTree
// (unlinks an object from the child lists below this one, clearing its
// sub-object slot in each root it passes; the compiler inlines the recursion),
// GetAttachPointMatrix (copies the matrix of the attach point with the given
// id from the mesh's attach-point table), GetSubObject and SetRotation. The
// classes are named by the mangled symbols; the members, the attach-point
// record and the flag-byte bit-field view are inferred from offsets.
void DebugMsg(const char*, ...);

class CMatrix {
public:
    CMatrix& operator=(const CMatrix&);

    float m[16];
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

class CHierObject {
public:
    bool RemoveObjectFromTree(CHierObject*, int);
    void GetAttachPointMatrix(int, CMatrix&);
    CHierObject* GetSubObject(int);
    void SetRotation(CQuaternion&);

    unsigned char unknown000[48];
    MeshView* m_mesh;
    unsigned char unknown034[332];
    CQuaternion m_rotation;
    unsigned char unknown190[1008];
    CHierObject* m_firstChild;
    CHierObject* m_nextSibling;
    bool m_paused : 1;
    unsigned char unknown588b6 : 1;
    bool m_isRoot : 1;
    unsigned char unknown588b4 : 5;
    unsigned char unknown589[23];
    CHierObject* m_subObjects[1];
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
