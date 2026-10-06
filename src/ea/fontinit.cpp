// FONT_init/FONT_restore: create the standard font once and destroy it again
// at exit.
extern "C" {
extern void* FONTstand;
extern char FONTstandfile[];
void* FONT_create(void*);
void FONT_destroy(void*);
void REAL_addexit(void (*)(void));
void REAL_removeexit(void (*)(void));

void FONT_restore(void) {
    if (FONTstand) {
        FONT_destroy(FONTstand);
        FONTstand = 0;
        REAL_removeexit(FONT_restore);
    }
}

void FONT_init(void) {
    if (!FONTstand) {
        FONTstand = FONT_create(FONTstandfile);
        REAL_addexit(FONT_restore);
    }
}
}
