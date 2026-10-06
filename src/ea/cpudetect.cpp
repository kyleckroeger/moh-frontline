extern "C" char* strcpy(char*, const char*);

// Only the size (24 bytes) and these members are established.
struct CPUINFO {
    int field00;
    unsigned int clock;
    char name[16];
};

static CPUINFO cpuinfo;

extern "C" unsigned int CPU_detect() {
    cpuinfo.clock = *reinterpret_cast<unsigned int*>(0x800000FC);
    cpuinfo.field00 = 0;
    strcpy(cpuinfo.name, "Gekko PowerPC");
    return *reinterpret_cast<unsigned int*>(0x800000FC);
}
