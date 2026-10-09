// A fragment of skybox.cpp (0x800a9234): the weak, empty CTexture default
// constructor (the sky box's texture array is built with it), and
// CSkyBox::DestroyInstance (the singleton deleted, screen clearing turned
// back on, the singleton cleared and the six face bins disabled) and
// GetInstance. The file name is this project's; the original record is
// skybox.cpp. The classes, functions and globals are named by the mangled
// symbols; CScreen's virtuals are declared in table order up to SetClear (as
// in Screen_vblank.cpp) and the bins' members are inferred (as in
// skybox_bin.cpp). The other virtuals are declared without bodies so no
// table is emitted here.
class CColor;
struct CRect;
class CDmaTag;
class CDmaPacket;

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

/* Only the virtual destructor (slot 12) is needed of the sky box. */
class CSkyBox {
public:
    virtual void MarkForDestruction(int);
    virtual ~CSkyBox();
    static void DestroyInstance();
    static CSkyBox* GetInstance();

    static CSkyBox* sm_pSingleton;
    static CScreen* sm_pScreen;
};

/* CRenderBin's non-polymorphic base (28 bytes; see
   dmesh_cremapbin_ctor.cpp); only its size matters here. */
struct CRenderBinData {
    unsigned char unknown00[28];
};

/* The face bins (44 bytes each; the enabled flag at +40, as in
   skybox_bin.cpp). */
class CSkyBoxRenderBin : public CRenderBinData {
public:
    virtual ~CSkyBoxRenderBin();

    CSkyBox* m_sky;
    int m_face;
    bool m_enabled;
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
