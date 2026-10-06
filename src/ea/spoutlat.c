// SNDPROFILE_outputlatency: the platform's output latency once the sound
// system is running. sndgs's layout is not known; the flag is read through an
// inferred offset.
extern "C" char sndgs[];
int SNDPLATFORM_outputlatency(void);

extern "C" int SNDPROFILE_outputlatency(void) {
    if (*(signed char*)(sndgs + 360))
        return SNDPLATFORM_outputlatency();
    return 0;
}
