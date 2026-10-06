// Sound streams: stream lookup and teardown. The stream records and
// sndgs/sndss are inferred views.
extern "C" char sndgs[];
extern "C" void* sndss[];

extern "C" {
int SNDSTRM_destroy(int);
void STREAM_release(void*);
}

// SNDSTRMI_calcdatarate comes first in the original file; it is drafted in
// scratch but differs in register numbering, so this unit starts after it.

int SNDSTRMI_destroyall(void) {
    for (int i = 0; i < *(unsigned char*)(sndgs + 71); i++)
        SNDSTRM_destroy(i);
    return 0;
}

void* SNDSTRMI_getstreamptr(int handle) {
    if (handle >= *(unsigned char*)(sndgs + 71) || handle < 0)
        return 0;
    return sndss[handle];
}

void SNDSTRMI_releasecallback(void* data, void*) {
    STREAM_release(*(void**)sndss[(unsigned char)**(int**)((char*)data - 4)]);
}

// SNDSTRMI_framescallback and the stream functions after it are not
// reconstructed.
