#ifndef _IVTENTRY_H_
#define _IVTENTRY_H_

#include "event.h"
#include "system.h"
#include "types.h"

class IVTEntry {
    public:
        IVTEntry(IVTNo entry, SyscallHandle handle);
        ~IVTEntry();

        static void RegisterEntry(IVTNo entry, KernelEvent* event);
        static void UnRegisterEntry(IVTNo entry);

        void callOldHandle();
        void signal();
    protected:
        friend class KernelEvent;

    private:
        SyscallHandle oldHandle;
        IVTNo entry;

        static KernelEvent** Events;
};

#define PREPAREENTRY(entry, callOld)\
void interrupt EntryInterrupt##entry(...);\
IVTEntry eventEntry##entry(entry,EntryInterrupt##entry);\
void interrupt EntryInterrupt##entry(...) {\
eventEntry##entry.signal();\
if(callOld) eventEntry##entry.callOldHandle();\
}

#endif