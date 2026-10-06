// The clipping table of the MPEG-2 reference decoder's fast IDCT. Only the
// initialisation is linked; the transform itself is not.
static short iclip[1024];
static short* iclp;

void Initialize_Fast_IDCT() {
    int i;

    iclp = iclip + 512;
    for (i = -512; i < 512; i++)
        iclp[i] = (i < -256) ? -256 : ((i > 255) ? 255 : i);
}
