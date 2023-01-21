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
    POTENTIOMETER_RAINBOW_LEVEL,    /* How gay is OK?                    */
    POTENTIOMETER_NUDE_LEVEL,       /* Nude task meter                   */
    POTENTIOMETER_SEXY_LEVEL,       /* Sexy task meter                   */
    POTENTIOMETER_KAISA,            /* How drunk Kaisa is                */
    POTENTIOMETER_GIRLS,            /* How brave/intense the girls are   */
    POTENTIOMETER_GUYS,             /* How brave/intense the guys are    */

    NUMBER_OF_DEFINED_POTENTIOMETERS
} potentiometer_T;

extern void pot_init(void);
extern void pot_cyclic_10ms(void);
extern int pot_getSelectedRange(potentiometer_T pot);


#endif /* HW_POT_H_ */
