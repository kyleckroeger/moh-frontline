// A fragment of bsbifunc.cpp (0x80026da0): script built-ins that return whether
// the invincibility cheat is on (the first byte of g_cheats) and set the first
// player's super-aim mode (a flag bit of the player). Each reads its argument
// below the script stack top and pops the built-in's arguments. The file name
// is this project's; the original record is bsbifunc.cpp and the built-ins
// around these are not reconstructed. The functions, classes and globals are
// named by the mangled symbols; the cheats (20 bytes, the size of g_cheats),
// player, scene and built-in record views are inferred (members at their
// offsets, names not original).
struct CheatsView {
    bool invincible;
    unsigned char unknown01[19];
};

class CPlayerObject {
public:
    unsigned char unknown000[920];
    unsigned char unknown398 : 1;
    unsigned char m_superAim : 1;
    unsigned char unknown398b : 6;
};

class CScene {
public:
    CPlayerObject* GetPlayer(int) const;

    unsigned char unknown000[328];
};

extern CheatsView g_cheats;
extern CScene g_scene;

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_IsPlayerInvincible(int** stack, void*) {
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = g_cheats.invincible;
}

void BIFunc_PlayerSuperAimMode(int** stack, void*) {
    bool enable = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)) != 0;
    g_scene.GetPlayer(0)->m_superAim = enable;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
