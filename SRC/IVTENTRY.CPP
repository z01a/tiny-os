#include <dos.h>

#include "ivtentry.h"
#include "event.h"
#include "section.h"

KernelEvent** IVTEntry::Events = new KernelEvent*[256];

void IVTEntry::RegisterEntry(IVTNo entry, KernelEvent* event) {
    LOCK()
    IVTEntry::Events[entry] = event;
    UNLOCK()
}

void IVTEntry::UnRegisterEntry(IVTNo entry) {
    LOCK()
    IVTEntry::Events[entry] = NULL;
    UNLOCK()
}

IVTEntry::IVTEntry(IVTNo entry, SyscallHandle handle) {
    LOCK()
    this->entry = entry;
    
    this->oldHandle = Syscall::UnRegister(entry);
    Syscall::Register(entry, handle);
    UNLOCK()
}

IVTEntry::~IVTEntry() {
    LOCK()
    Syscall::Register(this->entry, this->oldHandle);
    UNLOCK()
}

void IVTEntry::callOldHandle() {
    this->oldHandle();
}

void IVTEntry::signal() {
    LOCK()
    if(IVTEntry::Events[this->entry] != NULL) {
        IVTEntry::Events[this->entry]->signal();
    }
    UNLOCK()
}