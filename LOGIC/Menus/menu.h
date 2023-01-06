/*
 * menu.h
 *
 *  Created on: Oct 12, 2017
 *      Author: Joonatan
 */

#ifndef LOGIC_MENU_H_
#define LOGIC_MENU_H_

#include "typedefs.h"
#include "Bargraph.h"

typedef enum
{
    MENU_ACTION_NONE,                   //We don't do anything, basically means that this is not yet implemented.
    MENU_ACTION_SUBMENU,                //We enter a submenu.
    MENU_ACTION_WIDGET,                 //We activate a widget, like a bargraph
    MENU_ACTION_FUNCTION,               //We perform an attached function. This also means that we leave the menu.
    NUMBER_OF_MENU_ACTIONS
} MenuActionType;

typedef void (*MenuActionHandler)(void);

typedef struct
{
    const char * text;
    MenuActionType Action;

    union
    {
        struct _Selection_Menu_ *   subMenu_ptr;
        MenuActionHandler          function_ptr;
        struct __Bargraph__ *      bargraph_ptr;
    } ActionArg;

} MenuItem;

typedef struct _Selection_Menu_
{
    struct _Selection_Menu_ * parent; /* Reference to upper menu in case of submenu. */
    const MenuItem * items;
    const U8 number_of_items;
    U8 selected_item;
} SelectionMenu;

extern void menu_enterMenu(SelectionMenu * menu);
extern void menu_exitMenu(void);

extern void menu_setSelectedItem(SelectionMenu * menu, U8 selected_item);
extern const MenuItem * menu_getSelectedItem(SelectionMenu * menu);
extern void menu_MoveCursor(SelectionMenu * menu, Boolean dir);

#endif /* LOGIC_MENU_H_ */
