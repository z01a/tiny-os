#ifndef _SEMAPHOR_H_
#define _SEMAPHOR_H_

#include "types.h"
#include "queue.h"

class KernelSemaphore {
    public:
        KernelSemaphore(int val);
        int wait(Time maxTimeToWait);
        int signal(int n);
        int val() const;
        ~KernelSemaphore();
    private:
        void block();
        void deblock();

        int value;
        Queue blocked;
};

class Semaphore {
    public:
        Semaphore(int val = 1) {
            semaphoreImp = new KernelSemaphore(val);
        }
        virtual ~Semaphore() {
            delete semaphoreImp;

            semaphoreImp = NULL;
        }

        virtual int wait(Time maxTimeToWait) {
            return semaphoreImp->wait(maxTimeToWait);
        }
        
        virtual int signal(int n = 0) {
            return semaphoreImp->signal(n);
        }

        int val() const {
            return semaphoreImp->val();
        }

    private:
        KernelSemaphore* semaphoreImp;
};

#endif