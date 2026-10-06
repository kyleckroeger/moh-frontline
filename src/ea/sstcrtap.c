// SNDSTRM_createtap: create a stream (tap mode).
struct SNDPLAYOPTS;

int SNDSTRMI_create(SNDPLAYOPTS*, int, int, void*, int, int, int);

extern "C" int SNDSTRM_createtap(int arg0, SNDPLAYOPTS* opts, int arg2, int arg3, void* buffer, int size) {
    return SNDSTRMI_create(opts, arg2, arg3, buffer, size, arg0, 1);
}
