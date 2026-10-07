// A fragment of ShellMenu.cpp (0x800e558c): CShellMenu's stats messages. The
// weapon messages pick the weapon with the most kills (the first of equal
// counts; strings from 407 in the shell's string table), the evaluation
// messages one of five strings (from 372) for the largest evaluation count,
// chosen at random for the level; the totals' evaluation message is stored
// earlier and is empty when every count is zero, and both are empty without
// the record's first count. Two stats records of the same layout, for the
// totals at +44 and the level at +288, keep the chosen message indices. The
// file name is this project's; the original record is ShellMenu.cpp.
// CShellMenu and the functions are named by the mangled symbols; the members,
// the records, the string table and its inline lookup are inferred views
// (members at their offsets, names not original).
extern char* emptyString;

struct ShellStatsView {
    unsigned int m_value00;
    unsigned char unknown04[16];
    unsigned int m_eval[7];
    unsigned char unknown30[12];
    unsigned int m_weaponKills[45];
    unsigned char m_weaponMessage;
    unsigned char m_evalMessage;
    unsigned char unknownf2[2];
};

struct ShellStringView {
    char* text;
    unsigned char unknown4[4];
};

extern "C" int rand();

class CShellMenu {
public:
    char* GetWeaponMessageForTotalStats();
    char* GetEvalMessageForTotalStats();
    char* GetWeaponMessage();
    char* GetEvalMessage();

    char* GetString(int index) {
        if (index < m_stringCount)
            return m_strings[index].text;
        return 0;
    }

    unsigned char unknown0000[44];
    ShellStatsView m_totalStats;
    ShellStatsView m_levelStats;
    unsigned char unknown0214[6064];
    ShellStringView* m_strings;
    unsigned char unknown19c8[8];
    int m_stringCount;
};

char* CShellMenu::GetWeaponMessageForTotalStats() {
    int i = 0;
    unsigned int most = 0;
    unsigned int weapon = -1;
    for (; i < 45; i++) {
        if (m_totalStats.m_weaponKills[i] > most) {
            most = m_totalStats.m_weaponKills[i];
            weapon = i;
        }
    }
    if (!m_totalStats.m_value00 || weapon == -1)
        return emptyString;
    m_totalStats.m_weaponMessage = weapon;
    return GetString(m_totalStats.m_weaponMessage + 407);
}

char* CShellMenu::GetEvalMessageForTotalStats() {
    bool empty;
    if (m_totalStats.m_eval[0])
        empty = false;
    else if (m_totalStats.m_eval[3])
        empty = false;
    else if (m_totalStats.m_eval[6])
        empty = false;
    else if (m_totalStats.m_eval[4])
        empty = false;
    else if (m_totalStats.m_eval[1])
        empty = false;
    else if (m_totalStats.m_eval[5])
        empty = false;
    else if (m_totalStats.m_eval[2])
        empty = false;
    else
        empty = true;
    if (empty)
        return emptyString;
    return GetString(m_totalStats.m_evalMessage + 372);
}

char* CShellMenu::GetWeaponMessage() {
    int i = 0;
    unsigned int most = 0;
    unsigned int weapon = -1;
    for (; i < 45; i++) {
        if (m_levelStats.m_weaponKills[i] > most) {
            most = m_levelStats.m_weaponKills[i];
            weapon = i;
        }
    }
    if (!m_levelStats.m_value00 || weapon == -1)
        return emptyString;
    m_levelStats.m_weaponMessage = weapon;
    return GetString(m_levelStats.m_weaponMessage + 407);
}

char* CShellMenu::GetEvalMessage() {
    unsigned int most = 0;
    int eval = 0;
    for (int i = 0; i < 7; i++) {
        if (m_levelStats.m_eval[i] > most) {
            most = m_levelStats.m_eval[i];
            eval = i;
        }
    }
    m_levelStats.m_evalMessage = eval * 5 + rand() % 5;
    if (!m_levelStats.m_value00)
        return emptyString;
    return GetString(m_levelStats.m_evalMessage + 372);
}
