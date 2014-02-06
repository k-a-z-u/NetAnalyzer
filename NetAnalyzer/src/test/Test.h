/*
 * Test.h
 *
 *  Created on: Apr 21, 2013
 *      Author: kazu
 */

#ifndef TEST_H_
#define TEST_H_

#ifdef WITH_TESTS

/** include test-suite support */

#include <gtest/gtest.h>
#include "../Helper.h"

#define RUN_TESTS() {\
		int argc = 0;\
		::testing::InitGoogleTest(&argc, (char**)nullptr);\
		uint32_t ret = RUN_ALL_TESTS();\
		(void) ret;\
}

#define ASSERT_DATA_EQ(d1, d2, length)	{\
		bool wasOK = (memcmp(d1, d2, length) == 0);\
		if (!wasOK) {\
			std::cout << "expected:" << std::endl;\
			dumpHex(std::cerr, (uint8_t*) d1, length);\
			std::cout << "got:" << std::endl;\
			dumpHex(std::cerr, (uint8_t*) d2, length);\
		}\
		ASSERT_TRUE(wasOK);\
}

#define ASSERT_NOT_NULL(d1)				{ASSERT_TRUE(d1 != nullptr);}
#define ASSERT_NULL(d1)					{ASSERT_TRUE(d1 == nullptr);}


#else

/** without test-suite support */
#define RUN_TESTS() {\
		std::cerr << "test-suite support not compiled!" << std::endl;\
		exit(0);\
}

#endif

#endif /* TEST_H_ */
