// A fragment of weaponfactory.cpp (0x800d67fc): CWeaponFactory::
// LoadWeaponParameters, which loads the multiplayer or difficulty's parameter
// file, allocates the weapon property table and fills it from the file's
// weapon records (skipping the bullet records), and the weak
// EndianSwap(WeaponProperties_struct&) it calls. The names come from the
// mangled symbols; the resource, record and factory views are inferred, and
// the byte-order helpers are the inlined ones described in propdat.cpp
// (inferred; the converted fields are named by offset).
extern "C" void* memcpy(void*, const void*, unsigned long);
void DebugMsg(const char*, ...);
void TLT_CloseFile(void*);

enum TLTResourceID {};
struct LevelFileContentsStruct_;

// Inferred view of the level resource entry and its file names.
struct BulletFileNamesView {
    unsigned char unknown000[36];
    const char* m_easy;
    unsigned char unknown028[4];
    const char* m_normal;
    unsigned char unknown030[4];
    const char* m_hard;
    unsigned char unknown038[468];
    const char* m_multiplayer;
};

struct LevelResourceView {
    unsigned char unknown00[8];
    BulletFileNamesView* m_names;
};

LevelResourceView* TLT_FindNextResourceByType(TLTResourceID, LevelFileContentsStruct_*);
void* TLT_LoadFileFromLevelBigFile(const char*, int*);

class CShellMenu {
public:
    unsigned char Get_currentDifficulty();

    unsigned char unknown0000[0x19dc];
};

extern CShellMenu g_Shell;
extern bool g_bInMultiplayerMode;

inline void ChangeEndian(short& value) {
    unsigned char bytes[2];
    *reinterpret_cast<short*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[1];
    bytes[1] = c;
    value = *reinterpret_cast<short*>(bytes);
}

inline void ChangeEndian(int& value) {
    unsigned char bytes[4];
    *reinterpret_cast<int*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[3];
    bytes[3] = c;
    c = bytes[1];
    bytes[1] = bytes[2];
    bytes[2] = c;
    value = *reinterpret_cast<int*>(bytes);
}

inline void ChangeEndian(unsigned short& value) {
    ChangeEndian(*reinterpret_cast<short*>(&value));
}

template <class T> inline void ChangeEndian(T& value) {
    ChangeEndian(*reinterpret_cast<int*>(&value));
}


enum ERenderPriority {};

struct WeaponProperties_struct {
    unsigned char data[80];
};

void EndianSwap(WeaponProperties_struct&);

class CWeaponFactory {
public:
    void LoadWeaponParameters();

    WeaponProperties_struct* m_properties;
    unsigned char unknown04[20];
    int m_count;
};

void CWeaponFactory::LoadWeaponParameters() {
    short offset;
    char* file;
    LevelResourceView* resource = TLT_FindNextResourceByType((TLTResourceID)11, 0);
    if (g_bInMultiplayerMode) {
        file = (char*)TLT_LoadFileFromLevelBigFile(resource->m_names->m_multiplayer, 0);
    } else {
        switch (g_Shell.Get_currentDifficulty()) {
        case 1:
            file = (char*)TLT_LoadFileFromLevelBigFile(resource->m_names->m_easy, 0);
            break;
        case 2:
            file = (char*)TLT_LoadFileFromLevelBigFile(resource->m_names->m_normal, 0);
            break;
        case 3:
            file = (char*)TLT_LoadFileFromLevelBigFile(resource->m_names->m_hard, 0);
            break;
        default:
            DebugMsg("Unknown difficulty level.  Assuming Level TE\n");
            file = (char*)TLT_LoadFileFromLevelBigFile(resource->m_names->m_easy, 0);
            break;
        }
    }
    memcpy(&m_count, file, 4);
    offset = 12;
    ChangeEndian(m_count);
    m_properties = (WeaponProperties_struct*)new char[m_count * 80];
    for (int read = 0; read < m_count;) {
        short type;
        memcpy(&type, file + offset, 2);
        offset += 2;
        ChangeEndian(type);
        switch (type) {
        case 1: {
            short index;
            memcpy(&index, file + offset, 2);
            ChangeEndian(index);
            memcpy(&m_properties[index], file + (short)(offset + 2), 80);
            EndianSwap(m_properties[index]);
            offset += 82;
            read++;
            break;
        }
        case 2:
            offset += 66;
            break;
        case 3:
            offset += 78;
            break;
        }
    }
    TLT_CloseFile(file);
}

__declspec(weak) void EndianSwap(WeaponProperties_struct& p) {
    ChangeEndian(*(int*)(p.data + 0));
    ChangeEndian(*(short*)(p.data + 4));
    ChangeEndian(*(short*)(p.data + 6));
    ChangeEndian(*(short*)(p.data + 12));
    ChangeEndian(*(short*)(p.data + 14));
    ChangeEndian(*(short*)(p.data + 16));
    ChangeEndian(*(short*)(p.data + 18));
    ChangeEndian(*(int*)(p.data + 40));
    ChangeEndian(*(int*)(p.data + 64));
    ChangeEndian(*(short*)(p.data + 72));
    ChangeEndian(*(short*)(p.data + 74));
    ChangeEndian(*(int*)(p.data + 8));
    ChangeEndian(*(int*)(p.data + 68));
    ChangeEndian(*(int*)(p.data + 76));
}
