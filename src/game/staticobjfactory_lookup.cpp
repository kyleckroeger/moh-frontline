/* CStaticObjectFactory::GetMotionData and GetStaticMesh (a fragment of
   staticobjfactory.cpp): binary-search the sorted templates by id and variant
   and return the template's motion data or its mesh, if loaded.
   StaticObjectTemplate, CStaticObjectFactory and CStaticMesh are named by the
   mangled symbols; the template's members are inferred from offsets (a mesh
   slot whose index is -1 when empty, the object id at +20, motion data at +28,
   definition data at +36 and the variant at +40; the embedded mesh's size is
   not known). g_numTemplates and g_pStaticObjectTemplates are file-local in
   the original and are declared here as externals. The weak template
   destructor compiled here is a duplicate: the original linker kept the copy
   that follows DoesHelmetTypeStopBullets. */
extern "C" void* bsearch(const void*, const void*, unsigned long, unsigned long, int (*)(const void*, const void*));

class CStaticMesh {
public:
    virtual ~CStaticMesh();
};

struct MotionData;

/* inferred views */
struct HELMETVIEW {
    unsigned char unknown000[164];
    int field164;
};

struct TEMPLATEDATAVIEW {
    unsigned char unknown0[8];
    HELMETVIEW* helmet;
};

/* inferred: the template's mesh slot (an index, -1 when empty, and storage
   for the mesh); its own name is not known */
struct MESHSLOTVIEW {
    MESHSLOTVIEW() { m_index = -1; }
    ~MESHSLOTVIEW() {
        if (Get())
            Get()->~CStaticMesh();
        m_index = -1;
    }
    CStaticMesh* Get() { return m_index >= 0 ? (CStaticMesh*)m_mesh : 0; }

    int m_index;
    unsigned char m_mesh[16];
};

struct StaticObjectTemplate {
    MESHSLOTVIEW m_meshSlot;
    int m_id;
    unsigned char unknown18[4];
    MotionData* m_motionData;
    unsigned char unknown20[4];
    TEMPLATEDATAVIEW* m_data;
    int m_variant;
};

class CStaticObjectFactory {
public:
    static bool DoesHelmetTypeStopBullets(int);
    static MotionData* GetMotionData(int, int);
    static CStaticMesh* GetStaticMesh(int, int);
};

extern int g_numTemplates;
extern StaticObjectTemplate* g_pStaticObjectTemplates;

int CompareStaticObjectDefinitions(const void*, const void*);

MotionData* CStaticObjectFactory::GetMotionData(int id, int variant) {
    StaticObjectTemplate key;
    StaticObjectTemplate* found;

    key.m_id = id;
    key.m_variant = variant;
    found = (StaticObjectTemplate*)bsearch(&key, g_pStaticObjectTemplates, g_numTemplates, sizeof(StaticObjectTemplate), CompareStaticObjectDefinitions);
    if (!found)
        return 0;
    return found->m_motionData;
}

CStaticMesh* CStaticObjectFactory::GetStaticMesh(int id, int variant) {
    StaticObjectTemplate key;
    StaticObjectTemplate* found;

    key.m_id = id;
    key.m_variant = variant;
    found = (StaticObjectTemplate*)bsearch(&key, g_pStaticObjectTemplates, g_numTemplates, sizeof(StaticObjectTemplate), CompareStaticObjectDefinitions);
    if (!found)
        return 0;
    return found->m_meshSlot.Get();
}
