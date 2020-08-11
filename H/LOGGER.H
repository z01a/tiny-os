#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <stdio.h>

#include "types.h"

class Logger {
    public:
        enum Output {
            Console,
            Message
        };

        static void Start();
        static void Shutdown();

        static void Log(Output output, const char* format, ...);

        static Output GetLogOutput();

        static FILE* GetFileHandle();

    private:
        static const unsigned int K_MAX_LOG_LENGTH;

        static char * LogFormat;
        static FILE* FileHandle;

        static void LogConsole(const char* output);
        static void LogMessage(const char* output);
};

#endif