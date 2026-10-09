// A fragment of AIFilter.cpp (0x8005bda4): the CAIFilterSoldierObject
// constructor (CAIFilterObject's table, then its own, and a CAISoldierObject
// for this filter constructed in place in the filter's storage, rounded up to
// 16 bytes, and kept at +8), then CAIFilterObject's GetTargetScript (through
// the AI object's +196 record, if any), LinkGameObject (the game object kept
// at +4) and its empty or null target functions. The file name is this
// project's; the original record is AIFilter.cpp. The classes and functions
// are named by the mangled symbols; the members, the storage view, the
// script record view and the result types not given by the symbols are
// inferred. Both classes declare their destructors (defined elsewhere) first,
// as in their tables, so the global virtual tables are not emitted here.
enum aifilter_target_mode {};
struct aistatus_match;
class CAIFilterRealPosition;
class CAIFilterRealVector3;

/* Inferred: a record whose +8 member holds the script at +12. */
struct AIScriptOwnerView {
    unsigned char unknown00[8];
    struct {
        unsigned char unknown00[12];
        void* script;
    }* data;
};

class CAIObject {
public:
    virtual ~CAIObject();

    unsigned char unknown0004[192];
    AIScriptOwnerView* m_scriptOwner;
};

class CAISoldierObject : public CAIObject {
public:
    CAISoldierObject(void*);
    static void* operator new(unsigned long, void*);

    unsigned char unknown00c8[1792]; /* 1992 bytes in all, from the allocation */
};

class CAIFilterObject {
public:
    virtual ~CAIFilterObject();
    virtual void* ChooseTarget(float); /* result type inferred */
    virtual void SetTargetMode(aifilter_target_mode);
    virtual void ResetTargetMatchList(aifilter_target_mode);
    virtual void SetupTargetMatchList(aifilter_target_mode, aistatus_match*, int);
    virtual void SetMoveTarget(const CAIFilterRealPosition&);
    virtual void GetLookDirection(CAIFilterRealVector3&, CAIFilterRealVector3&);
    void* GetTargetScript() const; /* result type inferred */
    void LinkGameObject(void*);

    void* m_gameObject;
    CAIObject* m_aiObject;
};

class CAIFilterSoldierObject : public CAIFilterObject {
public:
    virtual ~CAIFilterSoldierObject();
    CAIFilterSoldierObject();

    unsigned char unknown0c[8];
    unsigned char m_storage[2008];
};

CAIFilterSoldierObject::CAIFilterSoldierObject() {
    char* place = (char*)m_storage;
    if ((unsigned long)place & 15)
        place = (char*)((unsigned long)place & ~15) + 16;
    m_aiObject = new (place) CAISoldierObject(this);
}

void* CAIFilterObject::GetTargetScript() const {
    AIScriptOwnerView* owner = m_aiObject->m_scriptOwner;
    if (owner)
        return owner->data->script;
    return 0;
}

void CAIFilterObject::LinkGameObject(void* object) {
    m_gameObject = object;
}

void CAIFilterObject::SetupTargetMatchList(aifilter_target_mode, aistatus_match*, int) {
}

void CAIFilterObject::ResetTargetMatchList(aifilter_target_mode) {
}

void CAIFilterObject::SetTargetMode(aifilter_target_mode) {
}

void* CAIFilterObject::ChooseTarget(float) {
    return 0;
}
