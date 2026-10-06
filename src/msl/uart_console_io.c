/* MSL uart_console_io.c: __write_console. Follows the Wind Waker
 * decompilation's version (CC0; src/tww/.../uart_console_io.c), with
 * __init_uart_console inlined as in Frontline, __write_console weak (as in the
 * target) and no __close_console. */
int InitializeUART(unsigned int);
int WriteUARTN(void*, unsigned long);
int __TRK_write_console(int, int, int*, int);

static inline int __init_uart_console(void) {
    static int initialized = 0;
    int ret = 0;

    if (initialized == 0) {
        ret = InitializeUART(0xE100);

        if (ret == 0) {
            initialized = 1;
        }
    }

    return ret;
}

__declspec(weak) int __write_console(int param_0, int param_1, int* param_2, int param_3) {
    if (__init_uart_console() != 0) {
        return 1;
    }

    if (WriteUARTN((void*)param_1, *param_2) != 0) {
        *param_2 = 0;
        return 1;
    }

    __TRK_write_console(param_0, param_1, param_2, param_3);
    return 0;
}
