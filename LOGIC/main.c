//*****************************************************************************
//
// MSP432 based Power Hour Controller machine main module.
//
//****************************************************************************

#include <LOGIC/PowerHourGame/clockDisplay.h>
#include "LOGIC/TextTools/MessageBox.h"
#include <misc.h>
#include "msp.h"
#include "typedefs.h"
#include "register.h"
#include "display_drv.h"
#include "buzzer.h"
#include "buttons.h"
#include "backlight.h"
#include "Menus/menu.h"
#include "Scheduler.h"
#include "speaker.h"
#include "pot.h"

/*  
	P1.5 -> Spi CLK
    P1.6 -> Spi Tx
    P1.7 -> A0
	
	P2.7 -> Buzzer
		
	P3.2 -> POT_LED3
	P3.3 -> POT_LED2
	P3.6 -> DISP_RESET
    
    P4.1 -> Potentiometer input.
    P4.3 -> CS	
	
    P4.6 -> POT_LED1
	P4.7 -> Red button   (UP)
    
    
    P5.4 -> Blue Button  (DOWN)
	P5.5 -> Green Button (LEFT)	
    P5.6 -> BCKL


    P6.0 -> POT_LED4
    P6.4 -> Black Button (RIGHT)
*/



//#define DEBUG_SEQUENCE

Private void timer_hi_prio(void);
Private void timer_lo_prio(void);

Private void showStartScreen(void);

Private void startGameHandler(void);
Private void startSnakeGame(void);
Private void showDedicationText(void);

//Callback for register.c
Public TimerHandler timer_10msec_callback = timer_hi_prio;
Public TimerHandler timer_50msec_callback = timer_lo_prio;

Private const char priv_version_string[] = "Machine 3.1";


/* Settings Menu Items */
Private const MenuItem SettingsMenuItemArray[] =
{
   { .text = "Brightness",    .Action = MENU_ACTION_WIDGET  , .ActionArg.bargraph_ptr = &BRIGHTNESS_BARGRAPH        },
   { .text = "Snake speed",   .Action = MENU_ACTION_WIDGET  , .ActionArg.bargraph_ptr = &SNAKE_SPEEED_BARGRAPH      },
   { .text = "Task frequency",.Action = MENU_ACTION_WIDGET  , .ActionArg.bargraph_ptr = &TASK_FREQUENCY_BARGRAPH    }
};

Private SelectionMenu SettingsMenu =
{
     .items = SettingsMenuItemArray,
     .number_of_items = NUMBER_OF_ITEMS(SettingsMenuItemArray),
     .selected_item = 0u
};


/** Start Menu Items.*/
Private const MenuItem StartMenuItemArray[] =
{
   { .text = "Start Game",  .Action = MENU_ACTION_FUNCTION    , .ActionArg.function_ptr =   startGameHandler        },
   { .text = "Play Snake",  .Action = MENU_ACTION_FUNCTION    , .ActionArg.function_ptr =   startSnakeGame          },
   { .text = "Settings",    .Action = MENU_ACTION_SUBMENU     , .ActionArg.subMenu_ptr  =   &SettingsMenu           },
   { .text = "About",       .Action = MENU_ACTION_FUNCTION    , .ActionArg.function_ptr =   &showDedicationText     },
};

Private SelectionMenu StartMenu =
{
     .items = StartMenuItemArray,
     .number_of_items = NUMBER_OF_ITEMS(StartMenuItemArray),
     .selected_item = 0u,
};

/** End of Start Menu Items. */


void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;           // Stop watchdog timer

    //Initialise HW layer.
    register_init();

    //Set backlight to 60 percent.
    backlight_set_level(60);

    //Initialise logic layer.
    Scheduler_initTasks();

    delay_msec(100);

    //Start all scheduler task
    Scheduler_StartTasks();
    delay_msec(100);

    //We show the initial start screen for a while.
    showStartScreen();
    delay_msec(4000);

    /* We pass control over to the menu handler. */
    menu_enterMenu(&StartMenu);

    //Currently this function never returns.
    register_enable_low_powermode();
}

void returnToMain(void)
{
    Scheduler_StopActiveApplication();
    menu_enterMenu(&StartMenu);
}

//Periodically called from interrupt context.
//Called every 10msec.
Private void timer_hi_prio(void)
{
    //All high priority tasks should be put in here.
    //This means hardware tasks, such as reading buttons, changing PWM etc.
    set_led_two_blue(isButton(BLUE_BUTTON));
    set_led_two_red(isButton(RED_BUTTON));
    set_led_two_green(isButton(GREEN_BUTTON));

    buttons_cyclic10msec();
    pot_cyclic_10ms();
}


//This is called every 50 milliseconds.
Private void timer_lo_prio(void)
{
    //Call the main scheduler for logic tasks.
    Scheduler_cyclic();
}

//#define BMP_TEST
//#define HOT_BMP_TEST

Private void showStartScreen(void)
{
    /* We don't want the driver to start drawing while we are filling the framebuffer.
     * In other places it is not really a problem, because the display cyclic function is called at the end of the thread as a
     * last step. Here however we are calling directly from main. */
    display_lock(TRUE);
    display_clear();

#ifdef BMP_TEST
    //display_drawBitmap(&girls2_bitmap, 0, 0, TRUE);
    //display_drawBitmap(&test_image_bitmap, 0, 0, TRUE);
    //delay_msec(12000);
    display_drawBitmap(&kaisa_w_laudur_bitmap, 0, 0, TRUE);
    delay_msec(16000);
#endif

#ifdef HOT_BMP_TEST
    display_drawBitmap(&kiss2_bitmap, 0, 0, TRUE);
    //display_drawBitmap(&hot_girl1_bitmap, 0, 0, TRUE);
    delay_msec(10000);
    display_clear();
    display_drawBitmap(&male_stripper2_bmp, 0, 0, TRUE);
    //display_drawBitmap(&hot_girl2_bitmap, 0, 0, TRUE);
    delay_msec(10000);
    display_clear();
    display_drawBitmap(&male_stripper5_bmp, 0, 0, TRUE);
    //display_drawBitmap(&hot_girl3_bitmap, 0, 0, TRUE);
    delay_msec(10000);
    display_clear();
    display_drawBitmap(&male_stripper3_bmp, 0, 0, TRUE);
    //display_drawBitmap(&kiss_bitmap, 0, 0, TRUE);
    delay_msec(10000);
    display_clear();
    display_drawBitmap(&kinky_bitmap, 0, 0, TRUE);
    delay_msec(10000);
    display_clear();
    display_drawBitmap(&kinky2_bitmap, 0, 0, TRUE);
    delay_msec(10000);
    display_clear();
#endif

    display_drawStringCenter("Power Hour", 64u, 5u, FONT_LARGE_FONT, FALSE);
    display_drawStringCenter(priv_version_string, 64u, 20u, FONT_LARGE_FONT, FALSE);
    display_drawStringCenter("Kaisa Edition", 64u, 40u, FONT_MEDIUM_FONT, FALSE);
    display_lock(FALSE);
}

/* Starts the main Power Hour game. */
Private void startGameHandler(void)
{
    Scheduler_SetActiveApplication(APPLICATION_POWER_HOUR);
}

/* Starts the snake game. */
Private void startSnakeGame(void)
{
    Scheduler_SetActiveApplication(APPLICATION_SNAKE);
}

Private void showDedicationText(void)
{
    /* Turns out we can't do this without making this into a dummy application for some reason. */
    Scheduler_SetActiveApplication(APPLICATION_DEDICATION);
}
