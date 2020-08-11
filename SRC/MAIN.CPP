#include <iostream.h>
#include <stdlib.h>
#include <dos.h>

#include "kernel.h"
#include "system.h"
#include "section.h"
#include "logger.h"
#include "types.h"
#include "scheduler.h"
#include "queue.h"
#include "pcb.h"
#include "ivtentry.h"
#include "event.h"

extern Kernel * kernel;

extern Thread * RunningThread;

int main(int argc, char ** argv) {

#ifdef DEBUG
	Logger::Start();
#endif

	kernel->Initialize();

	Thread* mainUserThread = new MainUserThread(argc, argv);
	mainUserThread->start();

	delete mainUserThread;
	
	Logger::Log(Logger::Message, "[Kernel::main] - MainUserThread exited with code %d", MainUserThread::GetThreadResult());
	
	kernel->UnInitialize();

#ifdef DEBUG
	Logger::Shutdown();
#endif

	return MainUserThread::GetThreadResult();
}