// A fragment of thrownbullet.cpp: CThrownBullet's array operator new (DWI_alloc with no name and
// the 1024-byte heap tag). The file name is this project's; the original
// record is thrownbullet.cpp. The class and functions are named by the mangled symbols;
// the third argument's meaning is inferred.
void* DWI_alloc(const char*, int, int);

class CThrownBullet {
public:
    static void* operator new[](unsigned long);
};

void* CThrownBullet::operator new[](unsigned long size) {
    return DWI_alloc(0, size, 1024);
}
