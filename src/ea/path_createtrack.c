/* PATH_createtrack (path.c, 0x8010cb8c): passes its arguments to
   PATH_createtrackfrombuf with no buffer (a zero last argument). The
   function names come from the symbols; the parameter count is read from
   the call (eight register arguments, one stack argument and the zero) and
   the parameter meanings are not established. PATH_addmapfile after it is
   drafted in scratch (register allocation differs). */
extern "C" int PATH_createtrackfrombuf(int, int, int, int, int, int, int, int, int, int);

extern "C" int PATH_createtrack(int a, int b, int c, int d, int e, int f, int g, int h, int i) {
    return PATH_createtrackfrombuf(a, b, c, d, e, f, g, h, i, 0);
}
