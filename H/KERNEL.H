#ifndef _KERNEL_H_
#define _KERNEL_H_

#include <dos.h>

#include "types.h"
#include "thread.h"
#include "system.h"
#include "logger.h"
#include "queue.h"

class Kernel {
    public:
        static Kernel * GetInstance();
        
        static Thread * RunningThread;

        static Thread * ThreadMain;
        static Thread * ThreadIdle;

        static ThreadQueue * ActiveThreads;
        static SleepQueue * SleepingThreads;

        static volatile int Lock;
        static volatile int RequestedDispatch;

        static volatile ID NextThreadId;

        static volatile long long int Time;

        /*
            This function should prepare Kernel for working
            in multithreading environment. Prepare MainThread, IdleThread
            and initialize all interrupt handlers in system.
        */
        void Initialize();

        /*
            Called when dipatch is requested from Kernel by timer.
        */
        static void Dispatch();

        /*
            Kernel lock primitives, for locking kernel preemption.
        */
        static void AcquireLock();

        static int IsLocked();

        static void ReleaseLock();

        /*
            Return all to normal. As nothing really happen to the system.
            Deallocate taken space on heap and restore all interrupt handlers.
        */
        void UnInitialize();

        ~Kernel();
    private:
        static Kernel * Instance;

        // Next created thread will have this ID
        ID nextId;

        Kernel();
};

class IdleThread : public Thread {
    public:
        IdleThread() : Thread(defaultStackSize, defaultTimeSlice) {
            // Empty
        }

        void run() {
            Logger::Log(Logger::Message, "[IdleThread::run] : Hello from IdleThread!");
            volatile long int i = 1;
            while(i) {
                Syscall::Syscall_Dispatch();
            }
        }
};

class MainThread : public Thread {
    public:
        MainThread() : Thread(defaultTimeSlice) {
            // Empty
        }

        void run() {
            // Empty
        }
};

int userMain(int argc, char ** argv);

class MainUserThread : public Thread {
    private:
        int m_argc;
        char ** m_argv;

        static int Result;
    public:
        MainUserThread(int argc, char ** argv) : Thread(defaultStackSize, defaultTimeSlice) {
            this->m_argc = argc;
            this->m_argv = argv;
        }

        void run() {
            MainUserThread::Result = userMain(m_argc, m_argv);
        }

        static int GetThreadResult() {
            return MainUserThread::Result;
        }

        ~MainUserThread() { waitToComplete(); }
};

#endif