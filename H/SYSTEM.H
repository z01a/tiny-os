#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#define INT(number)  asm { int number }

typedef void interrupt (*SyscallHandle)(...);

class Syscall {
    public:
        enum Service {
            OldTimer = 0x55,
            Timer = 0x8,
            Dispatch = 0x80,
            Count = 3,
            Invalid = -1
        };

        static void Register(int number, SyscallHandle handle);
        static void Trigger(int number);
        static SyscallHandle UnRegister(int number);

        // Interrupt Handles
        static void interrupt Syscall_DummyHandle(...);
        static void interrupt Syscall_Timer(...);
        static void interrupt Syscall_Dispatch(...);
};

#endif