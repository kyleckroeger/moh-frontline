// A fragment of soundschedule.cpp: SoundSchedule::Shutdown (0x800476e8). For
// each scheduled sound it triggers the object's end event (when it has one),
// ends the AEMS event and returns the record to the free list; then it clears
// the schedule and the free list. The file name is this project's; the original
// record is soundschedule.cpp (src/game/soundschedule.cpp holds the destructor
// onward), and SoundSchedule::Init between them is not reconstructed.
// SoundSchedule, SoundScheduleRegistrationRecord, BSObject and the statics are
// named by the symbols; the record (144 bytes) and schedule layouts are inferred.
class BSObject;

void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);
void AEMS_EndEvent(void*, int);

class SoundScheduleRegistrationRecord {
public:
    SoundScheduleRegistrationRecord() {}

    unsigned char unknown00[24];
    BSObject* object;
    int endEvent;
    unsigned char aemsEvent[80];
    int aemsHandle;
    unsigned char unknown74[24];
    SoundScheduleRegistrationRecord* next;
};

// File-local in the original; declared extern so this fragment can refer to it.
extern SoundScheduleRegistrationRecord* g_FreeRecordList;

class SoundSchedule {
public:
    SoundSchedule();
    ~SoundSchedule();
    void Shutdown();
    void Init();

    int m_count;
    SoundScheduleRegistrationRecord* m_records;
};

void SoundSchedule::Shutdown() {
    SoundScheduleRegistrationRecord* record = m_records;
    while (record) {
        SoundScheduleRegistrationRecord* next = record->next;
        if (record->object && record->endEvent != -1)
            BSObjectTriggerEvent(record->object, record->endEvent, 0, 0, false);
        AEMS_EndEvent(record->aemsEvent, record->aemsHandle);
        record->next = g_FreeRecordList;
        g_FreeRecordList = record;
        record = next;
        m_count--;
    }
    m_records = 0;
    m_count = 0;
    g_FreeRecordList = 0;
}
