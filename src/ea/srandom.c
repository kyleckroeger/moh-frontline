// The sound library's random generator state: six words seeded by adding a
// value to each original word (iSNDrandom steps it with add-with-carry).
unsigned int SNDIrandseedorig[6] = {0xf22d0e56, 0x883126e9, 0xc624dd2f, 0x0702c49c, 0x9e353f7d, 0x6fdf3b64};
unsigned int SNDIrandseed[6];

void SNDI_randomseed(unsigned int seed) {
    SNDIrandseed[0] = seed + SNDIrandseedorig[0];
    SNDIrandseed[1] = seed + SNDIrandseedorig[1];
    SNDIrandseed[2] = seed + SNDIrandseedorig[2];
    SNDIrandseed[3] = seed + SNDIrandseedorig[3];
    SNDIrandseed[4] = seed + SNDIrandseedorig[4];
    SNDIrandseed[5] = seed + SNDIrandseedorig[5];
}

// iSNDrandom follows in the original file; its carry chain is drafted in
// scratch but not matched, so this unit covers only the seeding.
