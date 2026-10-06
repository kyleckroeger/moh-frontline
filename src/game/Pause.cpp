// The pause screen, the functions at the start of the file: state changes
// (pausing and resuming music and ambient sound, the master volume) and an
// objective's completion status. Pause, PauseState and IStudio are named by
// the mangled symbols; the members and the objective list view are inferred,
// and Pause is a non-virtual view. SetObjectiveStatus and AddObjective follow
// and are off (the target indexes the list with index - 1 unfolded; draft in
// scratch/lib/Pause_wip.cpp), so the unit stops here.
enum PauseState {};

class IStudio {
public:
    void iSirenActivateScreen(unsigned short, unsigned short);
};

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

extern ObjectiveListView theObjectiveList;

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
