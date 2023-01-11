/*
 * pot.h
 *
 *  Created on: 16. dets 2019
 *      Author: JRE
 */

#ifndef HW_POT_H_
#define HW_POT_H_

#include "typedefs.h"

typedef enum
{
    POTENTIOMETER_ONE,
    POTENTIOMETER_TWO,

    NUMBER_OF_DEFINED_POTENTIOMETERS
} potentiometer_T;

extern void pot_init(void);
extern void pot_cyclic_10ms(void);
extern int pot_getSelectedRange(void);


#endif /* HW_POT_H_ */
