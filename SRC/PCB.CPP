#include <iostream.h>
#include <dos.h>

#include "types.h"
#include "kernel.h"
#include "logger.h"
#include "system.h"
#include "schedule.h"
#include "section.h"
#include "pcb.h"

extern Kernel * kernel;

extern Thread * RunningThread;

PCB::PCB(StackSize stackSize, Time timeSlice, Thread * owner) {
    LOCK();
    this->owner = owner;
    this->timeSlice = timeSlice;
    this->slice = timeSlice;
    this->pid = ++Kernel::NextThreadId;
    this->sleepingTime = 0;

    this->stack = new unsigned int[stackSize];

    this->stack[stackSize - 1] = 0x200;
    this->stack[stackSize - 2] = FP_SEG(PCB::Wrapper);
    this->stack[stackSize - 3] = FP_OFF(PCB::Wrapper);

    this->sp = FP_OFF(this->stack + stackSize - 12);
    this->ss = FP_SEG(this->stack + stackSize - 12);
    this->bp = this->sp;

    ThreadState::SetState(owner->getState(), ThreadState::NEW);

    UNLOCK();
    
}

PCB::PCB(Time timeSlice, Thread * owner) {
    LOCK();
    this->owner = owner;
    this->timeSlice = timeSlice;
    this->slice = timeSlice;
    this->pid = ++Kernel::NextThreadId;
    this->sleepingTime = 0;
    this->stack = NULL;

    ThreadState::SetState(owner->getState(), ThreadState::NEW);
    UNLOCK();

    Logger::Log(Logger::Message, "[PCB::PCB]: Created stackless thread (PID:%d)!", this->pid);

}

ThreadState& PCB::getState() {
    return this->state;
}

ID PCB::getId() {
    return this->pid;
}

void PCB::waitToComplete() {
    
    if(Kernel::RunningThread == owner) {
        return;
    }

    if(owner == Kernel::ThreadIdle) {
        return;
    }

    if(ThreadState::GetState(getState()) != ThreadState::TERMINATED) {
        LOCK()
        ThreadState::SetState(Kernel::RunningThread->getState(), ThreadState::BLOCKED);
        owner->blocked.Enqueue(Kernel::RunningThread);

        UNLOCK()

        Logger::Log(Logger::Message, "[PCB::waitToComplete]: Thread PID: %d is blocked by thread PID: %d!", Kernel::RunningThread->getId(), owner->getId());

        Syscall::Syscall_Dispatch();
    }
}

Time PCB::getSleepingTime() {
    return this->sleepingTime;
}

Time PCB::getSlice() {
    return this->slice;
}

Time PCB::getTimeSlice() {
    return this->timeSlice;
}

void PCB::wakeUp() {
    if(owner->IsSleeping() && getSleepingTime()) {
        this->sleepingTime--;

        if(this->sleepingTime == 0) {
            LOCK()
            Scheduler::put(this);
            UNLOCK()
            Logger::Log(Logger::Message, "[PCB::wakeUp]: Thread PID: %d wokeUp", owner->getId());
        }
    }
}

void PCB::start() {
    if(ThreadState::GetState(owner->getState()) == ThreadState::NEW) {
        LOCK()
        ThreadState::SetState(owner->getState(), ThreadState::RUNABLE);
        Scheduler::put(this);

        Kernel::ActiveThreads->Enqueue(owner);
        UNLOCK()

        Logger::Log(Logger::Message, "[PCB::start]: Thread PID: %d started", owner->getId());
    }
}

void PCB::sleep(Time timeToSleep) {
    if(timeToSleep) {
        LOCK();
        this->sleepingTime = timeToSleep;
        ThreadState::SetState(Kernel::RunningThread, ThreadState::SLEEPING);
        Kernel::SleepingThreads->Enqueue(Kernel::RunningThread);
        UNLOCK();

        Logger::Log(Logger::Message, "[PCB::sleep]: Thread PID: %d started sleeping for %d [n*55ms]", Kernel::RunningThread->getId(), timeToSleep);

        Syscall::Syscall_Dispatch();
    }
}

void PCB::Wrapper() {
    Logger::Log(Logger::Message, "[PCB::Wrapper]: Invoking run() on thread PID: %d", Kernel::RunningThread->getId());
    Kernel::RunningThread->run();
    Logger::Log(Logger::Message, "[PCB::Wrapper]: Thread PID: %d finished run() execution", Kernel::RunningThread->getId());
    
    // TODO: Put all blocked threads in Scheduler
    Thread* thread = Kernel::RunningThread->blocked.dequeue();
    if(thread != NULL) {
        Logger::Log(Logger::Message, "[PCB::Wrapper]: Thread PID: %d is unblocked because thread PID: %d finished execution", thread->getId(), Kernel::RunningThread->getId());
        LOCK();
        ThreadState::SetState(thread->getState(), ThreadState::RUNABLE);
        Scheduler::put(thread->pcb);
        UNLOCK();
    }

    ThreadState::SetState(Kernel::RunningThread->getState(), ThreadState::TERMINATED);

    Syscall::Syscall_Dispatch();
}

PCB::~PCB() {
    Logger::Log(Logger::Message, "[PCB::~PCB]: Destroying PCB for thread PID: %d", owner->getId());

    if(this->stack) {
        LOCK();
        delete [] this->stack;
        UNLOCK();
        Logger::Log(Logger::Message, "[PCB::~PCB]: Deleting stack for thread PID: %d", owner->getId());
    }
}