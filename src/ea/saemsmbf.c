/* SNDAEMS_asyncloadmodulebankdone, the last function of the AEMS module-bank
   loader: true once no asynchronous module-bank load is pending. sndaems is
   named by its symbol; the view of it (the pending flag at +40, 68 bytes in
   all) is inferred. */
struct SNDAEMSVIEW {
    unsigned char unknown00[40];
    unsigned char loading;
    unsigned char unknown29[27];
};

extern SNDAEMSVIEW sndaems;

extern "C" unsigned char SNDAEMS_asyncloadmodulebankdone() {
    return sndaems.loading == 0;
}
