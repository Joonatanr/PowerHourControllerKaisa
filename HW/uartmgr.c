/*
 * comm.c
 *
 *  Created on: 28. mai 2017
 *      Author: Joonatan
 */

/* This file handles communication over the UART interface.
 * Unfortunately driverlib already has a source file named uart.c */

#include <uart.h>
#include <driverlib.h>
#include <misc.h>
#include "uartmgr.h"
#include "display_drv.h"

Private U8 priv_receive_buffer[UART_BUF_LEN];
Private U8 priv_uart_buffer[UART_BUF_LEN];
Private U8 priv_receive_cnt;
Private U8 priv_receive_flag = 0;
Private Rectangle test_rect;

Private const eUSCI_UART_Config uartConfig =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        78,                                     // BRDIV = 78
        2,                                       // UCxBRF = 2
        0,                                       // UCxBRS = 0
        EUSCI_A_UART_NO_PARITY,                  // No Parity
        EUSCI_A_UART_LSB_FIRST,                  // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
        EUSCI_A_UART_MODE,                       // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION  // Oversampling
};

Public void comm_send_char(char c)
{
    UART_transmitData(EUSCI_A0_BASE, c);

    //So should we also wait until data has been sent out???
    //We should not send any chars out if UART is busy.
}

Public void comm_send_str(const char * str)
{
    const char * ps = str;
    while(*ps)
    {
        comm_send_char(*ps);
        ps++;
    }
}


Public void comm_send_number(long nr)
{
    U32 len = long2string(nr, sY);
    sY[len] = 0;
    comm_send_str(sY);
}

Public void comm_send_rn(void)
{
    comm_send_char('\r');
    comm_send_char('\n');
}

/* EUSCI A0 UART ISR */
/* Handles incoming data over the UART.*/
Public void EUSCIA0_IRQHandler(void)
{
    uint32_t status = UART_getEnabledInterruptStatus(EUSCI_A0_BASE);
    U8 c;

    UART_clearInterruptFlag(EUSCI_A0_BASE, status);

    if (status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
    {
        //MAP_UART_transmitData(EUSCI_A0_BASE, MAP_UART_receiveData(EUSCI_A0_BASE));
        c = UART_receiveData(EUSCI_A0_BASE);

        if ((priv_receive_cnt < UART_BUF_LEN) && (priv_receive_flag == 0u))
        {
            priv_receive_buffer[priv_receive_cnt] = c;
            priv_receive_cnt++;

            if (c == '\n')
            {
                //We got new data that needs to be processed.
                priv_receive_flag = 1u;
            }
        }
        else
        {
            //Silently discard data to prevent buffer overflow.
        }
    }
}


Public U8 comm_receiveData(U8 * dest)
{
    //Return length of received data. If none received then return 0.
    U8 res = 0u;

    if (priv_receive_flag)
    {
        res = priv_receive_cnt;
        (void)memcpy(dest, priv_receive_buffer, priv_receive_cnt);
        priv_receive_cnt = 0u;
        priv_receive_flag = 0u;
    }

    return res;
}


Public void uart_init(void)
{
    /* Configuring UART Module */
    MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig);

    /* Enable UART module */
    MAP_UART_enableModule(EUSCI_A0_BASE);

    /* Enabling interrupts */
    MAP_UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    MAP_Interrupt_enableInterrupt(INT_EUSCIA0);
}


Public void uart_cyclic(void)
{
    U8 msg_len;

    msg_len = comm_receiveData(priv_uart_buffer);

    if (msg_len > 0u)
    {
        if (priv_uart_buffer[0] == 'R')
        {
            if (parseRectangle((char *)priv_uart_buffer, &test_rect))
            {
                display_fillRectangle(test_rect.location.x,
                                      test_rect.location.y,
                                      test_rect.size.height,
                                      test_rect.size.width,
                                      PATTERN_GRAY);
                comm_send_str("OK");

                comm_send_str("X : ");
                comm_send_number(test_rect.location.x);

                comm_send_str(", Y : ");
                comm_send_number(test_rect.location.y);

                comm_send_str(", Height: ");
                comm_send_number(test_rect.size.height);

                comm_send_str(", Width: ");
                comm_send_number(test_rect.size.width);

                comm_send_rn();
            }
            else
            {
                comm_send_str("ERROR");
            }
        }
        else if(priv_uart_buffer[0] == 'C')
        {
            display_clear();
        }
    }
}


Public void uprintf(const char * str, ...)
{
    va_list arg;
    va_start(arg, str);

    const char *ps;
    char c;
    int i;
    char *s;

    for (ps = str; *ps != '\0'; ps++)
    {
        if( *ps != '%' )
        {
            comm_send_char(*ps);
        }
        else
        {
            ps++;

            /* Currently supporting most of the basic options of printf... */
            switch (*ps)
            {
            case 'c':
                c = va_arg(arg, char);
                comm_send_char(c);
                break;
            case 'd':
                i = va_arg(arg, int);
                long2string(i, sY);
                comm_send_str(sY);
                break;
            case 's':
                s = va_arg(arg, char *);
                comm_send_str(s);
                break;
            case 'x':
                i = va_arg(arg, unsigned int);
                hex2string(i, sY);
                comm_send_str(sY);
                break;
            default:
                    /* TODO: Consider error case... */
                break;
            }
        }
    }

    va_end(arg);
}



