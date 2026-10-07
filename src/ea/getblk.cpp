/* Adapted from getblk.c of the MPEG Software Simulation Group's mpeg2decode
   reference decoder (see docs/Licensing.md); its notice follows. */

/* Copyright (C) 1996, MPEG Software Simulation Group. All Rights Reserved. */

/*
 * Disclaimer of Warranty
 *
 * These software programs are available to the user without any license fee or
 * royalty on an "as is" basis.  The MPEG Software Simulation Group disclaims
 * any and all warranties, whether express, implied, or statuary, including any
 * implied warranties or merchantability or of fitness for a particular
 * purpose.  In no event shall the copyright-holder be liable for any
 * incidental, punitive, or consequential damages of any kind whatsoever
 * arising from the use of these programs.
 *
 * This disclaimer of warranty extends to the user of these programs and user's
 * customers, employees, agents, transferees, successors, and assigns.
 *
 * The MPEG Software Simulation Group does not represent or warrant that the
 * programs furnished hereunder are free of infringement of any third-party
 * patents.
 *
 * Commercial implementations of MPEG-1 and MPEG-2 video, including shareware,
 * are subject to royalty fees to patent holders.  Many of these patents are
 * general enough such that they are unavoidable regardless of implementation
 * design.
 *
 */

// getblk.cpp: the reference decoder's DCT coefficient decoders for MPEG-1 and
// MPEG-2 intra and non-intra blocks. Changes from the reference, as the game's
// code shows: data partitioning and the enhancement layer are gone, only the
// 4:2:0 quantizer matrices are used, all printing is absent, and the
// coefficients are scaled for the game's IDCT (intra DC shifted by 7 instead of
// 3, no dequantization shifts or mismatch control, saturation at +-16x 2048) and,
// for MPEG-2, stored through MPDtranspose_table. The bit readers are inlined
// helpers on the base layer; their form (Get_Bits with the flush expanded
// inline) and the layer-data layout (block at +1096) are inferred from the
// code. The constants are the reference's (mpeg2dec.h).
#define D_TYPE 4
#define ZIG_ZAG 0

struct layer_data {
  unsigned char *Rdptr;
  unsigned int Bfr;
  int Incnt;
  int Bitcnt;
  int intra_quantizer_matrix[64];
  int non_intra_quantizer_matrix[64];
  int chroma_intra_quantizer_matrix[64];
  int chroma_non_intra_quantizer_matrix[64];
  int load_intra_quantizer_matrix;
  int load_non_intra_quantizer_matrix;
  int load_chroma_intra_quantizer_matrix;
  int load_chroma_non_intra_quantizer_matrix;
  int MPEG2_Flag;
  int scalable_mode;
  int q_scale_type;
  int alternate_scan;
  int pict_scal;
  int priority_breakpoint;
  int quantizer_scale;
  int intra_slice;
  unsigned char unknown440[8];
  short block[12][64];
};

extern struct layer_data MPDbase;
extern struct layer_data *MPDld;
extern int MPDfault_flag;
extern int MPDpicture_coding_type;
extern int MPDintra_dc_precision;
extern int MPDintra_vlc_format;
extern unsigned char MPDscan[2][64];
extern unsigned char MPDtranspose_table[64];

int Get_Luma_DC_dct_diff();
int Get_Chroma_DC_dct_diff();

/* defined in getvlc.h */
typedef struct {
  char run, level, len;
} DCTtab;

extern DCTtab DCTtabfirst[],DCTtabnext[],DCTtab0[],DCTtab1[];
extern DCTtab DCTtab2[],DCTtab3[],DCTtab4[],DCTtab5[],DCTtab6[];
extern DCTtab DCTtab0a[],DCTtab1a[];

static inline int Show_Bits(int N)
{
  return MPDbase.Bfr >> (32-N);
}

static inline void Flush_Buffer(int N)
{
  int Incnt;

  MPDbase.Bfr <<= N;
  Incnt = MPDbase.Incnt - N;
  while (Incnt <= 24)
  {
    MPDbase.Bfr |= *MPDbase.Rdptr++ << (24 - Incnt);
    Incnt += 8;
  }
  MPDbase.Incnt = Incnt;
}

static inline unsigned int Get_Bits(int N)
{
  unsigned int Val;
  int Incnt;

  Val = Show_Bits(N);
  MPDbase.Bfr <<= N;
  Incnt = MPDbase.Incnt - N;
  while (Incnt <= 24)
  {
    MPDbase.Bfr |= *MPDbase.Rdptr++ << (24 - Incnt);
    Incnt += 8;
  }
  MPDbase.Incnt = Incnt;
  return Val;
}

/* decode one intra coded MPEG-1 block */

void Decode_MPEG1_Intra_Block(int comp, int dc_dct_pred[])
{
  int val, i, j, sign;
  unsigned int code;
  DCTtab *tab;
  short *bp;

  bp = MPDld->block[comp];

  /* ISO/IEC 11172-2 section 2.4.3.7: Block layer. */
  /* decode DC coefficients */
  if (comp<4)
    bp[0] = (dc_dct_pred[0]+=Get_Luma_DC_dct_diff()) << 7;
  else if (comp==4)
    bp[0] = (dc_dct_pred[1]+=Get_Chroma_DC_dct_diff()) << 7;
  else
    bp[0] = (dc_dct_pred[2]+=Get_Chroma_DC_dct_diff()) << 7;

  if (MPDfault_flag) return;

  /* D-pictures do not contain AC coefficients */
  if(MPDpicture_coding_type == D_TYPE)
    return;

  /* decode AC coefficients */
  for (i=1; ; i++)
  {
    code = Show_Bits(16);
    if (code>=16384)
      tab = &DCTtabnext[(code>>12)-4];
    else if (code>=1024)
      tab = &DCTtab0[(code>>8)-4];
    else if (code>=512)
      tab = &DCTtab1[(code>>6)-8];
    else if (code>=256)
      tab = &DCTtab2[(code>>4)-16];
    else if (code>=128)
      tab = &DCTtab3[(code>>3)-16];
    else if (code>=64)
      tab = &DCTtab4[(code>>2)-16];
    else if (code>=32)
      tab = &DCTtab5[(code>>1)-16];
    else if (code>=16)
      tab = &DCTtab6[code-16];
    else
    {
      MPDfault_flag = 1;
      return;
    }

    Flush_Buffer(tab->len);

    if (tab->run==64) /* end_of_block */
      return;

    if (tab->run==65) /* escape */
    {
      i+= Get_Bits(6);

      val = Get_Bits(8);
      if (val==0)
        val = Get_Bits(8);
      else if (val==128)
        val = Get_Bits(8) - 256;
      else if (val>128)
        val -= 256;

      if((sign = (val<0)))
        val = -val;
    }
    else
    {
      i+= tab->run;
      val = tab->level;
      sign = Get_Bits(1);
    }

    if (i>=64)
    {
      MPDfault_flag = 1;
      return;
    }

    j = MPDscan[ZIG_ZAG][i];
    val = (val*MPDld->quantizer_scale*MPDld->intra_quantizer_matrix[j]) << 1;

    /* saturation */
    if (!sign)
      bp[j] = (val>32752) ?  32752 :  val; /* positive */
    else
      bp[j] = (val>32768) ? -32768 : -val; /* negative */
  }
}

/* decode one non-intra coded MPEG-1 block */

void Decode_MPEG1_Non_Intra_Block(int comp)
{
  int val, i, j, sign;
  unsigned int code;
  DCTtab *tab;
  short *bp;

  bp = MPDld->block[comp];

  /* decode AC coefficients */
  for (i=0; ; i++)
  {
    code = Show_Bits(16);
    if (code>=16384)
    {
      if (i==0)
        tab = &DCTtabfirst[(code>>12)-4];
      else
        tab = &DCTtabnext[(code>>12)-4];
    }
    else if (code>=1024)
      tab = &DCTtab0[(code>>8)-4];
    else if (code>=512)
      tab = &DCTtab1[(code>>6)-8];
    else if (code>=256)
      tab = &DCTtab2[(code>>4)-16];
    else if (code>=128)
      tab = &DCTtab3[(code>>3)-16];
    else if (code>=64)
      tab = &DCTtab4[(code>>2)-16];
    else if (code>=32)
      tab = &DCTtab5[(code>>1)-16];
    else if (code>=16)
      tab = &DCTtab6[code-16];
    else
    {
      MPDfault_flag = 1;
      return;
    }

    Flush_Buffer(tab->len);

    if (tab->run==64) /* end_of_block */
      return;

    if (tab->run==65) /* escape */
    {
      i+= Get_Bits(6);

      val = Get_Bits(8);
      if (val==0)
        val = Get_Bits(8);
      else if (val==128)
        val = Get_Bits(8) - 256;
      else if (val>128)
        val -= 256;

      if((sign = (val<0)))
        val = -val;
    }
    else
    {
      i+= tab->run;
      val = tab->level;
      sign = Get_Bits(1);
    }

    if (i>=64)
    {
      MPDfault_flag = 1;
      return;
    }

    j = MPDscan[ZIG_ZAG][i];
    val = ((val<<1)+1)*MPDld->quantizer_scale*MPDld->non_intra_quantizer_matrix[j];

    /* saturation */
    if (!sign)
      bp[j] = (val>32752) ?  32752 :  val; /* positive */
    else
      bp[j] = (val>32768) ? -32768 : -val; /* negative */
  }
}

/* decode one intra coded MPEG-2 block */

void Decode_MPEG2_Intra_Block(int comp, int dc_dct_pred[])
{
  int val, i, j, sign, cc, run;
  unsigned int code;
  DCTtab *tab;
  short *bp;
  int *qmat;
  struct layer_data *ld1;

  ld1 = MPDld;
  bp = ld1->block[comp];

  cc = (comp<4) ? 0 : (comp&1)+1;

  qmat = ld1->intra_quantizer_matrix;

  /* ISO/IEC 13818-2 section 7.2.1: decode DC coefficients */
  if (cc==0)
    val = (dc_dct_pred[0]+= Get_Luma_DC_dct_diff());
  else if (cc==1)
    val = (dc_dct_pred[1]+= Get_Chroma_DC_dct_diff());
  else
    val = (dc_dct_pred[2]+= Get_Chroma_DC_dct_diff());

  if (MPDfault_flag) return;

  bp[MPDtranspose_table[0]] = val << (7-MPDintra_dc_precision);

  /* decode AC coefficients */
  for (i=1; ; i++)
  {
    code = Show_Bits(16);
    if (code>=16384 && !MPDintra_vlc_format)
      tab = &DCTtabnext[(code>>12)-4];
    else if (code>=1024)
    {
      if (MPDintra_vlc_format)
        tab = &DCTtab0a[(code>>8)-4];
      else
        tab = &DCTtab0[(code>>8)-4];
    }
    else if (code>=512)
    {
      if (MPDintra_vlc_format)
        tab = &DCTtab1a[(code>>6)-8];
      else
        tab = &DCTtab1[(code>>6)-8];
    }
    else if (code>=256)
      tab = &DCTtab2[(code>>4)-16];
    else if (code>=128)
      tab = &DCTtab3[(code>>3)-16];
    else if (code>=64)
      tab = &DCTtab4[(code>>2)-16];
    else if (code>=32)
      tab = &DCTtab5[(code>>1)-16];
    else if (code>=16)
      tab = &DCTtab6[code-16];
    else
    {
      MPDfault_flag = 1;
      return;
    }

    Flush_Buffer(tab->len);

    if (tab->run==64) /* end_of_block */
    {
      return;
    }

    if (tab->run==65) /* escape */
    {
      i+= run = Get_Bits(6);

      val = Get_Bits(12);
      if ((val&2047)==0)
      {
        MPDfault_flag = 1;
        return;
      }
      if((sign = (val>=2048)))
        val = 4096 - val;
    }
    else
    {
      i+= run = tab->run;
      val = tab->level;
      sign = Get_Bits(1);
    }

    if (i>=64)
    {
      MPDfault_flag = 1;
      return;
    }

    j = MPDscan[ld1->alternate_scan][i];
    val = val * ld1->quantizer_scale * qmat[j];
    bp[MPDtranspose_table[j]] = sign ? -val : val;
  }
}

/* decode one non-intra coded MPEG-2 block */

void Decode_MPEG2_Non_Intra_Block(int comp)
{
  int val, i, j, sign, run;
  unsigned int code;
  DCTtab *tab;
  short *bp;
  int *qmat;
  struct layer_data *ld1;

  ld1 = MPDld;
  bp = ld1->block[comp];

  qmat = ld1->non_intra_quantizer_matrix;

  /* decode AC coefficients */
  for (i=0; ; i++)
  {
    code = Show_Bits(16);
    if (code>=16384)
    {
      if (i==0)
        tab = &DCTtabfirst[(code>>12)-4];
      else
        tab = &DCTtabnext[(code>>12)-4];
    }
    else if (code>=1024)
      tab = &DCTtab0[(code>>8)-4];
    else if (code>=512)
      tab = &DCTtab1[(code>>6)-8];
    else if (code>=256)
      tab = &DCTtab2[(code>>4)-16];
    else if (code>=128)
      tab = &DCTtab3[(code>>3)-16];
    else if (code>=64)
      tab = &DCTtab4[(code>>2)-16];
    else if (code>=32)
      tab = &DCTtab5[(code>>1)-16];
    else if (code>=16)
      tab = &DCTtab6[code-16];
    else
    {
      MPDfault_flag = 1;
      return;
    }

    Flush_Buffer(tab->len);

    if (tab->run==64) /* end_of_block */
    {
      return;
    }

    if (tab->run==65) /* escape */
    {
      i+= run = Get_Bits(6);

      val = Get_Bits(12);
      if ((val&2047)==0)
      {
        MPDfault_flag = 1;
        return;
      }
      if((sign = (val>=2048)))
        val = 4096 - val;
    }
    else
    {
      i+= run = tab->run;
      val = tab->level;
      sign = Get_Bits(1);
    }

    if (i>=64)
    {
      MPDfault_flag = 1;
      return;
    }

    j = MPDscan[ld1->alternate_scan][i];
    val = (((val<<1)+1)*ld1->quantizer_scale*qmat[j]) >> 1;
    bp[MPDtranspose_table[j]] = sign ? -val : val;
  }
}
