// A singly linked list with head, tail and count.
class CLinkedListElement {
public:
    CLinkedListElement* next;

    ~CLinkedListElement();
    CLinkedListElement();
};

class CLinkedList {
public:
    CLinkedListElement* head;
    CLinkedListElement* tail;
    int count;

    bool IsEmpty();
    void RemoveElement(CLinkedListElement*);
    void AddElementAtEnd(CLinkedListElement*);
    void AddElementAtBeginning(CLinkedListElement*);
    ~CLinkedList();
    CLinkedList();
};

bool CLinkedList::IsEmpty() {
    return head == 0;
}

void CLinkedList::RemoveElement(CLinkedListElement* element) {
    if (element == head) {
        if (element->next) {
            head = element->next;
            element->next = 0;
        } else {
            head = 0;
            tail = 0;
        }
        count--;
        return;
    }

    for (CLinkedListElement* previous = head; previous; previous = previous->next) {
        if (previous->next == element) {
            if (element->next) {
                previous->next = element->next;
                element->next = 0;
            } else {
                previous->next = 0;
                tail = previous;
            }
            count--;
            return;
        }
    }
}

void CLinkedList::AddElementAtEnd(CLinkedListElement* element) {
    CLinkedListElement* last = tail;
    if (last) {
        element->next = last->next;
        last->next = element;
    } else {
        head = element;
    }
    tail = element;
    count++;
}

void CLinkedList::AddElementAtBeginning(CLinkedListElement* element) {
    if (head)
        element->next = head;
    else
        tail = element;
    head = element;
    count++;
}

CLinkedList::~CLinkedList() {
}

CLinkedList::CLinkedList() : head(0), tail(0), count(0) {
}

CLinkedListElement::~CLinkedListElement() {
}

CLinkedListElement::CLinkedListElement() : next(0) {
}
