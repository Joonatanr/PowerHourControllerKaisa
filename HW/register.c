/*
 * register.c
 *
 *  Created on: 28. mai 2017
 *      Author: Joonatan
 */

#include "register.h"
#include <driverlib.h>
#include <spi_drv.h>
#include "buzzer.h"
#include "buttons.h"
#include "backlight.h"
#include "speaker.h"
#include "pot.h"
#include <stdlib.h>
#include "tumbler.h"

//Hi priority timer runs at 10msec interval (might need to be faster)
Private const Timer_A_UpModeConfig hi_prio_timer_config =
{
     .captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE, /* We enable capture compare, since this is a periodic timer. */
     .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
     .clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_16, //Currently divided by 16.
     .timerClear = TIMER_A_DO_CLEAR,
     .timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE, //Disable general interrupt.
     .timerPeriod = 7500u
};


/* TODO : Move this to 32-bit timer so that we can free another timer to use for SD-card. */
//Lo priority timer runs at 50msec interval
Private const Timer_A_UpModeConfig lo_prio_timer_config =
{
     .captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE, /* We enable capture compare, since this is a periodic timer. */
     .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
     .clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_64, //Currently divided by 64.
     .timerClear = TIMER_A_DO_CLEAR,
     .timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE, //Disable general interrupt.
     .timerPeriod = 9375u //Configured for 50 msec interval.
};


/*****************************************************************************************************
 *
 * Function Prototypes
 *
 *****************************************************************************************************/

Private void clocks_init(void);
Private void ports_init(void);
Private void TA0_0_IRQHandler(void);
Private void TA1_0_IRQHandler(void);
Private void timerA_init(void);

/*****************************************************************************************************
 *
 * Private variables.
 *
 *****************************************************************************************************/

Public volatile U16 priv_delay_counter = 0u;

/*****************************************************************************************************
 *
 * Public Functions
 *
 *****************************************************************************************************/

Public void register_init(void)
{
    //Initialise system clocks first.
    clocks_init();

    //Initialise input/output pins.
    ports_init();

    //Initialise main timer.
    timerA_init();

    //Initialise SPI for LCD display.
    spi_init();

    //Initialise display backlight.
    backlight_init();

    //Initialize speaker.
    speaker_init();

    //Initialize adc and potentiometer handler.
    pot_init();

    //Initialize switch block.
    tumbler_init();

    //Not quite sure what this does yet.
    //MAP_Interrupt_enableSleepOnIsrExit();

    //Enable interrupts in general.
    Interrupt_enableMaster();

}

Public void register_enable_low_powermode(void)
{
    //Go to low power mode with interrupts.
    while(1)
    {
        PCM_gotoLPM0();
    }
}

#pragma FUNCTION_OPTIONS(delay_msec, "--opt_level=off")
Public void delay_msec(U16 msec)
{
    priv_delay_counter = msec / 10;
    while(priv_delay_counter > 0u);
}


Public U16 generate_random_number_rng(U16 min, U16 max)
{
    U16 range = max - min;
    U16 res = 0u;

    if (range > 0u)
    {
        res = generate_random_number(range);
        res += min;
    }

    return res;
}


Public U16 generate_random_number(U16 max)
{
    static U16 priv_seed = 0u;
    U16 res;
    if (priv_seed == 0u)
    {
        priv_seed = TA0R ^ TA1R;
        /* We initialize the pseudo random number generator. */
        srand(priv_seed);
    }

    /* Max might actually be legitimately 0 in some calculations. */
    if (max == 0u)
    {
        res = 0u;
    }
    else
    {
        res =  rand() % (max + 1u);
    }

    return res;
}

/*****************************************************************************************************
 *
 * Private Functions
 *
 *****************************************************************************************************/

Private void clocks_init(void)
{
    WDT_A_holdTimer();

    /* This should be set in case of higher frequency. */
    //MAP_PCM_setCoreVoltageLevel(PCM_VCORE1);

    /* Set 2 flash wait states for Flash bank 0 and 1, also required for 48MHz */
    //MAP_FlashCtl_setWaitState(FLASH_BANK0, 2);
    //MAP_FlashCtl_setWaitState(FLASH_BANK1, 2);

    //Lets configure the DCO to 12MHz
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);

    /* Initializing the clock source as follows:
     *      MCLK = MODOSC/2 = 12MHz
     *      ACLK = REFO/2 = 16kHz   --- TODO Not used, should remove.
     *      HSMCLK = DCO/2 = 6Mhz
     *      SMCLK = DCO =  12MHz
     *      BCLK  = REFO = 32kHz    --- TODO Not used, should remove.
     */
    MAP_CS_initClockSignal(CS_MCLK,     CS_MODOSC_SELECT,   CS_CLOCK_DIVIDER_2);
    MAP_CS_initClockSignal(CS_ACLK,     CS_REFOCLK_SELECT,  CS_CLOCK_DIVIDER_2);
    MAP_CS_initClockSignal(CS_HSMCLK,   CS_DCOCLK_SELECT,   CS_CLOCK_DIVIDER_2);
    MAP_CS_initClockSignal(CS_SMCLK,    CS_DCOCLK_SELECT,   CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_BCLK,     CS_REFOCLK_SELECT,  CS_CLOCK_DIVIDER_1);
}

Private void ports_init(void)
{
    //First lets set up LED ports as outputs.
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);

    //Lets set up S1 and S2 as input pins.
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4);

    // Set pin 4.3 as CS pin.
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN3);

    // Set pin 1.7 as A0 pin.
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN7);

    // Set pin 2.5 as reset pin.
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN5);

    //Clear other LEDs.
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);

    /* Selecting P1.2 and P1.3 in UART mode */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
            GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Set up reset pin in output mode. */
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN6);

    /* Set up green safety button. */
    GPIO_setAsInputPin(GPIO_PORT_P3, GPIO_PIN0);
}

Private void timerA_init(void)
{
    //Set up timer for high priority interrupts.
    Timer_A_configureUpMode(TIMER_A0_BASE, &hi_prio_timer_config);
    Timer_A_registerInterrupt(TIMER_A0_BASE, TIMER_A_CCR0_INTERRUPT, TA0_0_IRQHandler);
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);

    //Enable this interrupt in NVIC.
    //Interrupt_setPriority(INT_TA0_0, 254u);
    Interrupt_setPriority(INT_TA0_0, 4u); /* TODO : 4U has been chosen quite randomly... */
    Interrupt_enableInterrupt(INT_TA0_0);

    Timer_A_configureUpMode(TIMER_A1_BASE, &lo_prio_timer_config);
    Timer_A_registerInterrupt(TIMER_A1_BASE, TIMER_A_CCR0_INTERRUPT, TA1_0_IRQHandler);
    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);

    Interrupt_setPriority(INT_TA1_0, 255u);
    Interrupt_enableInterrupt(INT_TA1_0);
}

/* This should be fired every 10 msec */
//Hi priority interrupt handler.
Private void TA0_0_IRQHandler(void)
{
    Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
    if (priv_delay_counter > 0u)
    {
        priv_delay_counter--;
    }
    timer_10msec_callback();
}

//Fired every 50 mseconds, this is lo prio interrupt handler.
Private void TA1_0_IRQHandler(void)
{
    Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
    timer_50msec_callback();
}





/****************************************************************************************
 * INPUT OUTPUT PORTS
 ****************************************************************************************/

Public void set_led_one(U8 state)
{
    if (state == 1u)
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
    }
    else
    {
        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    }
}

Public void set_led_two_red(U8 state)
{
    if (state == 1u)
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
    }
    else
    {
        GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
    }
}

Public void set_led_two_green(U8 state)
{
    if (state == 1u)
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
    }
    else
    {
        GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
    }
}

Public void set_led_two_blue(U8 state)
{
    if (state == 1u)
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);
    }
    else
    {
        GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
    }
}

/* Set function for the RS or A0 pin. */
Public void set_reg_select(U8 state)
{
    if (state)
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN7);
    }
    else
    {
        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN7);
    }
}

/* Set function for Chip Select pin. */
Public void set_cs_pin(U8 state)
{
    if (state)
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN3);
    }
    else
    {
        GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);
    }
}


Public void set_disp_reset_pin(U8 state)
{
    if (state)
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN6);
    }
    else
    {
        GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN6);
    }
}


Public U8 isBtnOne(void)
{
    U8 res = 0x00u;
    if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1) == GPIO_INPUT_PIN_LOW)
    {
        res = 0x01u;
    }
    return res;
}

Public U8 isBtnTwo(void)
{
    U8 res = 0x00u;
    if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN4) == GPIO_INPUT_PIN_LOW)
    {
        res = 0x01u;
    }
    return res;
}


Public U8 isGreenSafetyBtn(void)
{
    U8 res = 0x00u;
    if (GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN0) == GPIO_INPUT_PIN_LOW)
    {
        res = 0x01u;
    }
    return res;
}

