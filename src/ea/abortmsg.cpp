// REAL abort messages: format the message, then hand it to the abort hook
// (REAL_abortmessage) or print it with the recorded file and line and exit
// (SYSTEM_abortmessage).
typedef struct {
    char gpr;
    char fpr;
    char reserved[2];
    char* input_arg_area;
    char* reg_save_area;
} va_list_struct;
typedef va_list_struct va_list[1];

extern "C" {
int vsprintf(char*, const char*, va_list);
void PRINT_string(int, const char*, ...);
void REAL_exit(void);
extern void (*REALabortmessage)(const char*, ...);

char* REALabortfilename;
int REALabortlinenum;

void REAL_abortmessage(const char* format, ...) {
    va_list args;
    char buffer[512];

    if (format) {
        __builtin_va_info(&args);
        vsprintf(buffer, format, args);
    } else {
        buffer[0] = 0;
    }
    REALabortmessage("%s", buffer);
}

void SYSTEM_abortmessage(const char* format, ...) {
    va_list args;
    char buffer[512];

    if (format) {
        __builtin_va_info(&args);
        vsprintf(buffer, format, args);
    } else {
        buffer[0] = 0;
    }
    PRINT_string(2, "ERROR: %s", buffer);
    if (REALabortfilename)
        PRINT_string(2, "FILE %s LINE %d\n", REALabortfilename, REALabortlinenum);
    REAL_exit();
}
}
