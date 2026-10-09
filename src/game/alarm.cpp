// The static initialisation of alarm.cpp (0x8005a3a8): the global alarm list
// starts empty and owning its nodes (its weak destructor registered). The
// rest of the file is in other units or not reconstructed; the list's range
// constructor before it and the weak GetLookDirection after it are not part
// of this unit. The template, AlarmInfo and g_alarms are named by the
// symbols; the list layout and its default constructor (the sentinel's element
// cleared word by word) are inferred (as in
// alarm_list.cpp).
struct AlarmInfo {
    unsigned char data[8];
};

namespace dwi {
template <class T>
struct list_node {
    list_node* m_prev;
    list_node* m_next;
    T m_data;
};

template <class T>
class list {
public:
    list() : m_count(0), m_unknown4(0), m_pool(0), m_free(0) {
        m_head.m_prev = &m_head;
        m_head.m_next = &m_head;
        *(int*)&m_head.m_data = 0;
        ((int*)&m_head.m_data)[1] = 0;
        m_owned = true;
    }
    ~list();

    int m_count;
    int m_unknown4;
    list_node<T>* m_pool;
    list_node<T>* m_free;
    list_node<T> m_head;
    bool m_owned;
};
}

dwi::list<AlarmInfo> g_alarms;
