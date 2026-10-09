// A fragment of skybox.cpp (0x800a9420): CSkyBox::InitClass (unless already
// ready: the render list and screen kept, and each of the six face bins
// given its face number and registered with the render list). The file name is this project's; the
// original record is skybox.cpp. The classes, functions and globals are
// named by the mangled symbols; the bins' members are inferred (as in
// skybox_bin.cpp). The ready flag is a guarded function-local static, as in
// the image (it is never set here). The bins' destructor is declared without
// a body so no table is emitted here.
class CScreen;

/* CRenderBin's non-polymorphic base (28 bytes; see
   dmesh_cremapbin_ctor.cpp); only its size matters here. */
struct CRenderBinData {
    unsigned char unknown00[28];
};

class CRenderBin : public CRenderBinData {
public:
    virtual ~CRenderBin();
};

class CSkyBox;

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

class CSkyBox {
public:
    static void InitClass(CRenderList*, CScreen*);

    static CRenderList* sm_pRenderList;
    static CScreen* sm_pScreen;
};

extern CSkyBoxRenderBin g_skyBoxRenderBin[6];

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
