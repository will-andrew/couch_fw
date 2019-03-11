/* 
 * File:   util.h
 * Author: will
 *
 * Created on 16 August 2017, 10:16 PM
 */

#ifndef UTIL_H
#define	UTIL_H

#define BIT(x) (1UL << (x))
#define MASK(x) (~BIT(x))
#define ABS(x) ((x) < 0 ? -(x) : (x))
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#define STATIC_ASSERT(COND, MSG) typedef char static_assertion_##MSG[(!!(COND))*2-1]

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define LIMIT(x, low, high) if (x < (low)) {x = (low); } else if (x > (high)) {x = (high); }

#define ADD_4_SAMPLES(x) (x[0] + x[1] + x[2] + x[3])

#endif	/* UTIL_H */

