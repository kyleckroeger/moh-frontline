// A fragment of ShellMenu.cpp (0x800e6524): the weak, empty iDebugMsg emitted
// in this file, then CShellMenu's level timer (accumulated unless the player's
// flag at +919 is set) and the finished-level records kept per mission and
// level (1-based, decremented in place; health, per-weapon ammo and a success
// flag; whether to use the previous level's record), and the multiplayer stats records and score. The
// file name is this project's; the original record is ShellMenu.cpp and
// Add_MPPlayerKilled after these is not reconstructed. The classes and
// functions are named by the mangled symbols; CShellMenu, the records and the
// player flag are inferred views (members at their offsets, names not
// original; 6 missions of 4 levels, 45 ammo slots and 256-byte multiplayer
// records are sizes implied by the strides), and the result types are
// inferred.
enum EWeaponTypes {};

class CPlayerObject {
public:
    unsigned char unknown000[919];
    unsigned char m_flag919 : 1;
    unsigned char unknown397 : 7;
};

struct FinishedLevelView {
    float health;
    unsigned int ammo[45];
    bool successful;
    unsigned char unknownb9[3];
};

struct MPPlayerStatsView {
    unsigned char unknown00[236];
    int score;
    unsigned char unknownf0[16];
};

void iDebugMsg(const char*, ...);

class CShellMenu {
public:
    void UpdateLevelTimer(CPlayerObject*, unsigned int);
    void ResetLevelTimer();
    bool ShouldIUsePlayersFinishedLevelData(int, int);
    unsigned int GetPlayersFinishedLevelAmmo(int, int, EWeaponTypes);
    float GetPlayersFinishedLevelHealth(int, int);
    void SetPlayersFinishedLevelAmmo(int, int, EWeaponTypes, unsigned int);
    void SetPlayersFinishedLevelHealth(int, int, float);
    void SetPlayersFinishedLevelSuccessfully(int, int);
    MPPlayerStatsView* GetMPPlayerStats(int);
    int Get_MPPlayerScore(int);

    unsigned char unknown0000[336];
    unsigned int m_levelTimer;
    unsigned char unknown0154[260];
    FinishedLevelView m_finished[6][4];
    unsigned char unknown13f8[212];
    MPPlayerStatsView m_mpStats[8];
};

__declspec(weak) void iDebugMsg(const char*, ...) {
}

void CShellMenu::UpdateLevelTimer(CPlayerObject* player, unsigned int elapsed) {
    if (!player->m_flag919)
        m_levelTimer += elapsed;
}

void CShellMenu::ResetLevelTimer() {
    m_levelTimer = 0;
}

bool CShellMenu::ShouldIUsePlayersFinishedLevelData(int mission, int level) {
    if (level > 1 && mission < 7) {
        mission--;
        level -= 2;
        return m_finished[mission][level].successful;
    }
    return false;
}

unsigned int CShellMenu::GetPlayersFinishedLevelAmmo(int mission, int level, EWeaponTypes weapon) {
    mission--;
    level--;
    return m_finished[mission][level].ammo[weapon];
}

float CShellMenu::GetPlayersFinishedLevelHealth(int mission, int level) {
    mission--;
    level--;
    return m_finished[mission][level].health;
}

void CShellMenu::SetPlayersFinishedLevelAmmo(int mission, int level, EWeaponTypes weapon, unsigned int ammo) {
    mission--;
    level--;
    m_finished[mission][level].ammo[weapon] = ammo;
}

void CShellMenu::SetPlayersFinishedLevelHealth(int mission, int level, float health) {
    mission--;
    level--;
    m_finished[mission][level].health = health;
}

void CShellMenu::SetPlayersFinishedLevelSuccessfully(int mission, int level) {
    mission--;
    level--;
    m_finished[mission][level].successful = true;
}

MPPlayerStatsView* CShellMenu::GetMPPlayerStats(int player) {
    return &m_mpStats[player];
}

int CShellMenu::Get_MPPlayerScore(int player) {
    return m_mpStats[player].score;
}
