/*
 * contains some helper methods like timestamps, etc
 *
 * Helper.h
 *
 *  Created on: Apr 8, 2013
 *      Author: kazu
 */

#ifndef HELPER_H_
#define HELPER_H_

#include <string>
#include <memory>
#include <utility>
#include <cstdint>

struct mallocStats {
	size_t allocated;
	size_t used;
};

/** set the affinity of the current thread to one core */
void bindCurrentThreadToCore(int coreNr);

/** get the number of milliseconds the tool runs */
uint64_t getUptimeMS();

/** get a current timestamp in milliseconds */
uint64_t getTimeStampMS();

/** get a current timestamp in microseconds */
uint64_t getTimeStampUS();

/** format the provided milliseconds to hh:mm:ss */
std::string getTimeFormatted(uint64_t milliseconds);


void printHex(const unsigned char* str, int len);


/** get the current date as string */
std::string getDateString();

/** get the current time as string */
std::string getTimeString();

/** get current date and time as string */
std::string getDateTimeString();


/** dump content in hex-format */
void dumpHex(std::ostream& target, const unsigned char* str, int len);

/** dump content as ascii text */
void dumpAscii(std::ostream& target, const unsigned char* str, int len);

/** dump content in binary format */
void dumpBin(std::ostream& target, const unsigned char* str, int len);

/** dump the given bits */
void dumpBin(std::ostream& target, uint32_t bitCode, uint8_t numBits);



/** convert int to string */
std::string intToString(long val);

/** convert float to string */
std::string floatToString(float val, uint32_t decimals);


// The C++11 standard forgot to include std::make_unique (see std::make_shared)
//  for std::unique_ptr initialization. Let's just add it outself for the time being.
//  Source:
//  http://herbsutter.com/gotw/_102/
//  http://stackoverflow.com/questions/10149840/c-arrays-and-make-unique
template<typename T, typename ...Args>
typename std::enable_if<!std::is_array<T>::value, std::unique_ptr<T>>::type
make_unique(Args&& ...args)
{
   return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template<typename T>
typename std::enable_if<std::is_array<T>::value, std::unique_ptr<T>>::type
make_unique(std::size_t n)
{
   typedef typename std::remove_extent<T>::type RT;
   return std::unique_ptr<T>(new RT[n]);
}

#endif /* HELPER_H_ */
