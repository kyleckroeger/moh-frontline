// A fragment of bsmessage.cpp (0x80039b84): BSMessageIsGroupCategoryMember,
// which reports whether two trigger objects share a group of the category in
// the high half of the match value: both group lists (on the object's core at
// +68, a count word followed by sorted values whose high half is the category)
// are skipped to the category and then merged. The core and list views are
// inferred from offsets.
struct TriggerCoreView {
    unsigned char unknown00[68];
    unsigned int* groups;
};

struct TriggerObject_struct {
    unsigned char unknown00[4];
    TriggerCoreView* core;
};

int BSMessageIsGroupCategoryMember(TriggerObject_struct* first, TriggerObject_struct* second, unsigned int match) {
    unsigned int bv;
    unsigned int av;
    unsigned int* a;
    unsigned int* b;
    unsigned int category;
    unsigned int* pa;
    unsigned int* lastA;
    unsigned int* lastB;
    TriggerCoreView* firstCore;
    TriggerCoreView* secondCore;
    unsigned int* pb;

    if (!first || !second)
        return 0;
    category = match >> 16;
    firstCore = first->core;
    secondCore = second->core;
    if ((a = firstCore->groups) != 0) {
        if ((b = secondCore->groups) != 0) {
            pa = a + 1;
            lastA = a + a[0];
            pb = b + 1;
            lastB = b + b[0];
            for (; pa <= lastA; pa++) {
                if ((*pa >> 16) >= category)
                    break;
            }
            for (; pb <= lastB; pb++) {
                if ((*pb >> 16) >= category)
                    break;
            }
            while (pa <= lastA && pb <= lastB) {
                av = *pa;
                if ((av >> 16) != category)
                    break;
                bv = *pb;
                if ((bv >> 16) != category)
                    break;
                if (av < bv)
                    pa++;
                else if (av > bv)
                    pb++;
                else
                    return 1;
            }
        }
    }
    return 0;
}
