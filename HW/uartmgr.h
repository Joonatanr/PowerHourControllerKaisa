/*
 * comm.h
 *
 *  Created on: 28. mai 2017
 *      Author: Joonatan
 */

#ifndef HW_COMM_H_
#define HW_COMM_H_

#include "typedefs.h"

#define UART_BUF_LEN 32

extern void comm_send_char(char c);
extern void comm_send_str(const char * str);
extern U8 comm_receiveData(U8 * dest);
extern void comm_send_number(long nr);
extern void comm_send_rn(void);
extern void uart_init(void);
extern void uart_cyclic(void);
extern void uprintf(const char * str, ...);

#endif /* HW_COMM_H_ */
