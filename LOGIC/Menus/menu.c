/*
 * menu.c
 *
 *  Created on: Oct 12, 2017
 *      Author: Joonatan
 */


#include "menu.h"
#include "display_drv.h"
#include "font.h"
#include "buttons.h"
#include "Bargraph.h"

#define MENU_FONT FONT_MEDIUM_FONT

/***** Private function definitions ***********/

Private void upButtonListener(void);
Private void downButtonListener(void);
Private void okButtonListener(void);
Private void backButtonListener(void);

Private void drawMenu(SelectionMenu * menu);


/******Private variable definitions ***********/
Private SelectionMenu * priv_active_menu_ptr = NULL;



Public void menu_enterMenu(SelectionMenu * menu)
{
    priv_active_menu_ptr = menu;

    display_clear();
    drawMenu(priv_active_menu_ptr);

    //Subscribe to buttons.
    buttons_subscribeListener(UP_BUTTON, upButtonListener);
    buttons_subscribeListener(OK_BUTTON, okButtonListener);
    buttons_subscribeListener(DOWN_BUTTON, downButtonListener);
    buttons_subscribeListener(CANCEL_BUTTON, backButtonListener);
}

/* Exit from menu unconditionally. */
Public void menu_exitMenu(void)
{
    if (priv_active_menu_ptr != NULL)
    {
        priv_active_menu_ptr = NULL;
        buttons_unsubscribeAll();
    }
}


Public void menu_setSelectedItem(SelectionMenu * menu, U8 selected_item)
{
    menu->selected_item = selected_item;
    drawMenu(menu);
}


Public void menu_MoveCursor(SelectionMenu * menu, Boolean dir)
{
    Boolean changed = FALSE;
    //dir is TRUE : move up, dir is FALSE, move down.
    if (dir)
    {
        if (menu->selected_item > 0u)
        {
            menu->selected_item --;
            changed = TRUE;
        }
    }
    else
    {
        if (menu->selected_item < (menu->number_of_items - 1))
        {
            menu->selected_item++;
            changed = TRUE;
        }
    }

    if (changed)
    {
        drawMenu(menu);
    }
}


Public const MenuItem * menu_getSelectedItem(SelectionMenu * menu)
{
    return &(menu->items[menu->selected_item]);
}


/* Private function definitions. */

Private void drawMenu(SelectionMenu * menu)
{
    //First lets see how large the menu to be drawn is.
    U8 height;
    U8 ypos;
    U8 x;
    U8 font_height;

    font_height = font_getFontHeight(MENU_FONT);

    height = (font_height + 2u) * menu->number_of_items;
    ypos = (NUMBER_OF_ROWS >> 1u) - (height >> 1u);

    for (x = 0u; x < menu->number_of_items; x++)
    {
        if(x == menu->selected_item)
        {
            display_drawStringCenter(menu->items[x].text, 64u, ypos, MENU_FONT, TRUE);
        }
        else
        {
            display_drawStringCenter(menu->items[x].text, 64u, ypos, MENU_FONT, FALSE);
        }
        ypos += font_height;
        ypos += 2u;
    }

}

/* This is the OK key. */
Private void okButtonListener(void)
{
    const MenuItem * item = menu_getSelectedItem(priv_active_menu_ptr);
    SelectionMenu * sub;
    Bargraph_T * bg_ptr;

    switch(item->Action)
    {
    case(MENU_ACTION_FUNCTION):
            /* We exit the menu and perform the attached function pointer. */
            menu_exitMenu();
            item->ActionArg.function_ptr();
            break;
    case(MENU_ACTION_SUBMENU):
            sub = item->ActionArg.subMenu_ptr;
            sub->parent = priv_active_menu_ptr;
            menu_enterMenu(sub);
            break;
    case(MENU_ACTION_WIDGET):
            /* TODO : Currently we have only 1 widget type, in the future this might become more complex. */
            bg_ptr = item->ActionArg.bargraph_ptr;
            bg_ptr->parent = priv_active_menu_ptr;
            enterBarGraph(bg_ptr);
            break;
    case(MENU_ACTION_NONE):
    default:
        break;
    }
}


/* This is the UP key   */
Private void upButtonListener(void)
{
    if (priv_active_menu_ptr != NULL)
    {
        menu_MoveCursor(priv_active_menu_ptr, TRUE);
    }
}

/* This is the DOWN key. */
Private void downButtonListener(void)
{
    if (priv_active_menu_ptr != NULL)
    {
        menu_MoveCursor(priv_active_menu_ptr, FALSE);
    }
}

/* This is the back button. */
Private void backButtonListener(void)
{
    if (priv_active_menu_ptr != NULL)
    {
        if (priv_active_menu_ptr->parent != NULL)
        {
            /* This is a submenu. */
            menu_enterMenu(priv_active_menu_ptr->parent);
        }
    }
}

