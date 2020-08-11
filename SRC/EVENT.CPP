#include "event.h"

#include "ivtentry.h"
#include "schedule.h"
#include "kernel.h"
#include "logger.h"
#include "section.h"
#include "system.h"
#include "pcb.h"

KernelEvent::KernelEvent(IVTNo entry) {
    LOCK();
    this->value = 0;
    this->blocked = NULL;
    this->owner = Kernel::RunningThread;
    this->entry = entry;

    Logger::Log(Logger::Message, "[KernelEvent::KernelEvent] - Create event on entry %d with thread owner PID: %d", this->entry, this->owner->getId());

    IVTEntry::RegisterEntry(entry, this);
    UNLOCK();
}

void KernelEvent::wait() {
    LOCK();
    if(this->owner == Kernel::RunningThread) {
        if(this->value) {
            this->value = 0;
        } else {
            ThreadState::SetState(this->owner->getState(), ThreadState::BLOCKED);
            this->blocked = Kernel::RunningThread;

            Logger::Log(Logger::Message, "[KernelEvent::wait] - Blocked thread PID: %d on event with entry %d", this->blocked->getId(), this->entry);

            Syscall::Syscall_Dispatch();
        }
    }
    UNLOCK();
}

void KernelEvent::signal() {
    LOCK();
    if(this->blocked) {
        ThreadState::SetState(this->owner->getState(), ThreadState::RUNABLE);
        Scheduler::put(blocked->pcb);

        Logger::Log(Logger::Message, "[KernelEvent::wait] - Unblocked thread PID: %d on event with entry %d", this->blocked->getId(), this->entry);

        this->blocked = NULL;
    } else {
        this->value = 1;
    }
    UNLOCK();
}

KernelEvent::~KernelEvent() {
    IVTEntry::UnRegisterEntry(entry);

    this->blocked = NULL;
}
