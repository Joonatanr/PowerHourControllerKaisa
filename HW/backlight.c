/*
 * backlight.c
 *
 *  Created on: Oct 12, 2017
 *      Author: Joonatan
 */

#include "backlight.h"
#include <driverlib.h>

/* Timer_A PWM Configuration Parameter */
Private Timer_A_PWMConfig pwmConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK, //SMCLK has frequency of 12MHz
        TIMER_A_CLOCKSOURCE_DIVIDER_16,
        1000,
        TIMER_A_CAPTURECOMPARE_REGISTER_1,
        TIMER_A_OUTPUTMODE_RESET_SET,
        0
};


Public void backlight_init(void)
{
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P5, GPIO_PIN6,
            GPIO_PRIMARY_MODULE_FUNCTION);

    MAP_Timer_A_generatePWM(TIMER_A2_BASE, &pwmConfig);
}


/* Level is between 0 and 100 percent. */
Public void backlight_set_level(U16 level)
{
   if (level > 100u)
   {
      pwmConfig.dutyCycle = 1000u;
   }
   else
   {
      pwmConfig.dutyCycle = level * 10u;
   }

   MAP_Timer_A_generatePWM(TIMER_A2_BASE, &pwmConfig);
}


