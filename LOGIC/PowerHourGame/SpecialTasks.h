/*
 * SpecialTasks.h
 *
 *  Created on: Sep 5, 2017
 *      Author: Joonatan
 */

#ifndef LOGIC_POWERHOURGAME_SPECIALTASKS_H_
#define LOGIC_POWERHOURGAME_SPECIALTASKS_H_


#include "typedefs.h"

typedef enum
{
    TASK_FOR_GIRLS,
    TASK_FOR_GUYS,
    TASK_FOR_EVERYONE,
    TASK_FOR_KAISA,

    NUMBER_OF_TASK_TYPES
} SpecialTaskType;

typedef Boolean (*OverrideFunc)(U8 sec); //Returns TRUE, if done with override.
typedef Boolean (*SpecialTaskFunc)(U8 sec, SpecialTaskType type);

extern Boolean girlsSpecialTask(U8 sec);
extern Boolean guysSpecialTask(U8 sec);
extern Boolean everybodySpecialTask(U8 sec);
extern Boolean kaisaSpecialTask(U8 sec);


#endif /* LOGIC_POWERHOURGAME_SPECIALTASKS_H_ */
