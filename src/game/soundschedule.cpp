// SoundSchedule, the last functions of the file: the destructor and
// constructor and the static construction of the 100 registration records
// (with the records' empty inline constructor, emitted here). Shutdown and
// Init before them are drafted in scratch/lib/soundschedule_wip.cpp (Init's
// free-list loop unrolls differently). SoundSchedule,
// SoundScheduleRegistrationRecord, BSObject and the statics are named by the
// symbols; the record (144 bytes) and schedule layouts are inferred.
class BSObject;

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

static SoundScheduleRegistrationRecord g_AllRecords[100];

class SoundSchedule {
public:
    SoundSchedule();
    ~SoundSchedule();

    int m_count;
    SoundScheduleRegistrationRecord* m_records;
};

SoundSchedule::~SoundSchedule() {
}

SoundSchedule::SoundSchedule() {
    m_records = 0;
    m_count = 0;
}
