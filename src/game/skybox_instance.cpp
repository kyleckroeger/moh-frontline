// A fragment of skybox.cpp (0x800a9234): the weak, empty CTexture default
// constructor (the sky box's texture array is built with it), and the
// CSkyBox class functions: DestroyInstance (the singleton deleted, screen
// clearing turned back on, the singleton cleared and the six face bins
// disabled), GetInstance, CreateInstance (unless it exists: a new sky box,
// its scene-node bases' inline constructors, six textures, six cleared
// shapes, drawing enabled, sixty vertices and two matrices built, then
// screen clearing turned off and the face bins enabled) and InitClass
// (unless already ready: the render list and screen kept, and each face bin
// given its face number and registered with the render list; the ready flag
// is a guarded function-local static, as in the image, never set here). The
// file name is this project's; the original record is skybox.cpp. The
// classes, functions and globals are named by the mangled symbols; CScreen's
// virtuals are declared in table order up to SetClear (as in
// Screen_vblank.cpp), the members are inferred (the bins as in
// skybox_bin.cpp; the vertex record is 16-aligned). CSkyBox declares Draw
// (defined elsewhere) first and the scene-node bases declare their first
// virtuals without bodies, so no global table is emitted here; the compiler's
// copy of ISceneNode's weak inline destructor is a weak duplicate.
class CDrawContext;
class CRenderList;
void* operator new(unsigned long);

class CScreen {
public:
    virtual ~CScreen();
    virtual void SetCurrent();
    virtual void Flip();
    virtual void Wait();
    virtual void SetClear(bool);
};

class CTexture {
public:
    CTexture();

    unsigned char unknown00[64];
};

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();

    float m[4][4];
    static bool s_ClassInit;
} __attribute__((aligned(16)));

class IDestructible {
public:
    IDestructible() {}
    virtual void MarkForDestruction(int);
    virtual ~IDestructible();
    virtual void Destroy();
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
    virtual void BeginUpdate(float);

    int data04;
    int data08;
    int data0c;
    int data10;
    int data14;
    int data18;
    int data1c;
    int data20;
};

class CSkyBox : public ISceneNode {
public:
    struct SVertex {
        SVertex();

        float x, y, z;
        float unknown0c;
        float u, v;
        float unknown18[2];
    } __attribute__((aligned(16)));

    virtual void Draw(CDrawContext&);
    virtual ~CSkyBox();
    CSkyBox() : data24(0), m_drawEnabled(true) {
        for (int i = 0; i < 6; i++)
            m_shapes[i] = 0;
    }
    static void DestroyInstance();
    static CSkyBox* GetInstance();
    static CSkyBox* CreateInstance();
    static void InitClass(CRenderList*, CScreen*);

    int data24;
    CTexture m_textures[6];
    void* m_shapes[6];
    bool m_drawEnabled;
    SVertex m_vertices[6][10];
    int m_vertexCounts[6];
    unsigned char unknown948[8];
    CMatrix m_matrix;
    CMatrix m_matrix2;

    static CRenderList* sm_pRenderList;
    static CScreen* sm_pScreen;
    static CSkyBox* sm_pSingleton;
};

/* CRenderBin's non-polymorphic base (28 bytes; see
   dmesh_cremapbin_ctor.cpp); only its size matters here. */
struct CRenderBinData {
    unsigned char unknown00[28];
};

class CRenderBin : public CRenderBinData {
public:
    virtual ~CRenderBin();
};

/* The face bins (44 bytes each, as in skybox_bin.cpp). */
class CSkyBoxRenderBin : public CRenderBin {
public:
    virtual ~CSkyBoxRenderBin();

    CSkyBox* m_sky;
    int m_face;
    bool m_enabled;
};

class CRenderList {
public:
    void Register(CRenderBin&);
};

extern CSkyBoxRenderBin g_skyBoxRenderBin[6];

__declspec(weak) CTexture::CTexture() {
}

void CSkyBox::DestroyInstance() {
    delete sm_pSingleton;
    sm_pScreen->SetClear(true);
    sm_pSingleton = 0;
    for (int i = 0; i < 6; i++)
        g_skyBoxRenderBin[i].m_enabled = false;
}

CSkyBox* CSkyBox::GetInstance() {
    return sm_pSingleton;
}

CSkyBox* CSkyBox::CreateInstance() {
    if (!sm_pSingleton) {
        sm_pSingleton = new CSkyBox;
        sm_pScreen->SetClear(false);
        for (int i = 0; i < 6; i++)
            g_skyBoxRenderBin[i].m_enabled = true;
    }
    return sm_pSingleton;
}

void CSkyBox::InitClass(CRenderList* list, CScreen* screen) {
    static bool ready = false;
    if (!ready) {
        sm_pRenderList = list;
        sm_pScreen = screen;
        for (int i = 0; i < 6; i++) {
            g_skyBoxRenderBin[i].m_face = i;
            sm_pRenderList->Register(g_skyBoxRenderBin[i]);
        }
    }
}
