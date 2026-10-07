// A fragment of animplayer.cpp (0x801099c4): weak copies of header inlines
// emitted in this file: CAnimatedPlayer's empty UpdateAI and AsAnimatedPlayerObject
// (itself). CAnimatedPlayer is named by the mangled symbols and is a
// non-virtual view. The rest of the file is not part of this unit.
class CAnimatedPlayer {
public:
    void UpdateAI(float);
    const CAnimatedPlayer* AsAnimatedPlayerObject() const;
    CAnimatedPlayer* AsAnimatedPlayerObject();
};

__declspec(weak) void CAnimatedPlayer::UpdateAI(float) {
}

__declspec(weak) const CAnimatedPlayer* CAnimatedPlayer::AsAnimatedPlayerObject() const {
    return this;
}

__declspec(weak) CAnimatedPlayer* CAnimatedPlayer::AsAnimatedPlayerObject() {
    return this;
}
