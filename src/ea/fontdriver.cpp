// FONT_installdriver: select the font driver. The driver's type is unknown.
extern "C" {
extern void* FONTcurrentdriver;

void FONT_installdriver(void* driver) {
    FONTcurrentdriver = driver;
}
}
