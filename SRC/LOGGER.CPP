#include <stdio.h>
#include <stdarg.h>

#include "logger.h"
#include "section.h"

char * Logger::LogFormat = "[%s]:%s";

unsigned const int Logger::K_MAX_LOG_LENGTH = 255;

FILE* Logger::FileHandle = NULL;

void Logger::Start() {
    LOCK();
    Logger::FileHandle = fopen("KERNEL.LOG", "w");
    UNLOCK();
}

void Logger::Shutdown() {
    fclose(Logger::FileHandle);
}

void Logger::Log(Output output, const char * format, ...) {
#ifdef DEBUG
    LOCK()
    va_list args;
    va_start(args, format);

    char message[Logger::K_MAX_LOG_LENGTH];
    vsprintf(message, format, args);

    switch(output)
    {
        case Logger::Console:
            Logger::LogConsole(message);
        break;
        case Logger::Message:
            Logger::LogMessage(message);
        break;
    }

    va_end(args);
    UNLOCK()
#endif
}

FILE* Logger::GetFileHandle() {
    return Logger::FileHandle;
}

void Logger::LogConsole(const char* output) {
#ifdef DEBUG
    // TODO: Guard this to write to console
    LOCK()
    puts(output);
    UNLOCK()
#endif
}

void Logger::LogMessage(const char* output) {
#ifdef DEBUG
    LOCK()
    FILE* file = Logger::GetFileHandle();
    if(file) {
        fprintf(file, output);
        fprintf(file, "\n");

        fflush(file);
    }

    UNLOCK()
#endif
}