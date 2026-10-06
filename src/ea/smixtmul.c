// MIX_settimemult: set a mixer channel's time-stretch ratio when it has a
// time-stretch stage. sndmix's layout is not known; the channel table is read
// through inferred offsets (80-byte entries).
struct TIMESTRETCHSTATE;

extern "C" char sndmix[];
void SFILTER_timestretchsetratio(TIMESTRETCHSTATE*, int);

extern "C" void MIX_settimemult(int channel, int ratio) {
    TIMESTRETCHSTATE* state = *(TIMESTRETCHSTATE**)(*(char**)(sndmix + 500) + channel * 80 + 60);

    if (state)
        SFILTER_timestretchsetratio(state, ratio);
}
