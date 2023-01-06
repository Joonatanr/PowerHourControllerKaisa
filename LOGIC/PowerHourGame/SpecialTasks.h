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
    TASK_FOR_ALUMNI,
    TASK_FOR_BOARD,
    TASK_FOR_KT,
    TASK_FOR_SOCRESP,
    TASK_FOR_PAX,
    TASK_FOR_CORETEAM,
    NUMBER_OF_TASK_TYPES
} SpecialTaskType;

typedef Boolean (*OverrideFunc)(U8 sec); //Returns TRUE, if done with override.
typedef Boolean (*SpecialTaskFunc)(U8 sec, SpecialTaskType type);

extern Boolean girlsSpecialTask(U8 sec);
extern Boolean guysSpecialTask(U8 sec);

extern Boolean alumniSpecialTask(U8 sec);
extern Boolean boardSpecialTask(U8 sec);
extern Boolean socRespSpecialTask(U8 sec);
extern Boolean KtSpecialTask(U8 sec);
extern Boolean PaxSpecialTask(U8 sec);
extern Boolean coreTeamSpecialTask(U8 sec);

#endif /* LOGIC_POWERHOURGAME_SPECIALTASKS_H_ */
