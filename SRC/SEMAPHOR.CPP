#include "semaphor.h"
#include "pcb.h"
#include "kernel.h"
#include "system.h"
#include "section.h"
#include "schedule.h"
#include "logger.h"

KernelSemaphore::KernelSemaphore(int value) {
    this->value = value;
}

int KernelSemaphore::wait(Time timeToWait) {
    LOCK();

    if(timeToWait > 0) {
        Kernel::RunningThread->sleep(timeToWait);
        return 0;
    }else if(--value < 0) {
        block();
        return 1;
    }

    UNLOCK();
}

int KernelSemaphore::signal(int n) {
    LOCK();
    if(n == 0) {
        n = 1;
    }

    int count = 0;
    
    while(count++ < n) {
        if(value++ < 0) {
            deblock();
        } else {
            break;
        }
    }

    UNLOCK();

    return count - 1;
}

int KernelSemaphore::val() const {
    return this->value;
}

void KernelSemaphore::block() {
    LOCK();
    ThreadState::SetState(Kernel::RunningThread->getState(), ThreadState::BLOCKED);
    blocked.enqueue(Kernel::RunningThread);

    Logger::Log(Logger::Message, "[KernelSemaphore::block]: Thread PID: %d is blocked on semaphore!", Kernel::RunningThread->getId());

    UNLOCK();
    Syscall::Syscall_Dispatch();
}

void KernelSemaphore::deblock() {
    LOCK();
        if(!blocked.empty()) {
            Thread* thread = blocked.dequeue();
            ThreadState::SetState(thread->getState(), ThreadState::RUNABLE);

            Logger::Log(Logger::Message, "[KernelSemaphore::deblock]: Thread (PID:%d) is deblocked on semaphore!", thread->getId());

            Scheduler::put(thread->pcb);
        }
    UNLOCK();
}

KernelSemaphore::~KernelSemaphore() {
    // Dummy
}