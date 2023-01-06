/*
 * clockDisplay.c
 *
 *  Created on: Aug 27, 2017
 *      Author: Joonatan
 */


#include <LOGIC/PowerHourGame/clockDisplay.h>
#include <LOGIC/PowerHourGame/ShotGlassAnimation.h>
#include <LOGIC/PowerHourGame/SpecialTasks.h>
#include "display_drv.h"
#include "buzzer.h"
#include "tumbler.h"
#include "register.h"
#include "buttons.h"
#include "LOGIC/TextTools/MessageBox.h"
#include "LOGIC/main.h"

#define BEERSHOT_X 86
#define BEERSHOT_Y 1


//Text box definitions.
#define UPPER_TEXT_XLOC 2u
#define UPPER_TEXT_YLOC 2u
#define UPPER_TEXT_HEIGHT 13u
#define UPPER_TEXT_WIDTH 80u
#define UPPER_TEXT_FONT FONT_MEDIUM_FONT

#define LOWER_TEXT_XLOC 2u
#define LOWER_TEXT_YLOC 51u
#define LOWER_TEXT_HEIGHT 13u
#define LOWER_TEXT_WIDTH 80u
#define LOWER_TEXT_FONT FONT_MEDIUM_FONT

#define CLOCK_FONT FONT_NUMBERS_HUGE

/*****************************************************************************************************
 *
 * Private type definitions
 *
 *****************************************************************************************************/

typedef enum
{
    CONTROLLER_INIT,
    CONTROLLER_WARNING_TEXT,
    CONTROLLER_WARNING_CONFIRM,
    CONTROLLER_COUNTING,
    CONTROLLER_OVERRIDDEN,
    CONTROLLER_FINAL,
    CONTROLLER_EXITING,
    CONTROLLER_NUMBER_OF_STATES
} controllerState;

typedef enum
{
    BEERSHOT_NO_ACTION,         //No action with bitmap.
    BEERSHOT_EMPTY,             //Draw initial beershot bitmap.
    BEERSHOT_BEGIN_FILLING,     //Initiate filling animation.
    BEERSHOT_FULL,              //Draw full beershot.
    BEERSHOT_BEGIN_EMPTYING,    //Initiate emptying animation.

    OVERRIDE_FUNCTION,          //Overrides ordinary function, displays bitmap instead.
    RESTORE_FUNCTION,           //Restore normal handling.
} beerShotAction;

typedef enum
{
    BEERSHOT_FROZEN,    //Nothing currently to draw.
    BEERSHOT_FILLING,
    BEERSHOT_EMPTYING
} beershotState;


typedef struct
{
    U8 second;                      // When the event should be triggered.
    const char * upperText;         // Text to be written to upper part of display.
    const char * lowerText;         // Text to be written to lower part of display.
    beerShotAction shot_action;     // Action to be performed on bitmap.
    OverrideFunc    func;           // Optional parameter - This function will override all behaviour, used for special actions.
} ControllerEvent;

typedef enum
{
    TASK_CATEGORY_GIRLS,
    TASK_CATEGORY_GUYS,
    TASK_CATEGORY_BOARD,
    TASK_CATEGORY_CORETEAM,
    TASK_CATEGORY_ALUMNI,
    TASK_CATEGORY_PAX,
    TASK_CATEGORY_KT,
    TASK_CATEGORY_SOC,
    NUMBER_OF_TASK_CATEGORIES
} SchedulerTaskCategory;

typedef struct
{
    const ControllerEvent * event_array;
    U8                        event_cnt;
    Tumbler_T             enable_tumblr;
} SchedulerTaskConf_T;

typedef struct
{
    U16 counter;
    Boolean is_enabled;
} SchedulerTaskState_T;

//Just an idea.
typedef struct
{
    const Bitmap * bmp_ptr;
    U8 bmp_x;
    U8 bmp_y;

    const char * text_str;
    U8 text_x;
    U8 text_y;
    FontType text_font;
    Boolean isInverted;
} IntroSequence;


/*****************************************************************************************************
 *
 * Private function Prototypes
 *
 *****************************************************************************************************/

Private void incrementTimer(void);
Private void convertTimerString(timekeeper_struct * t, char * dest_str);
Private void drawBeerShot(beerShotAction action);

Private Boolean genericIntroFunction(const IntroSequence * intro_ptr, U8 sec);

Private Boolean girlsSpecialIntro(U8 sec);
Private Boolean guysSpecialIntro(U8 sec);

Private Boolean alumniSpecialIntro(U8 sec);
Private Boolean boardSpecialIntro(U8 sec);
Private Boolean coreTeamSpecialIntro(U8 sec);
Private Boolean ktSpecialIntro(U8 sec);
Private Boolean paxSpecialIntro(U8 sec);
Private Boolean SocRespSpecialIntro(U8 sec);


Private void doFinalAction(void);
Private U8 selectRandomTaskIndex(void);

Private timekeeper_struct priv_timekeeper;
Private controllerState priv_state;

Private U8 getScheduledSpecialTask(const ControllerEvent ** event_ptr);
Private void setUpperText(const char * str);
Private void setLowerText(const char * str);

Private void clearUpperText(void);
Private void clearLowerText(void);

Private void subscribeButtonHandlers(void);

Private void HandleUpButton(void);
Private void HandleDownButton(void);
Private void HandleRightButton(void);
Private void HandleLeftButton(void);

Private void handleMessageBoxResponse(MsgBox_Response resp);

Private void drawTimer(void);

Private void showWarningText(void);

/*****************************************************************************************************
 *
 * Private variable declarations.
 *
 *****************************************************************************************************/

Private const ControllerEvent priv_initial_event =
{
 .second = 1u,
 .upperText = "Begin!",
 .lowerText = NULL,
 .shot_action = BEERSHOT_EMPTY
};


Private const ControllerEvent priv_normal_minute_events[] =
{
     { .second = 7u,  .upperText = "",              .lowerText = "",        .shot_action = BEERSHOT_EMPTY            , .func = NULL },
     { .second = 20u, .upperText = "Fill shots",    .lowerText = NULL,      .shot_action = BEERSHOT_BEGIN_FILLING    , .func = NULL },
     { .second = 45u, .upperText = "Ready",         .lowerText = NULL,      .shot_action = BEERSHOT_FULL             , .func = NULL },
     { .second = 59u, .upperText = "Proosit!",      .lowerText = "Cheers!", .shot_action = BEERSHOT_BEGIN_EMPTYING   , .func = NULL },
};

Private const ControllerEvent priv_guys_drink_events[] =
{
     { .second = 7u,  .upperText = "",              .lowerText = "",                    .shot_action = OVERRIDE_FUNCTION         , .func = &guysSpecialIntro   },
     { .second = 20u, .upperText = "Fill shots",    .lowerText = "Guys' round",         .shot_action = BEERSHOT_BEGIN_FILLING    , .func = NULL },
     { .second = 45u, .upperText = "Ready",         .lowerText = NULL,                  .shot_action = BEERSHOT_FULL             , .func = NULL },
     { .second = 59u, .upperText = "Proosit!",      .lowerText = "Cheers guys!",        .shot_action = OVERRIDE_FUNCTION         , .func = &guysSpecialTask    },
};

Private const ControllerEvent priv_girls_drink_events[] =
{
     { .second = 7u,  .upperText = "",              .lowerText = "",                    .shot_action = OVERRIDE_FUNCTION         , .func = &girlsSpecialIntro   },
     { .second = 20u, .upperText = "Fill shots",    .lowerText = "Girls round",         .shot_action = BEERSHOT_BEGIN_FILLING    , .func = NULL },
     { .second = 45u, .upperText = "Ready",         .lowerText = NULL,                  .shot_action = BEERSHOT_FULL             , .func = NULL },
     { .second = 59u, .upperText = "Proosit!",      .lowerText = "Cheers girls!",       .shot_action = OVERRIDE_FUNCTION         , .func = &girlsSpecialTask    },
};

Private const ControllerEvent priv_board_drink_events[] =
{
     { .second = 7u,  .upperText = "",              .lowerText = "",                    .shot_action = OVERRIDE_FUNCTION         , .func = boardSpecialIntro    },
     { .second = 20u, .upperText = "Fill shots",    .lowerText = "Board Round",         .shot_action = BEERSHOT_BEGIN_FILLING    , .func = NULL                 },
     { .second = 45u, .upperText = "Ready",         .lowerText = NULL,                  .shot_action = BEERSHOT_FULL             , .func = NULL                 },
     { .second = 59u, .upperText = "Proosit!",      .lowerText = "Cheers boardies!",    .shot_action = OVERRIDE_FUNCTION         , .func = boardSpecialTask     },
};

Private const ControllerEvent priv_cteam_drink_events[] =
{
     { .second = 7u,  .upperText = "",              .lowerText = "",                    .shot_action = OVERRIDE_FUNCTION         , .func = coreTeamSpecialIntro },
     { .second = 20u, .upperText = "Fill shots",    .lowerText = "CTeam Round",         .shot_action = BEERSHOT_BEGIN_FILLING    , .func = NULL                 },
     { .second = 45u, .upperText = "Ready",         .lowerText = NULL,                  .shot_action = BEERSHOT_FULL             , .func = NULL                 },
     { .second = 59u, .upperText = "Proosit!",      .lowerText = "Cheers Core Team!",   .shot_action = OVERRIDE_FUNCTION         , .func = coreTeamSpecialTask  },
};

Private const ControllerEvent priv_alumni_drink_events[] =
{
     { .second = 7u,  .upperText = "",              .lowerText = "",                    .shot_action = OVERRIDE_FUNCTION         , .func = alumniSpecialIntro   },
     { .second = 20u, .upperText = "Fill shots",    .lowerText = "Alumni Round",        .shot_action = BEERSHOT_BEGIN_FILLING    , .func = NULL                 },
     { .second = 45u, .upperText = "Ready",         .lowerText = NULL,                  .shot_action = BEERSHOT_FULL             , .func = NULL                 },
     { .second = 59u, .upperText = "Proosit!",      .lowerText = "Cheers Alumni!",      .shot_action = OVERRIDE_FUNCTION         , .func = alumniSpecialTask    },
};

Private const ControllerEvent priv_pax_drink_events[] =
{
     { .second = 7u,  .upperText = "",              .lowerText = "",                    .shot_action = OVERRIDE_FUNCTION         , .func = paxSpecialIntro      },
     { .second = 20u, .upperText = "Fill shots",    .lowerText = "PAX Round",           .shot_action = BEERSHOT_BEGIN_FILLING    , .func = NULL                 },
     { .second = 45u, .upperText = "Ready",         .lowerText = NULL,                  .shot_action = BEERSHOT_FULL             , .func = NULL                 },
     { .second = 59u, .upperText = "Proosit!",      .lowerText = "Cheers PAX!",         .shot_action = OVERRIDE_FUNCTION         , .func = PaxSpecialTask       },
};

Private const ControllerEvent priv_kt_drink_events[] =
{
     { .second = 7u,  .upperText = "",              .lowerText = "",                    .shot_action = OVERRIDE_FUNCTION         , .func = ktSpecialIntro       },
     { .second = 20u, .upperText = "Fill shots",    .lowerText = "KT Round",            .shot_action = BEERSHOT_BEGIN_FILLING    , .func = NULL                 },
     { .second = 45u, .upperText = "Ready",         .lowerText = NULL,                  .shot_action = BEERSHOT_FULL             , .func = NULL                 },
     { .second = 59u, .upperText = "Proosit!",      .lowerText = "Paragrahv 5!",        .shot_action = OVERRIDE_FUNCTION         , .func = KtSpecialTask        },
};

Private const ControllerEvent priv_soc_drink_events[] =
{
     { .second = 7u,  .upperText = "",              .lowerText = "",                    .shot_action = OVERRIDE_FUNCTION         , .func = SocRespSpecialIntro  },
     { .second = 20u, .upperText = "Fill shots",    .lowerText = "SocResp Round",       .shot_action = BEERSHOT_BEGIN_FILLING    , .func = NULL                 },
     { .second = 45u, .upperText = "Ready",         .lowerText = NULL,                  .shot_action = BEERSHOT_FULL             , .func = NULL                 },
     { .second = 59u, .upperText = "Proosit!",      .lowerText = "Cheers Soc!",         .shot_action = OVERRIDE_FUNCTION         , .func = socRespSpecialTask   },
};


/* This is a scheduler for special minutes.
 * It contains data about the frequency and offset of special minutes as well
 * as links to their respective actions. */
Private const SchedulerTaskConf_T priv_scheduler_conf[NUMBER_OF_TASK_CATEGORIES] =
{
     {.event_array = priv_girls_drink_events,   .event_cnt = NUMBER_OF_ITEMS(priv_girls_drink_events),  .enable_tumblr = TUMBLER_UPPER_2  },       /*TASK_CATEGORY_GIRLS     */
     {.event_array = priv_guys_drink_events,    .event_cnt = NUMBER_OF_ITEMS(priv_guys_drink_events),   .enable_tumblr = TUMBLER_UPPER_3  },       /*TASK_CATEGORY_GUYS      */
     {.event_array = priv_board_drink_events,   .event_cnt = NUMBER_OF_ITEMS(priv_board_drink_events),  .enable_tumblr = TUMBLER_UPPER_0  },       /*TASK_CATEGORY_BOARD     */
     {.event_array = priv_cteam_drink_events,   .event_cnt = NUMBER_OF_ITEMS(priv_cteam_drink_events),  .enable_tumblr = TUMBLER_UPPER_1  },       /*TASK_CATEGORY_CORETEAM  */
     {.event_array = priv_alumni_drink_events,  .event_cnt = NUMBER_OF_ITEMS(priv_alumni_drink_events), .enable_tumblr = TUMBLER_LOWER_0  },       /*TASK_CATEGORY_ALUMNI    */
     {.event_array = priv_pax_drink_events,     .event_cnt = NUMBER_OF_ITEMS(priv_pax_drink_events),    .enable_tumblr = TUMBLER_LOWER_1  },       /*TASK_CATEGORY_PAX       */
     {.event_array = priv_kt_drink_events,      .event_cnt = NUMBER_OF_ITEMS(priv_kt_drink_events),     .enable_tumblr = TUMBLER_LOWER_2  },       /*TASK_CATEGORY_KT        */
     {.event_array = priv_soc_drink_events,     .event_cnt = NUMBER_OF_ITEMS(priv_soc_drink_events),    .enable_tumblr = TUMBLER_LOWER_3  },       /*TASK_CATEGORY_SOC       */
};

Private SchedulerTaskState_T priv_scheduler_state[NUMBER_OF_TASK_CATEGORIES];

Private beershotState priv_beer_state;
Private Rectangle priv_timer_rect;
Private OverrideFunc priv_override_ptr;
Private char priv_timer_str[10];
Private U8 priv_override_counter;
Private U8 priv_task_frequency = 3u; /* Default value is a task every 3 minutes. */
Private Boolean priv_isFirstRun = TRUE;

/*****************************************************************************************************
 *
 * Public function definitions
 *
 *****************************************************************************************************/


Public void clockDisplay_init(void)
{
    U8 font_height;

    priv_timekeeper.minute = 0u;
    priv_timekeeper.second = 0u;

    font_height = font_getFontHeight(CLOCK_FONT);

    priv_timer_rect.location.x = 1u;

    priv_timer_rect.location.y = NUMBER_OF_ROWS >> 1u; //Divide by 2.
    priv_timer_rect.location.y -= (font_height >> 1u);

    priv_timer_rect.size.height = font_height;
    priv_timer_rect.size.width = 15u * 5u;

    priv_state = CONTROLLER_INIT;

}

volatile U32 priv_safety_button_timer = 0u;

Public void clockDisplay_start(void)
{
    U8 ix;

    display_clear();

    /* Set up critical variables, as we might want to restart the game. */
    priv_timekeeper.minute = 0u;
    priv_timekeeper.second = 0u;
    priv_state = CONTROLLER_INIT;

    /* Set up buttons */
    subscribeButtonHandlers();

    //We start counting.
    priv_isFirstRun = TRUE;
    priv_state = CONTROLLER_WARNING_TEXT;

    for (ix = 0u; ix < NUMBER_OF_TASK_CATEGORIES; ix++)
    {
        priv_scheduler_state[ix].counter = 0u;
    }
}


/* Should return all variables to their initial states. */
Public void clockDisplay_stop(void)
{
    priv_state = CONTROLLER_INIT;
    priv_timekeeper.minute = 0u;
    priv_timekeeper.second = 0u;
}


Public void clockDisplay_cyclic1000msec(void)
{
    U8 ix;
    const ControllerEvent * event_ptr = NULL;
    beerShotAction action = BEERSHOT_NO_ACTION;

    static const ControllerEvent * currentMinuteEvents_ptr = priv_normal_minute_events;
    static U8 controllerEvents_cnt = NUMBER_OF_ITEMS(priv_normal_minute_events);

    priv_safety_button_timer++;

    /* TODO : This should be changed to a better implementation. */
    if (priv_timekeeper.second == 59u)
    {
        //buzzer_playBuzzer(10u);
        buzzer_playBeeps(3u);
    }

    //Game ends and we enter final state.
    if (priv_timekeeper.minute == 60u)
    {
        doFinalAction();
        priv_state = CONTROLLER_FINAL;
    }

    switch(priv_state)
    {
    case CONTROLLER_INIT:
        //We do not do anything here.
        break;
    case CONTROLLER_WARNING_TEXT:
        /* Display the text here. */
        priv_safety_button_timer = 0u;
        showWarningText();
        priv_state = CONTROLLER_WARNING_CONFIRM;
        break;
    case CONTROLLER_WARNING_CONFIRM:
        if ((priv_safety_button_timer) > 60u || isGreenSafetyBtn())
        {
            display_clear();
            priv_state = CONTROLLER_COUNTING;
        }
        break;
    case CONTROLLER_COUNTING:
        if ((priv_timekeeper.second == 0u) && (priv_timekeeper.minute > 0u))
        {
            controllerEvents_cnt = getScheduledSpecialTask(&currentMinuteEvents_ptr);
        }

        incrementTimer();
        if (priv_isFirstRun)
        {
            event_ptr = &priv_initial_event;
            priv_isFirstRun = FALSE;
        }
        else
        {
            for (ix = 0u; ix < controllerEvents_cnt; ix++)
            {
                event_ptr = &currentMinuteEvents_ptr[ix];
                if (event_ptr->second == priv_timekeeper.second)
                {
                    break;
                }
                event_ptr = NULL;
            }
        }

        if (event_ptr != NULL)
        {
           if (event_ptr->upperText != NULL)
           {
               setUpperText(event_ptr->upperText);
           }

           if (event_ptr->lowerText != NULL)
           {
               setLowerText(event_ptr->lowerText);
           }

           action = event_ptr->shot_action;
        }

        //Currently we still finish this cycle and special handling begins on the next.
        if (action == OVERRIDE_FUNCTION)
        {
            priv_state = CONTROLLER_OVERRIDDEN;
            priv_override_ptr = event_ptr->func;
            priv_override_counter = 0u;
        }

        drawBeerShot(action);

        drawTimer();

        break;
    case CONTROLLER_OVERRIDDEN:
        if ((priv_timekeeper.second == 0u) && (priv_timekeeper.minute > 0u))
        {
            controllerEvents_cnt = getScheduledSpecialTask(&currentMinuteEvents_ptr);
        }

        //We still increment timer.
        incrementTimer();

        if (priv_override_ptr(priv_override_counter))
        {
            display_clear();
            drawTimer();
            drawBeerShot(BEERSHOT_EMPTY);
            priv_state = CONTROLLER_COUNTING;
        }
        priv_override_counter++;
        break;
    case CONTROLLER_FINAL:
        break;
    case CONTROLLER_EXITING:
        returnToMain();
        break;
    default:
        break;
    }
}

Public void clockDisplay_setTaskFrequency(U16 freq)
{
    priv_task_frequency = freq;
}

Public U16 clockDisplay_getTaskFrequency(void)
{
    return (U16)priv_task_frequency;
}

/********* Private function definitions **********/
Private void doFinalAction(void)
{
    display_clear();
    display_drawString("Game Over!", 20u, 15u, FONT_LARGE_FONT, FALSE);
    display_drawString("  Congratulations! \n You are now drunk", 5u, 37u, FONT_MEDIUM_FONT, FALSE);
}


//Increments timekeeper with 1 second.
Private void incrementTimer(void)
{
    priv_timekeeper.second++;
    if (priv_timekeeper.second >= 60u)
    {
        priv_timekeeper.second = 0u;
        priv_timekeeper.minute++;
    }
}

Private void convertTimerString(timekeeper_struct * t, char * dest_str)
{
    dest_str[0] = '0' + (t->minute / 10u);
    dest_str[1] = '0' + (t->minute % 10u);
    dest_str[2] = ':';
    dest_str[3] = '0' + (t->second / 10u);
    dest_str[4] = '0' + (t->second % 10u);
    dest_str[5] = 0;
}


Private void drawBeerShot(beerShotAction action)
{
   const Bitmap * bmp_ptr = NULL;

   switch(action)
   {
       case BEERSHOT_EMPTY:
           bmp_ptr = ShotGlassAnimation_GetFirst();
           priv_beer_state = BEERSHOT_FROZEN;
           break;
       case BEERSHOT_FULL:
           bmp_ptr = ShotGlassAnimation_GetLast();
           priv_beer_state = BEERSHOT_FROZEN;
           break;
       case BEERSHOT_BEGIN_FILLING:
           bmp_ptr = ShotGlassAnimation_GetFirst();
           priv_beer_state = BEERSHOT_FILLING;
           break;
       case BEERSHOT_BEGIN_EMPTYING:
           bmp_ptr = ShotGlassAnimation_GetLast();
           priv_beer_state = BEERSHOT_EMPTYING;
           break;
       case BEERSHOT_NO_ACTION:
       default:
           break;
   }

   switch(priv_beer_state)
   {
       case(BEERSHOT_FROZEN):
           //Nothing new to draw.
           break;
       case(BEERSHOT_EMPTYING):
           if(bmp_ptr == NULL)
           {
               bmp_ptr = ShotGlassAnimation_GetPrev();
           }
           break;
       case(BEERSHOT_FILLING):
           if(bmp_ptr == NULL)
           {
               bmp_ptr = ShotGlassAnimation_GetNext();
           }
           break;
       default:
           break;
   }

   //If bmp_ptr is not NULL, then we have something to draw.
   if (bmp_ptr != NULL)
   {
       display_drawBitmap(bmp_ptr, BEERSHOT_X, BEERSHOT_Y, FALSE);
   }
}


//Return default normal cycle if no special events are scheduled.
Private U8 getScheduledSpecialTask(const ControllerEvent ** event_ptr)
{
    U8 ix;
    U8 res;

    Boolean is_any_enabled = FALSE;

    /* First we have to check if we have any tasks enabled at all... */

    for (ix = 0u; ix < NUMBER_OF_TASK_CATEGORIES; ix++)
    {
        if (tumbler_getState(priv_scheduler_conf[ix].enable_tumblr))
        {
            priv_scheduler_state[ix].is_enabled = TRUE;
            is_any_enabled = TRUE;
        }
        else
        {
            priv_scheduler_state[ix].is_enabled = FALSE;
        }
    }

    if (((priv_timekeeper.minute % priv_task_frequency) == 0u) && (is_any_enabled == TRUE))
    {
        ix = selectRandomTaskIndex();
        *event_ptr = priv_scheduler_conf[ix].event_array;
        res = priv_scheduler_conf[ix].event_cnt;
    }
    else
    {
        *event_ptr = priv_normal_minute_events;
        res = NUMBER_OF_ITEMS(priv_normal_minute_events);
    }

    return res;
}


/* It returns a random enabled task. However the function should prefer tasks that have not yet been displayed or have been displayed less than others. */
Private U8 selectRandomTaskIndex(void)
{
    U8 max_count = 0u;
    U8 min_count = 0xffu;
    U8 ix;
    U8 res = 0u;

    U8 index_array[NUMBER_OF_TASK_CATEGORIES];
    U8 index_length = 0u;

    /* Lets first establish the MAX and MIN count that we have. */
    for (ix = 0u; ix < NUMBER_OF_TASK_CATEGORIES; ix++)
    {
        if (priv_scheduler_state[ix].is_enabled)
        {
            max_count = MAX(priv_scheduler_state[ix].counter, max_count);
            min_count = MIN(priv_scheduler_state[ix].counter, min_count);
        }
    }

    if (max_count == min_count)
    {
        /* We have no preference in this case. We add all indexes that are enabled to the array. */
        for (ix = 0u; ix < NUMBER_OF_TASK_CATEGORIES; ix++)
        {
            if (priv_scheduler_state[ix].is_enabled)
            {
                index_array[index_length] = ix;
                index_length++;
            }
        }

        max_count++;
    }
    else
    {
        /* We prefer tasks that have not been selected yet. */
        for (ix = 0u; ix < NUMBER_OF_TASK_CATEGORIES; ix++)
        {
            if (priv_scheduler_state[ix].is_enabled && (priv_scheduler_state[ix].counter < max_count))
            {
                index_array[index_length] = ix;
                index_length++;
            }
        }
    }

    res = index_array[generate_random_number(index_length - 1u)];


    /* We set the selected count to the max. */
    priv_scheduler_state[res].counter = max_count;

    return res;
}


Private void drawTimer(void)
{
    convertTimerString(&priv_timekeeper, priv_timer_str);

    display_fillRectangle(priv_timer_rect.location.x,
                          priv_timer_rect.location.y,
                          priv_timer_rect.size.height,
                          priv_timer_rect.size.width,
                          PATTERN_WHITE);

    display_drawString(priv_timer_str,
                       priv_timer_rect.location.x,
                       priv_timer_rect.location.y,
                       CLOCK_FONT,
                       FALSE);
}



Private void setUpperText(const char * str)
{
    clearUpperText();
    display_drawString(str, UPPER_TEXT_XLOC, UPPER_TEXT_YLOC, UPPER_TEXT_FONT, FALSE);
}

Private void clearUpperText(void)
{
    display_fillRectangle(UPPER_TEXT_XLOC, UPPER_TEXT_YLOC, UPPER_TEXT_HEIGHT, UPPER_TEXT_WIDTH, PATTERN_WHITE);
}

Private void setLowerText(const char * str)
{
    clearLowerText();
    display_drawString(str, LOWER_TEXT_XLOC, LOWER_TEXT_YLOC, LOWER_TEXT_FONT, FALSE);
}

Private void clearLowerText(void)
{
    display_fillRectangle(LOWER_TEXT_XLOC, LOWER_TEXT_YLOC, LOWER_TEXT_HEIGHT, LOWER_TEXT_WIDTH, PATTERN_WHITE);
}


Private void showWarningText(void)
{
    display_clear();

    /* Display the Warning. */
    display_drawString("Warning! The makers of this",           0u, 0u,     FONT_ARIAL_TINY, FALSE);
    display_drawString("device are not responsible",            0u, 11u,    FONT_ARIAL_TINY, FALSE);
    display_drawString("for injury, drunkenness and",           0u, 22u,    FONT_ARIAL_TINY, FALSE);
    display_drawString("/or nudity that may occur.",            0u, 33u,    FONT_ARIAL_TINY, FALSE);
    display_drawString("Press LAUNCH to continue",              0u, 44u,    FONT_ARIAL_TINY, FALSE);
    display_drawString("You have been warned!  ",               0u, 55u,    FONT_ARIAL_TINY, FALSE);
}

/*****************************************************************************************************
 *
 * Intro functions.
 *
 *****************************************************************************************************/

Private Boolean genericIntroFunction(const IntroSequence * intro_ptr, U8 sec)
{
    Boolean res = FALSE;

    switch(sec)
    {
    case(1u):
        display_clear();
        display_drawBitmap(intro_ptr->bmp_ptr, intro_ptr->bmp_x, intro_ptr->bmp_y, intro_ptr->isInverted);
        break;
    case(2u):
        display_drawString(intro_ptr->text_str, intro_ptr->text_x, intro_ptr->text_y, intro_ptr->text_font, FALSE);
        break;
    case(10u):
        res = TRUE;
    break;
    default:
        break;
    }

    return res;
}


Private const IntroSequence priv_guys_intros[] =
{
     {.bmp_ptr = &strong_dude_bitmap, .bmp_x = 0u, .bmp_y = 0u, .text_str = "Guys Round!", .text_x = 58u, .text_y = 4u, .text_font = FONT_MEDIUM_FONT , .isInverted = FALSE },
     {.bmp_ptr = &chad_bitmap,        .bmp_x = 0u, .bmp_y = 0u, .text_str = "Guys Round!", .text_x = 50u, .text_y = 4u, .text_font = FONT_MEDIUM_FONT , .isInverted = FALSE },
     {.bmp_ptr = &man3_bitmap,        .bmp_x = 0u, .bmp_y = 0u, .text_str = "Guys Round!", .text_x = 20u, .text_y = 4u, .text_font = FONT_MEDIUM_FONT , .isInverted = TRUE  },
     {.bmp_ptr = &dude4_bitmap,       .bmp_x = 0u, .bmp_y = 0u, .text_str = "Guys Round!", .text_x = 50u, .text_y = 4u, .text_font = FONT_MEDIUM_FONT , .isInverted = TRUE  },
};


Private Boolean guysSpecialIntro(U8 sec)
{
    static const IntroSequence * intro_ptr = &priv_guys_intros[0];
    static U8 intro_ix;

    if (sec == 1u)
    {
        intro_ptr = &priv_guys_intros[intro_ix];
        intro_ix++;
        if (intro_ix >= NUMBER_OF_ITEMS(priv_guys_intros))
        {
            intro_ix = 0u;
        }
    }

    return genericIntroFunction(intro_ptr, sec);
}


Private const IntroSequence priv_girl_intros[] =
{
     { .bmp_ptr = &girl_1_bitmap,        .bmp_x = 0u, .bmp_y = 0u, .text_str = "Girls round!", .text_x = 50u, .text_y = 4u, .text_font = FONT_MEDIUM_FONT , .isInverted = FALSE},
     { .bmp_ptr = &girl_2_bitmap,        .bmp_x = 6u, .bmp_y = 0u, .text_str = "Girls round!", .text_x = 50u, .text_y = 4u, .text_font = FONT_MEDIUM_FONT , .isInverted = FALSE},
     { .bmp_ptr = &girl_3_bitmap,        .bmp_x = 6u, .bmp_y = 0u, .text_str = "Girls round!", .text_x = 50u, .text_y = 4u, .text_font = FONT_MEDIUM_FONT, .isInverted = TRUE },
     { .bmp_ptr = &girl_9_bitmap,        .bmp_x = 0u, .bmp_y = 0u, .text_str = "Girls round!", .text_x = 50u, .text_y = 4u, .text_font = FONT_MEDIUM_FONT, .isInverted = TRUE },
     { .bmp_ptr = &girl_Sasha_bitmap,    .bmp_x = 0u, .bmp_y = 0u, .text_str = "Girls round!", .text_x = 10u, .text_y = 4u, .text_font = FONT_MEDIUM_FONT, .isInverted = TRUE },
};

//We start displaying a special task.
Private Boolean girlsSpecialIntro(U8 sec)
{
    static const IntroSequence * intro_ptr = &priv_girl_intros[0];
    static U8 intro_ix;

    if (sec == 1u)
    {
        intro_ptr = &priv_girl_intros[intro_ix];
        intro_ix++;
        if (intro_ix >= NUMBER_OF_ITEMS(priv_girl_intros))
        {
            intro_ix = 0u;
        }
    }

    return genericIntroFunction(intro_ptr, sec);
}

Private const IntroSequence priv_alumni_intros[] =
{
     { .bmp_ptr = &alumni1_bitmap,  .bmp_x = 0u, .bmp_y = 0u, .text_str = "Alumni round!", .text_x = 4u , .text_y = 40u, .text_font = FONT_MEDIUM_FONT , .isInverted = TRUE },
     { .bmp_ptr = &alumni2_bitmap,  .bmp_x = 0u, .bmp_y = 0u, .text_str = "Alumni round!", .text_x = 40u, .text_y = 4u,  .text_font = FONT_MEDIUM_FONT , .isInverted = TRUE },
     { .bmp_ptr = &alumni3_bitmap,  .bmp_x = 0u, .bmp_y = 0u, .text_str = "Alumni round!", .text_x = 40u, .text_y = 4u,  .text_font = FONT_MEDIUM_FONT , .isInverted = TRUE },
};


Private Boolean alumniSpecialIntro(U8 sec)
{
    static const IntroSequence * intro_ptr = &priv_alumni_intros[0];
    static U8 intro_ix;

    if (sec == 1u)
    {
        intro_ptr = &priv_alumni_intros[intro_ix];
        intro_ix++;
        if (intro_ix >= NUMBER_OF_ITEMS(priv_alumni_intros))
        {
            intro_ix = 0u;
        }
    }

    return genericIntroFunction(intro_ptr, sec);
}

Private const IntroSequence priv_board_intros[] =
{
     { .bmp_ptr = &board1_bitmap,  .bmp_x = 0u, .bmp_y = 0u, .text_str = "Board round!", .text_x = 40u, .text_y = 4u, .text_font = FONT_MEDIUM_FONT , .isInverted = TRUE },
     { .bmp_ptr = &board2_bitmap,  .bmp_x = 0u, .bmp_y = 0u, .text_str = "Board round!", .text_x = 40u, .text_y = 4u, .text_font = FONT_MEDIUM_FONT , .isInverted = TRUE },
};


Private Boolean boardSpecialIntro(U8 sec)
{
    static const IntroSequence * intro_ptr = &priv_board_intros[0];
    static U8 intro_ix;

    if (sec == 1u)
    {
        intro_ptr = &priv_board_intros[intro_ix];
        intro_ix++;
        if (intro_ix >= NUMBER_OF_ITEMS(priv_board_intros))
        {
            intro_ix = 0u;
        }
    }

    return genericIntroFunction(intro_ptr, sec);
}

Private const IntroSequence priv_coreteam_intros[] =
{
     { .bmp_ptr = &coreteam1_bitmap,  .bmp_x = 0u, .bmp_y = 0u, .text_str = "Core Team round!", .text_x = 20u, .text_y = 4u, .text_font = FONT_MEDIUM_FONT , .isInverted = TRUE },
};

Private Boolean coreTeamSpecialIntro(U8 sec)
{
    static const IntroSequence * intro_ptr = &priv_coreteam_intros[0];
    static U8 intro_ix;

    if (sec == 1u)
    {
        intro_ptr = &priv_coreteam_intros[intro_ix];
        intro_ix++;
        if (intro_ix >= NUMBER_OF_ITEMS(priv_coreteam_intros))
        {
            intro_ix = 0u;
        }
    }

    return genericIntroFunction(intro_ptr, sec);
}

Private const IntroSequence priv_kt_intros[] =
{
     { .bmp_ptr = &kt1_bitmap,  .bmp_x = 0u, .bmp_y = 0u, .text_str = "KT round!", .text_x = 40u, .text_y = 4u, .text_font = FONT_MEDIUM_FONT , .isInverted = TRUE },
     { .bmp_ptr = &kt2_bitmap,  .bmp_x = 0u, .bmp_y = 0u, .text_str = "KT round!", .text_x = 40u, .text_y = 4u, .text_font = FONT_MEDIUM_FONT , .isInverted = TRUE },
};


Private Boolean ktSpecialIntro(U8 sec)
{
    static const IntroSequence * intro_ptr = &priv_kt_intros[0];
    static U8 intro_ix;

    if (sec == 1u)
    {
        intro_ptr = &priv_kt_intros[intro_ix];
        intro_ix++;
        if (intro_ix >= NUMBER_OF_ITEMS(priv_kt_intros))
        {
            intro_ix = 0u;
        }
    }

    return genericIntroFunction(intro_ptr, sec);
}

Private const IntroSequence priv_pax_intros[] =
{
     { .bmp_ptr = &pax1_bitmap,  .bmp_x = 0u, .bmp_y = 0u, .text_str = "Pax round!", .text_x = 30u, .text_y = 4u, .text_font = FONT_MEDIUM_FONT , .isInverted = TRUE },
     { .bmp_ptr = &pax2_bitmap,  .bmp_x = 0u, .bmp_y = 0u, .text_str = "Pax round!", .text_x = 40u, .text_y = 4u, .text_font = FONT_MEDIUM_FONT , .isInverted = TRUE },
     { .bmp_ptr = &pax3_bitmap,  .bmp_x = 0u, .bmp_y = 0u, .text_str = "Pax round!", .text_x = 40u, .text_y = 4u, .text_font = FONT_MEDIUM_FONT , .isInverted = TRUE },
};


Private Boolean paxSpecialIntro(U8 sec)
{
    static const IntroSequence * intro_ptr = &priv_pax_intros[0];
    static U8 intro_ix;

    if (sec == 1u)
    {
        intro_ptr = &priv_pax_intros[intro_ix];
        intro_ix++;
        if (intro_ix >= NUMBER_OF_ITEMS(priv_pax_intros))
        {
            intro_ix = 0u;
        }
    }

    return genericIntroFunction(intro_ptr, sec);
}

Private const IntroSequence priv_socresp_intros[] =
{
     { .bmp_ptr = &socresp1_bitmap,  .bmp_x = 0u, .bmp_y = 0u, .text_str = "Social Resp. Round!", .text_x = 20u, .text_y = 4u, .text_font = FONT_MEDIUM_FONT , .isInverted = TRUE },
     { .bmp_ptr = &socresp2_bitmap,  .bmp_x = 0u, .bmp_y = 0u, .text_str = "Social Resp. Round!", .text_x = 40u, .text_y = 4u, .text_font = FONT_MEDIUM_FONT , .isInverted = TRUE },
};


Private Boolean SocRespSpecialIntro(U8 sec)
{
    static const IntroSequence * intro_ptr = &priv_socresp_intros[0];
    static U8 intro_ix;

    if (sec == 1u)
    {
        intro_ptr = &priv_socresp_intros[intro_ix];
        intro_ix++;
        if (intro_ix >= NUMBER_OF_ITEMS(priv_socresp_intros))
        {
            intro_ix = 0u;
        }
    }

    return genericIntroFunction(intro_ptr, sec);
}


/********** Button Handlers  ********/

Private void subscribeButtonHandlers(void)
{
    buttons_subscribeListener(UP_BUTTON,    HandleUpButton);
    buttons_subscribeListener(DOWN_BUTTON,  HandleDownButton);
    buttons_subscribeListener(RIGHT_BUTTON, HandleRightButton);
    buttons_subscribeListener(LEFT_BUTTON,  HandleLeftButton);
}


Private void HandleUpButton(void)
{
    /* Placeholder. */
}

Private void HandleDownButton(void)
{
    /* Placeholder. */
}


Private void HandleRightButton(void)
{
    /* This will open up a message box that will allow the user to cancel the game. */
    MessageBox_SetResponseHandler(handleMessageBoxResponse);
    MessageBox_ShowWithOkCancel("Quit game?");
}

Private void HandleLeftButton(void)
{
    /* Placeholder. */
}


/* MessageBox Handler. */
Private void handleMessageBoxResponse(MsgBox_Response resp)
{
    /* In reality this is the only type of response that we can get. */
    /* This is really just for testing right now... */
    if (resp == RESPONSE_OK)
    {
        /* We quit the game. */
        priv_state = CONTROLLER_EXITING;
    }
    else if (resp == RESPONSE_CANCEL)
    {
        /* We resume the game. */
        /* Re subscribe the button handlers. */
        subscribeButtonHandlers();
    }
    else
    {
        /* Should not really happen. */
    }
}

