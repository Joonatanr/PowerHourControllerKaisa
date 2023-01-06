/*
 * speaker.c
 *
 *  Created on: 7. sept 2018
 *      Author: JRE
 */

#include "speaker.h"
#include "driverlib.h"
#include "register.h"

/* We try to drive a speaker using PWM and an amplifier in between... */

/* Need to use TIMERA3, as other timers are already being used by other modules... */
/* P10.4/TA3.0 */

#define TIMER_FREQUENCY 750000u

Private Timer_A_PWMConfig priv_pwmConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK, //SMCLK has frequency of 12MHz
        TIMER_A_CLOCKSOURCE_DIVIDER_16,
        10000u,
        TIMER_A_CAPTURECOMPARE_REGISTER_0,
        TIMER_A_OUTPUTMODE_TOGGLE_SET,
        0
};

#define G5_NOTE     784u
#define F5_NOTE     698u
#define E5_NOTE     659u
#define D5_NOTE     587u
#define C5_NOTE     523u
#define B4_NOTE     494u
#define A4_NOTE     440u

#define G4_NOTE     392u
#define F4_UP_NOTE  370u
#define F4_NOTE     349u
#define E4_NOTE     324u
#define D4_NOTE     288u
#define C4_NOTE     262u /* Note to self - The octave changes at C. */
#define B3_NOTE     247u
#define A3_NOTE     220u

#define G3_NOTE     196u
#define E3_NOTE     165u
#define C3_NOTE     131u
#define F3_NOTE     175u
#define D3_NOTE     147u
#define F3_UP_NOTE  185u




/*
 * --0--                                                A5
 *                                    0                 G5
 * -------------------------------0------------------   F5
 *                             0                        E5
 * -------------------------0------------------------   D5
 *                       0                              C5
 * -------------------0------------------------------   B4
 *                 0                                    A4
 * -------------0------------------------------------   G4
 *           0                                          F4
 * --------0-----------------------------------------   E4
 *      0                                               D4
 *---0---                                               C4
 *
 * */



/* Private type definitions */
typedef struct
{
    U8 length; //In units of 1/16
    U16 frequency;
    Boolean add_break;
} Note_T;

typedef struct
{
    const Note_T n1;
    const Note_T n2;
} NotePair_T;

Private Boolean priv_speaker_state = FALSE;


/************* Private function forward declarations ****************/

Private void setFrequencyInterval(U16 interval);
Private void setSpeakerFrequency(U16 frequency);

Private void play_note(const Note_T * note);



/************************* Public function definitions  *****************************/
Public void speaker_init(void)
{
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P10, GPIO_PIN4,
            GPIO_PRIMARY_MODULE_FUNCTION);
    priv_speaker_state = TRUE;
}

Private const Note_T roll_me_over[] =
{
 // Stanza
     { 6, G3_NOTE, FALSE },
     //{ 3, G3_NOTE, TRUE },

     { 3 ,C4_NOTE, TRUE },
     { 2 ,C4_NOTE, TRUE },
     { 3 ,C4_NOTE, TRUE },

     { 2, C4_NOTE, TRUE },
     { 5, C4_NOTE, TRUE },

     { 4, B3_NOTE, TRUE },
     { 2, C4_NOTE, TRUE },
     { 3, D4_NOTE, TRUE },

     { 2, A3_NOTE, TRUE },
     { 3, A3_NOTE, TRUE },
     { 2, A3_NOTE, TRUE },
     { 5, A3_NOTE, TRUE },

     { 3, A3_NOTE, TRUE  },
     { 2, A3_NOTE, FALSE },

     { 3, G3_NOTE, FALSE },

     { 2, D4_NOTE, TRUE  },
     { 3, D4_NOTE, TRUE  },
     { 2, D4_NOTE, TRUE  },
     { 3, D4_NOTE, FALSE },

     { 2, G3_NOTE, FALSE  },
     { 3, A3_NOTE, FALSE  },
     { 2, B3_NOTE, FALSE  },
     { 3, C4_NOTE, FALSE  },

     { 12, 0, FALSE },

 //Chorus...
     { 4, G4_NOTE, TRUE   },
     { 2, G4_NOTE, TRUE   },

     { 6, G4_NOTE, TRUE   },
     { 5 ,E4_NOTE, TRUE   },

     { 5, C4_NOTE, TRUE   },

     { 3, B3_NOTE, TRUE   },
     { 2, B3_NOTE, TRUE   },
     { 5, A3_NOTE, TRUE   },

     { 5, F4_NOTE, TRUE   },
     { 5, F4_NOTE, TRUE   },
     { 3, F4_NOTE, TRUE   },
     { 2, F4_NOTE, TRUE   },

     { 3, E4_NOTE, TRUE   },
     { 2, E4_NOTE, TRUE   },
     { 3, E4_NOTE, TRUE   },
     { 2, E4_NOTE, TRUE   },

     { 3, D4_NOTE, TRUE   },
     { 2, D4_NOTE, TRUE   },
     { 3, D4_NOTE, TRUE   },
     { 2, D4_NOTE, TRUE   },

     {3 , C4_NOTE, TRUE  },
     {2 , C4_NOTE, TRUE  },
     {3 , B3_NOTE, TRUE  },
     {2 , B3_NOTE, TRUE  },

     {5 , A3_NOTE, TRUE  },

     {2 , G3_NOTE, FALSE },
     {2 , A3_NOTE, FALSE },
     {2 , B3_NOTE, FALSE },

     {3 , C4_NOTE, TRUE  },
     {2 , C4_NOTE, TRUE  },
     {3 , B3_NOTE, TRUE  },
     {2 , B3_NOTE, TRUE  },

     {5 , A3_NOTE, TRUE  },

     {2 , G3_NOTE, FALSE },
     {2 , A3_NOTE, FALSE },
     {2 , B3_NOTE, FALSE },

     {8 , C4_NOTE, FALSE },
};

Private const Note_T sample_music[] =
{
     { .length = 2,    .frequency = A4_NOTE        },
     { .length = 2,    .frequency = F4_UP_NOTE     },
     { .length = 2,    .frequency = A4_NOTE        },
     { .length = 2,    .frequency = F4_UP_NOTE     },

     { .length = 4,   .frequency = A4_NOTE        },
     { .length = 4,   .frequency = A4_NOTE        },

     { .length = 2,    .frequency = G4_NOTE        },
     { .length = 2,    .frequency = F4_UP_NOTE     },
     { .length = 2,    .frequency = G4_NOTE        },
     { .length = 2,    .frequency = A4_NOTE        },

     { .length = 8,      .frequency = F4_UP_NOTE     },

     { .length = 2,    .frequency = A4_NOTE        },
     { .length = 2,    .frequency = F4_UP_NOTE     },
     { .length = 2,    .frequency = A4_NOTE        },
     { .length = 2,    .frequency = F4_UP_NOTE     },

     { .length = 4,   .frequency = A4_NOTE        },
     { .length = 4,   .frequency = A4_NOTE        },

     { .length = 2,    .frequency = G4_NOTE        },
     { .length = 2,    .frequency = F4_UP_NOTE     },
     { .length = 2,    .frequency = G4_NOTE        },
     { .length = 2,    .frequency = A4_NOTE        },

     { .length = 8,      .frequency = F4_UP_NOTE     },

     /* Row... */
     { .length = 2,    .frequency = F4_UP_NOTE,    .add_break = TRUE },
     { .length = 2,    .frequency = F4_UP_NOTE,    .add_break = TRUE },
     { .length = 2,    .frequency = F4_UP_NOTE,    .add_break = TRUE },
     { .length = 2,    .frequency = F4_UP_NOTE,    .add_break = TRUE },

     { .length = 2,    .frequency = G4_NOTE        },
     { .length = 2,    .frequency = F4_UP_NOTE     },
     { .length = 2,    .frequency = E4_NOTE        },
     { .length = 2,    .frequency = D4_NOTE        },

     { .length = 2,    .frequency = E4_NOTE, .add_break = TRUE },
     { .length = 2,    .frequency = E4_NOTE, .add_break = TRUE },
     { .length = 2,    .frequency = E4_NOTE        },
     { .length = 2,    .frequency = G4_NOTE        },

     { .length = 2,    .frequency = F4_UP_NOTE,    .add_break = TRUE },
     { .length = 2,    .frequency = E4_NOTE        },
     { .length = 4,   .frequency = D4_NOTE        },

     /* Row... */
     { .length = 2,    .frequency = F4_UP_NOTE,    .add_break = TRUE },
     { .length = 2,    .frequency = F4_UP_NOTE,    .add_break = TRUE },
     { .length = 2,    .frequency = F4_UP_NOTE,    .add_break = TRUE },
     { .length = 2,    .frequency = F4_UP_NOTE,    .add_break = TRUE },

     { .length = 2,    .frequency = G4_NOTE        },
     { .length = 2,    .frequency = F4_UP_NOTE     },
     { .length = 2,    .frequency = E4_NOTE        },
     { .length = 2,    .frequency = D4_NOTE        },

     { .length = 2,    .frequency = E4_NOTE, .add_break = TRUE },
     { .length = 2,    .frequency = E4_NOTE, .add_break = TRUE },
     { .length = 2,    .frequency = E4_NOTE        },
     { .length = 2,    .frequency = G4_NOTE        },

     { .length = 2,    .frequency = F4_UP_NOTE,    .add_break = TRUE },
     { .length = 2,    .frequency = E4_NOTE        },
     { .length = 2,    .frequency = A4_NOTE        },

};


Public void speaker_test(void)
{
    U8 ix;
    const Note_T * note_ptr;

    for (ix = 0u; ix < NUMBER_OF_ITEMS(roll_me_over); ix++)
    {
        note_ptr = &roll_me_over[ix];
        play_note(note_ptr);
    }

    speaker_mute();
}


Public void speaker_mute(void)
{
    MAP_Timer_A_stopTimer(TIMER_A3_BASE);

    MAP_GPIO_setAsOutputPin(GPIO_PORT_P10, GPIO_PIN4);
    GPIO_setOutputLowOnPin(GPIO_PORT_P10, GPIO_PIN4);
    priv_speaker_state = FALSE;
}


/************************* Private function definitions  *****************************/

Private void setSpeakerFrequency(U16 frequency)
{
    U32 interval = TIMER_FREQUENCY / frequency;
    setFrequencyInterval(interval);
}

Private void setFrequencyInterval(U16 interval)
{
    /* Since this is a frequency generator then the duty cycle will always be 50% */
    Timer_A_PWMConfig * conf_ptr = &priv_pwmConfig;
    U32 duty_cycle;

    /* Lets test */
#if 0
    speaker_mute();
    delay_msec(30);
    speaker_init();
    /* End of test code. */
#endif

    if (interval < 0xffffu)
    {
        duty_cycle = interval / 2;
        conf_ptr->timerPeriod = interval;
        conf_ptr->dutyCycle = (U16)duty_cycle;
        MAP_Timer_A_generatePWM(TIMER_A3_BASE, conf_ptr);
    }
}

#define NOTE_BASE 120u

Private void play_note(const Note_T * note)
{
    U8 ix;

    if (!priv_speaker_state)
    {
        speaker_init();
    }

    if(note->frequency > 0)
    {
        setSpeakerFrequency(note->frequency);
    }
    else
    {
        speaker_mute();
    }

    for (ix = 0u; ix < note->length; ix++)
    {
        delay_msec(NOTE_BASE);
    }


    if (note->add_break)
    {
        speaker_mute();
        __delay_cycles(10000);
        speaker_init();
    }
}





