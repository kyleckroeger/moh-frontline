// The pause screen, the functions at the start of the file: state changes
// (pausing and resuming music and ambient sound, the master volume) and the
// objective list (reading and setting an objective's completion status, adding
// an objective from the objective-text table). Pause, PauseState, IStudio and
// CShellMenu are named by the mangled symbols; the members, the objective list
// and the objective-text table views are inferred, and Pause is a non-virtual
// view. The setters index the list through an int local (index - 1), which
// keeps the target's unfolded (index - 1) * 8 addressing.
enum PauseState {};

class IStudio {
public:
    void iSirenActivateScreen(unsigned short, unsigned short);
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

class Pause {
public:
    void SetState(PauseState);

    unsigned char unknown00[108];
    bool m_paused;
    unsigned char unknown6d[3];
    IStudio m_studio;
    unsigned char unknown74[20];
    PauseState m_state;
    float m_volume;
    float m_volumeFade;
};

void Pause::SetState(PauseState state) {
    if (state != m_state) {
        switch (state) {
        case 3:
            m_studio.iSirenActivateScreen(0, 0);
            MUSIC_Pause(true);
            AEMS_Pause(true);
            break;
        case 4:
            SoundSetMasterVolume(0);
            m_volumeFade = 25.4f;
            MUSIC_Pause(false);
            AEMS_Pause(false);
            break;
        case 6:
            if (m_volume < 127.0f)
                SoundSetMasterVolume(127);
            break;
        }
    }
    m_state = state;
}

bool GetObjectiveStatus(unsigned int index) {
    return theObjectiveList.objectives[index - 1].completed;
}

void SetObjectiveStatus(unsigned int index, bool completed) {
    int i = index - 1;
    theObjectiveList.objectives[i].completed = completed;
    if (completed)
        g_Shell.CompletedAnotherObjective();
}

void AddObjective(unsigned int index, unsigned int id) {
    int type = (int)id < umTable.count ? umTable.entries[id].type : 0;
    unsigned int text = (int)id < umTable.count ? umTable.entries[id].text : 0;
    int i = index - 1;

    theObjectiveList.objectives[i].text = text;
    theObjectiveList.objectives[i].completed = false;
    theObjectiveList.objectives[i].type = type;
    theObjectiveList.count++;
}
