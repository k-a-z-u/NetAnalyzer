#ifndef DEBUG_H_
#define DEBUG_H_


// FIXME
// i don't like this at all.. a .cpp for only one variable (dbgLevel)
// and debuging s a header file... to many includes ...
// but it allows easy str-concats like debug("id is:" << id);



#define DBG_LVL_INFO	5
#define DBG_LVL_DEBUG	3
#define DBG_LVL_WARN	2
#define DBG_LVL_ERR		1


extern int dbgLevel;

#include <iomanip>
#include <iostream>
#include <sstream>

#define error(src, msg) {\
		std::cerr << std::left\
		<< "[ERR!]"\
		<< "[" << std::setw(8) << src << "]: "\
		<< msg << std::endl;\
		breakpoint();\
		exit(0);\
}



#ifdef WITH_DEBUG

#include <string>
#include <cstdlib>

void breakpoint();

const std::string DBG_NAMES[] = {
		"???",
		"ERR!",
		"WARN",
		"DBG",
		"???",
		"INFO",
};

#define debug(src, lvl, msg)\
		if (dbgLevel >= lvl) {\
			std::cerr << std::left\
			<< "[" << std::setw(4) << DBG_NAMES[lvl] << "]"\
			<< "[" << std::setw(8) << src << "]: "\
			<< msg << std::endl;\
		}

struct AssertException : public std::exception {
	std::string type;
	std::string msg;
	AssertException(std::string type, std::string msg) : type(type), msg(msg) {}
	const char* what() const throw() { return (type + ": " + msg).c_str(); }
};

#define assertNotNull(val, msg) {\
		if (val == nullptr) {\
			std::stringstream ss;\
			ss << msg;\
			throw AssertException("NotNull", ss.str());\
		}\
}

#define assertTrue(val, msg) {\
		if (!(val))			{\
			std::stringstream ss;\
			ss << msg;\
			throw AssertException("True", ss.str());\
		}\
}

#define assertEQ(val1, val2, msg) {\
		if (val1 != val2)	{\
			std::stringstream ss;\
			ss << msg;\
			throw AssertException("EQ", ss.str());\
		}\
}

/** assert that the given string is all lower-case */
#define assertLowerCase(val, msg) {\
		for (size_t i = 0; i < val.length(); ++i) {\
			if (std::isupper(val[i])) {\
				std::stringstream ss;\
				ss << msg;\
				throw AssertException("LowerCase", ss.str());\
			}\
		}\
}

#else

#define debug(src, lvl, msg)		;

#define assertNotNull(val, msg)		;
#define assertTrue(val, msg)		;
#define assertLowerCase(str, msg)	;
#define assertEQ(val1, val2, msg)	(void) val1; (void) val2;

#define breakpoint()				;

#endif


#endif /* DEBUG_H_ */
