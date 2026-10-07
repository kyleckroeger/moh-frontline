// A fragment of ShellMenu.cpp (0x800de420): the SKU mode (always 1) and
// setting up the multiplayer buttons of each
// open window's player in the controller configuration. CShellMenu,
// CDualConfig and the functions are named by the mangled symbols; the
// members, the 28-byte player records and the configuration fields are
// inferred. MoveStatsIntoProperAreaForIstudio before this (the start of the
// record) is drafted in scratch/game/movestats_wip.cpp; the rest of the file
// is not part of this unit.
class CDualConfig {
public:
    void SetupButtonsForMultiplayerGame();

    unsigned char unknown00[56];
    int m_player;
    unsigned char unknown3c[4];
    int m_controller;
};

extern CDualConfig g_ControllerConfig;

struct MPPlayerButtonsView {
    unsigned char unknown00[24];
    unsigned char controller;
    unsigned char unknown19[3];
};

class CShellMenu {
public:
    int GetSKUMode();
    void SetupAllPlayersButtonsForMultiplayerGame();

    unsigned char unknown0000[5196];
    MPPlayerButtonsView m_players[4];
    unsigned char unknown14bc[1040];
    int m_activeWindows[4];
};

int CShellMenu::GetSKUMode() {
    return 1;
}

void CShellMenu::SetupAllPlayersButtonsForMultiplayerGame() {
    int i;

    for (i = 0; i < 4; i++) {
        if (m_activeWindows[i] != 0) {
            g_ControllerConfig.m_controller = m_players[i].controller;
            g_ControllerConfig.m_player = i;
            g_ControllerConfig.SetupButtonsForMultiplayerGame();
        }
    }
}
