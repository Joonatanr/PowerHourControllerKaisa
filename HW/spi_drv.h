/*
 * spi.h
 *
 *  Created on: Aug 13, 2017
 *      Author: Joonatan
 */

#ifndef HW_SPI_DRV_H_
#define HW_SPI_DRV_H_


#include "typedefs.h"

Public void spi_init(void);
Public void spi_transmit(U8 * data, U16 data_len);
Public void spi_transmit_byte(U8 byte, Boolean reg_select);


#endif /* HW_SPI_DRV_H_ */
