// A fragment of ShellMenu.cpp (0x800e74f4): CShellMenu::SetMoviePlaytime sets
// the movie's end time (+6548) to 30 seconds past the current game time.
// CShellMenu and the functions are named by the mangled symbols; the member
// and its meaning are inferred. The rest of the file is not part of this unit.
float GetGameTime();

class CShellMenu {
public:
    void SetMoviePlaytime();

    unsigned char unknown0000[6548];
    float m_moviePlaytime;
};

void CShellMenu::SetMoviePlaytime() {
    m_moviePlaytime = 30.0f + GetGameTime();
}
