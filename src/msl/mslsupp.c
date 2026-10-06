/* MetroTRK's MSL file hooks (mslsupp.c), reconstructed from the disassembly.
 * Each hook needs a connected debugger, makes one TRK file request and maps
 * the TRK result (0 ok, 2 end of file, anything else error) to MSL's I/O
 * result. The original object carries .line/.debug sections, and it is
 * built with inlining off: convertFileMode is called, not inlined. */
typedef unsigned char u8;
typedef unsigned long size_t;

typedef struct {
    unsigned int open_mode : 2;
    unsigned int io_mode : 3;
    unsigned int buffer_mode : 2;
    unsigned int file_kind : 3;
    unsigned int file_orientation : 2;
    unsigned int binary_io : 1;
} __file_modes;

int GetTRKConnected(void);
u8 TRKAccessFile(u8 command, unsigned long handle, size_t* length, unsigned char* buffer);
u8 TRKOpenFile(u8 command, unsigned long name, u8 mode, unsigned long* handle);
u8 TRKCloseFile(u8 command, unsigned long handle);
u8 TRKPositionFile(u8 command, unsigned long handle, long* position, u8 mode);


static unsigned int convertFileMode(__file_modes* mode) {
    unsigned int openMode = mode->open_mode;
    unsigned int ioMode = mode->io_mode;
    unsigned int binary = mode->binary_io;
    unsigned int result = 0;

    switch (openMode) {
    case 0:
        result |= 0x01;
        break;
    case 2:
        result |= 0x02;
        break;
    case 1:
        result |= 0x04;
        break;
    }

    switch (ioMode) {
    case 1:
        result |= 0x01;
        break;
    case 2:
        result |= 0x02;
        break;
    case 6:
        result |= 0x04;
        break;
    case 3:
        result |= 0x12;
        break;
    case 7:
        result |= 0x07;
        break;
    }

    if (binary == 1)
        result = (result | 0x08) & 0xFF;

    return result;
}

int __position_file(unsigned long handle, long* position, int mode, void* idle) {
    int positionMode = 0;

    if (!GetTRKConnected())
        return 1;

    if (mode == 0)
        positionMode = 0;
    else if (mode == 1)
        positionMode = 1;
    else if (mode == 2)
        positionMode = 2;

    switch (TRKPositionFile(0xD4, handle, position, positionMode)) {
    case 0:
        return 0;
    case 2:
        return 2;
    case 1:
    default:
        return 1;
    }
}

int __close_file(unsigned long handle) {
    if (!GetTRKConnected())
        return 1;

    switch (TRKCloseFile(0xD3, handle)) {
    case 0:
        return 0;
    case 2:
        return 2;
    case 1:
    default:
        return 1;
    }
}

int __open_file(const char* name, __file_modes mode, unsigned long* handle) {
    unsigned int dsMode;

    if (!GetTRKConnected())
        return 1;

    dsMode = convertFileMode(&mode);
    switch (TRKOpenFile(0xD2, (unsigned long)name, dsMode, handle)) {
    case 0:
        return 0;
    case 2:
        return 2;
    case 1:
    default:
        return 1;
    }
}

int __write_file(unsigned long handle, unsigned char* buffer, size_t* count, void* idle) {
    size_t length;
    u8 result;

    if (!GetTRKConnected())
        return 1;

    length = *count;
    result = TRKAccessFile(0xD0, handle, &length, buffer);
    *count = length;

    switch (result) {
    case 0:
        return 0;
    case 2:
        return 2;
    case 1:
    default:
        return 1;
    }
}

int __read_file(unsigned long handle, unsigned char* buffer, size_t* count, void* idle) {
    size_t length;
    u8 result;

    if (!GetTRKConnected())
        return 1;

    length = *count;
    result = TRKAccessFile(0xD1, handle, &length, buffer);
    *count = length;

    switch (result) {
    case 0:
        return 0;
    case 2:
        return 2;
    case 1:
    default:
        return 1;
    }
}

int __close_console(unsigned long handle) {
    if (!GetTRKConnected())
        return 1;

    switch (TRKCloseFile(0xD3, handle)) {
    case 0:
        return 0;
    case 2:
        return 2;
    case 1:
    default:
        return 1;
    }
}
