#include <iostream.h>
#include <dos.h>

#include "system.h"
#include "kernel.h"
#include "logger.h"
#include "section.h"
#include "thread.h"
#include "schedule.h"
#include "pcb.h"

unsigned tsp, tss, tbp;

void Syscall::Register(int number, SyscallHandle handle)
{
    LOCK()
    setvect(number, handle);
    UNLOCK()

    Logger::Log(Logger::Message, "[Syscall::Register]: Register new syscall on entry %d", number);
}

void Syscall::Trigger(int number)
{
    // EMPTY
}

SyscallHandle Syscall::UnRegister(int number)
{
    LOCK()
    SyscallHandle handle = getvect(number);
    setvect(number, Syscall::Syscall_DummyHandle);

    Logger::Log(Logger::Message, "[Syscall::Unegister]: UnRegister syscall on entry %d", number);
    UNLOCK()

    return handle;
}

void interrupt Syscall::Syscall_DummyHandle(...)
{
    Logger::Log(Logger::Message, "[Syscall::Syscall_DummyHandle]: Hello from DummyHandle!");
}

void interrupt Syscall::Syscall_Timer(...)
{
    LOCK();

    Kernel::SleepingThreads->Tick();

    tick();

    // Implement other way for triggering interrupts
    INT(0x55);

    if (!Kernel::RunningThread->IsInfinite())
    {
        if (Kernel::RunningThread->pcb->slice > 0)
        {
            Kernel::RunningThread->pcb->slice--;
        }

        if (Kernel::RunningThread->pcb->slice == 0)
        {
            if (!Kernel::IsLocked())
            {
                Syscall::Syscall_Dispatch();
            }
        }
    }

    UNLOCK();
}

void interrupt Syscall::Syscall_Dispatch(...)
{
    LOCK();

    asm {
        mov tsp, sp
        mov tss, ss
        mov tbp, bp
    }

    Kernel::RunningThread->pcb->sp = tsp;
    Kernel::RunningThread->pcb->ss = tss;
    Kernel::RunningThread->pcb->bp = tbp;

    if (Kernel::RunningThread->IsRunning() && !Kernel::RunningThread->IsIdleThread())
    {
        ThreadState::SetState(Kernel::RunningThread->getState(), ThreadState::RUNABLE);
        Scheduler::put(Kernel::RunningThread->pcb);

        Logger::Log(Logger::Message, "[Syscall::Syscall_Dispatch]: Swtiching from thread PID: %d", Kernel::RunningThread->getId());
    }

    PCB *current = Scheduler::get();
    if (current == NULL)
    {
        Kernel::RunningThread = Kernel::ThreadIdle;
    }
    else
    {
        Kernel::RunningThread = current->owner;
        Logger::Log(Logger::Message, "[Syscall::Syscall_Dispatch]: Swtiching to thread PID: %d", Kernel::RunningThread->getId());
    }

    tsp = Kernel::RunningThread->pcb->sp;
    tss = Kernel::RunningThread->pcb->ss;
    tbp = Kernel::RunningThread->pcb->bp;

    ThreadState::SetState(Kernel::RunningThread->getState(), ThreadState::RUNNING);
    Kernel::RunningThread->pcb->slice = Kernel::RunningThread->pcb->timeSlice;

    asm {
        mov sp, tsp
        mov ss, tss
        mov bp, tbp
    }

    UNLOCK();
}