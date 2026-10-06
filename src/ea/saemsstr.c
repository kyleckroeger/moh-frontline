/* SNDAEMSI_linkstream, the last function of the AEMS streaming file: it
   installs the stream player's update function in sndaems. sndaems and the
   functions are named by their symbols; the view of sndaems (the update hook
   at +36, 68 bytes in all) is inferred. */
struct AEMSCOMPDYNAMICPLAYER;

void SNDAEMSI_updateplayerstream(AEMSCOMPDYNAMICPLAYER*);

struct SNDAEMSVIEW {
    unsigned char unknown00[36];
    void (*updateplayer)(AEMSCOMPDYNAMICPLAYER*);
    unsigned char unknown28[28];
};

extern SNDAEMSVIEW sndaems;

void SNDAEMSI_linkstream() {
    sndaems.updateplayer = SNDAEMSI_updateplayerstream;
}
