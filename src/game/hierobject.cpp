// Hierarchy-object helpers: the matrix post-multiply and the path-following
// query (both weak), the per-frame update that runs unless the object is
// paused, and CleanUp (cleans up each child through the virtual CleanUp,
// then either clears the root's value at +1440 or removes the object from
// its parent's tree, frees its two box volumes and returns it to the
// static-object factory). CMatrix, CStaticObject, CStaticObjectFactory and
// CHierObject are named by the mangled symbols; the members and the
// flag-byte bit-field view are inferred; CHierObject is declared with its
// virtual functions up to CleanUp (+356 in __vt__11CHierObject; the earlier
// slots are placeholders named by offset) and the other classes are
// non-virtual views. The rest of the file is not part of this unit.
class CMatrix {
public:
    void Multiply(const CMatrix&, const CMatrix&);
    void PostMultiply(const CMatrix&);
};

class CStaticObject {
public:
    bool IsFollowingPath() const;

    unsigned char unknown000[260];
    int m_pathState;
};

class CStaticObjectFactory {
public:
    static void DeallocateStaticObject(CStaticObject*);
};

void FreeBoxVolume(void*);

class CHierObject {
public:
    virtual void unknown008();
    virtual void unknown00c();
    virtual void unknown010();
    virtual void unknown014();
    virtual void unknown018();
    virtual void unknown01c();
    virtual void unknown020();
    virtual void unknown024();
    virtual void unknown028();
    virtual void unknown02c();
    virtual void unknown030();
    virtual void unknown034();
    virtual void unknown038();
    virtual void unknown03c();
    virtual void unknown040();
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown04c();
    virtual void unknown050();
    virtual void unknown054();
    virtual void unknown058();
    virtual void unknown05c();
    virtual void unknown060();
    virtual void unknown064();
    virtual void unknown068();
    virtual void unknown06c();
    virtual void unknown070();
    virtual void unknown074();
    virtual void unknown078();
    virtual void unknown07c();
    virtual void unknown080();
    virtual void unknown084();
    virtual void unknown088();
    virtual void unknown08c();
    virtual void unknown090();
    virtual void unknown094();
    virtual void unknown098();
    virtual void unknown09c();
    virtual void unknown0a0();
    virtual void unknown0a4();
    virtual void unknown0a8();
    virtual void unknown0ac();
    virtual void unknown0b0();
    virtual void unknown0b4();
    virtual void unknown0b8();
    virtual void unknown0bc();
    virtual void unknown0c0();
    virtual void unknown0c4();
    virtual void unknown0c8();
    virtual void unknown0cc();
    virtual void unknown0d0();
    virtual void unknown0d4();
    virtual void unknown0d8();
    virtual void unknown0dc();
    virtual void unknown0e0();
    virtual void unknown0e4();
    virtual void unknown0e8();
    virtual void unknown0ec();
    virtual void unknown0f0();
    virtual void unknown0f4();
    virtual void unknown0f8();
    virtual void unknown0fc();
    virtual void unknown100();
    virtual void unknown104();
    virtual void unknown108();
    virtual void unknown10c();
    virtual void unknown110();
    virtual void unknown114();
    virtual void unknown118();
    virtual void unknown11c();
    virtual void unknown120();
    virtual void unknown124();
    virtual void unknown128();
    virtual void unknown12c();
    virtual void unknown130();
    virtual void unknown134();
    virtual void unknown138();
    virtual void unknown13c();
    virtual void unknown140();
    virtual void unknown144();
    virtual void unknown148();
    virtual void unknown14c();
    virtual void unknown150();
    virtual void unknown154();
    virtual void unknown158();
    virtual void unknown15c();
    virtual void unknown160();
    virtual void CleanUp();

    void BeginUpdate(float);
    void DoBeginUpdate(float, bool);
    void RemoveObjectFromTree(CHierObject*, int);

    unsigned char data004[264];
    void* m_volume10c;
    unsigned char data110[4];
    void* m_volume114;
    unsigned char data118[1128];
    CHierObject* m_firstChild;
    CHierObject* m_nextSibling;
    bool paused : 1;
    unsigned char flags : 1;
    unsigned char m_isRoot : 1;
    unsigned char flags3 : 5;
    unsigned char data589[3];
    int m_slot;
    CHierObject* m_parent;
    unsigned char data590[12];
    int m_value5a0;
};

__declspec(weak) void CMatrix::PostMultiply(const CMatrix& m) {
    Multiply(*this, m);
}

__declspec(weak) bool CStaticObject::IsFollowingPath() const {
    bool following = false;
    if (m_pathState != 0 && m_pathState != 1)
        following = true;
    return following;
}

void CHierObject::BeginUpdate(float time) {
    if (!paused)
        DoBeginUpdate(time, true);
}

void CHierObject::CleanUp() {
    CHierObject* child = m_firstChild;
    while (child) {
        CHierObject* next = child->m_nextSibling;
        child->CleanUp();
        child = next;
    }
    if (m_isRoot)
        m_value5a0 = 0;
    else
        m_parent->RemoveObjectFromTree(this, m_slot);
    FreeBoxVolume(m_volume114);
    FreeBoxVolume(m_volume10c);
    CStaticObjectFactory::DeallocateStaticObject((CStaticObject*)this);
}
