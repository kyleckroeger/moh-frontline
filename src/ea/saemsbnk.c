/* SNDAEMS_linkbank, the last function of the AEMS bank player file: it
   installs the bank player's update function in sndaems. sndaems and the
   functions are named by their symbols; the view of sndaems (the bank
   update hook at +32, 68 bytes in all) is inferred. */
struct AEMSCOMPDYNAMICPLAYER;

void SNDAEMSI_updateplayerbank(AEMSCOMPDYNAMICPLAYER*);

struct SNDAEMSVIEW {
    unsigned char unknown00[32];
    void (*updateplayerbank)(AEMSCOMPDYNAMICPLAYER*);
    unsigned char unknown24[32];
};

extern SNDAEMSVIEW sndaems;

void SNDAEMS_linkbank() {
    sndaems.updateplayerbank = SNDAEMSI_updateplayerbank;
}
