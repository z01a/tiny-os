#ifndef _SECTION_H_
#define _SECTION_H_

#include "kernel.h"

// Very bad lock/unlock, but it works!
#define LOCK() asm { cli; };
#define UNLOCK() asm { sti; };

#endif