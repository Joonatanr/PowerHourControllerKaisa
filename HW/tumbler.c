/*
 * tumbler.c
 *
 *  Created on: Dec 17, 2020
 *      Author: Joona
 */

/* Contains access functions for all the configured tumbler units in the block. */


#include "tumbler.h"
#include <driverlib.h>

typedef struct
{
    const U8            port;
    const U8            pin;

} TumblerConf;


Private TumblerConf priv_conf[NUMBER_OF_TUMBLERS] =
{
     {  .port = GPIO_PORT_P3, .pin = GPIO_PIN7 },      /* TUMBLER_0      */
     {  .port = GPIO_PORT_P3, .pin = GPIO_PIN5 },      /* TUMBLER_1      */
     {  .port = GPIO_PORT_P5, .pin = GPIO_PIN1 },      /* TUMBLER_2      */
     {  .port = GPIO_PORT_P2, .pin = GPIO_PIN3 },      /* TUMBLER_3      */
     {  .port = GPIO_PORT_P6, .pin = GPIO_PIN7 },      /* TUMBLER_4      */
     {  .port = GPIO_PORT_P6, .pin = GPIO_PIN6 },      /* TUMBLER_5      */
     {  .port = GPIO_PORT_P2, .pin = GPIO_PIN4 },      /* TUMBLER_6      */
     {  .port = GPIO_PORT_P2, .pin = GPIO_PIN6 },      /* TUMBLER_7      */
};

Public void tumbler_init(void)
{
    U8 ix;

    for (ix = 0u; ix < NUMBER_OF_TUMBLERS; ix++)
    {
        GPIO_setAsInputPin(priv_conf[ix].port, priv_conf[ix].pin);
    }
}


Public Boolean tumbler_getState(Tumbler_T t)
{
    Boolean res = FALSE;
    if (t < NUMBER_OF_TUMBLERS)
    {
        if (GPIO_getInputPinValue(priv_conf[t].port, priv_conf[t].pin))
        {
            res = FALSE;
        }
        else
        {
            res = TRUE;
        }
    }
    return res;
}


