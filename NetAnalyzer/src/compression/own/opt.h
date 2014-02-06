/*
 * opt.h
 *
 *  Created on: May 12, 2013
 *      Author: kazu
 */

#ifndef OPT_H_
#define OPT_H_


#define likely(arg)		__builtin_expect( (arg), 1)
#define unlikely(arg)	__builtin_expect( (arg), 0)


#endif /* OPT_H_ */
