// A fragment of skybox.cpp (0x800a907c): CSkyBox::SetTextures (the level's
// sky-box resource (type 13) names six shape files, which are loaded from
// the level's big file and set as the faces' textures; a missing resource is
// reported through DebugMsg). The file name is this project's; the original
// record is skybox.cpp. The classes and functions are named by the mangled
// symbols; the resource record and the sky box's members are inferred (as in
// skybox_instance.cpp). CSkyBox declares Draw (defined elsewhere) first so its
// table is not emitted here. The message links to its pool entry.
class CDrawContext;
struct ShapeFile;
struct LevelFileContentsStruct_;
enum TLTResourceID {};

void DebugMsg(const char*, ...);
void* TLT_LoadFileFromLevelBigFile(const char*, int*);

/* Inferred: the sky-box resource's six face entries (8 bytes each, the shape
   file name at +4). */
struct SSkyBoxFace {
    int unknown00;
    const char* m_name;
};

struct SSkyBoxResource {
    unsigned char unknown00[8];
    SSkyBoxFace m_faces[6];
};

struct TLTResourceView {
    unsigned char unknown00[8];
    SSkyBoxResource* data;
};

TLTResourceView* TLT_FindNextResourceByType(TLTResourceID, LevelFileContentsStruct_*);

class CTexture {
public:
    void Set(ShapeFile*, bool);

    unsigned char unknown00[64];
};

class CSkyBox {
public:
    virtual void Draw(CDrawContext&);
    void SetTextures();

    unsigned char unknown04[36];
    CTexture m_textures[6];
    ShapeFile* m_shapes[6];
};

void CSkyBox::SetTextures() {
    TLTResourceView* resource = TLT_FindNextResourceByType((TLTResourceID)13, 0);
    if (!resource) {
        DebugMsg("No Skybox textures found in level contents file\n");
        return;
    }
    m_shapes[0] = (ShapeFile*)TLT_LoadFileFromLevelBigFile(resource->data->m_faces[0].m_name, 0);
    m_shapes[1] = (ShapeFile*)TLT_LoadFileFromLevelBigFile(resource->data->m_faces[1].m_name, 0);
    m_shapes[2] = (ShapeFile*)TLT_LoadFileFromLevelBigFile(resource->data->m_faces[2].m_name, 0);
    m_shapes[3] = (ShapeFile*)TLT_LoadFileFromLevelBigFile(resource->data->m_faces[3].m_name, 0);
    m_shapes[4] = (ShapeFile*)TLT_LoadFileFromLevelBigFile(resource->data->m_faces[4].m_name, 0);
    m_shapes[5] = (ShapeFile*)TLT_LoadFileFromLevelBigFile(resource->data->m_faces[5].m_name, 0);
    for (int i = 0; i < 6; i++) {
        if (m_shapes[i])
            m_textures[i].Set(m_shapes[i], false);
    }
}
