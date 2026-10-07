// SND::CEAXABLKDecf's decoder state accessors (a fragment of eaxadecf.cpp,
// 0x80168ea0): GetState returns the two previous samples by value and
// SetState restores them. CEAXABLKDecf and XAFSTATE are named by the
// mangled symbols; the members are inferred (see eaxadecf.cpp).
namespace SND {
struct XAFSTATE {
    float previous0;
    float previous1;
};

class CEAXABLKDecf {
public:
    XAFSTATE GetState();
    void SetState(XAFSTATE*);

    char field00[152];
    XAFSTATE m_state;
};

XAFSTATE CEAXABLKDecf::GetState() {
    XAFSTATE state;

    state.previous0 = m_state.previous0;
    state.previous1 = m_state.previous1;
    return state;
}

void CEAXABLKDecf::SetState(XAFSTATE* state) {
    m_state = *state;
}
}
