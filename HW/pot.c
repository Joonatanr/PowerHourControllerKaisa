/*
 * pot.c
 *
 *  Created on: 16. dets 2019
 *      Author: JRE
 */

#include "pot.h"
#include "driverlib.h"

/* We control the potentiometer and indicator leds in this module. */

/* Private type definitions */
typedef struct
{
    U32 upper_range;
    U32 lower_range;
} pot_range_T;

typedef struct
{
    uint8_t port;
    uint16_t pin;
} gpioConf_T;

typedef struct
{
    gpioConf_T led1;
    gpioConf_T led2;
    gpioConf_T led3;
    gpioConf_T led4;
} indicatorLedConf_T;

typedef struct
{
    /* GPIO */
    gpioConf_T input;
    indicatorLedConf_T leds;

    /* ADC */
    uint32_t adc_mem;
    uint32_t adc_ch;

    /* interrupt vector */
    uint64_t intvec;
} pot_conf_T;

/* Private const definitions */

Private const pot_conf_T priv_conf[NUMBER_OF_DEFINED_POTENTIOMETERS] =
{
 /* POTENTIOMETER_ONE -> port 4.1, A12*/
 {
      .input = {.port = GPIO_PORT_P4, .pin = GPIO_PIN1 },
      .leds =
      {
        .led1 = {.port = GPIO_PORT_P4, .pin = GPIO_PIN6 },
        .led2 = {.port = GPIO_PORT_P3, .pin = GPIO_PIN3 },
        .led3 = {.port = GPIO_PORT_P3, .pin = GPIO_PIN2 },
        .led4 = {.port = GPIO_PORT_P6, .pin = GPIO_PIN0 },
      },
      .adc_mem = ADC_MEM12,
      .adc_ch = ADC_INPUT_A12,
      .intvec = ADC_INT12,
 },

 /* POTENTIOMETER_TWO -> port 4.0, A13 */
 {
      .input = {.port = GPIO_PORT_P4, .pin = GPIO_PIN0 },
      .leds =
      {
       .led1 = {.port = GPIO_PORT_P8, .pin = GPIO_PIN5 },
       .led2 = {.port = GPIO_PORT_P9, .pin = GPIO_PIN0 },
       .led3 = {.port = GPIO_PORT_P8, .pin = GPIO_PIN4 },
       .led4 = {.port = GPIO_PORT_P9, .pin = GPIO_PIN2 },
      },
      .adc_mem = ADC_MEM13,
      .adc_ch = ADC_INPUT_A13,
      .intvec = ADC_INT13,
 },

 /* POTENTIOMETER_THREE ->  port 4.5, A8    */
 {
      .input = {.port = GPIO_PORT_P4, .pin = GPIO_PIN5 },
      .leds =
      {
       .led1 = {.port = GPIO_PORT_P8, .pin = GPIO_PIN6 },
       .led2 = {.port = GPIO_PORT_P8, .pin = GPIO_PIN7 },
       .led3 = {.port = GPIO_PORT_P9, .pin = GPIO_PIN1 },
       .led4 = {.port = GPIO_PORT_P8, .pin = GPIO_PIN3 },
      },
      .adc_mem = ADC_MEM8,
      .adc_ch = ADC_INPUT_A8,
      .intvec = ADC_INT8,
 },

 /* POTENTIOMETER_FOUR ->   port 4.4, A9    */
 {
      .input = {.port = GPIO_PORT_P4, .pin = GPIO_PIN4 },
      .leds =
      {
       .led1 = {.port = GPIO_PORT_P6, .pin = GPIO_PIN2 },
       .led2 = {.port = GPIO_PORT_P7, .pin = GPIO_PIN3 },
       .led3 = {.port = GPIO_PORT_P7, .pin = GPIO_PIN1 },
       .led4 = {.port = GPIO_PORT_P9, .pin = GPIO_PIN4 },
      },
      .adc_mem = ADC_MEM9,
      .adc_ch = ADC_INPUT_A9,
      .intvec = ADC_INT9,
 },

 /* POTENTIOMETER_FIVE ->   port 4.2, A11   */
 {
      .input = {.port = GPIO_PORT_P4, .pin = GPIO_PIN2 },
      .leds =
      {
       .led1 = {.port = GPIO_PORT_P5, .pin = GPIO_PIN3 },
       .led2 = {.port = GPIO_PORT_P9, .pin = GPIO_PIN3 },
       .led3 = {.port = GPIO_PORT_P6, .pin = GPIO_PIN3 },
       .led4 = {.port = GPIO_PORT_P7, .pin = GPIO_PIN2 },
      },
      .adc_mem = ADC_MEM11,
      .adc_ch = ADC_INPUT_A11,
      .intvec = ADC_INT11,
 },

 /* POTENTIOMETER_SIX  ->   port 6.1, A14   */
 {
      .input = {.port = GPIO_PORT_P6, .pin = GPIO_PIN1 },
      .leds =
      {
       .led1 = {.port = GPIO_PORT_P7, .pin = GPIO_PIN0 },
       .led2 = {.port = GPIO_PORT_P9, .pin = GPIO_PIN5 },
       .led3 = {.port = GPIO_PORT_P9, .pin = GPIO_PIN7 },
       .led4 = {.port = GPIO_PORT_P7, .pin = GPIO_PIN5 },
      },
      .adc_mem = ADC_MEM14,
      .adc_ch = ADC_INPUT_A14,
      .intvec = ADC_INT14,
 },
};

#define HYSTERESIS_VALUE 50u

/* POT is connected to output 4.1 or A12 */

/* POT_LED1 -> 4.6
 * POT_LED2 -> 3.3
 * POT_LED3 -> 3.2
 * POT_LED4 -> 6.0
 *
 */

#define NUMBER_OF_POT_RANGES 4

/* ADC value is 14 bit, so between 0 and 16384 , 4096 per quadrant*/
Private const pot_range_T priv_pot_ranges[NUMBER_OF_POT_RANGES] =
{
     { .lower_range = 12288u + HYSTERESIS_VALUE,  .upper_range = 0x4000u                     },
     { .lower_range = 8192u  + HYSTERESIS_VALUE,  .upper_range = 12287u - HYSTERESIS_VALUE   },
     { .lower_range = 4096u  + HYSTERESIS_VALUE,  .upper_range = 8193u  - HYSTERESIS_VALUE   },
     { .lower_range = 0u,                         .upper_range = 4095u  - HYSTERESIS_VALUE   }
};



/* Private variable declarations.*/
Private volatile uint16_t curADCResult[NUMBER_OF_DEFINED_POTENTIOMETERS] = { 0u };
Private volatile int currentRange[NUMBER_OF_DEFINED_POTENTIOMETERS] = { 0 };


/* Private function forward declarations. */
Private void setPotLeds(potentiometer_T pot, int range);
Private void setPotLed(const gpioConf_T * gpio, Boolean state);


Public void pot_init(void)
{
    const pot_conf_T * pot_conf_ptr;
    U8 ix;

    /* Enable the FPU for floating point operation. */
    FPU_enableModule();
    FPU_enableLazyStacking();

    /* Initializing ADC (MCLK/1/4) */
    ADC14_enableModule();
    ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_4, 0);

    /* Enable the LED pins as general purpose inouts. */
    for (ix = 0u; ix < NUMBER_OF_DEFINED_POTENTIOMETERS; ix++)
    {
        pot_conf_ptr = &priv_conf[ix];
        GPIO_setAsOutputPin(pot_conf_ptr->leds.led1.port, pot_conf_ptr->leds.led1.pin);
        GPIO_setAsOutputPin(pot_conf_ptr->leds.led2.port, pot_conf_ptr->leds.led2.pin);
        GPIO_setAsOutputPin(pot_conf_ptr->leds.led3.port, pot_conf_ptr->leds.led3.pin);
        GPIO_setAsOutputPin(pot_conf_ptr->leds.led4.port, pot_conf_ptr->leds.led4.pin);

        /* Configuring ADC inputs */
        GPIO_setAsPeripheralModuleFunctionInputPin(pot_conf_ptr->input.port, pot_conf_ptr->input.pin,
        GPIO_TERTIARY_MODULE_FUNCTION); /* NOTE!, IF DOESNT WORK, THEN TRY OTHER PRIMARY AND SECONDARY. */

        /* Configuring ADC Memory */

        MAP_ADC14_configureConversionMemory(pot_conf_ptr->adc_mem, ADC_VREFPOS_AVCC_VREFNEG_VSS,
        pot_conf_ptr->adc_ch, false);

        /* Enabling interrupts */
        MAP_ADC14_enableInterrupt(pot_conf_ptr->intvec);
    }

    MAP_ADC14_configureMultiSequenceMode(ADC_MEM8, ADC_MEM14, true);

    /* Configuring Sample Timer */
    MAP_ADC14_enableSampleTimer(ADC_MANUAL_ITERATION);

    /* Enabling/Toggling Conversion */
    MAP_ADC14_enableConversion();

    /* Enabling interrupts */
    MAP_Interrupt_enableInterrupt(INT_ADC14);
    MAP_Interrupt_enableMaster();
}


Public void pot_cyclic_10ms(void)
{
    U8 ix;
    U8 pot_ix;
    U16 adc_value;
    int measured_range;

    for (pot_ix = 0u; pot_ix < NUMBER_OF_DEFINED_POTENTIOMETERS; pot_ix++)
    {
        measured_range = -1;
        adc_value = curADCResult[pot_ix];

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
            currentRange[pot_ix] = measured_range;
        }

        setPotLeds((potentiometer_T)pot_ix, currentRange[pot_ix]);
    }

    MAP_ADC14_toggleConversionTrigger();
}


Public int pot_getSelectedRange(potentiometer_T pot)
{
    if(pot < NUMBER_OF_DEFINED_POTENTIOMETERS)
    {
        return currentRange[pot];
    }
    else
    {
        return -1;
    }
}


/* ADC Interrupt Handler. This handler is called whenever there is a conversion
 * that is finished for ADC_MEM0.
 */
void ADC14_IRQHandler(void)
{
    U8 ix;

    uint64_t status = ADC14_getEnabledInterruptStatus();
    ADC14_clearInterruptFlag(status);

    for (ix = 0u; ix < NUMBER_OF_DEFINED_POTENTIOMETERS; ix++)
    {
        if (priv_conf[ix].intvec & status)
        {
            curADCResult[ix] = MAP_ADC14_getResult(priv_conf[ix].adc_mem);
        }
    }
}


Private void setPotLeds(potentiometer_T pot, int range)
{
    const pot_conf_T * conf_ptr;

    if (pot < NUMBER_OF_DEFINED_POTENTIOMETERS)
    {
        conf_ptr = &priv_conf[pot];


        switch(range)
        {
            case 0:
                setPotLed(&conf_ptr->leds.led1, TRUE);
                setPotLed(&conf_ptr->leds.led2, FALSE);
                setPotLed(&conf_ptr->leds.led3, FALSE);
                setPotLed(&conf_ptr->leds.led4, FALSE);
                break;
            case 1:
                setPotLed(&conf_ptr->leds.led1, TRUE);
                setPotLed(&conf_ptr->leds.led2, TRUE);
                setPotLed(&conf_ptr->leds.led3, FALSE);
                setPotLed(&conf_ptr->leds.led4, FALSE);
                break;
            case 2:
                setPotLed(&conf_ptr->leds.led1, TRUE);
                setPotLed(&conf_ptr->leds.led2, TRUE);
                setPotLed(&conf_ptr->leds.led3, TRUE);
                setPotLed(&conf_ptr->leds.led4, FALSE);
                break;
            case 3:
                setPotLed(&conf_ptr->leds.led1, TRUE);
                setPotLed(&conf_ptr->leds.led2, TRUE);
                setPotLed(&conf_ptr->leds.led3, TRUE);
                setPotLed(&conf_ptr->leds.led4, TRUE);
                break;
            default:
                break;
        }
    }
}


Private void setPotLed(const gpioConf_T * gpio, Boolean state)
{
    if (gpio != NULL)
    {
        if (state)
        {
            GPIO_setOutputHighOnPin(gpio->port, gpio->pin);
        }
        else
        {
            GPIO_setOutputLowOnPin(gpio->port, gpio->pin);
        }
    }
}

