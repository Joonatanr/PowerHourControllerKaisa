/*
 * SnakeExtra.c
 *
 *  Created on: Sep 5, 2018
 *      Author: Joonatan
 */


#include "SnakeExtra.h"
#include "display_drv.h"
#include "buzzer.h"
#include "LOGIC/TextTools/MessageBox.h"

/* This contains all the stuff that is not related directly to the snake game itself.*/

/* Everything should happen inside this rectangle, except for message boxes. */
Private const Rectangle * priv_my_rect;


Public void SnakeExtra_setRectangle(const Rectangle *r)
{
    priv_my_rect = r;
}


Public void SnakeExtra_start(void)
{
    /*
    display_fillRectangle(priv_my_rect->location.x,
                          priv_my_rect->location.y,
                          priv_my_rect->size.height,
                          priv_my_rect->size.width,
                          PATTERN_GRAY);
    */
    //MessageBox_Show("Test", 3000u);
}

/* Really basic stuff for now.*/
Public void SnakeExtraIncreaseScore(U16 score)
{
    const Bitmap * shot = &small_shot_bitmap;
    static Boolean clear_flag = FALSE;

    Rectangle textRect;

    textRect.location.x = priv_my_rect->location.x + 2u;
    textRect.location.y = priv_my_rect->location.y + shot->height + 7u;
    textRect.size.height = 12u;
    textRect.size.width = priv_my_rect->size.width - 2u;


    if ((score % 25u) == 0u)
    {
        display_drawBitmap(shot,
                           priv_my_rect->location.x + 5u,
                           priv_my_rect->location.y + 4u,
                           FALSE);

        display_drawTextBox(&textRect, "SHOT!", FONT_SMALL_FONT);

        buzzer_playBeeps(2u);

        clear_flag = TRUE;
    }
    else if(clear_flag)
    {
        /* Clear on next scoring. */
        display_fillRectangle(priv_my_rect->location.x,
                              priv_my_rect->location.y,
                              priv_my_rect->size.height,
                              priv_my_rect->size.width,
                              PATTERN_WHITE);
        clear_flag = FALSE;
    }
}

