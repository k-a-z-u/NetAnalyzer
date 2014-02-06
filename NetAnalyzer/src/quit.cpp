/*
 * quit.cpp
 *
 *  Created on: Apr 29, 2013
 *      Author: kazu
 */




#include <csignal>

void quit() {
	raise(SIGINT);
}
