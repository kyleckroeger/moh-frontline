// A fragment of cdbobject.cpp (0x8007164c): CCDBObject::DisablePassThru (a
// static member storing the file-local pass-through flag) and the
// CCDBObject constructors: the copy constructor (the database pointer and
// two vectors, copied as doubleword pairs) and the constructor from a CDB
// (the database pointer). The file name is this project's; the original
// record is cdbobject.cpp. IVolume, CCDBObject and CDB are named by the
// mangled symbols; the members and their names are inferred views (the
// vectors are 16-byte, 8-aligned, copied as two doubles), and only the
// virtuals the constructors need are declared. The flag is file-local in the
// original; this fragment declares it non-static so it links to the original
// object. CCDBObject declares its TestCollision override for IVolume (defined
// elsewhere) before its destructor, so its global virtual table is not
// emitted here. IVolume's destructor is inline (weak in the original), so the
// compiler's copies of it and of IVolume's weak virtual table are weak
// duplicates, linked to the original copies.
class CCollision;
class CDB;

/* inferred: a 16-byte vector copied as two doubles */
struct CVector3View {
    double pair[2];
};

class IVolume {
public:
    IVolume() {}
    virtual ~IVolume() {}
};

extern bool g_bDisablePassThru;

class CCDBObject : public IVolume {
public:
    virtual bool TestCollision(const IVolume&, CCollision&, bool) const;
    virtual ~CCDBObject();
    static void DisablePassThru(bool);
    CCDBObject(const CCDBObject&);
    CCDBObject(CDB*);

    CDB* m_db;
    CVector3View data08;
    CVector3View data18;
};

void CCDBObject::DisablePassThru(bool disable) {
    g_bDisablePassThru = disable;
}

CCDBObject::CCDBObject(const CCDBObject& other) : m_db(other.m_db), data08(other.data08), data18(other.data18) {
}

CCDBObject::CCDBObject(CDB* db) : m_db(db) {
}
