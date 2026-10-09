// A fragment of skybox.cpp (0x800a9840): CSkyBoxRenderBin's weak Render
// (the sky box given as the render data is kept and, with the bin's face,
// added to the packet), Link (for a non-empty tag list: the GX vertex
// format and TEV state for one textured, unlit stage blended over the frame,
// then for each tag the sky box's matrix is loaded, the face's texture is
// loaded and the face is drawn as a triangle fan of white vertices from the
// kept sky box; the current matrix is then reset) and IsUsed (a kept
// sky box and the enabled flag). The file name is this project's; the
// original record is skybox.cpp. The classes and functions are named by the
// mangled symbols (CRenderBinData by CRenderBin's RTTI record); the members,
// the tag data and the sky box layout are inferred. The tag is tested twice
// before the state setup, as in the image. CSkyBoxRenderBin declares its
// destructor (weak, in skybox_renderbin.cpp) first so its table is not
// emitted here. The projection matrix is file-local in the original; it is
// declared extern to link to it.
#include <dolphin/gx.h>

class CDmaTag;
class CRenderList;

extern float g_ProjectionMatrix[4][4];

/* Inferred: the packet's write cursor at +8. */
class CDmaPacket {
public:
    void Add(unsigned int word) {
        *m_cur = word;
        m_cur++;
    }

    unsigned char unknown00[8];
    unsigned int* m_cur;
};

/* Only the 16-aligned layout and the GX export of the matrix. */
class CMatrix {
public:
    void GetGAMECUBEMatrix34(float (&)[3][4]) const;

    float m[4][4];
} __attribute__((aligned(16)));

/* Only the 64-byte size and Load. */
class CTexture {
public:
    void Load(CDmaPacket&, CRenderList*);

    unsigned char unknown00[64];
};

/* Inferred: a sky vertex's position and texture coordinates (32 bytes). */
struct SSkyVertex {
    float x, y, z;
    float unknown0c;
    float u, v;
    float unknown18[2];
};

/* Inferred: the six faces' textures, vertices (ten per face) and vertex
   counts, and the box's matrix. */
class CSkyBox {
public:
    unsigned char unknown00[40];
    CTexture m_textures[6];
    unsigned char unknown1a8[40];
    SSkyVertex m_vertices[6][10];
    int m_vertexCounts[6];
    unsigned char unknown988[8];
    CMatrix m_matrix;
};

/* Inferred: a render tag's link and its data: the sky box and a face. */
struct SSkyFace {
    CSkyBox* m_sky;
    int m_face;
};

class CDmaTag {
public:
    CDmaTag* m_next;
    SSkyFace* m_data;
};

class CRenderBin;

/* CRenderBin's non-polymorphic base (see dmesh_cremapbin_ctor.cpp). */
struct CRenderBinData {
    int data00;
    CRenderBin* m_next;
    CRenderBin* m_children;
    int data0c;
    int data10;
    int m_priority;
    int data18;
};

/* CRenderBin's virtuals in table order. */
class CRenderBin : public CRenderBinData {
public:
    virtual ~CRenderBin();
    virtual int Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();
};

class CSkyBoxRenderBin : public CRenderBin {
public:
    virtual ~CSkyBoxRenderBin();
    virtual int Render(CDmaPacket&, void*);
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();

    CSkyBox* m_sky;
    int m_face;
    bool m_enabled;
};

__declspec(weak) int CSkyBoxRenderBin::Render(CDmaPacket& packet, void* data) {
    m_sky = (CSkyBox*)data;
    m_sky = (CSkyBox*)data;
    packet.Add((unsigned int)m_sky);
    packet.Add(m_face);
    return 0;
}

__declspec(weak) void CSkyBoxRenderBin::Link(CDmaTag* tag, CDmaPacket& packet) {
    if (tag) {
        CDmaTag* t = tag;
        if (t) {
            GXClearVtxDesc();
            GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
            GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
            GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
            GXSetProjection(g_ProjectionMatrix, GX_PERSPECTIVE);
            GXSetCurrentMtx(3);
            GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
            GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
            GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
            GXSetNumChans(1);
            GXSetNumTexGens(1);
            GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 60, GX_FALSE, 125);
            GXSetNumTevStages(1);
            GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);
            GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
            GXSetCullMode(GX_CULL_NONE);
            GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
            GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_RASC, GX_CC_ZERO);
            GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_ZERO);
            GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            for (; t; t = t->m_next) {
                CSkyBox* sky = t->m_data->m_sky;
                int face = t->m_data->m_face;
                float matrix[3][4];
                sky->m_matrix.GetGAMECUBEMatrix34(matrix);
                GXLoadPosMtxImm(matrix, 3);
                sky->m_textures[face].Load(packet, 0);
                GXBegin(GX_TRIANGLEFAN, GX_VTXFMT0, sky->m_vertexCounts[face]);
                for (int i = 0; i < sky->m_vertexCounts[face]; i++) {
                    SSkyVertex& v = m_sky->m_vertices[face][i];
                    GXPosition3f32(v.x, v.y, v.z);
                    GXColor4u8(255, 255, 255, 255);
                    GXTexCoord2f32(v.u, v.v);
                }
            }
            GXSetCurrentMtx(0);
        }
    }
}

__declspec(weak) bool CSkyBoxRenderBin::IsUsed() {
    return m_sky && m_enabled;
}
