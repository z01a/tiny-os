#include <iostream.h>

#include "types.h"
#include "thread.h"
#include "kernel.h"
#include "pcb.h"
#include "logger.h"

Thread::Thread(StackSize stackSize, Time timeSlice) {
    this->pcb = new PCB(stackSize, timeSlice, this);
}

Thread::Thread(Time timeSlice) {
    this->pcb = new PCB(timeSlice, this);
}

void Thread::start() {
    this->pcb->start();
}

void Thread::wakeUp() {
    this->pcb->wakeUp();
}

int Thread::IsIdleThread() {
    return (this == Kernel::ThreadIdle);
}

int Thread::IsMainThread() {
    return (this == Kernel::ThreadMain);
}

int Thread::IsSleeping() {
    return (ThreadState::GetState(this) == ThreadState::SLEEPING);
}

int Thread::IsRunning() {
    return (ThreadState::GetState(this) == ThreadState::RUNNING);
}

int Thread::IsBlocked() {
    return (ThreadState::GetState(this) == ThreadState::BLOCKED);
}

int Thread::IsTerminated() {
    return (ThreadState::GetState(this) == ThreadState::TERMINATED);
}

int Thread::IsInfinite() {
    return !this->pcb->getTimeSlice();
}

Time Thread::getSleepingTime() {
    return this->pcb->getSleepingTime();
}

void Thread::sleep(Time timeToSleep) {
    this->pcb->sleep(timeToSleep);
}

ThreadState& Thread::getState() {
    return this->pcb->getState();
}

ID Thread::getId() {
    return this->pcb->getId();
}

ID Thread::getRunningId() {
    return Kernel::RunningThread->getId();
}

Thread * Thread::getThreadById(ID id) {
    return Kernel::ActiveThreads->GetThreadById(id);
}

void Thread::waitToComplete() {
    this->pcb->waitToComplete();
}

Thread::~Thread() {
    waitToComplete();
    delete this->pcb;
}

// MUST IMPLEMENT
void dispatch() {
    Syscall::Syscall_Dispatch();
}