// MSL C++ NewMore.cp: std::exception's destructor and what(). Reconstructed
// from the disassembly. what() precedes the destructor here because
// -inline deferred emits functions in reverse source order. Both are weak in
// the target, as header-inline members emitted with the vtable would be.
namespace std {

class exception {
public:
    exception() {}
    virtual ~exception();
    virtual const char* what() const;
};

__declspec(weak) const char* exception::what() const {
    return "exception";
}

__declspec(weak) exception::~exception() {
}

}
