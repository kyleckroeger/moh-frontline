// PATHX_overheadtrack (pathxSND.c, 0x800e99f0): the memory a path track
// needs - 104 bytes plus 12 per node, plus the stream's overhead when it
// streams - rounded up past the next 16-byte boundary.
extern "C" int SNDSTRM_overhead(int, int);

extern "C" int PATHX_overheadtrack(int nodes, int streams) {
    int size = nodes * 12 + 104;

    if (streams > 0)
        size += SNDSTRM_overhead(nodes, streams);
    size += 16 - size % 16;
    return size;
}
