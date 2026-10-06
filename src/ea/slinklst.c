// Doubly linked lists with head, tail and count. SNDLINKLIST and SNDLINKNODE
// are named by the mangled symbols; their members are inferred from offsets.
struct SNDLINKNODE {
    SNDLINKNODE* next;
    SNDLINKNODE* prev;
};

struct SNDLINKLIST {
    SNDLINKNODE* head;
    SNDLINKNODE* tail;
    int count;
};

void SNDLINKI_init(SNDLINKLIST* list) {
    list->head = 0;
    list->tail = 0;
    list->count = 0;
}

void SNDLINKI_push(SNDLINKLIST* list, SNDLINKNODE* node) {
    node->next = list->head;
    node->prev = 0;
    if (list->head)
        list->head->prev = node;
    else
        list->tail = node;
    list->head = node;
    list->count++;
}

void SNDLINKI_pushtail(SNDLINKLIST* list, SNDLINKNODE* node) {
    node->next = 0;
    node->prev = list->tail;
    if (list->tail)
        list->tail->next = node;
    else
        list->head = node;
    list->tail = node;
    list->count++;
}

SNDLINKNODE* SNDLINKI_pop(SNDLINKLIST* list) {
    SNDLINKNODE* node = list->head;

    if (list->head) {
        list->head = list->head->next;
        if (!list->head)
            list->tail = 0;
        else
            list->head->prev = 0;
        list->count--;
    }
    return node;
}

void SNDLINKI_remove(SNDLINKLIST* list, SNDLINKNODE* node) {
    if (node == list->head)
        list->head = list->head->next;
    if (node == list->tail)
        list->tail = list->tail->prev;
    if (node->prev)
        node->prev->next = node->next;
    if (node->next)
        node->next->prev = node->prev;
    list->count--;
}
