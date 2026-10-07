// Behaviour-script schedules: circular lists of registration records (from a
// pool of 128) for objects run by a schedule's callback. BSSchedule,
// BSObject and BSScheduleRegistrationRecord_struct are named by the mangled
// symbols; the members and the inline record search are inferred.
struct BSObject {
    unsigned char field00[16];
    int field10;
};

struct BSScheduleRegistrationRecord_struct {
    BSObject* object;
    int field04;
    int field08;
    int field0C;
    int field10;
    BSScheduleRegistrationRecord_struct* next;
    BSScheduleRegistrationRecord_struct* prev;
    bool field1C;
    unsigned short field1E;
};

typedef bool (*BSScheduleCallback)(BSObject*, int, int, int, int*, int*);

void* BSUtilGetMemory(int, int);
enum ETimerReplaceMethod {};
void BSRegisterTimerEvent(int, unsigned short, BSObject*, void*, ETimerReplaceMethod);
void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

static BSScheduleRegistrationRecord_struct* g_AllRecords;
static BSScheduleRegistrationRecord_struct* g_FreeRecordList;

class BSSchedule {
public:
    BSSchedule();
    ~BSSchedule();
    void Init(BSScheduleCallback, int, int);
    void Reset(BSScheduleCallback, int, int);
    void Shutdown();
    BSScheduleRegistrationRecord_struct* Register(BSObject*, unsigned short, bool, int, int, int);
    void Unregister(BSScheduleRegistrationRecord_struct*);
    void Unregister(BSObject*);
    void Update();

private:
    BSScheduleRegistrationRecord_struct* Find(BSObject* object) {
        BSScheduleRegistrationRecord_struct* record = m_head;

        for (int i = 0; i < m_count; i++) {
            if (record->object == object)
                return record;
            record = record->next;
        }
        return 0;
    }

    bool m_active;
    int m_count;
    BSScheduleRegistrationRecord_struct* m_head;
    BSScheduleCallback m_callback;
    int m_field10;
    int m_field14;
    int m_field18;
};

// The file is compiled with deferred inlining, which emits functions in
// reverse source order (Unregister(BSObject*) inlines the record version
// defined above it), so the source lists them from the end of the image.
// BSSchedule::Update, the first function in the image, comes last: every
// m_field14 calls it runs up to m_field18 live records through the callback
// (each returning event data and a delay: a delayed timer event or an
// immediate trigger, then dropping one-shot records) and frees records whose
// object stamp has changed, resuming from where it stopped next time.

int BSScheduleGetMemoryRequirements() {
    return 4096;
}

void BSInitSchedule() {
    BSScheduleRegistrationRecord_struct* record;
    int i;

    g_AllRecords = (BSScheduleRegistrationRecord_struct*)BSUtilGetMemory(4096, 0);
    record = g_AllRecords;
    for (i = 0; i < 127; i++, record++) {
        record->next = record + 1;
        record->prev = 0;
    }
    record->next = 0;
    g_FreeRecordList = g_AllRecords;
}

void BSEndSchedule() {
    g_AllRecords = 0;
    g_FreeRecordList = 0;
}

BSSchedule::BSSchedule() {
    m_active = false;
}

BSSchedule::~BSSchedule() {
}

void BSSchedule::Init(BSScheduleCallback callback, int field18, int field14) {
    m_callback = callback;
    m_field18 = field18;
    m_field14 = field14;
    m_head = 0;
    m_active = true;
    m_count = 0;
    m_field10 = 0;
}

void BSSchedule::Reset(BSScheduleCallback callback, int field18, int field14) {
    m_callback = callback;
    m_field18 = field18;
    m_field14 = field14;
    m_head = 0;
    m_active = true;
    m_count = 0;
    m_field10 = 0;
}

void BSSchedule::Shutdown() {
    BSScheduleRegistrationRecord_struct* head;

    m_active = false;
    head = m_head;
    if (head) {
        head->prev->next = g_FreeRecordList;
        g_FreeRecordList = head;
    }
    m_head = 0;
}

BSScheduleRegistrationRecord_struct* BSSchedule::Register(BSObject* object, unsigned short field1E, bool field1C,
                                                           int field08, int field0C, int field10) {
    bool found = true;
    BSScheduleRegistrationRecord_struct* record = Find(object);

    if (!record) {
        record = g_FreeRecordList;
        found = false;
        g_FreeRecordList = record->next;
    }
    record->object = object;
    record->field1E = field1E;
    record->field08 = field08;
    record->field0C = field0C;
    record->field10 = field10;
    record->field04 = object->field10;
    record->field1C = field1C;
    if (found)
        return record;
    record->next = m_head;
    if (m_head) {
        record->prev = m_head->prev;
        m_head->prev->next = record;
        m_head->prev = record;
    } else {
        m_head = record;
        record->next = record;
        record->prev = record;
    }
    m_count++;
    return record;
}

void BSSchedule::Unregister(BSScheduleRegistrationRecord_struct* record) {
    if (m_count > 1) {
        record->prev->next = record->next;
        record->next->prev = record->prev;
        if (m_head == record)
            m_head = record->next;
    } else {
        m_head = 0;
    }
    record->next = g_FreeRecordList;
    record->prev = 0;
    g_FreeRecordList = record;
    m_count--;
}

void BSSchedule::Unregister(BSObject* object) {
    BSScheduleRegistrationRecord_struct* record = Find(object);

    if (record)
        Unregister(record);
}

void BSSchedule::Update() {
    m_field10++;
    if (m_field10 != m_field14)
        return;
    m_field10 = 0;
    if (!m_head)
        return;
    BSScheduleRegistrationRecord_struct* record = m_head;
    int processed = 0;
    while (processed < m_count) {
        if (record->field04 == record->object->field10) {
            if (++processed <= m_field18) {
                BSScheduleRegistrationRecord_struct* current = record;
                record = record->next;
                int data;
                int delay = 0;
                if (m_callback(current->object, current->field08, current->field0C, current->field10, &data, &delay)) {
                    if (delay)
                        BSRegisterTimerEvent(delay, current->field1E, current->object, (void*)data, (ETimerReplaceMethod)1);
                    else
                        BSObjectTriggerEvent(current->object, current->field1E, (void*)data, 0, false);
                    if (current->field1C)
                        Unregister(current);
                }
            } else {
                m_head = record;
                return;
            }
        } else {
            BSScheduleRegistrationRecord_struct* current = record;
            record = record->next;
            Unregister(current);
        }
        if (m_count == 0)
            return;
    }
    m_head = record;
}
