/* CStaticObjectFactory::Reset, GetMotionData and GetStaticMesh (a fragment of
   staticobjfactory.cpp). Reset links each object pool into its free list
   (static, hierarchy, attachable, thrown and static AI objects; the next
   pointer at +36 of each slot; the thrown and static AI loops run to the pool
   size), clears the recycling and allocator indices, points the allocator
   free-list table at three of the free lists and allocates the shared grenade,
   stick grenade, knife, wrench and bazooka objects by id. GetMotionData and
   GetStaticMesh binary-search the sorted templates by id and variant
   and return the template's motion data or its mesh, if loaded.
   StaticObjectTemplate, CStaticObjectFactory and CStaticMesh are named by the
   mangled symbols; the template's members are inferred from offsets (a mesh
   slot whose index is -1 when empty, the object id at +20, motion data at +28,
   definition data at +36 and the variant at +40; the embedded mesh's size is
   not known). g_numTemplates and g_pStaticObjectTemplates are file-local in
   the original and are declared here as externals. The weak template
   destructor compiled here is a duplicate: the original linker kept the copy
   that follows DoesHelmetTypeStopBullets. The pool slot views (their sizes
   from the strides, the link at +36) and the globals' types are inferred. */
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

struct StaticObjectSlotView {
    unsigned char unknown000[36];
    StaticObjectSlotView* m_next;
    unsigned char unknown028[600];
};

struct HierObjectSlotView {
    unsigned char unknown000[36];
    HierObjectSlotView* m_next;
    unsigned char unknown028[1816];
};

struct AttachableObjectSlotView {
    unsigned char unknown000[36];
    AttachableObjectSlotView* m_next;
    unsigned char unknown028[1368];
};

struct ThrownObjectSlotView {
    unsigned char unknown000[36];
    ThrownObjectSlotView* m_next;
    unsigned char unknown028[728];
};

struct StaticAIObjectSlotView {
    unsigned char unknown000[36];
    StaticAIObjectSlotView* m_next;
    unsigned char unknown028[4056];
};

class CStaticObject;

extern StaticObjectSlotView* g_pStaticObjectArray;
extern HierObjectSlotView* g_pHierObjectArray;
extern StaticAIObjectSlotView* g_pStaticAIObjectArray;
extern AttachableObjectSlotView* g_pAttachableObjectArray;
extern ThrownObjectSlotView* g_pThrownObjectArray;
extern StaticObjectSlotView* g_pStaticObjectFreeList;
extern HierObjectSlotView* g_pHierObjectFreeList;
extern StaticAIObjectSlotView* g_pStaticAIObjectFreeList;
extern AttachableObjectSlotView* g_pAttachableObjectFreeList;
extern ThrownObjectSlotView* g_pThrownObjectFreeList;
extern int g_iStaticObjectPoolSize;
extern int g_iHierObjectPoolSize;
extern int g_iAttachObjectPoolSize;
extern int g_iMaxThrownObjects;
extern int g_iStaticAIObjectPoolSize;
extern int g_iRecycledThrownObjectsIndex;
extern int g_currentAlternateAllocatorArrayIndex;
extern void** g_ppAllocatorFreeLists[];
extern CStaticObject* g_pStaticObjectGrenade;
extern CStaticObject* g_pStaticObjectStickGrenade;
extern CStaticObject* g_pStaticObjectKnife;
extern CStaticObject* g_pStaticObjectWrench;
extern CStaticObject* g_pStaticObjectBazooka;

class CStaticObjectFactory {
public:
    static void Reset();
    static CStaticObject* AllocateStaticObject(int, int);
    static bool DoesHelmetTypeStopBullets(int);
    static MotionData* GetMotionData(int, int);
    static CStaticMesh* GetStaticMesh(int, int);
};

extern int g_numTemplates;
extern StaticObjectTemplate* g_pStaticObjectTemplates;

int CompareStaticObjectDefinitions(const void*, const void*);

void CStaticObjectFactory::Reset() {
    g_pStaticObjectFreeList = g_pStaticObjectArray;
    g_pHierObjectFreeList = g_pHierObjectArray;
    g_pStaticAIObjectFreeList = g_pStaticAIObjectArray;
    g_pAttachableObjectFreeList = g_pAttachableObjectArray;
    g_pThrownObjectFreeList = g_pThrownObjectArray;
    for (int i = 0; i < g_iStaticObjectPoolSize - 1; i++)
        g_pStaticObjectArray[i].m_next = &g_pStaticObjectArray[i + 1];
    g_pStaticObjectArray[g_iStaticObjectPoolSize - 1].m_next = 0;
    for (int i = 0; i < g_iHierObjectPoolSize - 1; i++)
        g_pHierObjectArray[i].m_next = &g_pHierObjectArray[i + 1];
    g_pHierObjectArray[g_iHierObjectPoolSize - 1].m_next = 0;
    for (int i = 0; i < g_iAttachObjectPoolSize - 1; i++)
        g_pAttachableObjectArray[i].m_next = &g_pAttachableObjectArray[i + 1];
    g_pAttachableObjectArray[g_iAttachObjectPoolSize - 1].m_next = 0;
    for (int i = 0; i < g_iMaxThrownObjects; i++)
        g_pThrownObjectArray[i].m_next = &g_pThrownObjectArray[i + 1];
    g_pThrownObjectArray[g_iMaxThrownObjects - 1].m_next = 0;
    for (int i = 0; i < g_iStaticAIObjectPoolSize; i++)
        g_pStaticAIObjectArray[i].m_next = &g_pStaticAIObjectArray[i + 1];
    g_pStaticAIObjectArray[g_iStaticAIObjectPoolSize - 1].m_next = 0;
    g_iRecycledThrownObjectsIndex = 0;
    g_currentAlternateAllocatorArrayIndex = 0;
    g_ppAllocatorFreeLists[0] = (void**)&g_pHierObjectFreeList;
    g_ppAllocatorFreeLists[1] = (void**)&g_pAttachableObjectFreeList;
    g_ppAllocatorFreeLists[2] = (void**)&g_pThrownObjectFreeList;
    g_pStaticObjectGrenade = AllocateStaticObject(0xa55dda2d, -1);
    g_pStaticObjectStickGrenade = AllocateStaticObject(0x83674487, -1);
    g_pStaticObjectKnife = AllocateStaticObject(0x7b81b72c, -1);
    g_pStaticObjectWrench = AllocateStaticObject(0x60531a56, -1);
    g_pStaticObjectBazooka = AllocateStaticObject(0xd23d51cf, -1);
}

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
