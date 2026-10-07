// A fragment of ShellMenu.cpp (0x800e6950): CShellMenu's statistics counters,
// each capped at 9999999. The multiplayer records (from +5324, 256 bytes per
// player) count hits per shot location and shots fired (not once the level
// has ended) and remember who last shot a player; resetting clears four
// records and sets each last shooter to -1. The current level's counters count
// shots fired, hits, hits taken, enemies killed (and kills per weapon type of
// the first player's current weapon) and hits per shot location. The classes,
// enums and functions are named by the mangled symbols; CShellMenu, the
// records, the weapon view and the counter names are inferred (the scene view
// only needs a size outside small data), and the unused
// bool parameters are left unnamed. Add_MPPlayerKilled before this and the
// rest of the file are not part of this unit.
enum EShotLocations {};

extern "C" void* memset(void*, int, unsigned long);

class CWeapon {
public:
    unsigned char unknown000[664];
    int m_type;
};

class CPlayerObject {
public:
    CWeapon* GetCurrentWeapon() const;
};

class CScene {
public:
    CPlayerObject* GetPlayer(int) const;

    unsigned char data[16];
};

extern CScene g_scene;
extern int g_bMPEndLevel;

struct MPStatsView {
    int shotsFired;
    int locations[61];
    int lastShooter;
    unsigned char unknown0fc[4];
};

class CShellMenu {
public:
    void Add_MPShotLocation(int, int, int, EShotLocations);
    void Add_MPShotFired(int, int);
    void ResetMPPlayersStats();
    void Set_shotLocation(unsigned int, EShotLocations, bool);
    void Set_enemiesKilled(unsigned int, bool);
    void Set_hitsTaken(unsigned int, bool);
    void Set_hits(unsigned int, bool);
    void Set_shotsFired(unsigned int, bool);

    unsigned char unknown0000[288];
    unsigned int m_shotsFired;
    unsigned int m_hits;
    unsigned int m_hitsTaken;
    unsigned char unknown012c[4];
    unsigned int m_enemiesKilled;
    unsigned int m_shotLocations[10];
    unsigned int m_weaponKills[1]; /* indexed by weapon type; length unknown */
    unsigned char unknown0160[4972];
    MPStatsView m_mpStats[4];
};

void CShellMenu::Add_MPShotLocation(int shooter, int target, int count, EShotLocations location) {
    if (g_bMPEndLevel)
        return;
    m_mpStats[shooter].locations[location] += count;
    if (m_mpStats[shooter].locations[location] >= 9999999)
        m_mpStats[shooter].locations[location] = 9999999;
    m_mpStats[target].lastShooter = shooter;
}

void CShellMenu::Add_MPShotFired(int player, int count) {
    if (g_bMPEndLevel)
        return;
    m_mpStats[player].shotsFired += count;
    if (m_mpStats[player].shotsFired >= 9999999)
        m_mpStats[player].shotsFired = 9999999;
}

void CShellMenu::ResetMPPlayersStats() {
    memset(m_mpStats, 0, sizeof(m_mpStats));
    m_mpStats[0].lastShooter = -1;
    m_mpStats[1].lastShooter = -1;
    m_mpStats[2].lastShooter = -1;
    m_mpStats[3].lastShooter = -1;
}

void CShellMenu::Set_shotLocation(unsigned int count, EShotLocations location, bool) {
    if (m_shotLocations[location] < 9999999)
        m_shotLocations[location] += count;
}

void CShellMenu::Set_enemiesKilled(unsigned int count, bool) {
    CPlayerObject* player;

    if (m_enemiesKilled < 9999999)
        m_enemiesKilled += count;
    player = g_scene.GetPlayer(0);
    if (player->GetCurrentWeapon()) {
        int type = player->GetCurrentWeapon()->m_type;
        if (m_weaponKills[type] < 9999999)
            m_weaponKills[type] += count;
    }
}

void CShellMenu::Set_hitsTaken(unsigned int count, bool) {
    if (m_hitsTaken < 9999999)
        m_hitsTaken += count;
}

void CShellMenu::Set_hits(unsigned int count, bool) {
    if (m_hits < 9999999)
        m_hits += count;
}

void CShellMenu::Set_shotsFired(unsigned int count, bool) {
    if (m_shotsFired < 9999999)
        m_shotsFired += count;
}
