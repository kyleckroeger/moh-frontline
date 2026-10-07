// A fragment of ShellMenu.cpp (0x800e14f8): CShellMenu's language subtitle
// index and suffix getters, and copying a player's 13-byte controller button
// map (at +544, one per player) out of or into the shell. CShellMenu and the
// functions are named by the mangled symbols; the members and the result
// types are inferred. The rest of the file is not part of this unit.
class CShellMenu {
public:
    int Get_LanguageSubtitleIndex();
    int Get_LanguageSuffix();
    void RetrieveShellControllerButtons(char*, int);
    void CopyControllerButtons(char*, int);

    unsigned char unknown0000[544];
    char m_buttons[4][13];
    unsigned char unknown0254[5776];
    int m_languageSuffix;
    int m_languageSubtitleIndex;
};

int CShellMenu::Get_LanguageSubtitleIndex() {
    return m_languageSubtitleIndex;
}

int CShellMenu::Get_LanguageSuffix() {
    return m_languageSuffix;
}

void CShellMenu::RetrieveShellControllerButtons(char* buttons, int player) {
    int i;

    for (i = 0; i < 13; i++)
        buttons[i] = m_buttons[player][i];
}

void CShellMenu::CopyControllerButtons(char* buttons, int player) {
    int i;

    for (i = 0; i < 13; i++)
        m_buttons[player][i] = buttons[i];
}
