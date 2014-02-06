/*
 * Helper.cpp
 *
 *  Created on: Apr 10, 2013
 *      Author: kazu
 */

#include "Helper.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <sys/time.h>

#include <pthread.h>

void bindCurrentThreadToCore(int coreNr) {

	// whole process
	cpu_set_t my_set;
	CPU_ZERO(&my_set);
	CPU_SET(coreNr, &my_set);
	//sched_setaffinity(0, sizeof(cpu_set_t), &my_set);

	// one thread only
	//unsigned long mask = 1;
	int ret = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &my_set);
	if (ret != 0) {std::cout << "error while settings thread affinity" << std::endl; exit(1);}

}

uint64_t getUptimeMS() {
	static uint64_t started = getTimeStampMS();
	return getTimeStampMS() - started;
}

uint64_t getTimeStampMS() {
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	return tv.tv_sec * 1000UL + tv.tv_usec / 1000;
}

uint64_t getTimeStampUS() {
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	return tv.tv_sec * 1000UL * 1000UL + tv.tv_usec;
}


std::string getTimeFormatted(uint64_t milliseconds) {
	static std::stringstream ss;
	ss.str("");
	ss.clear();
	milliseconds /= 1000;
	ss << std::setfill('0') << std::setw(2) << (milliseconds / 3600) << ":";
	ss << std::setfill('0') << std::setw(2) << (milliseconds / 60 % 60) << ":";
	ss << std::setfill('0') << std::setw(2) << (milliseconds % 60);
	return ss.str();
}

void printHex(const unsigned char* str, int len) {
	for (int i = 0; i < len; ++i) {
		printf("%02x ", *str);
		++str;
	}
	printf("\n");
}

void dumpHex(std::ostream& target, const unsigned char* str, int len) {

	int i;
	unsigned char buff[17] = {0};
	const unsigned char* pc = str;

	target << std::setfill('0');

	// Process every byte in the data.
	for (i = 0; i < len; i++) {
		// Multiple of 16 means new line (with line offset).

		if ((i % 16) == 0) {
			// Just don't print ASCII for the zeroth line.
			if (i != 0)
				target << "  " << buff << std::endl;

			// Output the offset.
			target << std::setw(4) << std::hex << i << "  ";
		}

		// Now the hex code for the specific character.
		target << std::setw(2) << std::hex << (int) pc[i] << " ";

		// And store a printable ASCII character for later.
		if ((pc[i] < 0x20) || (pc[i] > 0x7e))
			buff[i % 16] = '.';
		else
			buff[i % 16] = pc[i];
		buff[(i % 16) + 1] = '\0';
	}

	// Pad out last line if not exactly 16 characters.
	while ((i % 16) != 0) {
		target << "   ";
		i++;
	}

	// And print the final ASCII bit.
	target << "  " << buff << std::endl;

	// reset output formats
	target << std::setfill(' ') << std::dec;

}

void dumpBin(std::ostream& target, const unsigned char* str, int len) {
	for (int i = 0; i < len; ++i) {

		for (int j = 0; j < 8; ++j) {
			int val = (*str<<j) & 0b10000000;
			target << ( (val) ? ('1') : ('0') );
		}
		if (i % 4 == 3) {printf("   ");}
		if (i % 8 == 7) {printf("\n");}
		++str;
	}
	printf("\n\n");
}

void dumpBin(std::ostream& target, uint32_t bitCode, uint8_t numBits) {
	for (uint32_t i = 0; i < numBits; ++i) {
		target << (int) ( bitCode & 1);
		bitCode >>= 1;
	}
	std::cout << std::endl;
}


void dumpAscii(std::ostream& target, const unsigned char* str, int len) {
	for (int i = 0; i < len; ++i) {
		target << *str;
		++str;
	}
	target << "-----------------------------" << std::endl;
}




std::string intToString(long val) {
	static std::stringstream ss;
	ss.str("");
	ss.clear();
	// TODO: This is here to get the comma-delimiter for large numbers.
	//       Find a fix for systems without this specific locale installed.
	ss.imbue(std::locale(""));		// use default locale. still bad..
	ss << val;
	return ss.str();
}

std::string floatToString(float val, uint32_t decimals) {
	static std::stringstream ss;
	ss.str("");
	ss.clear();
	ss << std::setiosflags(std::ios::fixed);
	ss << std::setprecision(decimals);
	ss << val;
	return ss.str();
}


std::string getDateString() {
	static std::stringstream ss;
	ss.str("");
	ss.clear();
	time_t t = time(0);
	struct tm * now = localtime( & t );
	ss << std::setfill('0');
	ss << (now->tm_year + 1900) << '-'
			<< std::setw(2) << (now->tm_mon + 1) << '-'
			<< std::setw(2) << (now->tm_mday);
	return ss.str();
}

std::string getTimeString() {
	static std::stringstream ss;
	ss.str("");
	ss.clear();
	time_t t = time(0);
	struct tm * now = localtime( & t );
	ss << std::setfill('0');
	ss << std::setw(2) << (now->tm_hour)
			<< std::setw(2) << (now->tm_min)
			<< std::setw(2) << (now->tm_sec);
	return ss.str();
}

std::string getDateTimeString() {
	return getDateString() + "_" + getTimeString();
}

