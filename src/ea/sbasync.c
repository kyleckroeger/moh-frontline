// Asynchronous sound-bank loading, the functions at the start of the file:
// completing and issuing the ring of four file reads. sndbas holds the load
// in progress and the read priority; the load and read-record views are
// inferred from offsets and are not original. The header transfer, downloads,
// server and the public load functions that follow are not reconstructed.
extern "C" {
int FILESYS_completeop(int);
int FILESYS_opstatus(int);
int FILESYS_read(int, int, void*, int, int, void*);
}

struct SndBankReadView {
    int op;
    unsigned char unknown04[4];
    void* buffer;
    signed char state;
    unsigned char unknown0d[3];
};

struct SndBankLoadView {
    unsigned char unknown00[12];
    unsigned int streamed;
    unsigned char unknown10[4];
    int fileOffset;
    int file;
    unsigned char unknown1c[4];
    int size;
    unsigned char unknown24[16];
    unsigned short chunkSize;
    unsigned char unknown36[2];
    unsigned short issued;
    unsigned short completed;
    unsigned char unknown3c[4];
    SndBankReadView reads[4];
};

struct SndBankAsyncView {
    SndBankLoadView* load;
    int priority;
    int unknown08;
};

extern SndBankAsyncView sndbas;

void SNDBANKI_asynccompletereads(void) {
    SndBankReadView* read;
    int i = sndbas.load->completed % 4;

    while (sndbas.load->issued > sndbas.load->completed) {
        read = &sndbas.load->reads[i];
        if (read->state != 1)
            break;
        if (FILESYS_opstatus(read->op) != 1)
            break;
        if (FILESYS_completeop(read->op) > 0)
            read->state = 2;
        else
            read->state = 0;
        i++;
        sndbas.load->completed++;
        if (i >= 4)
            i = 0;
    }
}

void SNDBANKI_asyncissuereads(void) {
    SndBankLoadView* load;
    SndBankReadView* read;
    int i;
    int size;
    int offset;

    size = 0xFFFFFF;
    i = sndbas.load->issued % 4;
    while (sndbas.load->completed > sndbas.load->issued - 4) {
        load = sndbas.load;
        offset = load->issued * load->chunkSize;
        if (load->streamed) {
            size = load->size - offset;
            if (size <= 0)
                break;
        }
        if (size > load->chunkSize)
            size = load->chunkSize;
        read = &load->reads[i];
        if (read->state)
            break;
        read->op = FILESYS_read(load->file, load->fileOffset + offset, read->buffer, size, sndbas.priority, 0);
        read->state = 1;
        i++;
        sndbas.load->issued++;
        if (i >= 4)
            i = 0;
    }
}
