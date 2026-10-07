/* Adapted from recon.c of the MPEG Software Simulation Group's mpeg2decode
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

// A fragment of recon.cpp (0x801067d4): the reference decoder's motion
// compensation entry point form_predictions, with form_prediction inlined.
// Changes from the reference: the decoder's globals carry the game's MPD names,
// chroma is fixed at 4:2:0 (the game has no chroma_format), the
// invalid-motion-type messages are absent, and the definitions are ordered so
// that form_prediction precedes its use. The globals' array types and the
// constants are taken from the reference (global.h); form_component_prediction
// before this function (static in the original) is not reconstructed: its code
// is larger than the reference's and is declared here without static.
#define MACROBLOCK_MOTION_FORWARD 8
#define MACROBLOCK_MOTION_BACKWARD 4
#define P_TYPE 2
#define TOP_FIELD 1
#define BOTTOM_FIELD 2
#define FRAME_PICTURE 3
#define MC_FIELD 1
#define MC_FRAME 2
#define MC_16X8 2
#define MC_DMV 3

extern unsigned char* MPDforward_reference_frame[3];
extern unsigned char* MPDbackward_reference_frame[3];
extern unsigned char* MPDcurrent_frame[3];
extern int MPDcoded_picture_width;
extern int MPDsecond_field;
extern int MPDpicture_coding_type;
extern int MPDpicture_structure;

void Dual_Prime_Arithmetic(int DMV[][2], int* dmvector, int mvx, int mvy);

void form_component_prediction(unsigned char* src, unsigned char* dst, int lx, int lx2, int w, int h, int x, int y, int dx, int dy, int average_flag);

static inline void form_prediction(unsigned char* src[], int sfield, unsigned char* dst[], int dfield, int lx, int lx2, int w, int h, int x, int y, int dx, int dy, int average_flag)
{
  /* Y */
  form_component_prediction(src[0]+(sfield?lx2>>1:0),dst[0]+(dfield?lx2>>1:0),
    lx,lx2,w,h,x,y,dx,dy,average_flag);

  lx>>=1; lx2>>=1; w>>=1; x>>=1; dx/=2;
  h>>=1; y>>=1; dy/=2;

  /* Cb */
  form_component_prediction(src[1]+(sfield?lx2>>1:0),dst[1]+(dfield?lx2>>1:0),
    lx,lx2,w,h,x,y,dx,dy,average_flag);

  /* Cr */
  form_component_prediction(src[2]+(sfield?lx2>>1:0),dst[2]+(dfield?lx2>>1:0),
    lx,lx2,w,h,x,y,dx,dy,average_flag);
}

void form_predictions(int bx, int by, int macroblock_type, int motion_type, int PMV[2][2][2], int motion_vertical_field_select[2][2], int dmvector[2], int stwtype)
{
  int currentfield;
  unsigned char **predframe;
  int DMV[2][2];
  int stwtop, stwbot;

  stwtop = stwtype%3; /* 0:temporal, 1:(spat+temp)/2, 2:spatial */
  stwbot = stwtype/3;

  if ((macroblock_type & MACROBLOCK_MOTION_FORWARD) 
   || (MPDpicture_coding_type==P_TYPE))
  {
    if (MPDpicture_structure==FRAME_PICTURE)
    {
      if ((motion_type==MC_FRAME) 
        || !(macroblock_type & MACROBLOCK_MOTION_FORWARD))
      {
        /* frame-based prediction (broken into top and bottom halves
             for spatial scalability prediction purposes) */
        if (stwtop<2)
          form_prediction(MPDforward_reference_frame,0,MPDcurrent_frame,0,
            MPDcoded_picture_width,MPDcoded_picture_width<<1,16,8,bx,by,
            PMV[0][0][0],PMV[0][0][1],stwtop);

        if (stwbot<2)
          form_prediction(MPDforward_reference_frame,1,MPDcurrent_frame,1,
            MPDcoded_picture_width,MPDcoded_picture_width<<1,16,8,bx,by,
            PMV[0][0][0],PMV[0][0][1],stwbot);
      }
      else if (motion_type==MC_FIELD) /* field-based prediction */
      {
        /* top field prediction */
        if (stwtop<2)
          form_prediction(MPDforward_reference_frame,motion_vertical_field_select[0][0],
            MPDcurrent_frame,0,MPDcoded_picture_width<<1,MPDcoded_picture_width<<1,16,8,
            bx,by>>1,PMV[0][0][0],PMV[0][0][1]>>1,stwtop);

        /* bottom field prediction */
        if (stwbot<2)
          form_prediction(MPDforward_reference_frame,motion_vertical_field_select[1][0],
            MPDcurrent_frame,1,MPDcoded_picture_width<<1,MPDcoded_picture_width<<1,16,8,
            bx,by>>1,PMV[1][0][0],PMV[1][0][1]>>1,stwbot);
      }
      else if (motion_type==MC_DMV) /* dual prime prediction */
      {
        /* calculate derived motion vectors */
        Dual_Prime_Arithmetic(DMV,dmvector,PMV[0][0][0],PMV[0][0][1]>>1);

        if (stwtop<2)
        {
          /* predict top field from top field */
          form_prediction(MPDforward_reference_frame,0,MPDcurrent_frame,0,
            MPDcoded_picture_width<<1,MPDcoded_picture_width<<1,16,8,bx,by>>1,
            PMV[0][0][0],PMV[0][0][1]>>1,0);

          /* predict and add to top field from bottom field */
          form_prediction(MPDforward_reference_frame,1,MPDcurrent_frame,0,
            MPDcoded_picture_width<<1,MPDcoded_picture_width<<1,16,8,bx,by>>1,
            DMV[0][0],DMV[0][1],1);
        }

        if (stwbot<2)
        {
          /* predict bottom field from bottom field */
          form_prediction(MPDforward_reference_frame,1,MPDcurrent_frame,1,
            MPDcoded_picture_width<<1,MPDcoded_picture_width<<1,16,8,bx,by>>1,
            PMV[0][0][0],PMV[0][0][1]>>1,0);

          /* predict and add to bottom field from top field */
          form_prediction(MPDforward_reference_frame,0,MPDcurrent_frame,1,
            MPDcoded_picture_width<<1,MPDcoded_picture_width<<1,16,8,bx,by>>1,
            DMV[1][0],DMV[1][1],1);
        }
      }
    }
    else /* TOP_FIELD or BOTTOM_FIELD */
    {
      /* field picture */
      currentfield = (MPDpicture_structure==BOTTOM_FIELD);

      /* determine which frame to use for prediction */
      if ((MPDpicture_coding_type==P_TYPE) && MPDsecond_field
         && (currentfield!=motion_vertical_field_select[0][0]))
        predframe = MPDbackward_reference_frame; /* same frame */
      else
        predframe = MPDforward_reference_frame; /* previous frame */

      if ((motion_type==MC_FIELD)
        || !(macroblock_type & MACROBLOCK_MOTION_FORWARD))
      {
        /* field-based prediction */
        if (stwtop<2)
          form_prediction(predframe,motion_vertical_field_select[0][0],MPDcurrent_frame,0,
            MPDcoded_picture_width<<1,MPDcoded_picture_width<<1,16,16,bx,by,
            PMV[0][0][0],PMV[0][0][1],stwtop);
      }
      else if (motion_type==MC_16X8)
      {
        if (stwtop<2)
        {
          form_prediction(predframe,motion_vertical_field_select[0][0],MPDcurrent_frame,0,
            MPDcoded_picture_width<<1,MPDcoded_picture_width<<1,16,8,bx,by,
            PMV[0][0][0],PMV[0][0][1],stwtop);

          /* determine which frame to use for lower half prediction */
          if ((MPDpicture_coding_type==P_TYPE) && MPDsecond_field
             && (currentfield!=motion_vertical_field_select[1][0]))
            predframe = MPDbackward_reference_frame; /* same frame */
          else
            predframe = MPDforward_reference_frame; /* previous frame */

          form_prediction(predframe,motion_vertical_field_select[1][0],MPDcurrent_frame,0,
            MPDcoded_picture_width<<1,MPDcoded_picture_width<<1,16,8,bx,by+8,
            PMV[1][0][0],PMV[1][0][1],stwtop);
        }
      }
      else if (motion_type==MC_DMV) /* dual prime prediction */
      {
        if (MPDsecond_field)
          predframe = MPDbackward_reference_frame; /* same frame */
        else
          predframe = MPDforward_reference_frame; /* previous frame */

        /* calculate derived motion vectors */
        Dual_Prime_Arithmetic(DMV,dmvector,PMV[0][0][0],PMV[0][0][1]);

        /* predict from field of same parity */
        form_prediction(MPDforward_reference_frame,currentfield,MPDcurrent_frame,0,
          MPDcoded_picture_width<<1,MPDcoded_picture_width<<1,16,16,bx,by,
          PMV[0][0][0],PMV[0][0][1],0);

        /* predict from field of opposite parity */
        form_prediction(predframe,!currentfield,MPDcurrent_frame,0,
          MPDcoded_picture_width<<1,MPDcoded_picture_width<<1,16,16,bx,by,
          DMV[0][0],DMV[0][1],1);
      }
    }
    stwtop = stwbot = 1;
  }

  if (macroblock_type & MACROBLOCK_MOTION_BACKWARD)
  {
    if (MPDpicture_structure==FRAME_PICTURE)
    {
      if (motion_type==MC_FRAME)
      {
        /* frame-based prediction */
        if (stwtop<2)
          form_prediction(MPDbackward_reference_frame,0,MPDcurrent_frame,0,
            MPDcoded_picture_width,MPDcoded_picture_width<<1,16,8,bx,by,
            PMV[0][1][0],PMV[0][1][1],stwtop);

        if (stwbot<2)
          form_prediction(MPDbackward_reference_frame,1,MPDcurrent_frame,1,
            MPDcoded_picture_width,MPDcoded_picture_width<<1,16,8,bx,by,
            PMV[0][1][0],PMV[0][1][1],stwbot);
      }
      else /* field-based prediction */
      {
        /* top field prediction */
        if (stwtop<2)
          form_prediction(MPDbackward_reference_frame,motion_vertical_field_select[0][1],
            MPDcurrent_frame,0,MPDcoded_picture_width<<1,MPDcoded_picture_width<<1,16,8,
            bx,by>>1,PMV[0][1][0],PMV[0][1][1]>>1,stwtop);

        /* bottom field prediction */
        if (stwbot<2)
          form_prediction(MPDbackward_reference_frame,motion_vertical_field_select[1][1],
            MPDcurrent_frame,1,MPDcoded_picture_width<<1,MPDcoded_picture_width<<1,16,8,
            bx,by>>1,PMV[1][1][0],PMV[1][1][1]>>1,stwbot);
      }
    }
    else /* TOP_FIELD or BOTTOM_FIELD */
    {
      /* field picture */
      if (motion_type==MC_FIELD)
      {
        /* field-based prediction */
        form_prediction(MPDbackward_reference_frame,motion_vertical_field_select[0][1],
          MPDcurrent_frame,0,MPDcoded_picture_width<<1,MPDcoded_picture_width<<1,16,16,
          bx,by,PMV[0][1][0],PMV[0][1][1],stwtop);
      }
      else if (motion_type==MC_16X8)
      {
        form_prediction(MPDbackward_reference_frame,motion_vertical_field_select[0][1],
          MPDcurrent_frame,0,MPDcoded_picture_width<<1,MPDcoded_picture_width<<1,16,8,
          bx,by,PMV[0][1][0],PMV[0][1][1],stwtop);

        form_prediction(MPDbackward_reference_frame,motion_vertical_field_select[1][1],
          MPDcurrent_frame,0,MPDcoded_picture_width<<1,MPDcoded_picture_width<<1,16,8,
          bx,by+8,PMV[1][1][0],PMV[1][1][1],stwtop);
      }
    }
  }
}
