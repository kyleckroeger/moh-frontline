// A fragment of bsbifunc.cpp (0x8002aaa8): the script built-in that shows a
// prompt message (none for -1) through the pop-up message handler. It reads its
// argument below the script stack top and pops the built-in's arguments. The
// file name is this project's; the original record is bsbifunc.cpp and the
// built-ins around it are not reconstructed. The functions and globals are
// named by the mangled symbols; g_pPopUpMessageHandler is a 16-byte object
// whose first word is passed, declared here as four handler pointers
// (inferred), and the built-in record view is inferred.
class PopUpMessageHandler;

extern PopUpMessageHandler* g_pPopUpMessageHandler[4];

void PromptUserMessage(unsigned int, PopUpMessageHandler*);

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_DisplayPrompt(int** stack, void*) {
    int message = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    if (message != -1)
        PromptUserMessage(message, g_pPopUpMessageHandler[0]);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
