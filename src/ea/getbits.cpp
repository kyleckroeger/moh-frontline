/* Adapted from getbits.c of the MPEG Software Simulation Group's mpeg2decode
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

// getbits.cpp: the reference decoder's bit reader, reworked in the game to read
// from a buffer in memory: Initialize_Buffer takes the buffer, the refills,
// system-stream handling and file reading are absent (as are Fill_Buffer,
// Get_Byte and Get_Word), and Flush_Buffer and Flush_Buffer32 read further
// bytes while at most 24 bits are buffered. Flush_Buffer32 and Get_Bits32 are
// not in the reference copy used and are written to match the game's code. The
// layer-data view (read pointer, bit buffer and bit count at +0, +4, +8) is
// inferred. The unit is compiled with deferred inlining (-inline
// auto,deferred), which emits the functions in reverse order, as in the game.
struct LayerDataView {
    unsigned char* Rdptr;
    unsigned int Bfr;
    int Incnt;
};

extern LayerDataView* MPDld;

unsigned int Get_Bits(int N);
void Flush_Buffer(int N);

void Initialize_Buffer(unsigned char* buffer)
{
  MPDld->Incnt = 0;
  MPDld->Rdptr = buffer;
  MPDld->Bfr = 0;
  Flush_Buffer(0);
}

unsigned int Show_Bits(int N)
{
  return MPDld->Bfr >> (32-N);
}

unsigned int Get_Bits1()
{
  return Get_Bits(1);
}

void Flush_Buffer(int N)
{
  int Incnt;

  MPDld->Bfr <<= N;

  Incnt = MPDld->Incnt - N;

  while (Incnt <= 24)
  {
    MPDld->Bfr |= *MPDld->Rdptr++ << (24 - Incnt);
    Incnt += 8;
  }
  MPDld->Incnt = Incnt;
}

unsigned int Get_Bits(int N)
{
  unsigned int Val;

  Val = Show_Bits(N);
  Flush_Buffer(N);

  return Val;
}

void Flush_Buffer32()
{
  int Incnt;

  MPDld->Bfr = 0;

  Incnt = MPDld->Incnt - 32;

  while (Incnt <= 24)
  {
    MPDld->Bfr |= *MPDld->Rdptr++ << (24 - Incnt);
    Incnt += 8;
  }
  MPDld->Incnt = Incnt;
}

unsigned int Get_Bits32()
{
  unsigned int l;

  l = Show_Bits(32);
  Flush_Buffer32();

  return l;
}
