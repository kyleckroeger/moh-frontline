// A fragment of Pause.cpp (0x800dd758): Pause::PauseShutdown (releases the
// controller lock and shuts the studio's siren down), Pause::UnpauseGame
// (releases the lock, resumes music and ambient sound, clears the paused flag
// and the cheat-sequence index) and Pause::bIsGamePaused. The file name is
// this project's; the original record is Pause.cpp (Pause.cpp and
// Pause_tail.cpp hold other parts). Pause, IStudio and the globals are named
// by the mangled symbols; the members are inferred and Pause is a non-virtual
// view.
// The pause screen, the functions at the start of the file: state changes
// (pausing and resuming music and ambient sound, the master volume), the
// objective list and the pause/unpause entry points. Pause, PauseState,
// IStudio and CShellMenu are named by the mangled symbols; the members, the
// objective list and the objective-text table views are inferred. Pause is a
// non-virtual view.
enum PauseState {};

class IStudio {
public:
    void iSirenActivateScreen(unsigned short, unsigned short);
    void iSirenShutDown();
};

class CShellMenu {
public:
    void CompletedAnotherObjective();

    unsigned char data[16];
};

extern CShellMenu g_Shell;

void MUSIC_Pause(bool);
void AEMS_Pause(bool);
void SoundSetMasterVolume(int);

struct ObjectiveView {
    unsigned int text;
    unsigned char type;
    bool completed;
    unsigned char unknown06[2];
};

struct ObjectiveListView {
    int count;
    ObjectiveView objectives[100];
};

struct ObjectiveTextView {
    unsigned int text;
    unsigned char type;
    unsigned char unknown05[3];
};

struct ObjectiveTextTableView {
    ObjectiveTextView* entries;
    unsigned char unknown04[8];
    int count;
};

extern ObjectiveListView theObjectiveList;
extern ObjectiveTextTableView umTable;
extern int g_PauseScreenLockedController;
extern int g_cheatSequenceIndex;

class Pause {
public:
    void SetState(PauseState);
    void PauseShutdown();
    void UnpauseGame();
    bool bIsGamePaused();

    unsigned char unknown00[108];
    bool m_paused;
    unsigned char unknown6d[3];
    IStudio m_studio;
    unsigned char unknown74[20];
    PauseState m_state;
    float m_volume;
    float m_volumeFade;
};

void Pause::SetState(PauseState state);

bool GetObjectiveStatus(unsigned int index);

void SetObjectiveStatus(unsigned int index, bool completed);

void AddObjective(unsigned int index, unsigned int id);

void Pause::PauseShutdown() {
    g_PauseScreenLockedController = -1;
    m_studio.iSirenShutDown();
}

void Pause::UnpauseGame() {
    g_PauseScreenLockedController = -1;
    MUSIC_Pause(false);
    AEMS_Pause(false);
    m_paused = false;
    g_cheatSequenceIndex = 0;
}

bool Pause::bIsGamePaused() {
    return m_paused;
}


