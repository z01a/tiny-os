#ifndef _EVENT_H_
#define _EVENT_H_

#include "thread.h"
#include "types.h"

class KernelEvent {
    public:
        KernelEvent(IVTNo entry);
        ~KernelEvent();

        void wait();
        void signal();
    private:
        Thread* owner;
        Thread* blocked;

        int value;
        IVTNo entry;
};

class Event {
    public:
        Event(IVTNo ivtNo) {
            eventImp = new KernelEvent(ivtNo);
        }

        ~Event() {
            delete eventImp;

            eventImp = NULL;
        }

        void wait() {
            eventImp->wait();
        }

    protected:
        friend class KernelEvent;
        
        void signal() {
            eventImp->signal();
        }

    private:
        KernelEvent* eventImp;
};

#endif