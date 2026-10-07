// A fragment of ShellMenu.cpp (0x800dfa18): copying the shell's status arrays
// out for the interface studio, each returning its entry count: the secrets
// as (found, value - 1) pairs (zero pairs for unset secrets), the medals, the
// levels' star ratings and the levels completed (any positive unlock status
// becomes 1). CShellMenu and the functions are named by the mangled symbols;
// the result type and the meanings of the values are inferred. The rest of the
// file is not part of this unit.
extern "C" void* memcpy(void*, const void*, unsigned long);

class CShellMenu {
public:
    int PassSecretsToIStudio(int*);
    int PassMedalStatusToIStudio(int*);
    int PassLevelsRatingsToIStudio(int*);
    int PassLevelsCompletedToIStudio(int*);
    void GetSecretsStatus(int*);
    void GetMedalStatus(int*);
    void GetAllLevelsStarStatus(int*);
    void GetLevelUnlockedStatus(int*);
};

int CShellMenu::PassSecretsToIStudio(int* out) {
    int status[10];
    int values[20];
    int i;

    GetSecretsStatus(status);
    for (i = 0; i < 10; i++) {
        if (status[i] > 0) {
            values[i * 2] = 1;
            values[i * 2 + 1] = status[i] - 1;
        } else {
            values[i * 2] = 0;
            values[i * 2 + 1] = 0;
        }
    }
    memcpy(out, values, sizeof(values));
    return 10;
}

int CShellMenu::PassMedalStatusToIStudio(int* out) {
    int status[9];

    GetMedalStatus(status);
    memcpy(out, status, sizeof(status));
    return 9;
}

int CShellMenu::PassLevelsRatingsToIStudio(int* out) {
    int ratings[24];

    GetAllLevelsStarStatus(ratings);
    memcpy(out, ratings, sizeof(ratings));
    return 24;
}

int CShellMenu::PassLevelsCompletedToIStudio(int* out) {
    int status[24];
    int i;

    GetLevelUnlockedStatus(status);
    for (i = 0; i < 24; i++) {
        if (status[i] > 0)
            status[i] = 1;
    }
    memcpy(out, status, sizeof(status));
    return 24;
}
