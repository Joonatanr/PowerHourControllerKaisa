/*
 * MessageBox.h
 *
 *  Created on: Sep 4, 2018
 *      Author: Joonatan
 */

#ifndef LOGIC_TEXTTOOLS_MESSAGEBOX_H_
#define LOGIC_TEXTTOOLS_MESSAGEBOX_H_

#include "typedefs.h"

typedef enum
{
    RESPONSE_NONE,
    RESPONSE_OK,
    RESPONSE_CANCEL,
    NUMBER_OF_RESPONSES
} MsgBox_Response;


typedef void (* MsgBoxResponseHandler)(MsgBox_Response resp_code);


extern void MessageBox_Show(const char * text, U16 period);
extern void MessageBox_ShowWithOk(const char * text);
extern void MessageBox_ShowWithOkCancel(const char * text);

extern void MessageBox_SetResponseHandler(MsgBoxResponseHandler handler);

extern void MessageBox_init(void);
extern void MessageBox_cyclic100msec(void);

#endif /* LOGIC_TEXTTOOLS_MESSAGEBOX_H_ */
