/*
 * tumbler.h
 *
 *  Created on: Dec 17, 2020
 *      Author: Joona
 */

#ifndef HW_TUMBLER_H_
#define HW_TUMBLER_H_

#include "typedefs.h"

typedef enum
{
    TUMBLER_0,
    TUMBLER_1,
    TUMBLER_2,
    TUMBLER_3,
    TUMBLER_4,
    TUMBLER_5,
    TUMBLER_6,
    TUMBLER_7,
    NUMBER_OF_TUMBLERS
} Tumbler_T;

#define TUMBLER_UPPER_0 TUMBLER_1
#define TUMBLER_UPPER_1 TUMBLER_0
#define TUMBLER_UPPER_2 TUMBLER_2
#define TUMBLER_UPPER_3 TUMBLER_6
#define TUMBLER_LOWER_0 TUMBLER_3
#define TUMBLER_LOWER_1 TUMBLER_4
#define TUMBLER_LOWER_2 TUMBLER_5
#define TUMBLER_LOWER_3 TUMBLER_7



extern void tumbler_init(void);
extern Boolean tumbler_getState(Tumbler_T t);


#endif /* HW_TUMBLER_H_ */
