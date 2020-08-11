#ifndef _PCB_H_
#define _PCB_H_

#include "types.h"
#include "thread.h"

class ThreadState {
    public:
        enum State {
            NEW,
            RUNABLE,
            RUNNING,
            BLOCKED,
            SLEEPING,
            TERMINATED,
            INVALID = -1
        };

        ThreadState() {
            this->m_State = NEW;
        }

        static void SetState(ThreadState& threadState, ThreadState::State state) {
            threadState.SetState(state);
        }

        static ThreadState::State GetState(ThreadState& threadState) {
            return threadState.GetState();
        }

        static void SetState(Thread* thread, ThreadState::State state) {
            ThreadState::SetState(thread->getState(), state);
        }

        static ThreadState::State GetState(Thread* thread) {
            return ThreadState::GetState(thread->getState());
        }

    private:
        void SetState(ThreadState::State state) {
            this->m_State = state;
        }

        ThreadState::State GetState() {
            return this->m_State;
        }

    private:
        State m_State;
};

class PCB {
    public:
        // Thread stack pointer
        unsigned int sp;

        // Thread stack segment
        unsigned int ss;
        
        // Thread base pointer
        unsigned int bp;

        // Unique Thread Identifier
        ID pid;

        // Time for Thread execution
        Time slice;

        // Current time of execution
        Time timeSlice;

        // Sleeping time
        Time sleepingTime;

        // What Thread owns this PCB
        Thread * owner;

        // Pointer to allocated stack
        unsigned int * stack;

        // State of a Thread
        ThreadState state;

        // Constructor for PCB with Stack
        PCB(StackSize, Time, Thread *);

        // Constructor for PCB with out Stack
        PCB(Time, Thread *);

        // Entry point for all Threads
        static void Wrapper();

        ID getId();
        ThreadState& getState();
        void start();
        void waitToComplete();
        void sleep(Time timeToSleep);
        void wakeUp();
        
        Time getSleepingTime();
        Time getSlice();
        Time getTimeSlice();

        // PCB Destructor
        ~PCB();
};

#endif