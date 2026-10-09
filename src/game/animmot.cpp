// The end of animmot.cpp (0x80066050): the static initialisation of the
// motion frame list (its 24 weight frames built through their constructor by
// __construct_array) and the weak, empty AnimMotWeightFrame_t constructor
// emitted after it. The rest of the file is not reconstructed.
// AnimMotWeightFrame_t and _AnimMot_FrameList are named by the symbols; the
// list's size comes from the symbol, while the frame size (80), the 8-byte
// header and the list's type (its name is not known: AnimMotFrameListView is
// this project's) are inferred.
struct AnimMotWeightFrame_t {
    AnimMotWeightFrame_t() {}

    unsigned char unknown00[80];
};

struct AnimMotFrameListView {
    unsigned char unknown00[8];
    AnimMotWeightFrame_t frames[24];
};

static AnimMotFrameListView _AnimMot_FrameList;
