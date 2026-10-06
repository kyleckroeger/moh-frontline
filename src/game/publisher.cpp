// ISubject's destruction and notification: marking for destruction notifies
// the observers with event 2, and notifications go through the publisher.
// ISubject, IDestructible, CPublisher and ESubjectEvent are named by the
// mangled symbols; the classes are non-virtual views. NotifyObservers is
// inline in the original (a weak symbol), so it is defined __declspec(weak).
// The publisher's own functions and the rest of the file are not part of this
// unit.
enum ESubjectEvent {};

class ISubject;

class CPublisher {
public:
    static void PublishEvent(ISubject&, ESubjectEvent);
};

class IDestructible {
public:
    void MarkForDestruction(int);
};

class ISubject : public IDestructible {
public:
    void MarkForDestruction(int);
    void NotifyObservers(ESubjectEvent);
};

void ISubject::MarkForDestruction(int flag) {
    IDestructible::MarkForDestruction(flag);
    NotifyObservers((ESubjectEvent)2);
}

__declspec(weak) void ISubject::NotifyObservers(ESubjectEvent event) {
    CPublisher::PublishEvent(*this, event);
}
