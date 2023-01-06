/*
 * MessageBox.c
 *
 *  Created on: Sep 4, 2018
 *      Author: Joonatan
 */

#include "MessageBox.h"
#include "display_drv.h"
#include "misc.h"
#include "register.h"
#include "buttons.h"
#include "LOGIC/Scheduler.h"

#define MSGBOX_FONT FONT_MEDIUM_FONT
//#define MSGBOX_FONT FONT_SMALL_FONT

#define MSGBOX_MIN_HEIGHT   25u
#define MSGBOX_MIN_WIDTH    75u
#define MSGBOX_MARGIN       10u
#define BUTTON_AREA_HEIGHT  18u

typedef enum
{
    STATE_IDLE,
    STATE_PENDING,
    STATE_SHOWING,
    NUMBER_OF_MSGBOX_STATES
} MsgBox_State;

typedef enum
{
    TYPE_TEXT,
    TYPE_OK,
    TYPE_OK_CANCEL,
    NUMBER_OF_TYPES
} MsgBox_Type;

/**************************** Private function forward declarations **************************/

Private void drawMessageBox(const char * text, Boolean includeButtonArea);
Private void drawButton(const char * text, U8 xloc);
Private Size getMessageSize(const char * text);
Private void clearBoxDisplay(void);

Private void handleOkPress(void);
Private void handleCancelPress(void);

Private void closeMessageBox(void);

Private Rectangle priv_msg_box;

/* Variables for storing previously displayed image. */
/* TODO : Implement this. */
Private U8 priv_prev_image_data[NUMBER_OF_COLUMNS * NUMBER_OF_PAGES];

Private Bitmap priv_prev_image =
{
     .bmp_data = priv_prev_image_data,
     .height = 0u,
     .width = 0u
};

/* Flags for setting up a displayed messagebox. */
Private MsgBox_State            priv_state;
Private MsgBox_Type             priv_type;
Private char                    priv_text[64];
Private U16                     priv_duration_period;
Private MsgBoxResponseHandler   priv_response_handler = NULL;

/**************************** Public function definitions **************************/

Public void MessageBox_init(void)
{
    priv_state = STATE_IDLE;
}


Public void MessageBox_cyclic100msec(void)
{
    if (priv_duration_period > 0u)
    {
        priv_duration_period--;
    }

    switch(priv_state)
    {
        case STATE_PENDING:
            switch(priv_type)
            {
            case TYPE_TEXT:
                drawMessageBox(priv_text, FALSE);
                break;
            case TYPE_OK:
                drawMessageBox(priv_text, TRUE);
                buttons_unsubscribeAll();
                buttons_subscribeListener(OK_BUTTON , handleOkPress);

                drawMessageBox(priv_text, TRUE);
                drawButton("OK", 63u);
                break;
            case TYPE_OK_CANCEL:
                drawMessageBox(priv_text, TRUE);
                buttons_unsubscribeAll();

                buttons_subscribeListener(OK_BUTTON , handleOkPress);
                buttons_subscribeListener(CANCEL_BUTTON, handleCancelPress);

                drawMessageBox(priv_text, TRUE);
                drawButton("OK", 39u);
                drawButton("CANCEL", 76u);
                break;
            default:
                /* Should not happen. */
                break;
            }

            /* We pause the active module until messagebox has been closed. */
            Scheduler_SetActiveApplicationPause(TRUE);
            priv_state = STATE_SHOWING;
            break;

        case STATE_SHOWING:
            if ((priv_type == TYPE_TEXT) && (priv_duration_period == 0u))
            {
                closeMessageBox();
            }
            break;
        case STATE_IDLE:
        default:
            break;
    }
}


/* Sets callback for messagebox response. */
Public void MessageBox_SetResponseHandler(MsgBoxResponseHandler handler)
{
    priv_response_handler = handler;
}


Public void MessageBox_Show(const char * text, U16 period)
{
    strcpy(priv_text, text);
    priv_duration_period = period;
    priv_state = STATE_PENDING;
    priv_type = TYPE_TEXT;
}


Public void MessageBox_ShowWithOk(const char * text)
{
    strcpy(priv_text, text);
    priv_duration_period = 0u;
    priv_state = STATE_PENDING;
    priv_type = TYPE_OK;
}


Public void MessageBox_ShowWithOkCancel(const char * text)
{
    strcpy(priv_text, text);
    priv_duration_period = 0u;
    priv_state = STATE_PENDING;
    priv_type = TYPE_OK_CANCEL;
}

/**************************** Private function definitions **************************/

Private void drawMessageBox(const char * text, Boolean includeButtonArea)
{
    Size textSize = getMessageSize(text);
    Size rectSize;
    Point rectLocation;
    Rectangle box;

    rectSize.height = MAX(MSGBOX_MIN_HEIGHT, (textSize.height + MSGBOX_MARGIN));
    rectSize.width =  MAX(MSGBOX_MIN_WIDTH,  (textSize.width + MSGBOX_MARGIN));

    if (includeButtonArea)
    {
        rectSize.height += BUTTON_AREA_HEIGHT;
    }

    rectSize.height = MIN(rectSize.height, NUMBER_OF_ROWS);
    rectSize.width = MIN(rectSize.width, NUMBER_OF_COLUMNS);

    rectLocation.x = GET_X_FROM_CENTER(NUMBER_OF_COLUMNS / 2, rectSize.width);
    rectLocation.y = GET_Y_FROM_CENTER(NUMBER_OF_ROWS / 2, rectSize.height);

    box.location = rectLocation;
    box.size = rectSize;
#if 0
    //Still under test.
    /* Copy previous image to buffer... */
    display_getDisplayedImage(&box, priv_prev_image_data);
    priv_prev_image.height = box.size.height;
    priv_prev_image.width = box.size.width;
#endif
    /* Draw messagebox. */
    if (includeButtonArea)
    {
        box.size.height -= BUTTON_AREA_HEIGHT;
    }

    /* Draw just 1 line for now. */
    display_drawTextBox(&box, text, MSGBOX_FONT);

    /* This area won't be cleared otherwise. */
    if (includeButtonArea)
    {
        display_fillRectangle(rectLocation.x , rectLocation.y + box.size.height, BUTTON_AREA_HEIGHT, rectSize.width, PATTERN_WHITE);
    }

    //Draw the border.
    display_drawRectangle(rectLocation.x, rectLocation.y, rectSize.height, rectSize.width, 2u);

    priv_msg_box.location = rectLocation;
    priv_msg_box.size = rectSize;
}


Private void drawButton(const char * text, U8 xloc)
{
    U8 yloc;
    Rectangle buttonBox;
    Size textSize;

    yloc = (priv_msg_box.location.y + priv_msg_box.size.height) - BUTTON_AREA_HEIGHT;
    yloc++;

    textSize = getMessageSize(text);
    textSize.width += 8u; //Create some margins here.

    buttonBox.location.x = GET_X_FROM_CENTER(xloc, textSize.width);
    buttonBox.location.y = yloc;
    buttonBox.size = textSize;

    display_drawTextBox(&buttonBox, text, MSGBOX_FONT);
    display_drawRectangle(buttonBox.location.x,
                          buttonBox.location.y,
                          buttonBox.size.height,
                          buttonBox.size.width,
                          1u);
}


/* Should be called at the end to clear up the box. */
Private void clearBoxDisplay(void)
{
    /* TODO : Should record display buffer before opening box, and then restore that here... */
    display_fillRectangle(priv_msg_box.location.x,
                          priv_msg_box.location.y,
                          priv_msg_box.size.height,
                          priv_msg_box.size.width,
                          PATTERN_WHITE);
}

/* Returns physical size of the text to be displayed */
Private Size getMessageSize(const char * text)
{
    Size ret;
    U8 number_of_rows = 1u;
    U8 current_row_width = 0u;
    U8 max_row_width = 0u;
    U8 fontsize = font_getFontHeight(MSGBOX_FONT);

    const char *ps = text;
    while (*ps)
    {
        if (*ps == '\n')
        {
          number_of_rows++;
          max_row_width = MAX(current_row_width, max_row_width);
          current_row_width = 0u;
        }
        else
        {
            current_row_width += font_getCharWidth(*ps, MSGBOX_FONT);
            current_row_width++; //Account for one bit in between chars.
        }
        ps++;
    }

    max_row_width = MAX(current_row_width, max_row_width);
    /* Lets try this with 1 row at first, and later test with multiple rows... */
    ret.height = fontsize * number_of_rows;
    ret.width = max_row_width;

    return ret;
}


Private void handleOkPress(void)
{
    closeMessageBox();
    /* Lets call this last, so that the active module can draw something on the display etc... */
    if (priv_response_handler != NULL)
    {
        priv_response_handler(RESPONSE_OK);
    }
    priv_response_handler = NULL;
}

Private void handleCancelPress(void)
{
    closeMessageBox();
    /* Lets call this last, so that the active module can draw something on the display etc... */
    if (priv_response_handler != NULL)
    {
        priv_response_handler(RESPONSE_CANCEL);
    }
    priv_response_handler = NULL;
}


Private void closeMessageBox(void)
{
    /* TODO : Also draw the previous image, so it does not get lost. */

    //buttons_unsubscribeAll();
    Scheduler_SetActiveApplicationPause(FALSE);
    clearBoxDisplay();
    priv_state = STATE_IDLE;
}
