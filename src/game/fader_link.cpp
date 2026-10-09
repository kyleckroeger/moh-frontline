// A fragment of fader.cpp (0x8007b970): CFaderBin::IsUsed (the fade colour's
// alpha is not zero) and Link (under a temporary orthographic projection of
// the 640x448 screen and an identity view pushed back to -1200, one
// untextured quad covering the screen in the fade colour, each channel
// doubled and clamped; the previous projection is restored). The file name
// is this project's; the original record is fader.cpp. CFaderBin, CColor and
// MathFunClamp are named by the mangled symbols; the colour is the file-local
// g_color (declared extern to link to it) and the members are inferred.
// CFaderBin's destructor (weak, in fader_dtor.cpp) is declared first so its
// table is not emitted here; the clamp template's weak copy
// (fader_init.cpp) is declared only. The constants link to their pool
// entries.
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

class CDmaTag;
class CDmaPacket;

struct CColor {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

extern CColor g_color;

template <class T> T MathFunClamp(T, T, T);

class CRenderBin {
    unsigned char unknown00[28]; /* members before the table pointer */

public:
    virtual ~CRenderBin();
    virtual int Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();
};

class CFaderBin : public CRenderBin {
public:
    virtual ~CFaderBin();
    virtual bool IsUsed();
    virtual void Link(CDmaTag*, CDmaPacket&);
};

bool CFaderBin::IsUsed() {
    return g_color.a != 0;
}

void CFaderBin::Link(CDmaTag*, CDmaPacket&) {
    float projection[GX_PROJECTION_SZ];
    Mtx view;
    Mtx44 ortho;
    GXGetProjectionv(projection);
    MTXOrtho(ortho, 224.0f, -224.0f, -320.0f, 320.0f, 5.0f, 2000.0f);
    GXSetProjection(ortho, GX_ORTHOGRAPHIC);
    MTXIdentity(view);
    view[2][3] = -1200.0f;
    GXLoadPosMtxImm(view, GX_PNMTX0);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_NONE);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetNumChans(1);
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);
    GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    GXSetCullMode(GX_CULL_NONE);
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3s16(-320, 224, 0);
    GXColor4u8(MathFunClamp<int>(g_color.r * 2, 0, 255), MathFunClamp<int>(g_color.g * 2, 0, 255),
               MathFunClamp<int>(g_color.b * 2, 0, 255), MathFunClamp<int>(g_color.a * 2, 0, 255));
    GXPosition3s16(320, 224, 0);
    GXColor4u8(MathFunClamp<int>(g_color.r * 2, 0, 255), MathFunClamp<int>(g_color.g * 2, 0, 255),
               MathFunClamp<int>(g_color.b * 2, 0, 255), MathFunClamp<int>(g_color.a * 2, 0, 255));
    GXPosition3s16(320, -224, 0);
    GXColor4u8(MathFunClamp<int>(g_color.r * 2, 0, 255), MathFunClamp<int>(g_color.g * 2, 0, 255),
               MathFunClamp<int>(g_color.b * 2, 0, 255), MathFunClamp<int>(g_color.a * 2, 0, 255));
    GXPosition3s16(-320, -224, 0);
    GXColor4u8(MathFunClamp<int>(g_color.r * 2, 0, 255), MathFunClamp<int>(g_color.g * 2, 0, 255),
               MathFunClamp<int>(g_color.b * 2, 0, 255), MathFunClamp<int>(g_color.a * 2, 0, 255));
    GXSetProjectionv(projection);
}
