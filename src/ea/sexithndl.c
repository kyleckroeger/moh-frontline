extern "C" void SNDSYS_restore();

void SNDREAL_exithandler() {
    SNDSYS_restore();
}
