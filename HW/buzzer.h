/*
 * buzzer.h
 *
 *  Created on: Sep 7, 2017
 *      Author: Joonatan
 */

#ifndef HW_BUZZER_H_
#define HW_BUZZER_H_

#include "typedefs.h"


extern void buzzer_init(void);
extern void buzzer_setBuzzer(Boolean state);
extern void buzzer_playBuzzer(U8 sec);
extern void buzzer_playBeeps(U8 beeps);
extern void buzzer_cyclic100msec(void);

#endif /* HW_BUZZER_H_ */
