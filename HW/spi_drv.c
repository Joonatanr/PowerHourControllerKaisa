/*
 * spi.c
 *
 *  Created on: Aug 13, 2017
 *      Author: Joonatan
 */

#include <driverlib.h>
#include <spi_drv.h>
#include "register.h"

/* SPI Master Configuration Parameter */
const eUSCI_SPI_MasterConfig spiMasterConfig =
{
        EUSCI_B_SPI_CLOCKSOURCE_SMCLK,             // SMCLK Clock Source
        12000000,                                  // SMCLK = DCO = 12MHZ
        1000000,                                   // SPICLK = 1MHz
        EUSCI_B_SPI_MSB_FIRST,                     // MSB First
        EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT,    // Phase
        EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH, // High polarity
        EUSCI_B_SPI_3PIN                           // 3Wire SPI Mode
};


Public void spi_init(void)
{
    /* Selecting P1.5 and P1.6 in SPI mode */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
            GPIO_PIN5 | GPIO_PIN6 , GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring SPI in 3wire master mode */
    SPI_initMaster(EUSCI_B0_BASE, &spiMasterConfig);

    /* Enable SPI module */
    SPI_enableModule(EUSCI_B0_BASE);

    /* Enabling interrupts */
    SPI_enableInterrupt(EUSCI_B0_BASE, EUSCI_B_SPI_RECEIVE_INTERRUPT);
    Interrupt_enableInterrupt(INT_EUSCIB0);
}

Public void spi_transmit(U8 * data, U16 data_len)
{
    U8 * data_ptr = data;

    while (data_len > 0)
    {
        spi_transmit_byte(*data_ptr, FALSE);
        data_ptr++;
        data_len--;
    }
}

volatile Boolean isReadyToTransmit = TRUE;


//******************************************************************************
//
//This is the EUSCI_B0 interrupt vector service routine.
//
//******************************************************************************
void EUSCIB0_IRQHandler(void)
{
    uint32_t status = SPI_getEnabledInterruptStatus(EUSCI_B0_BASE);

    SPI_clearInterruptFlag(EUSCI_B0_BASE, status);

    if (status & EUSCI_B_SPI_RECEIVE_INTERRUPT)
    {
        isReadyToTransmit = TRUE;
    }
}


Public void spi_transmit_byte(U8 byte, Boolean reg_select)
{
    static Boolean reg_select_state = FALSE;

    //Set A0 pin to proper state.
    if(reg_select != reg_select_state)
    {
        //We need to change A0 pin.
        while(!isReadyToTransmit);
    }
    else
    {
        /* USCI_B0 TX buffer ready? */
        /* We don't need for previous transmission to finish, we just need to make sure that register is ready. */
        while (!(SPI_getInterruptStatus(EUSCI_B0_BASE, EUSCI_B_SPI_TRANSMIT_INTERRUPT)));
    }

    if (reg_select)
    {
        reg_select_state = TRUE;
        set_reg_select(1u);
    }
    else
    {
        reg_select_state = FALSE;
        set_reg_select(0u);
    }

    isReadyToTransmit = FALSE;
    //Transmit data to slave.
    SPI_transmitData(EUSCI_B0_BASE, byte);
}



