#include <dolphin.h>
#include <dolphin/gx.h>
#include "__gx.h"

#define GXCOLOR_AS_U32(color) (*((u32*)&(color)))

void GXSetChanAmbColor(GXChannelID chan, GXColor amb_color) {
    u32 reg;
    u32 rgb;
    u32 colIdx;

    CHECK_GXBEGIN(661, "GXSetChanAmbColor");

    switch (chan) {
    case GX_COLOR0:
        reg = __GXData->ambColor[GX_COLOR0];
        rgb = GXCOLOR_AS_U32(amb_color) >> 8;
        OLD_SET_REG_FIELD(675, reg, 24, 8, rgb);
        colIdx = 0;
        break;
    case GX_COLOR1:
        reg = __GXData->ambColor[GX_COLOR1];
        rgb = GXCOLOR_AS_U32(amb_color) >> 8;
        OLD_SET_REG_FIELD(690, reg, 24, 8, rgb);
        colIdx = 1;
        break;
    case GX_ALPHA0:
        reg = __GXData->ambColor[GX_COLOR0];
        SET_REG_FIELD(696, reg, 8, 0, amb_color.a);
        colIdx = 0;
        break;
    case GX_ALPHA1:
        reg = __GXData->ambColor[GX_COLOR1];
        SET_REG_FIELD(702, reg, 8, 0, amb_color.a);
        colIdx = 1;
        break;
    case GX_COLOR0A0:
        reg = GXCOLOR_AS_U32(amb_color);
        colIdx = 0;
        break;
    case GX_COLOR1A1:
        reg = GXCOLOR_AS_U32(amb_color);
        colIdx = 1;
        break;
    default:
        ASSERTMSGLINE(731, 0, "GXSetChanAmbColor: Invalid Channel Id");
        return;
    }

    GX_WRITE_XF_REG(colIdx + 10, reg);
    __GXData->bpSentNot = 1;
    __GXData->ambColor[colIdx] = reg;
}

void GXSetChanMatColor(GXChannelID chan, GXColor mat_color) {
    u32 reg;
    u32 rgb;
    u32 colIdx;

    CHECK_GXBEGIN(762, "GXSetChanMatColor");

    switch (chan) {
    case GX_COLOR0:
        reg = __GXData->matColor[GX_COLOR0];
        rgb = GXCOLOR_AS_U32(mat_color) >> 8;
        OLD_SET_REG_FIELD(776, reg, 24, 8, rgb);
        colIdx = 0;
        break;
    case GX_COLOR1:
        reg = __GXData->matColor[GX_COLOR1];
        rgb = GXCOLOR_AS_U32(mat_color) >> 8;
        OLD_SET_REG_FIELD(791, reg, 24, 8, rgb);
        colIdx = 1;
        break;
    case GX_ALPHA0:
        reg = __GXData->matColor[GX_COLOR0];
        SET_REG_FIELD(797, reg, 8, 0, mat_color.a);
        colIdx = 0;
        break;
    case GX_ALPHA1:
        reg = __GXData->matColor[GX_COLOR1];
        SET_REG_FIELD(803, reg, 8, 0, mat_color.a);
        colIdx = 1;
        break;
    case GX_COLOR0A0:
        reg = GXCOLOR_AS_U32(mat_color);
        colIdx = 0;
        break;
    case GX_COLOR1A1:
        reg = GXCOLOR_AS_U32(mat_color);
        colIdx = 1;
        break;
    default:
        ASSERTMSGLINE(832, 0, "GXSetChanMatColor: Invalid Channel Id");
        return;
    }

    GX_WRITE_XF_REG(colIdx + 12, reg);
    __GXData->bpSentNot = 1;
    __GXData->matColor[colIdx] = reg;
}

void GXSetNumChans(u8 nChans) {
    CHECK_GXBEGIN(857, "GXSetNumChans");
    ASSERTMSGLINE(858, nChans <= 2, "GXSetNumChans: nChans > 2");

    SET_REG_FIELD(860, __GXData->genMode, 3, 4, nChans);
    GX_WRITE_XF_REG(9, nChans);
    __GXData->dirtyState |= 4;
}

void GXSetChanCtrl(GXChannelID chan, GXBool enable, GXColorSrc amb_src, GXColorSrc mat_src, u32 light_mask, GXDiffuseFn diff_fn, GXAttnFn attn_fn) {
    u32 reg;
    u32 idx;

    CHECK_GXBEGIN(892, "GXSetChanCtrl");

    ASSERTMSGLINE(895, chan >= GX_COLOR0 && chan <= GX_COLOR1A1, "GXSetChanCtrl: Invalid Channel Id");

#if DEBUG
    if (chan == GX_COLOR0A0)
        idx = 0;
    else if (chan == GX_COLOR1A1)
        idx = 1;
    else
        idx = chan;
#else
    idx = chan & 0x3;
#endif

    reg = 0;
    SET_REG_FIELD(907, reg, 1, 1, enable);
    SET_REG_FIELD(908, reg, 1, 0, mat_src);
    SET_REG_FIELD(909, reg, 1, 6, amb_src);
    
    SET_REG_FIELD(911, reg, 2, 7, (attn_fn == 0) ? 0 : diff_fn);
    SET_REG_FIELD(912, reg, 1, 9, (attn_fn != 2));
    SET_REG_FIELD(913, reg, 1, 10, (attn_fn != 0));

    OLD_SET_REG_FIELD(925, reg, 4, 2, light_mask & 0xF);
    OLD_SET_REG_FIELD(926, reg, 4, 11, (light_mask >> 4) & 0xF);

    GX_WRITE_XF_REG(idx + 14, reg);
    
    if (chan == GX_COLOR0A0) {
        GX_WRITE_XF_REG(16, reg);
    } else if (chan == GX_COLOR1A1) {
        GX_WRITE_XF_REG(17, reg);
    }

    __GXData->bpSentNot = 1;
}
