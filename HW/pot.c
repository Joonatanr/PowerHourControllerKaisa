/*
 * pot.c
 *
 *  Created on: 16. dets 2019
 *      Author: JRE
 */

#include "pot.h"
#include "driverlib.h"

/* We control the potentiometer and indicator leds in this module. */



#define HYSTERESIS_VALUE 50u
/* POT is connected to output 4.1 or A12 */

/* POT_LED1 -> 4.6
 * POT_LED2 -> 3.3
 * POT_LED3 -> 3.2
 * POT_LED4 -> 6.0
 *
 */

typedef struct
{
    U32 upper_range;
    U32 lower_range;
} pot_range_T;

#define NUMBER_OF_POT_RANGES 4

/* ADC value is 14 bit, so between 0 and 16384 , 4096 per quadrant*/
Private const pot_range_T priv_pot_ranges[NUMBER_OF_POT_RANGES] =
{
     { .lower_range = 12288u + HYSTERESIS_VALUE,  .upper_range = 0x4000u                     },
     { .lower_range = 8192u  + HYSTERESIS_VALUE,  .upper_range = 12287u - HYSTERESIS_VALUE   },
     { .lower_range = 4096u  + HYSTERESIS_VALUE,  .upper_range = 8193u  - HYSTERESIS_VALUE   },
     { .lower_range = 0u,                         .upper_range = 4095u  - HYSTERESIS_VALUE   }
};

static volatile uint16_t curADCResult = 0u;
//static volatile float normalizedADCRes;

Private void setPotLeds(int range);
Private void setPotLed(int id, Boolean state);
Private int currentRange = 0;


Public void pot_init(void)
{
    /* Enable the LED pins as general purpose inouts. */
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN6);
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN3);
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN2);
    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN0);

    /* Enable the FPU for floating point operation. */
    FPU_enableModule();
    FPU_enableLazyStacking();

    /* Initializing ADC (MCLK/1/4) */
    ADC14_enableModule();
    ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_4, 0);

    /* Configuring GPIOs (4.1 A12) */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN1,
    GPIO_TERTIARY_MODULE_FUNCTION); /* NOTE!, IF DOESNT WORK, THEN TRY OTHER PRIMARY AND SECONDARY. */

    /* Configuring ADC Memory */
    MAP_ADC14_configureSingleSampleMode(ADC_MEM12, true);
    MAP_ADC14_configureConversionMemory(ADC_MEM12, ADC_VREFPOS_AVCC_VREFNEG_VSS,
    ADC_INPUT_A12, false);

    /* Configuring Sample Timer */
    MAP_ADC14_enableSampleTimer(ADC_MANUAL_ITERATION);

    /* Enabling/Toggling Conversion */
    MAP_ADC14_enableConversion();
    //MAP_ADC14_toggleConversionTrigger();

    /* Enabling interrupts */
    MAP_ADC14_enableInterrupt(ADC_INT12);
    MAP_Interrupt_enableInterrupt(INT_ADC14);
    MAP_Interrupt_enableMaster();
}


Public void pot_cyclic_10ms(void)
{
    float adc_value = curADCResult;
    int measured_range = -1;
    U8 ix;

    for (ix = 0u; ix < NUMBER_OF_POT_RANGES; ix++)
    {
        if (adc_value >= priv_pot_ranges[ix].lower_range && adc_value <= priv_pot_ranges[ix].upper_range)
        {
            measured_range = ix;
            break;
        }
    }

    if (measured_range != -1)
    {
        currentRange = measured_range;
    }

    setPotLeds(currentRange);

    MAP_ADC14_toggleConversionTrigger();
}


Public int pot_getSelectedRange()
{
    return currentRange;
}


/* ADC Interrupt Handler. This handler is called whenever there is a conversion
 * that is finished for ADC_MEM0.
 */
void ADC14_IRQHandler(void)
{
    uint64_t status = ADC14_getEnabledInterruptStatus();
    ADC14_clearInterruptFlag(status);

    if (ADC_INT12 & status)
    {
        curADCResult = MAP_ADC14_getResult(ADC_MEM12);
    }
}


Private void setPotLeds(int range)
{
    switch(range)
    {
        case 0:
            setPotLed(0, TRUE);
            setPotLed(1, FALSE);
            setPotLed(2, FALSE);
            setPotLed(3, FALSE);
            break;
        case 1:
            setPotLed(0, TRUE);
            setPotLed(1, TRUE);
            setPotLed(2, FALSE);
            setPotLed(3, FALSE);
            break;
        case 2:
            setPotLed(0, TRUE);
            setPotLed(1, TRUE);
            setPotLed(2, TRUE);
            setPotLed(3, FALSE);
            break;
        case 3:
            setPotLed(0, TRUE);
            setPotLed(1, TRUE);
            setPotLed(2, TRUE);
            setPotLed(3, TRUE);
            break;
        default:
            break;
    }
}


Private void setPotLed(int id, Boolean state)
{
    uint32_t port;
    uint32_t pins;


    switch(id)
    {
        case 0u:
            port = GPIO_PORT_P4;
            pins = GPIO_PIN6;
            break;
        case 3u:
            port = GPIO_PORT_P3;
            pins = GPIO_PIN3;
            break;
        case 1u:
            port = GPIO_PORT_P3;
            pins = GPIO_PIN2;
            break;
        case 2u:
            port = GPIO_PORT_P6;
            pins = GPIO_PIN0;
            break;
        default:
         /* Should not happen. */
         return;
    }

    if (state)
    {
        GPIO_setOutputHighOnPin(port, pins);
    }
    else
    {
        GPIO_setOutputLowOnPin(port, pins);
    }
}




