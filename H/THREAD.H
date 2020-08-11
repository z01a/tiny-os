#ifndef _THREAD_H_
#define _THREAD_H_

#include "types.h"
#include "queue.h"

const StackSize defaultStackSize = 4096;
const Time defaultTimeSlice = 2;

class PCB;
class ThreadState;
class KernelSemaphore;
class KernelEvent;

class Thread {
    public:
        void start();
        void waitToComplete();
        void sleep(Time timeToSleep);
        void wakeUp();

        int IsSleeping();
        int IsBlocked();
        int IsTerminated();
        int IsRunning();
        int IsInfinite();

        int IsMainThread();
        int IsIdleThread();

        Time getSleepingTime();

        virtual ~Thread();

        ID getId();
        ThreadState& getState();
        static ID getRunningId();
        static Thread * getThreadById(ID id);

    protected:
        friend class PCB;
        friend class ThreadState;
        friend class Kernel;
        friend class Syscall;
        friend class KernelSemaphore;
        friend class KernelEvent;

        Thread(StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice);

        Thread(Time timeSlice);

        virtual void run() {
            // Empty
        }

    private:
        PCB * pcb;
        
        // Threads that are blocked by this thread
        Queue blocked;
};

void dispatch();
void tick();

#endif