#include <iostream.h>
#include <assert.h> 
#include <dos.h>

#include "kernel.h"
#include "schedule.h"
#include "section.h"
#include "pcb.h"

// Kernel Instance for singleton
Kernel * Kernel::Instance = NULL;

// Global Kernel instance
Kernel * kernel = Kernel::GetInstance();

// Current running thread
Thread * Kernel::RunningThread = NULL;

// Main Thread
Thread * Kernel::ThreadMain = NULL;

// Idle Thread
Thread * Kernel::ThreadIdle = NULL;

// List of all threads
ThreadQueue * Kernel::ActiveThreads = NULL;

// List of threads that are sleeping
SleepQueue * Kernel::SleepingThreads = NULL;

// Next ID for new thread
volatile ID Kernel::NextThreadId = -1;

// Global lock for kernel
volatile int Kernel::Lock = 0;

// Is dispatch requested
volatile int Kernel::RequestedDispatch = 0;

// Return value of the userMain function
int MainUserThread::Result = 0;

unsigned ttsp, ttss, ttbp;

Kernel::Kernel() {
    // Logger::Log(Logger::Message, "[Kernel::Kernel]: Constructor!");
    this->nextId = 0;
}

/*
    - Initialize interrupts
        - Initialize timer
    - Inititalize IDLE thread
*/
void Kernel::Initialize() {
    LOCK()
    Logger::Log(Logger::Message, "[Kernel::Initialize]: Initializing kernel started!");
        
    Kernel::ThreadMain = new MainThread();
    Kernel::RunningThread = Kernel::ThreadMain;
    ThreadState::SetState(Kernel::RunningThread->getState(), ThreadState::RUNNING);

    Logger::Log(Logger::Message, "[Kernel::Initialize]: Current running thread PID: %d", Kernel::RunningThread->getId());

    Kernel::ThreadIdle = new IdleThread();

    Logger::Log(Logger::Message, "[Kernel::Initialize]: Idle thread PID: %d", Kernel::ThreadIdle->getId());

    Kernel::ActiveThreads = new ThreadQueue();
    Kernel::SleepingThreads = new SleepQueue();

    SyscallHandle timerHandle = Syscall::UnRegister(Syscall::Timer);
    Syscall::Register(Syscall::OldTimer, timerHandle);

    Syscall::Register(Syscall::Dispatch, Syscall::Syscall_Dispatch);
    Syscall::Register(Syscall::Timer, Syscall::Syscall_Timer);

    Logger::Log(Logger::Message, "[Kernel::Initialize]: Initializing kernel finished!");
    UNLOCK()
}

void Kernel::Dispatch() {
    // Empty
}

void Kernel::AcquireLock() {
    Kernel::Lock = 1;
}

int Kernel::IsLocked() {
    return Kernel::Lock;
}

void Kernel::ReleaseLock() {
    Kernel::Lock = 0;
}

void Kernel::UnInitialize() {
    Logger::Log(Logger::Message, "[Kernel::UnInitialize]: UnInitializing kernel started!");

    SyscallHandle systemTimer = Syscall::UnRegister(Syscall::OldTimer);
    Syscall::Register(Syscall::Timer, systemTimer);
    Syscall::UnRegister(Syscall::Dispatch);

    // Maybe we should not delete it cuz it's still working
    LOCK()
    delete Kernel::ThreadMain;
    delete Kernel::ThreadIdle;
    UNLOCK()

    LOCK()
    delete Kernel::SleepingThreads;
    delete Kernel::ActiveThreads;
    UNLOCK()

    Logger::Log(Logger::Message, "[Kernel::UnInitialize]: UnInitializing kernel finished!");
}

Kernel * Kernel::GetInstance() {
    if(Kernel::Instance == NULL) {
        Kernel::Instance = new Kernel();
    }

    return Kernel::Instance;
}

Kernel::~Kernel() {
    Logger::Log(Logger::Message, "[Kernel::~Kernel]: Destructor!");
}