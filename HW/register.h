/*
 * register.h
 *
 *  Created on: 28. mai 2017
 *      Author: Joonatan
 */

#ifndef HW_REGISTER_H_
#define HW_REGISTER_H_

#include "typedefs.h"

typedef void (*TimerHandler)(void);

extern TimerHandler timer_10msec_callback;
extern TimerHandler timer_50msec_callback;

extern void register_init(void);
extern void register_enable_low_powermode(void);

extern void set_led_one(U8 state);
extern void set_led_two_red(U8 state);
extern void set_led_two_green(U8 state);
extern void set_led_two_blue(U8 state);

extern void set_reg_select(U8 state);
extern void set_cs_pin(U8 state);
extern void set_disp_reset_pin(U8 state);

extern U8 isBtnOne(void);
extern U8 isBtnTwo(void);

extern void delay_msec(U16 msec);
extern U16 generate_random_number(U16 max);
extern U16 generate_random_number_rng(U16 min, U16 max);

#endif /* HW_REGISTER_H_ */
