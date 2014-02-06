/*
 * Debug.cpp
 *
 *  Created on: Apr 12, 2013
 *      Author: kazu
 */


#include "Debug.h"
#include <signal.h>

int dbgLevel = 0;

#ifdef WITH_DEBUG

void breakpoint() {
	raise(SIGINT);
}



#endif
