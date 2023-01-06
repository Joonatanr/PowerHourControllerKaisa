/*
 * clockDisplay.h
 *
 *  Created on: Aug 27, 2017
 *      Author: Joonatan
 */

#ifndef LOGIC_POWERHOURGAME_CLOCKDISPLAY_H_
#define LOGIC_POWERHOURGAME_CLOCKDISPLAY_H_

#include "typedefs.h"

typedef struct
{
    U8 minute;
    U8 second;
} timekeeper_struct;

extern void clockDisplay_init(void);
extern void clockDisplay_start(void);
extern void clockDisplay_cyclic1000msec(void);
extern void clockDisplay_stop(void);

extern void clockDisplay_setTaskFrequency(U16 freq);
extern U16 clockDisplay_getTaskFrequency(void);


#endif /* LOGIC_POWERHOURGAME_CLOCKDISPLAY_H_ */
