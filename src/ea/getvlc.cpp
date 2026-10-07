/* Adapted from getvlc.c and getvlc.h of the MPEG Software Simulation
   Group's mpeg2decode reference decoder (see docs/Licensing.md); its notice
   follows. */

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

// getvlc.cpp: the reference decoder's variable-length code tables (all of
// getvlc.h, its DCT tables included as globals) and readers. Changes from the
// reference: the decoder's globals carry the game's MPD names, all printing is
// absent, and the readers are defined in the game's order (the reverse of the
// reference's, with the static macroblock-type readers, inlined, before
// Get_macroblock_type). The layer-data view (its scalable mode at +1060 and
// spatial flag at +1072) is inferred; the constants are the reference's
// (mpeg2dec.h).
#define I_TYPE 1
#define P_TYPE 2
#define B_TYPE 3
#define D_TYPE 4
#define SC_SNR 3
#define MACROBLOCK_INTRA 1
#define MACROBLOCK_PATTERN 2
#define MACROBLOCK_MOTION_BACKWARD 4
#define MACROBLOCK_MOTION_FORWARD 8
#define MACROBLOCK_QUANT 16
#define SPATIAL_TEMPORAL_WEIGHT_CODE_FLAG 32
#define PERMITTED_SPATIAL_TEMPORAL_WEIGHT_CLASS 64
#define MB_WEIGHT 32
#define MB_CLASS4 64
#define ERROR (-1)

struct LayerDataView {
    unsigned char unknown000[1060];
    int scalable_mode;
    unsigned char unknown428[8];
    int pict_scal;
};

extern LayerDataView* MPDld;
extern int MPDfault_flag;
extern int MPDpicture_coding_type;

int Get_Bits(int n);
int Get_Bits1();
int Show_Bits(int n);
void Flush_Buffer(int n);

typedef struct {
  char val, len;
} VLCtab;

typedef struct {
  char run, level, len;
} DCTtab;

/* Table B-3, macroblock_type in P-pictures, codes 001..1xx */
static VLCtab PMBtab0[8] = {
  {ERROR,0},
  {MACROBLOCK_MOTION_FORWARD,3},
  {MACROBLOCK_PATTERN,2}, {MACROBLOCK_PATTERN,2},
  {MACROBLOCK_MOTION_FORWARD|MACROBLOCK_PATTERN,1}, 
  {MACROBLOCK_MOTION_FORWARD|MACROBLOCK_PATTERN,1},
  {MACROBLOCK_MOTION_FORWARD|MACROBLOCK_PATTERN,1}, 
  {MACROBLOCK_MOTION_FORWARD|MACROBLOCK_PATTERN,1}
};

/* Table B-3, macroblock_type in P-pictures, codes 000001..00011x */
static VLCtab PMBtab1[8] = {
  {ERROR,0},
  {MACROBLOCK_QUANT|MACROBLOCK_INTRA,6},
  {MACROBLOCK_QUANT|MACROBLOCK_PATTERN,5}, {MACROBLOCK_QUANT|MACROBLOCK_PATTERN,5},
  {MACROBLOCK_QUANT|MACROBLOCK_MOTION_FORWARD|MACROBLOCK_PATTERN,5}, {MACROBLOCK_QUANT|MACROBLOCK_MOTION_FORWARD|MACROBLOCK_PATTERN,5},
  {MACROBLOCK_INTRA,5}, {MACROBLOCK_INTRA,5}
};

/* Table B-4, macroblock_type in B-pictures, codes 0010..11xx */
static VLCtab BMBtab0[16] = {
  {ERROR,0}, 
  {ERROR,0},
  {MACROBLOCK_MOTION_FORWARD,4},
  {MACROBLOCK_MOTION_FORWARD|MACROBLOCK_PATTERN,4},
  {MACROBLOCK_MOTION_BACKWARD,3}, 
  {MACROBLOCK_MOTION_BACKWARD,3},
  {MACROBLOCK_MOTION_BACKWARD|MACROBLOCK_PATTERN,3}, 
  {MACROBLOCK_MOTION_BACKWARD|MACROBLOCK_PATTERN,3},
  {MACROBLOCK_MOTION_FORWARD|MACROBLOCK_MOTION_BACKWARD,2}, 
  {MACROBLOCK_MOTION_FORWARD|MACROBLOCK_MOTION_BACKWARD,2},
  {MACROBLOCK_MOTION_FORWARD|MACROBLOCK_MOTION_BACKWARD,2}, 
  {MACROBLOCK_MOTION_FORWARD|MACROBLOCK_MOTION_BACKWARD,2},
  {MACROBLOCK_MOTION_FORWARD|MACROBLOCK_MOTION_BACKWARD|MACROBLOCK_PATTERN,2},
  {MACROBLOCK_MOTION_FORWARD|MACROBLOCK_MOTION_BACKWARD|MACROBLOCK_PATTERN,2},
  {MACROBLOCK_MOTION_FORWARD|MACROBLOCK_MOTION_BACKWARD|MACROBLOCK_PATTERN,2},
  {MACROBLOCK_MOTION_FORWARD|MACROBLOCK_MOTION_BACKWARD|MACROBLOCK_PATTERN,2}
};

/* Table B-4, macroblock_type in B-pictures, codes 000001..00011x */
static VLCtab BMBtab1[8] = {
  {ERROR,0},
  {MACROBLOCK_QUANT|MACROBLOCK_INTRA,6},
  {MACROBLOCK_QUANT|MACROBLOCK_MOTION_BACKWARD|MACROBLOCK_PATTERN,6},
  {MACROBLOCK_QUANT|MACROBLOCK_MOTION_FORWARD|MACROBLOCK_PATTERN,6},
  {MACROBLOCK_QUANT|MACROBLOCK_MOTION_FORWARD|MACROBLOCK_MOTION_BACKWARD|MACROBLOCK_PATTERN,5},
  {MACROBLOCK_QUANT|MACROBLOCK_MOTION_FORWARD|MACROBLOCK_MOTION_BACKWARD|MACROBLOCK_PATTERN,5},
  {MACROBLOCK_INTRA,5}, 
  {MACROBLOCK_INTRA,5}
};

/* Table B-5, macroblock_type in spat. scal. I-pictures, codes 0001..1xxx */
static VLCtab spIMBtab[16] = {
  {ERROR,0},
  {PERMITTED_SPATIAL_TEMPORAL_WEIGHT_CLASS,4},
  {MACROBLOCK_QUANT|MACROBLOCK_INTRA,4},
  {MACROBLOCK_INTRA,4},
  {PERMITTED_SPATIAL_TEMPORAL_WEIGHT_CLASS|MACROBLOCK_QUANT|MACROBLOCK_PATTERN,2}, {PERMITTED_SPATIAL_TEMPORAL_WEIGHT_CLASS|MACROBLOCK_QUANT|MACROBLOCK_PATTERN,2},
  {PERMITTED_SPATIAL_TEMPORAL_WEIGHT_CLASS|MACROBLOCK_QUANT|MACROBLOCK_PATTERN,2}, {PERMITTED_SPATIAL_TEMPORAL_WEIGHT_CLASS|MACROBLOCK_QUANT|MACROBLOCK_PATTERN,2},
  {PERMITTED_SPATIAL_TEMPORAL_WEIGHT_CLASS|MACROBLOCK_PATTERN,1}, {PERMITTED_SPATIAL_TEMPORAL_WEIGHT_CLASS|MACROBLOCK_PATTERN,1},
  {PERMITTED_SPATIAL_TEMPORAL_WEIGHT_CLASS|MACROBLOCK_PATTERN,1}, {PERMITTED_SPATIAL_TEMPORAL_WEIGHT_CLASS|MACROBLOCK_PATTERN,1},
  {PERMITTED_SPATIAL_TEMPORAL_WEIGHT_CLASS|MACROBLOCK_PATTERN,1}, {PERMITTED_SPATIAL_TEMPORAL_WEIGHT_CLASS|MACROBLOCK_PATTERN,1},
  {PERMITTED_SPATIAL_TEMPORAL_WEIGHT_CLASS|MACROBLOCK_PATTERN,1}, {PERMITTED_SPATIAL_TEMPORAL_WEIGHT_CLASS|MACROBLOCK_PATTERN,1}
};

/* Table B-6, macroblock_type in spat. scal. P-pictures, codes 0010..11xx */
static VLCtab spPMBtab0[16] =
{
  {ERROR,0},
  {ERROR,0},
  {MACROBLOCK_MOTION_FORWARD,4},
  {SPATIAL_TEMPORAL_WEIGHT_CODE_FLAG|MACROBLOCK_MOTION_FORWARD,4},
  {MACROBLOCK_QUANT|MACROBLOCK_MOTION_FORWARD|MACROBLOCK_PATTERN,3}, {MACROBLOCK_QUANT|MACROBLOCK_MOTION_FORWARD|MACROBLOCK_PATTERN,3},
  {SPATIAL_TEMPORAL_WEIGHT_CODE_FLAG|MACROBLOCK_MOTION_FORWARD|MACROBLOCK_PATTERN,3}, {SPATIAL_TEMPORAL_WEIGHT_CODE_FLAG|MACROBLOCK_MOTION_FORWARD|MACROBLOCK_PATTERN,3},
  {MACROBLOCK_MOTION_FORWARD|MACROBLOCK_PATTERN,2}, 
  {MACROBLOCK_MOTION_FORWARD|MACROBLOCK_PATTERN,2},
  {MACROBLOCK_MOTION_FORWARD|MACROBLOCK_PATTERN,2}, 
  {MACROBLOCK_MOTION_FORWARD|MACROBLOCK_PATTERN,2},
  {SPATIAL_TEMPORAL_WEIGHT_CODE_FLAG|MACROBLOCK_QUANT|MACROBLOCK_MOTION_FORWARD|MACROBLOCK_PATTERN,2},
  {SPATIAL_TEMPORAL_WEIGHT_CODE_FLAG|MACROBLOCK_QUANT|MACROBLOCK_MOTION_FORWARD|MACROBLOCK_PATTERN,2},
  {SPATIAL_TEMPORAL_WEIGHT_CODE_FLAG|MACROBLOCK_QUANT|MACROBLOCK_MOTION_FORWARD|MACROBLOCK_PATTERN,2},
  {SPATIAL_TEMPORAL_WEIGHT_CODE_FLAG|MACROBLOCK_QUANT|MACROBLOCK_MOTION_FORWARD|MACROBLOCK_PATTERN,2}
};

/* Table B-6, macroblock_type in spat. scal. P-pictures, codes 0000010..000111x */
static VLCtab spPMBtab1[16] = {
  {ERROR,0},
  {ERROR,0},
  {PERMITTED_SPATIAL_TEMPORAL_WEIGHT_CLASS|MACROBLOCK_QUANT|MACROBLOCK_PATTERN,7},
  {PERMITTED_SPATIAL_TEMPORAL_WEIGHT_CLASS,7},
  {MACROBLOCK_PATTERN,7},
  {PERMITTED_SPATIAL_TEMPORAL_WEIGHT_CLASS|MACROBLOCK_PATTERN,7},
  {MACROBLOCK_QUANT|MACROBLOCK_INTRA,7},
  {MACROBLOCK_INTRA,7},
  {MACROBLOCK_QUANT|MACROBLOCK_PATTERN,6}, 
  {MACROBLOCK_QUANT|MACROBLOCK_PATTERN,6},
  {SPATIAL_TEMPORAL_WEIGHT_CODE_FLAG|MACROBLOCK_QUANT|MACROBLOCK_PATTERN,6}, 
  {SPATIAL_TEMPORAL_WEIGHT_CODE_FLAG|MACROBLOCK_QUANT|MACROBLOCK_PATTERN,6},
  {SPATIAL_TEMPORAL_WEIGHT_CODE_FLAG,6}, 
  {SPATIAL_TEMPORAL_WEIGHT_CODE_FLAG,6},
  {SPATIAL_TEMPORAL_WEIGHT_CODE_FLAG|MACROBLOCK_PATTERN,6}, 
  {SPATIAL_TEMPORAL_WEIGHT_CODE_FLAG|MACROBLOCK_PATTERN,6}
};

/* Table B-7, macroblock_type in spat. scal. B-pictures, codes 0010..11xx */
static VLCtab spBMBtab0[14] = {
  {MACROBLOCK_MOTION_FORWARD,4},
  {MACROBLOCK_MOTION_FORWARD|MACROBLOCK_PATTERN,4},
  {MACROBLOCK_MOTION_BACKWARD,3}, 
  {MACROBLOCK_MOTION_BACKWARD,3},
  {MACROBLOCK_MOTION_BACKWARD|MACROBLOCK_PATTERN,3}, 
  {MACROBLOCK_MOTION_BACKWARD|MACROBLOCK_PATTERN,3},
  {MACROBLOCK_MOTION_FORWARD|MACROBLOCK_MOTION_BACKWARD,2}, 
  {MACROBLOCK_MOTION_FORWARD|MACROBLOCK_MOTION_BACKWARD,2},
  {MACROBLOCK_MOTION_FORWARD|MACROBLOCK_MOTION_BACKWARD,2}, 
  {MACROBLOCK_MOTION_FORWARD|MACROBLOCK_MOTION_BACKWARD,2},
  {MACROBLOCK_MOTION_FORWARD|MACROBLOCK_MOTION_BACKWARD|MACROBLOCK_PATTERN,2},
  {MACROBLOCK_MOTION_FORWARD|MACROBLOCK_MOTION_BACKWARD|MACROBLOCK_PATTERN,2},
  {MACROBLOCK_MOTION_FORWARD|MACROBLOCK_MOTION_BACKWARD|MACROBLOCK_PATTERN,2},
  {MACROBLOCK_MOTION_FORWARD|MACROBLOCK_MOTION_BACKWARD|MACROBLOCK_PATTERN,2}
};

/* Table B-7, macroblock_type in spat. scal. B-pictures, codes 0000100..000111x */
static VLCtab spBMBtab1[12] = {
  {MACROBLOCK_QUANT|MACROBLOCK_MOTION_FORWARD|MACROBLOCK_PATTERN,7},
  {MACROBLOCK_QUANT|MACROBLOCK_MOTION_BACKWARD|MACROBLOCK_PATTERN,7},
  {MACROBLOCK_INTRA,7},
  {MACROBLOCK_QUANT|MACROBLOCK_MOTION_FORWARD|MACROBLOCK_MOTION_BACKWARD|MACROBLOCK_PATTERN,7},
  {SPATIAL_TEMPORAL_WEIGHT_CODE_FLAG|MACROBLOCK_MOTION_FORWARD,6}, 
  {SPATIAL_TEMPORAL_WEIGHT_CODE_FLAG|MACROBLOCK_MOTION_FORWARD,6},
  {SPATIAL_TEMPORAL_WEIGHT_CODE_FLAG|MACROBLOCK_MOTION_FORWARD|MACROBLOCK_PATTERN,6}, 
  {SPATIAL_TEMPORAL_WEIGHT_CODE_FLAG|MACROBLOCK_MOTION_FORWARD|MACROBLOCK_PATTERN,6},
  {SPATIAL_TEMPORAL_WEIGHT_CODE_FLAG|MACROBLOCK_MOTION_BACKWARD,6}, 
  {SPATIAL_TEMPORAL_WEIGHT_CODE_FLAG|MACROBLOCK_MOTION_BACKWARD,6},
  {SPATIAL_TEMPORAL_WEIGHT_CODE_FLAG|MACROBLOCK_MOTION_BACKWARD|MACROBLOCK_PATTERN,6}, 
  {SPATIAL_TEMPORAL_WEIGHT_CODE_FLAG|MACROBLOCK_MOTION_BACKWARD|MACROBLOCK_PATTERN,6}
};

/* Table B-7, macroblock_type in spat. scal. B-pictures, codes 00000100x..000001111 */
static VLCtab spBMBtab2[8] = {
  {MACROBLOCK_QUANT|MACROBLOCK_INTRA,8}, 
  {MACROBLOCK_QUANT|MACROBLOCK_INTRA,8},
  {SPATIAL_TEMPORAL_WEIGHT_CODE_FLAG|MACROBLOCK_QUANT|MACROBLOCK_MOTION_FORWARD|MACROBLOCK_PATTERN,8},
  {SPATIAL_TEMPORAL_WEIGHT_CODE_FLAG|MACROBLOCK_QUANT|MACROBLOCK_MOTION_FORWARD|MACROBLOCK_PATTERN,8},
  {SPATIAL_TEMPORAL_WEIGHT_CODE_FLAG|MACROBLOCK_QUANT|MACROBLOCK_MOTION_BACKWARD|MACROBLOCK_PATTERN,9},
  {PERMITTED_SPATIAL_TEMPORAL_WEIGHT_CLASS|MACROBLOCK_QUANT|MACROBLOCK_PATTERN,9},
  {PERMITTED_SPATIAL_TEMPORAL_WEIGHT_CLASS,9},
  {PERMITTED_SPATIAL_TEMPORAL_WEIGHT_CLASS|MACROBLOCK_PATTERN,9}
};

/* Table B-8, macroblock_type in spat. scal. B-pictures, codes 001..1xx */
static VLCtab SNRMBtab[8] = {
  {ERROR,0},
  {0,3},
  {MACROBLOCK_QUANT|MACROBLOCK_PATTERN,2}, 
  {MACROBLOCK_QUANT|MACROBLOCK_PATTERN,2},
  {MACROBLOCK_PATTERN,1}, 
  {MACROBLOCK_PATTERN,1}, 
  {MACROBLOCK_PATTERN,1}, 
  {MACROBLOCK_PATTERN,1}
};

/* Table B-10, motion_code, codes 0001 ... 01xx */
static VLCtab MVtab0[8] =
{ {ERROR,0}, {3,3}, {2,2}, {2,2}, {1,1}, {1,1}, {1,1}, {1,1}
};

/* Table B-10, motion_code, codes 0000011 ... 000011x */
static VLCtab MVtab1[8] =
{ {ERROR,0}, {ERROR,0}, {ERROR,0}, {7,6}, {6,6}, {5,6}, {4,5}, {4,5}
};

/* Table B-10, motion_code, codes 0000001100 ... 000001011x */
static VLCtab MVtab2[12] =
{ {16,9}, {15,9}, {14,9}, {13,9},
  {12,9}, {11,9}, {10,8}, {10,8},
  {9,8},  {9,8},  {8,8},  {8,8}
};

/* Table B-9, coded_block_pattern, codes 01000 ... 111xx */
static VLCtab CBPtab0[32] =
{ {ERROR,0}, {ERROR,0}, {ERROR,0}, {ERROR,0},
  {ERROR,0}, {ERROR,0}, {ERROR,0}, {ERROR,0},
  {62,5}, {2,5},  {61,5}, {1,5},  {56,5}, {52,5}, {44,5}, {28,5},
  {40,5}, {20,5}, {48,5}, {12,5}, {32,4}, {32,4}, {16,4}, {16,4},
  {8,4},  {8,4},  {4,4},  {4,4},  {60,3}, {60,3}, {60,3}, {60,3}
};

/* Table B-9, coded_block_pattern, codes 00000100 ... 001111xx */
static VLCtab CBPtab1[64] =
{ {ERROR,0}, {ERROR,0}, {ERROR,0}, {ERROR,0},
  {58,8}, {54,8}, {46,8}, {30,8},
  {57,8}, {53,8}, {45,8}, {29,8}, {38,8}, {26,8}, {37,8}, {25,8},
  {43,8}, {23,8}, {51,8}, {15,8}, {42,8}, {22,8}, {50,8}, {14,8},
  {41,8}, {21,8}, {49,8}, {13,8}, {35,8}, {19,8}, {11,8}, {7,8},
  {34,7}, {34,7}, {18,7}, {18,7}, {10,7}, {10,7}, {6,7},  {6,7},
  {33,7}, {33,7}, {17,7}, {17,7}, {9,7},  {9,7},  {5,7},  {5,7},
  {63,6}, {63,6}, {63,6}, {63,6}, {3,6},  {3,6},  {3,6},  {3,6},
  {36,6}, {36,6}, {36,6}, {36,6}, {24,6}, {24,6}, {24,6}, {24,6}
};

/* Table B-9, coded_block_pattern, codes 000000001 ... 000000111 */
static VLCtab CBPtab2[8] =
{ {ERROR,0}, {0,9}, {39,9}, {27,9}, {59,9}, {55,9}, {47,9}, {31,9}
};

/* Table B-1, macroblock_address_increment, codes 00010 ... 011xx */
static VLCtab MBAtab1[16] =
{ {ERROR,0}, {ERROR,0}, {7,5}, {6,5}, {5,4}, {5,4}, {4,4}, {4,4},
  {3,3}, {3,3}, {3,3}, {3,3}, {2,3}, {2,3}, {2,3}, {2,3}
};

/* Table B-1, macroblock_address_increment, codes 00000011000 ... 0000111xxxx */
static VLCtab MBAtab2[104] =
{
  {33,11}, {32,11}, {31,11}, {30,11}, {29,11}, {28,11}, {27,11}, {26,11},
  {25,11}, {24,11}, {23,11}, {22,11}, {21,10}, {21,10}, {20,10}, {20,10},
  {19,10}, {19,10}, {18,10}, {18,10}, {17,10}, {17,10}, {16,10}, {16,10},
  {15,8},  {15,8},  {15,8},  {15,8},  {15,8},  {15,8},  {15,8},  {15,8},
  {14,8},  {14,8},  {14,8},  {14,8},  {14,8},  {14,8},  {14,8},  {14,8},
  {13,8},  {13,8},  {13,8},  {13,8},  {13,8},  {13,8},  {13,8},  {13,8},
  {12,8},  {12,8},  {12,8},  {12,8},  {12,8},  {12,8},  {12,8},  {12,8},
  {11,8},  {11,8},  {11,8},  {11,8},  {11,8},  {11,8},  {11,8},  {11,8},
  {10,8},  {10,8},  {10,8},  {10,8},  {10,8},  {10,8},  {10,8},  {10,8},
  {9,7},   {9,7},   {9,7},   {9,7},   {9,7},   {9,7},   {9,7},   {9,7},
  {9,7},   {9,7},   {9,7},   {9,7},   {9,7},   {9,7},   {9,7},   {9,7},
  {8,7},   {8,7},   {8,7},   {8,7},   {8,7},   {8,7},   {8,7},   {8,7},
  {8,7},   {8,7},   {8,7},   {8,7},   {8,7},   {8,7},   {8,7},   {8,7}
};

/* Table B-12, dct_dc_size_luminance, codes 00xxx ... 11110 */
static VLCtab DClumtab0[32] =
{ {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2},
  {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2},
  {0, 3}, {0, 3}, {0, 3}, {0, 3}, {3, 3}, {3, 3}, {3, 3}, {3, 3},
  {4, 3}, {4, 3}, {4, 3}, {4, 3}, {5, 4}, {5, 4}, {6, 5}, {ERROR, 0}
};

/* Table B-12, dct_dc_size_luminance, codes 111110xxx ... 111111111 */
static VLCtab DClumtab1[16] =
{ {7, 6}, {7, 6}, {7, 6}, {7, 6}, {7, 6}, {7, 6}, {7, 6}, {7, 6},
  {8, 7}, {8, 7}, {8, 7}, {8, 7}, {9, 8}, {9, 8}, {10,9}, {11,9}
};

/* Table B-13, dct_dc_size_chrominance, codes 00xxx ... 11110 */
static VLCtab DCchromtab0[32] =
{ {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2},
  {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2}, {1, 2},
  {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2},
  {3, 3}, {3, 3}, {3, 3}, {3, 3}, {4, 4}, {4, 4}, {5, 5}, {ERROR, 0}
};

/* Table B-13, dct_dc_size_chrominance, codes 111110xxxx ... 1111111111 */
static VLCtab DCchromtab1[32] =
{ {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6},
  {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6},
  {7, 7}, {7, 7}, {7, 7}, {7, 7}, {7, 7}, {7, 7}, {7, 7}, {7, 7},
  {8, 8}, {8, 8}, {8, 8}, {8, 8}, {9, 9}, {9, 9}, {10,10}, {11,10}
};

/* Table B-14, DCT coefficients table zero,
 * codes 0100 ... 1xxx (used for first (DC) coefficient)
 */
DCTtab DCTtabfirst[12] =
{
  {0,2,4}, {2,1,4}, {1,1,3}, {1,1,3},
  {0,1,1}, {0,1,1}, {0,1,1}, {0,1,1},
  {0,1,1}, {0,1,1}, {0,1,1}, {0,1,1}
};

/* Table B-14, DCT coefficients table zero,
 * codes 0100 ... 1xxx (used for all other coefficients)
 */
DCTtab DCTtabnext[12] =
{
  {0,2,4},  {2,1,4},  {1,1,3},  {1,1,3},
  {64,0,2}, {64,0,2}, {64,0,2}, {64,0,2}, /* EOB */
  {0,1,2},  {0,1,2},  {0,1,2},  {0,1,2}
};

/* Table B-14, DCT coefficients table zero,
 * codes 000001xx ... 00111xxx
 */
DCTtab DCTtab0[60] =
{
  {65,0,6}, {65,0,6}, {65,0,6}, {65,0,6}, /* Escape */
  {2,2,7}, {2,2,7}, {9,1,7}, {9,1,7},
  {0,4,7}, {0,4,7}, {8,1,7}, {8,1,7},
  {7,1,6}, {7,1,6}, {7,1,6}, {7,1,6},
  {6,1,6}, {6,1,6}, {6,1,6}, {6,1,6},
  {1,2,6}, {1,2,6}, {1,2,6}, {1,2,6},
  {5,1,6}, {5,1,6}, {5,1,6}, {5,1,6},
  {13,1,8}, {0,6,8}, {12,1,8}, {11,1,8},
  {3,2,8}, {1,3,8}, {0,5,8}, {10,1,8},
  {0,3,5}, {0,3,5}, {0,3,5}, {0,3,5},
  {0,3,5}, {0,3,5}, {0,3,5}, {0,3,5},
  {4,1,5}, {4,1,5}, {4,1,5}, {4,1,5},
  {4,1,5}, {4,1,5}, {4,1,5}, {4,1,5},
  {3,1,5}, {3,1,5}, {3,1,5}, {3,1,5},
  {3,1,5}, {3,1,5}, {3,1,5}, {3,1,5}
};

/* Table B-15, DCT coefficients table one,
 * codes 000001xx ... 11111111
*/
DCTtab DCTtab0a[252] =
{
  {65,0,6}, {65,0,6}, {65,0,6}, {65,0,6}, /* Escape */
  {7,1,7}, {7,1,7}, {8,1,7}, {8,1,7},
  {6,1,7}, {6,1,7}, {2,2,7}, {2,2,7},
  {0,7,6}, {0,7,6}, {0,7,6}, {0,7,6},
  {0,6,6}, {0,6,6}, {0,6,6}, {0,6,6},
  {4,1,6}, {4,1,6}, {4,1,6}, {4,1,6},
  {5,1,6}, {5,1,6}, {5,1,6}, {5,1,6},
  {1,5,8}, {11,1,8}, {0,11,8}, {0,10,8},
  {13,1,8}, {12,1,8}, {3,2,8}, {1,4,8},
  {2,1,5}, {2,1,5}, {2,1,5}, {2,1,5},
  {2,1,5}, {2,1,5}, {2,1,5}, {2,1,5},
  {1,2,5}, {1,2,5}, {1,2,5}, {1,2,5},
  {1,2,5}, {1,2,5}, {1,2,5}, {1,2,5},
  {3,1,5}, {3,1,5}, {3,1,5}, {3,1,5},
  {3,1,5}, {3,1,5}, {3,1,5}, {3,1,5},
  {1,1,3}, {1,1,3}, {1,1,3}, {1,1,3},
  {1,1,3}, {1,1,3}, {1,1,3}, {1,1,3},
  {1,1,3}, {1,1,3}, {1,1,3}, {1,1,3},
  {1,1,3}, {1,1,3}, {1,1,3}, {1,1,3},
  {1,1,3}, {1,1,3}, {1,1,3}, {1,1,3},
  {1,1,3}, {1,1,3}, {1,1,3}, {1,1,3},
  {1,1,3}, {1,1,3}, {1,1,3}, {1,1,3},
  {1,1,3}, {1,1,3}, {1,1,3}, {1,1,3},
  {64,0,4}, {64,0,4}, {64,0,4}, {64,0,4}, /* EOB */
  {64,0,4}, {64,0,4}, {64,0,4}, {64,0,4},
  {64,0,4}, {64,0,4}, {64,0,4}, {64,0,4},
  {64,0,4}, {64,0,4}, {64,0,4}, {64,0,4},
  {0,3,4}, {0,3,4}, {0,3,4}, {0,3,4},
  {0,3,4}, {0,3,4}, {0,3,4}, {0,3,4},
  {0,3,4}, {0,3,4}, {0,3,4}, {0,3,4},
  {0,3,4}, {0,3,4}, {0,3,4}, {0,3,4},
  {0,1,2}, {0,1,2}, {0,1,2}, {0,1,2},
  {0,1,2}, {0,1,2}, {0,1,2}, {0,1,2},
  {0,1,2}, {0,1,2}, {0,1,2}, {0,1,2},
  {0,1,2}, {0,1,2}, {0,1,2}, {0,1,2},
  {0,1,2}, {0,1,2}, {0,1,2}, {0,1,2},
  {0,1,2}, {0,1,2}, {0,1,2}, {0,1,2},
  {0,1,2}, {0,1,2}, {0,1,2}, {0,1,2},
  {0,1,2}, {0,1,2}, {0,1,2}, {0,1,2},
  {0,1,2}, {0,1,2}, {0,1,2}, {0,1,2},
  {0,1,2}, {0,1,2}, {0,1,2}, {0,1,2},
  {0,1,2}, {0,1,2}, {0,1,2}, {0,1,2},
  {0,1,2}, {0,1,2}, {0,1,2}, {0,1,2},
  {0,1,2}, {0,1,2}, {0,1,2}, {0,1,2},
  {0,1,2}, {0,1,2}, {0,1,2}, {0,1,2},
  {0,1,2}, {0,1,2}, {0,1,2}, {0,1,2},
  {0,1,2}, {0,1,2}, {0,1,2}, {0,1,2},
  {0,2,3}, {0,2,3}, {0,2,3}, {0,2,3},
  {0,2,3}, {0,2,3}, {0,2,3}, {0,2,3},
  {0,2,3}, {0,2,3}, {0,2,3}, {0,2,3},
  {0,2,3}, {0,2,3}, {0,2,3}, {0,2,3},
  {0,2,3}, {0,2,3}, {0,2,3}, {0,2,3},
  {0,2,3}, {0,2,3}, {0,2,3}, {0,2,3},
  {0,2,3}, {0,2,3}, {0,2,3}, {0,2,3},
  {0,2,3}, {0,2,3}, {0,2,3}, {0,2,3},
  {0,4,5}, {0,4,5}, {0,4,5}, {0,4,5},
  {0,4,5}, {0,4,5}, {0,4,5}, {0,4,5},
  {0,5,5}, {0,5,5}, {0,5,5}, {0,5,5},
  {0,5,5}, {0,5,5}, {0,5,5}, {0,5,5},
  {9,1,7}, {9,1,7}, {1,3,7}, {1,3,7},
  {10,1,7}, {10,1,7}, {0,8,7}, {0,8,7},
  {0,9,7}, {0,9,7}, {0,12,8}, {0,13,8},
  {2,3,8}, {4,2,8}, {0,14,8}, {0,15,8}
};

/* Table B-14, DCT coefficients table zero,
 * codes 0000001000 ... 0000001111
 */
DCTtab DCTtab1[8] =
{
  {16,1,10}, {5,2,10}, {0,7,10}, {2,3,10},
  {1,4,10}, {15,1,10}, {14,1,10}, {4,2,10}
};

/* Table B-15, DCT coefficients table one,
 * codes 000000100x ... 000000111x
 */
DCTtab DCTtab1a[8] =
{
  {5,2,9}, {5,2,9}, {14,1,9}, {14,1,9},
  {2,4,10}, {16,1,10}, {15,1,9}, {15,1,9}
};

/* Table B-14/15, DCT coefficients table zero / one,
 * codes 000000010000 ... 000000011111
 */
DCTtab DCTtab2[16] =
{
  {0,11,12}, {8,2,12}, {4,3,12}, {0,10,12},
  {2,4,12}, {7,2,12}, {21,1,12}, {20,1,12},
  {0,9,12}, {19,1,12}, {18,1,12}, {1,5,12},
  {3,3,12}, {0,8,12}, {6,2,12}, {17,1,12}
};

/* Table B-14/15, DCT coefficients table zero / one,
 * codes 0000000010000 ... 0000000011111
 */
DCTtab DCTtab3[16] =
{
  {10,2,13}, {9,2,13}, {5,3,13}, {3,4,13},
  {2,5,13}, {1,7,13}, {1,6,13}, {0,15,13},
  {0,14,13}, {0,13,13}, {0,12,13}, {26,1,13},
  {25,1,13}, {24,1,13}, {23,1,13}, {22,1,13}
};

/* Table B-14/15, DCT coefficients table zero / one,
 * codes 00000000010000 ... 00000000011111
 */
DCTtab DCTtab4[16] =
{
  {0,31,14}, {0,30,14}, {0,29,14}, {0,28,14},
  {0,27,14}, {0,26,14}, {0,25,14}, {0,24,14},
  {0,23,14}, {0,22,14}, {0,21,14}, {0,20,14},
  {0,19,14}, {0,18,14}, {0,17,14}, {0,16,14}
};

/* Table B-14/15, DCT coefficients table zero / one,
 * codes 000000000010000 ... 000000000011111
 */
DCTtab DCTtab5[16] =
{
  {0,40,15}, {0,39,15}, {0,38,15}, {0,37,15},
  {0,36,15}, {0,35,15}, {0,34,15}, {0,33,15},
  {0,32,15}, {1,14,15}, {1,13,15}, {1,12,15},
  {1,11,15}, {1,10,15}, {1,9,15}, {1,8,15}
};

/* Table B-14/15, DCT coefficients table zero / one,
 * codes 0000000000010000 ... 0000000000011111
 */
DCTtab DCTtab6[16] =
{
  {1,18,16}, {1,17,16}, {1,16,16}, {1,15,16},
  {6,3,16}, {16,2,16}, {15,2,16}, {14,2,16},
  {13,2,16}, {12,2,16}, {11,2,16}, {31,1,16},
  {30,1,16}, {29,1,16}, {28,1,16}, {27,1,16}
};

int Get_Chroma_DC_dct_diff()
{
  int code, size, dct_diff;


  /* decode length */
  code = Show_Bits(5);

  if (code<31)
  {
    size = DCchromtab0[code].val;
    Flush_Buffer(DCchromtab0[code].len);

  }
  else
  {
    code = Show_Bits(10) - 0x3e0;
    size = DCchromtab1[code].val;
    Flush_Buffer(DCchromtab1[code].len);

  }


  if (size==0)
    dct_diff = 0;
  else
  {
    dct_diff = Get_Bits(size);
    if ((dct_diff & (1<<(size-1)))==0)
      dct_diff-= (1<<size) - 1;
  }


  return dct_diff;
}

int Get_Luma_DC_dct_diff()
{
  int code, size, dct_diff;


  /* decode length */
  code = Show_Bits(5);

  if (code<31)
  {
    size = DClumtab0[code].val;
    Flush_Buffer(DClumtab0[code].len);
  }
  else
  {
    code = Show_Bits(9) - 0x1f0;
    size = DClumtab1[code].val;
    Flush_Buffer(DClumtab1[code].len);

  }


  if (size==0)
    dct_diff = 0;
  else
  {
    dct_diff = Get_Bits(size);
    if ((dct_diff & (1<<(size-1)))==0)
      dct_diff-= (1<<size) - 1;
  }


  return dct_diff;
}

int Get_macroblock_address_increment()
{
  int code, val;


  val = 0;

  while ((code = Show_Bits(11))<24)
  {
    if (code!=15) /* if not macroblock_stuffing */
    {
      if (code==8) /* if macroblock_escape */
      {

        val+= 33;
      }
      else
      {

        MPDfault_flag = 1;
        return 1;
      }
    }
    else /* macroblock suffing */
    {
    }

    Flush_Buffer(11);
  }

  /* macroblock_address_increment == 1 */
  /* ('1' is in the MSB position of the lookahead) */
  if (code>=1024)
  {
    Flush_Buffer(1);
    return val + 1;
  }

  /* codes 00010 ... 011xx */
  if (code>=128)
  {
    /* remove leading zeros */
    code >>= 6;
    Flush_Buffer(MBAtab1[code].len);


    
    return val + MBAtab1[code].val;
  }
  
  /* codes 00000011000 ... 0000111xxxx */
  code-= 24; /* remove common base */
  Flush_Buffer(MBAtab2[code].len);


  return val + MBAtab2[code].val;
}

int Get_coded_block_pattern()
{
  int code;


  if ((code = Show_Bits(9))>=128)
  {
    code >>= 4;
    Flush_Buffer(CBPtab0[code].len);


    return CBPtab0[code].val;
  }

  if (code>=8)
  {
    code >>= 1;
    Flush_Buffer(CBPtab1[code].len);


    return CBPtab1[code].val;
  }

  if (code<1)
  {
    MPDfault_flag = 1;
    return 0;
  }

  Flush_Buffer(CBPtab2[code].len);


  return CBPtab2[code].val;
}

int Get_dmvector()
{

  if (Get_Bits(1))
  {
    return Get_Bits(1) ? -1 : 1;
  }
  else
  {
    return 0;
  }
}

int Get_motion_code()
{
  int code;


  if (Get_Bits1())
  {
    return 0;
  }

  if ((code = Show_Bits(9))>=64)
  {
    code >>= 6;
    Flush_Buffer(MVtab0[code].len);


    return Get_Bits1()?-MVtab0[code].val:MVtab0[code].val;
  }

  if (code>=24)
  {
    code >>= 3;
    Flush_Buffer(MVtab1[code].len);


    return Get_Bits1()?-MVtab1[code].val:MVtab1[code].val;
  }

  if ((code-=12)<0)
  {
/* HACK */
    MPDfault_flag=1;
    return 0;
  }

  Flush_Buffer(MVtab2[code].len);


  return Get_Bits1() ? -MVtab2[code].val : MVtab2[code].val;
}

static inline int Get_I_macroblock_type()
{

  if (Get_Bits1())
  {
    return 1;
  }

  if (!Get_Bits1())
  {
    MPDfault_flag = 1;
  }


  return 17;
}

static inline int Get_P_macroblock_type()
{
  int code;


  if ((code = Show_Bits(6))>=8)
  {
    code >>= 3;
    Flush_Buffer(PMBtab0[code].len);
    return PMBtab0[code].val;
  }

  if (code==0)
  {
    MPDfault_flag = 1;
    return 0;
  }

  Flush_Buffer(PMBtab1[code].len);


  return PMBtab1[code].val;
}

static inline int Get_B_macroblock_type()
{
  int code;


  if ((code = Show_Bits(6))>=8)
  {
    code >>= 2;
    Flush_Buffer(BMBtab0[code].len);


    return BMBtab0[code].val;
  }

  if (code==0)
  {
    MPDfault_flag = 1;
    return 0;
  }

  Flush_Buffer(BMBtab1[code].len);


  return BMBtab1[code].val;
}

static inline int Get_D_macroblock_type()
{
  if (!Get_Bits1())
  {
    MPDfault_flag=1;
  }

  return 1;
}

static inline int Get_I_Spatial_macroblock_type()
{
  int code;


  code = Show_Bits(4);

  if (code==0)
  {
    MPDfault_flag = 1;
    return 0;
  }


  Flush_Buffer(spIMBtab[code].len);
  return spIMBtab[code].val;
}

static inline int Get_P_Spatial_macroblock_type()
{
  int code;


  code = Show_Bits(7);

  if (code<2)
  {
    MPDfault_flag = 1;
    return 0;
  }

  if (code>=16)
  {
    code >>= 3;
    Flush_Buffer(spPMBtab0[code].len);


    return spPMBtab0[code].val;
  }

  Flush_Buffer(spPMBtab1[code].len);


  return spPMBtab1[code].val;
}

static inline int Get_B_Spatial_macroblock_type()
{
  int code;
  VLCtab *p;


  code = Show_Bits(9);

  if (code>=64)
    p = &spBMBtab0[(code>>5)-2];
  else if (code>=16)
    p = &spBMBtab1[(code>>2)-4];
  else if (code>=8)
    p = &spBMBtab2[code-8];
  else
  {
    MPDfault_flag = 1;
    return 0;
  }

  Flush_Buffer(p->len);


  return p->val;
}

static inline int Get_SNR_macroblock_type()
{
  int code;

#ifdef TRACE			/* *CH* */
  if (Trace_Flag)
#endif //TRACE

  code = Show_Bits(3);

  if (code==0)
  {
    MPDfault_flag = 1;
    return 0;
  }

  Flush_Buffer(SNRMBtab[code].len);

#ifdef TRACE			/* *CH* */
  if (Trace_Flag)
  {
    Print_Bits(code,3,SNRMBtab[code].len);
  }
#endif //TRACE


  return SNRMBtab[code].val;
}

int Get_macroblock_type()
{
  int macroblock_type = 0;

  if (MPDld->scalable_mode==SC_SNR)
    macroblock_type = Get_SNR_macroblock_type();
  else
  {
    switch (MPDpicture_coding_type)
    {
    case I_TYPE:
      macroblock_type = MPDld->pict_scal ? Get_I_Spatial_macroblock_type() : Get_I_macroblock_type();
      break;
    case P_TYPE:
      macroblock_type = MPDld->pict_scal ? Get_P_Spatial_macroblock_type() : Get_P_macroblock_type();
      break;
    case B_TYPE:
      macroblock_type = MPDld->pict_scal ? Get_B_Spatial_macroblock_type() : Get_B_macroblock_type();
      break;
    case D_TYPE:
      macroblock_type = Get_D_macroblock_type();
      break;
    default:
      break;
    }
  }

  return macroblock_type;
}
