/*
 * SnakeMain.c
 *
 *  Created on: 23. mai 2018
 *      Author: JRE
 */

#include "SnakeMain.h"
#include "SnakeExtra.h"
#include "display_drv.h"
#include "buttons.h"
#include <stdlib.h>
#include "LOGIC/TextTools/MessageBox.h"
#include "buzzer.h"
#include "register.h"
#include "misc.h"
#include "LOGIC/main.h"

/*
This is the coordinate system used. Each game coordinate corresponds to 2x2 pixels.

px  0 1 2 3 4 5 6 7 8 9 ...
  crd 0   1  2   3   4
0   * *|* *|* *|* *|* *|
1 0 * *|* *|* *|* *|* *|
    ---+---+---+---+---+...
2   * *|   |   |   |   |
3 1 * *|   |   |   |   |
    ---+---+---+---+---+...
4   * *|   |   |   |   |
5 2 * *|   |   |   |   |
    ---+---+---+---+---+...
6   * *|   |   |   |   |
7 3 * *|   |   |   |   |
    ---+---+---+---+---+...
8   * *|   |   |   |   |
9 4 * *|   |   |   |   |
    ---+---+---+---+---+...
...

*/

#define DISABLE_BUZZER
#define ENABLE_EXTRA

/* This file will contain the long-planned Snake Game on the power hour machine...*/

#define GAME_BORDER_WIDTH_PX   2u   /* Set this at 2 pixels.    */
#define GAME_BORDER_AREA_Y_PX  64u  /* 64  pixels               */
#define GAME_BORDER_AREA_X_PX  96u  /* 108 pixels               */

#define DEFAULT_SNAKE_SPEED       4u /* Set at 400 ms intervals. */

/* Game area definitions begin here. Nothing is in pixels anymore. */
#define GAME_BORDER_WIDTH GAME_BORDER_WIDTH_PX / 2u
#define GAME_AREA_X_SIZE GAME_BORDER_AREA_X_PX / 2u
#define GAME_AREA_Y_SIZE GAME_BORDER_AREA_Y_PX / 2u

#define MAX_COORD_X GAME_AREA_X_SIZE - 1u
#define MAX_COORD_Y GAME_AREA_Y_SIZE - 1u

#define NUMBER_OF_GAME_SQUARES GAME_AREA_X_SIZE * GAME_AREA_Y_SIZE

#define INITIAL_SNAKE_LENGTH 8u
#define INITIAL_SNAKE_X 32u
#define INITIAL_SNAKE_Y 16u

typedef enum
{
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
    NUMBER_OF_DIRECTIONS
} SnakeDirection;

typedef struct __SnakeElement__
{
    Point                      begin; /* Note that each snake part takes 2*2 pixels */
    U8                        length;
    SnakeDirection               dir;
    struct __SnakeElement__   * next;
} SnakeElement;


/************************** Private variable declarations. *****************************/

Private U8 priv_snake_speed = DEFAULT_SNAKE_SPEED;

Private SnakeElement *  priv_head = NULL;
Private SnakeElement *  priv_tail = NULL;

Private Boolean priv_isDirSet = FALSE; /* Prevents from changing direction twice during the cycle. */
Private Boolean priv_isGameOver = FALSE;

#ifdef ENABLE_EXTRA
Private Rectangle priv_xRect =
{
     .location =
     {
      .x = GAME_BORDER_AREA_X_PX + 1u,
      .y = 20u
     },
     .size =
     {
       .width = DISPLAY_NUMBER_OF_COLUMNS - GAME_BORDER_AREA_X_PX - 2u,
       .height = 43u
     }
};
#endif

/* Since we only store boolean values, then we use one byte for 8 rows. */
#define GAME_SQUARE_ARR_X GAME_AREA_X_SIZE
#define GAME_SQUARE_ARR_Y GAME_AREA_Y_SIZE >> 3

Private U8 priv_game_squares[GAME_SQUARE_ARR_X][GAME_SQUARE_ARR_Y];

Private Point priv_food;
Private U16 priv_score = 0u;
Private Rectangle pointsRectangle;

/* Macros for accessing the priv_game_squares array */
#define GET_BITMASK(b) (1u << ((b) % 8))
#define GET_SQUARE_VALUE(x, y) (((priv_game_squares[(x)][(y) >> 3] &   GET_BITMASK((y))) > 0u) ? 1u : 0u)
#define SET_SQUARE_VALUE(x, y)    priv_game_squares[(x)][(y) >> 3] |=  GET_BITMASK((y))
#define CLR_SQUARE_VALUE(x, y)    priv_game_squares[(x)][(y) >> 3] &= ~GET_BITMASK((y))


/************************** Private function forward declarations. *****************************/


Private void setSnakeDirection(SnakeDirection dir);
Private Point getTailOfElement(const SnakeElement * elem);

/* Drawing functions. */
Private void drawBackGround(void);
Private void drawSnakeElement(SnakeElement * elem, Boolean isBlack);
Private void drawPoint(Point p, Boolean value);
Private void eraseTail(void);
Private void moveHeadForward(void);
Private void createFood(void);
Private void handleEating(void);
Private Point getRandomFreePoint(void);

Private Boolean isCollision(void);
Private void handleGameOver(void);
Private void handleMessageBoxResponse(MsgBox_Response resp);

Private void HandleUpButton(void);
Private void HandleDownButton(void);
Private void HandleRightButton(void);
Private void HandleLeftButton(void);

/************************** Public function definitions. *****************************/

Public void snake_init(void)
{
    priv_isGameOver = FALSE;
    priv_score = 0u;
    memset(priv_game_squares, 0x00u, (GAME_SQUARE_ARR_X * GAME_SQUARE_ARR_Y));
}

Public void snake_start(void)
{
    /* Draw the background and border. */
    display_clear();
    drawBackGround();

    /* Subscribe to buttons. */
    buttons_unsubscribeAll();

    buttons_subscribeListener(UP_BUTTON,    HandleUpButton);
    buttons_subscribeListener(DOWN_BUTTON,  HandleDownButton);
    buttons_subscribeListener(RIGHT_BUTTON, HandleRightButton);
    buttons_subscribeListener(LEFT_BUTTON,  HandleLeftButton);

    buttons_setButtonMode(UP_BUTTON,    RISING_EDGE);
    buttons_setButtonMode(DOWN_BUTTON,  RISING_EDGE);
    buttons_setButtonMode(RIGHT_BUTTON, RISING_EDGE);
    buttons_setButtonMode(LEFT_BUTTON,  RISING_EDGE);

    /* Create initial snake... */
    priv_head = (SnakeElement *)malloc(sizeof(SnakeElement));

    if (priv_head != NULL)
    {
        U8 x;

        priv_head->begin = (Point){ INITIAL_SNAKE_X, INITIAL_SNAKE_Y };
        priv_head->dir = DIR_RIGHT;
        priv_head->length = INITIAL_SNAKE_LENGTH;
        priv_head->next = NULL;

        priv_tail = priv_head; //Initially only one segment.

        /* Set up the game squares with the first segment. */
        for (x = INITIAL_SNAKE_X; x > (INITIAL_SNAKE_X - INITIAL_SNAKE_LENGTH); x--)
        {
            SET_SQUARE_VALUE(x, INITIAL_SNAKE_Y);
        }

        /* Set up the first food element. */
        createFood();
    }
    else
    {
        /* TODO : Create proper error manager.*/
        MessageBox_Show("Memory fault", 20000u);
    }

#ifdef ENABLE_EXTRA
    {
        SnakeExtra_setRectangle(&priv_xRect);
        SnakeExtra_start();
    }
#endif
}


/* TODO : Current interval of 100ms is arbitrarily chosen. */
Public void snake_cyclic50ms(void)
{
    static U8 cycle_counter = 0u;

    if (priv_isGameOver)
    {
        priv_isGameOver = FALSE;
        returnToMain(); //Return to main menu.
        return;
    }

    if (++cycle_counter <= priv_snake_speed)
    {
        return;
    }
    else
    {
        cycle_counter = 0u;
    }

    moveHeadForward();

    /* Check for collisions */
    if (isCollision())
    {
        handleGameOver();
        return;
    }

    /* Mark the head new location as occupied */
    SET_SQUARE_VALUE(priv_head->begin.x, priv_head->begin.y);

    /* Lets see if we can eat...*/
    if( (priv_head->begin.x == priv_food.x ) &&
        (priv_head->begin.y == priv_food.y ) )
    {
        /* We skip erasing the tail for this round to grow the snake. */
        /* And we create a new food item somewhere. */
        handleEating();
    }
    else
    {
        eraseTail();
    }

    /* TODO : We do not have to redraw both segments each time... */
    drawSnakeElement(priv_head, TRUE);
    drawSnakeElement(priv_tail, TRUE);

    priv_isDirSet = FALSE;
}


Public void snake_stop(void)
{
    /* TODO : Must clean up all resources here. */
    SnakeElement * elem_ptr = priv_tail;
    SnakeElement * prev = NULL;

    while (elem_ptr != NULL)
    {
        prev = elem_ptr;
        elem_ptr = elem_ptr->next;
        free(prev);
    }
}

/* Currently can be between 1-5 */
Public void snake_setSpeed(U16 speed)
{
    if (speed > 5u)
    {
        return;
    }

    speed--;

    priv_snake_speed = (5u - speed);
}

/************************** Private function definitions. *****************************/

Private void drawBackGround(void)
{
    display_drawRectangle(0u, 0u, GAME_BORDER_AREA_Y_PX, GAME_BORDER_AREA_X_PX, GAME_BORDER_WIDTH_PX);

    /* Draw points area for testing. */
    pointsRectangle.location.x = GAME_BORDER_AREA_X_PX + 1u;
    pointsRectangle.location.y = 2u;

    pointsRectangle.size.height = 16u;
    pointsRectangle.size.width = DISPLAY_NUMBER_OF_COLUMNS - GAME_BORDER_AREA_X_PX - 2u;

    display_drawTextBox(&pointsRectangle, "000", FONT_SMALL_FONT);

    display_drawRectangle(GAME_BORDER_AREA_X_PX, 0u, 64u, DISPLAY_NUMBER_OF_COLUMNS - GAME_BORDER_AREA_X_PX, 1u);
}


Private void setSnakeDirection(SnakeDirection request)
{
    if ((priv_head->dir != request)                             &&
        !(request == DIR_DOWN && priv_head->dir == DIR_UP)      &&
        !(request == DIR_UP   && priv_head->dir == DIR_DOWN)    &&
        !(request == DIR_LEFT && priv_head->dir == DIR_RIGHT)   &&
        !(request == DIR_LEFT && priv_head->dir == DIR_RIGHT))
   {
        if (priv_isDirSet == FALSE)
        {
            priv_isDirSet = TRUE;

            SnakeElement * new_elem = (SnakeElement*)malloc(sizeof(SnakeElement));
            if (new_elem != NULL)
            {
                /* So we begin inside the old segment. */
                new_elem->begin.x = priv_head->begin.x;
                new_elem->begin.y = priv_head->begin.y;
                new_elem->length = 0u;  /* Lets hope the zero length won't cause issues... */
                new_elem->dir = request;
                new_elem->next = NULL;

                priv_head->next = new_elem;
                priv_head = new_elem;
            }
            else
            {
                /* TODO : Add fault handling. */
            }
        }
   }
}


/* This is needed, because we use 4 pixels for 1 point. */
Private void drawPoint(Point p, Boolean value)
{
    Point pxl;

    pxl.x = p.x * 2;
    pxl.y = p.y * 2;

    display_setPixel(pxl.x,     pxl.y,     value);
    display_setPixel(pxl.x + 1, pxl.y,     value);
    display_setPixel(pxl.x,     pxl.y + 1, value);
    display_setPixel(pxl.x + 1, pxl.y + 1, value);
}


/* Erase the last segment of the tail */
Private void eraseTail(void)
{
    Point tailPoint = getTailOfElement(priv_tail);
    drawPoint(tailPoint, FALSE);

    priv_tail->length--;

    if (priv_tail->length == 0u)
    {
        SnakeElement * prev = priv_tail;
        priv_tail = priv_tail->next;
        free(prev);
    }

    /* Mark the square as no longer occupied. */
    CLR_SQUARE_VALUE(tailPoint.x, tailPoint.y);
}


/* Move head segment forward */
Private void moveHeadForward(void)
{
    switch(priv_head->dir)
    {
    case DIR_UP:
        priv_head->begin.y--;
        break;
    case DIR_DOWN:
        priv_head->begin.y++;
        break;
    case DIR_LEFT:
        priv_head->begin.x--;
        break;
    case DIR_RIGHT:
        priv_head->begin.x++;
        break;
    default:
        break;
        //Should not happen.
    }

    priv_head->length++;

    /* Mark the new square as occupied */
    //SET_SQUARE_VALUE(priv_head->begin.x, priv_head->begin.y);
    /* Cannot do that here as that would cause collision with self. */
}


Private void drawSnakeElement(SnakeElement * elem, Boolean isBlack)
{
    Point BeginPixel;
    Point EndPixel;
    Point end;  /* This is in the game coordinate system. */
    Point head; /* This is in the game coordinate system.  */

    if (elem == NULL)
    {
        return;
    }

    end = getTailOfElement(elem);
    /* Perform sanity check. */
    /* If element is out of bounds, then we still draw it, but cut off the out of bounds part. */

    head.x = MIN(elem->begin.x , MAX_COORD_X);
    head.y = MIN(elem->begin.y , MAX_COORD_Y);
    end.x = MIN(end.x, MAX_COORD_X);
    end.y = MIN(end.y, MAX_COORD_Y);


    /* Initialize these to top left pixels. */
    BeginPixel.x = head.x * 2;
    BeginPixel.y = head.y * 2;

    EndPixel.x = end.x * 2;
    EndPixel.y = end.y * 2;

    /* Initial try  - Probably can do this better. */
    switch(elem->dir)
    {
    case DIR_UP:
        EndPixel.y++;
        display_drawLine(BeginPixel, EndPixel, isBlack);
        BeginPixel.x++;
        EndPixel.x++;
        display_drawLine(BeginPixel, EndPixel, isBlack);
        break;
    case DIR_DOWN:
        BeginPixel.y++;
        display_drawLine(BeginPixel, EndPixel, isBlack);
        BeginPixel.x++;
        EndPixel.x++;
        display_drawLine(BeginPixel, EndPixel, isBlack);
        break;
    case DIR_LEFT:
        EndPixel.x++;
        display_drawLine(BeginPixel, EndPixel, isBlack);
        BeginPixel.y++;
        EndPixel.y++;
        display_drawLine(BeginPixel, EndPixel, isBlack);
        break;
    case DIR_RIGHT:
        BeginPixel.x++;
        display_drawLine(BeginPixel, EndPixel, isBlack);
        BeginPixel.y++;
        EndPixel.y++;
        display_drawLine(BeginPixel, EndPixel, isBlack);
        break;
    default:
        break;
        //Should not happen.
    }
}

/* Returns end point of segment in the snake coordinate system. */
Private Point getTailOfElement(const SnakeElement * elem)
{
    Point res;

    switch(elem->dir)
    {
        case DIR_UP:
            res.x = elem->begin.x;
            res.y = elem->begin.y + elem->length;
            break;
        case DIR_DOWN:
            res.x = elem->begin.x;
            res.y = elem->begin.y - elem->length;
            break;
        case DIR_LEFT:
            res.x = elem->begin.x + elem->length;
            res.y = elem->begin.y;
            break;
        case DIR_RIGHT:
            res.x = elem->begin.x - elem->length;
            res.y = elem->begin.y;

            break;
    default:
        //Really should not happen.
        return (Point){0,0};
    }

    return res;
}


Private Boolean isCollision(void)
{
    Boolean res = FALSE;

    /* Check for collision with border. */
    if (priv_head->begin.x < GAME_BORDER_WIDTH)
    {
        res = TRUE;
    }

    if (priv_head->begin.x > (MAX_COORD_X - GAME_BORDER_WIDTH))
    {
        res = TRUE;
    }

    if (priv_head->begin.y < GAME_BORDER_WIDTH)
    {
        res =  TRUE;
    }

    if (priv_head->begin.y > (MAX_COORD_Y - GAME_BORDER_WIDTH))
    {
        res = TRUE;
    }

    /* Check for collision with objects or snake itself. */
    if (GET_SQUARE_VALUE(priv_head->begin.x, priv_head->begin.y) == 1u)
    {
        res = TRUE;
    }

    return res;
}


Private void handleEating(void)
{
    char scoreString[6];
    char * ps = scoreString;
    /* Creates new food block randomly */
    createFood();

    /* Increment the score by 5 points */
    priv_score += 5u;

    /* Simple way of handling leading zeroes.. */
    if (priv_score < 10u)
    {
        ps[0] = '0';
        ps[1] = '0';
        ps +=2;
    }
    else if(priv_score < 100u)
    {
        ps[0] = '0';
        ps++;
    }

    long2string(priv_score, ps);
    display_drawTextBox(&pointsRectangle, scoreString, FONT_SMALL_FONT);

#ifdef ENABLE_EXTRA
    SnakeExtraIncreaseScore(priv_score);
#endif
}


Private void createFood(void)
{
    Point p = getRandomFreePoint();

    if ((p.x < GAME_AREA_X_SIZE) && (p.y < GAME_AREA_Y_SIZE))
    {
        priv_food.x = p.x;
        priv_food.y = p.y;

        drawPoint(p, TRUE);
    }
}


Private Point getRandomFreePoint(void)
{
    U16 x, y, ix;

    Point p;

    /* We use 4 tries to get a free random point on the board. */
    for (ix = 0u; ix < 4u; ix++)
    {
        x = generate_random_number_rng(1u, MAX_COORD_X - 1u);
        y = generate_random_number_rng(1u, MAX_COORD_Y - 1u);

        p.x = x;
        p.y = y;

        if (GET_SQUARE_VALUE(p.x, p.y) == 0u)
        {
            return p;
        }
    }

    /* Looks like all were full... */
    /* Lets search for the next free point */
    for (; p.x < MAX_COORD_X; p.x++)
    {
        for (; p.y < MAX_COORD_Y; p.y++)
        {
            if (GET_SQUARE_VALUE(p.x, p.y) == 0u)
            {
                return p;
            }
        }
    }

    /* Very special case, we have to start from the beginning... */
    for (p.x = 1u; p.x < MAX_COORD_X; p.x++)
    {
        for (p.y = 1u; p.y < MAX_COORD_Y; p.y++)
        {
            if (GET_SQUARE_VALUE(p.x, p.y) == 0u)
            {
                return p;
            }
        }
    }

    /* Really should not end up here :) */
    return p;
}


Private void handleGameOver(void)
{
#ifndef DISABLE_BUZZER
        buzzer_playBeeps(2u);
#endif
        MessageBox_SetResponseHandler(handleMessageBoxResponse);
        MessageBox_ShowWithOk("Game over!");
}


Private void handleMessageBoxResponse(MsgBox_Response resp)
{
    /* In reality this is the only type of response that we can get. */
    /* This is really just for testing right now... */
    if (resp == RESPONSE_OK)
    {
        buzzer_playBeeps(1u);
        priv_isGameOver = TRUE;
    }
}


/********** Button Handlers  ********/

Private void HandleUpButton(void)
{
    setSnakeDirection(DIR_UP);
}

Private void HandleDownButton(void)
{
    setSnakeDirection(DIR_DOWN);
}

Private void HandleRightButton(void)
{
    setSnakeDirection(DIR_RIGHT);
}

Private void HandleLeftButton(void)
{
    setSnakeDirection(DIR_LEFT);
}

