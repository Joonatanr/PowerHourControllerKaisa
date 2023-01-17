/*
 * SpecialTasks.c
 *
 *  Created on: Sep 5, 2017
 *      Author: Joonatan
 */

#include <LOGIC/PowerHourGame/SpecialTasks.h>
#include "bitmaps.h"
#include "font.h"
#include "display_drv.h"
#include "register.h"
#include <stdlib.h>
#include "pot.h"

/*****************************************************************************************************
 *
 * Private defines
 *
 *****************************************************************************************************/

#define SMALL_SHOT_X 20u
#define SMALL_SHOT_Y 32u
#define SMALL_SHOT_INTERVAL 20u


/*****************************************************************************************************
 *
 * Private type definitions
 *
 *****************************************************************************************************/

typedef struct
{
    const char * upper_text;
    const char * middle_text;
    const char * lower_text;
    U8 counter;             /* Number of times, this task has been selected.                        */

    /* Valid values for these are 0-3*/
    const U8 nude_level;    /* Minimum nudity setting for this task to appear.                      */
    const U8 gay_level;     /* Minimum girl-on-girl and boy-on-boy setting for this task to appear  */
    const U8 sexy_level;    /* Minimum sexy setting for this task to appear                         */ /* Can't believe I'm coding this... */
} Task_T;

typedef struct
{
    Task_T * task_array;
    U16 number_of_array_items;
} Task_Array_Descriptor;


/*****************************************************************************************************
 *
 * Private function forward declarations
 *
 *****************************************************************************************************/

//Private Boolean DrinkTwiceTask(U8 sec, const char * headerWord);
Private Boolean DrinkTwiceTask(U8 sec, SpecialTaskType type);
Private Boolean SpecialTaskWithRandomText(U8 sec, SpecialTaskType type);
Private const Task_T * getRandomTaskFromArray(Task_T * array, U8 array_size);
Private Boolean isTaskEnabled(const Task_T * task_ptr, U8 sexy_level, U8 nude_level, U8 gay_level);


/*****************************************************************************************************
 *
 * Private constant declarations
 *
 *****************************************************************************************************/
Private const SpecialTaskFunc priv_special_tasks_girls_array[] =
{
  &SpecialTaskWithRandomText,
  &DrinkTwiceTask,
  &SpecialTaskWithRandomText,
  &SpecialTaskWithRandomText,
  &SpecialTaskWithRandomText,
  &SpecialTaskWithRandomText,
  &SpecialTaskWithRandomText,
};


Private const SpecialTaskFunc priv_special_tasks_guys_array[] =
{
  &SpecialTaskWithRandomText,
  &DrinkTwiceTask,
  &SpecialTaskWithRandomText,
  &SpecialTaskWithRandomText,
  &SpecialTaskWithRandomText,
  &SpecialTaskWithRandomText,
  &SpecialTaskWithRandomText,
};

Private const SpecialTaskFunc priv_special_tasks_everybody_array[] =
{
  &SpecialTaskWithRandomText,
  &DrinkTwiceTask,
  &SpecialTaskWithRandomText,
  &SpecialTaskWithRandomText,
  &SpecialTaskWithRandomText,
  &SpecialTaskWithRandomText,
  &SpecialTaskWithRandomText,
};

Private const SpecialTaskFunc priv_special_tasks_kaisa_array[] =
{
  &SpecialTaskWithRandomText,
  &DrinkTwiceTask,
  &SpecialTaskWithRandomText,
  &SpecialTaskWithRandomText,
  &SpecialTaskWithRandomText,
  &SpecialTaskWithRandomText,
  &SpecialTaskWithRandomText,
};


/* Easy tasks. -> Really softcore.  */
Private Task_T priv_TextArrayGirlsLevel1[] =
{
     { "The girl with ",     "the fanciest clothes"  , "drinks 2x"         , .counter = 0u  , .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u }, /* 1  */
     { NULL,                 "Only girls drink"      , NULL                , .counter = 0u  , .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u }, /* 2  */
     { "Girls drink",        "without "      ,         "using hands"       , .counter = 0u  , .nude_level = 0u, .sexy_level = 1u, .gay_level = 0u }, /* 3  */
     { "Choose one girl",    "who drinks 3x ",         NULL                , .counter = 0u  , .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u }, /* 4  */
     { "The girl with",      "the longest hair ",      "drinks a shot"     , .counter = 0u  , .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u }, /* 5  */
     { "The girl with",      "the highest voice",      "drinks a shot"     , .counter = 0u  , .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u }, /* 6  */
     { "All blondes",        "drink 2x ",              NULL                , .counter = 0u  , .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u }, /* 7  */
     { "All brunettes",      "drink 2x ",              NULL                , .counter = 0u  , .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u }, /* 8  */
     { "The tallest girl",   "drinks 2x ",             NULL                , .counter = 0u  , .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u }, /* 9  */
     { "The youngest girl",  "drinks 2x ",             NULL                , .counter = 0u  , .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u }, /* 10 */
     { "All girl BESTies",   "drink  2x ",             NULL                , .counter = 0u  , .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u }, /* 11 */
     { "The shortest girl",  "drinks 2x ",             NULL                , .counter = 0u  , .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u }, /* 12 */
     { "Girls propose",      "the next toast ",        NULL                , .counter = 0u  , .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u }, /* 13 */
     { "All redheads",       "drink 2x ",              NULL                , .counter = 0u  , .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u }, /* 14 */
     { "All girls",          "with purple hair ",      "drink 2x"          , .counter = 0u  , .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u }, /* 15 */
     { "All girls",          "with glasses ",          "drink 2x"          , .counter = 0u  , .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u }, /* 16 */
     { "Choose 1 girl",      "that drinks 2x",        NULL                 , .counter = 0u  , .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u }, /* 17 */
     { "All girls",          "wearing black",         "drink 2x"           , .counter = 0u  , .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u }, /* 18 */
};

/* Easy tasks. */
Private Task_T priv_TextArrayGuysLevel1[] =
{
     {  NULL                    , "Only guys drink",            NULL          , .counter = 0u  , .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u}, /* 1  */
     {  "Guys drink"            , "without",                 "using hands"    , .counter = 0u  , .nude_level = 0u, .sexy_level = 1u, .gay_level = 0u}, /* 2  */
     {  "The toughest guy"      , "drinks 3x",                  NULL          , .counter = 0u  , .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u}, /* 3  */
     {  "The biggest playboy"   , "drinks 3x",                  NULL          , .counter = 0u  , .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u}, /* 4  */
     {  NULL                    , "Guys must sing",         "a song together" , .counter = 0u  , .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u}, /* 5  */
     {  "Last guy to put his"   , "finger on his nose",        "drinks 2x"    , .counter = 0u  , .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u}, /* 6  */
     {  "Choose one guy"        , "who drinks 3x ",             NULL          , .counter = 0u  , .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u}, /* 7  */
     {  "All guys"              , "drop and do 10  ",        "pushups"        , .counter = 0u  , .nude_level = 0u, .sexy_level = 1u, .gay_level = 0u}, /* 8  */
     {  "All guys with"         , "a six-pack ",             "drink 3x"       , .counter = 0u  , .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u}, /* 9  */
     {  "The most wasted"       , "guy drinks",              "water-shot"     , .counter = 0u  , .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u}, /* 10 */
     {  "Guys with"             , "hair gel",                "drink 3x"       , .counter = 0u  , .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u}, /* 11 */
     {  "Single Guys "          , "drink vodka",             NULL             , .counter = 0u  , .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u}, /* 12 */
     {  "The youngest guy"      , "drinks 2x",               NULL             , .counter = 0u  , .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u}, /* 14 */
     {  "All guys that"         , "are in the army",         "drink 3x"       , .counter = 0u  , .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u}, /* 15 */
     {  "All guys",               "with glasses ",           "drink 2x"       , .counter = 0u  , .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u}, /* 16 */
     {  "One guy",                "must say a toast",        "that rhymes"    , .counter = 0u  , .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u}, /* 17 */
     { "All guys whose" ,         "name starts with",     "S drinks 2x"       , .counter = 0u  , .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u}, /* 18 */
};

/* Easy Tasks */
Private Task_T priv_TextArrayAllLevel1[] =
{
/* Casual tasks : These should not contain any extra criteria. */
     {  NULL                    , "Everybody drink 2x",            NULL       , .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u    }, /* 1  */
     {  "Everybody drinks"      , "without using",              "hands"       , .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u    }, /* 2  */
     {  "Most sober person"     , "drinks 3x",                     NULL       , .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u    }, /* 3  */
     {  "Grab your bottles"     , "and do a ",              "waterfall"       , .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u    }, /* 4  */
     {  "Those in relationship" , "drink  2x",                     NULL       , .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u    }, /* 5  */
     {  "Those who are single"  , "drink  2x",                     NULL       , .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u    }, /* 6  */
     {  "Vodka round"           , "for everybody",                 NULL       , .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u    }, /* 7  */
     {  "Who is closest to "    , "the machine must",    "tell dirty joke"    , .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u    }, /* 8  */
     {  "Everybody who is "     , "wearing sweaters",    "drink 2x"           , .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u    }, /* 9  */
     {  "Rock-Paper-Scissors "  , "tournament in 60 sec", "loser drinks vodka", .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u    }, /* 10 */
     {  "Last person to"        , "put hand on their", "nose drinks 3x"       , .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u    }, /* 2  */


/* Tasks that need criteria. */
     {  "Everyone takes off"     , "one sock",                     NULL       , .counter = 0u,  .nude_level = 1u, .sexy_level = 0u, .gay_level = 0u    },
     {  "Those that have"        , "french-kissed today",    "drink 3x"       , .counter = 0u,  .nude_level = 0u, .sexy_level = 1u, .gay_level = 0u    },
     {  "Everybody who has"      , "made out with Kaisa",    "drinks vodka"   , .counter = 0u,  .nude_level = 0u, .sexy_level = 1u, .gay_level = 0u    },
     {  "One closest to machine" , "gets slapped on",        "the ass"        , .counter = 0u,  .nude_level = 0u, .sexy_level = 2u, .gay_level = 0u    },


/* Strange cases, where somebody is playing on easy mode, but with maximum nudity. */
     {  "Everyone takes off"     , "one item of",             "clothing"      , .counter = 0u,  .nude_level = 3u, .sexy_level = 0u, .gay_level = 0u   },
     {  "Everyone takes off"     , "one item of",             "clothing"      , .counter = 0u,  .nude_level = 3u, .sexy_level = 0u, .gay_level = 0u   },
     {  "Everyone takes off"     , "one item of",             "clothing"      , .counter = 0u,  .nude_level = 3u, .sexy_level = 0u, .gay_level = 0u   },
     {  "Everyone takes off"     , "one item of",             "clothing"      , .counter = 0u,  .nude_level = 3u, .sexy_level = 0u, .gay_level = 0u   },

/* Strange cases, where somebody is playing on easy mode, but with maximum sexyness */
     {  "One closest to"         , "machine spins",         "the bottle"      , .counter = 0u,  .nude_level = 0u, .sexy_level = 3u, .gay_level = 0u   },
     {  "Everybody finds"        , "somebody to",           "kiss"            , .counter = 0u,  .nude_level = 0u, .sexy_level = 3u, .gay_level = 0u   },

/* Strange cases, where somebody is playing on easy mode, but with maximum gay */
     {  "Guys drink vodka" ,       "Girls have to",         "kiss"            , .counter = 0u,  .nude_level = 0u, .sexy_level = 1u, .gay_level = 3u   },
};

/* Easy tasks */
Private Task_T priv_TextArrayKaisaLevel1[] =
{
/* Casual tasks : These should not contain any extra criteria. */
     {  NULL                    , "Kaisa drinks 2x",               NULL       , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u    }, /* 1  */
     {  "Kaisa must take"       , "vodka shot",                    NULL       , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u    }, /* 2  */
     {  NULL                    , "Kaisa drinks 3x",               NULL       , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u    }, /* 3  */
     {  "Kaisa makes"           , "a toast",                       NULL       , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u    }, /* 4  */
     {  "Kaisa begins a"        , "waterfall",                     NULL       , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u    }, /* 4  */
     {  "Kaisa can order"       , "one person to",    "take vodka shot"       , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u    }, /* 2  */

/* Tasks that need criteria. */
     {  "For each player who"   , " Kaisa has kissed", "she drinks 1x"        , .counter = 0u, .nude_level = 0u, .sexy_level = 1u, .gay_level = 0u    }, /* 5  */
     {  "Each player who"       , " has never kissed", "Kaisa, drinks 3x"     , .counter = 0u, .nude_level = 0u, .sexy_level = 1u, .gay_level = 0u    }, /* 5  */

/* Strange cases, where somebody is playing on easy mode, but with maximum nudity. */
     {  "Kaisa can confiscate"  , "one item of",             "clothing"       , .counter = 0u, .nude_level = 3u, .sexy_level = 0u, .gay_level = 0u    },
     {  "Kaisa loses"           , "one item of",             "clothing"       , .counter = 0u, .nude_level = 3u, .sexy_level = 0u, .gay_level = 0u    },
     {  "Kaisa takes the"       , "the shirts of",       "all the guys"       , .counter = 0u, .nude_level = 3u, .sexy_level = 0u, .gay_level = 0u    },

/* Strange cases, where somebody is playing on easy mode, but with maximum sexyness */
     {  "Kaisa gets to"         , "spin the ",                 "bottle"       , .counter = 0u, .nude_level = 0u, .sexy_level = 3u, .gay_level = 0u    },
/* Strange cases, where somebody is playing on easy mode, but with maximum gay */
     {  "Kaisa chooses 2 guys"  , "who either make out",     "or drink vodka" , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 3u    },
     {  "Kaisa chooses 2 girls" , "who either make out",     "or drink vodka" , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 3u    },

};

/* Medium tasks */
Private Task_T priv_TextArrayGirlsLevel2[] =
{
 /* Casual tasks : These should not contain any extra criteria. */
     { "Girls",              "I have never ever"     , NULL                , .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u  }, /* 2  */
     { "All couples  ",      "drink 2x"              , NULL                , .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u  }, /* 4  */
     { "All girls whose" ,   "name starts with",     "S drinks 2x"         , .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u  }, /* 5  */
     { "All bad girls",      "drink 2x ",              NULL                , .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u  }, /* 6  */
     { "All female" ,        "organisers",           "drink 2x"            , .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u  }, /* 9  */
     { "All good girls",     "drink 2x ",              NULL                , .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u  }, /* 7  */
     { "Last girl to" ,      "put finger on",        "nose drinks 2x"      , .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u  }, /* 10 */
     { "All girls that",     "have boyfriends ",     "drink 2x"            , .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u  }, /* 11 */
     { "All blondes",        "drink vodka ",              NULL             , .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u  }, /* 12 */
     { "All brunettes",      "drink vodka ",              NULL             , .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u  }, /* 13 */
     { "All girls whose" ,   "name starts with",     "L drinks 2x"         , .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u  }, /* 16 */
     { "All girls with" ,    "blue eyes",            "drink 2x"            , .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u  }, /* 18 */
     { "All girls with" ,    "a pony tail",          "drink 2x"            , .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u  }, /* 19 */

 /* Tasks that need criteria. */
     { "The girl with ",     "the sexiest voice"     , "drinks 2x "        , .counter = 0u,  .nude_level = 0u, .sexy_level = 1u, .gay_level = 0u  }, /* 1  */
     { "The girl with  ",    "the largest boobs"     , "drinks 2x"         , .counter = 0u,  .nude_level = 0u, .sexy_level = 1u, .gay_level = 0u  }, /* 3  */
     { "The girls with" ,    "the shortest skirt",   "drinks 2x"           , .counter = 0u,  .nude_level = 1u, .sexy_level = 0u, .gay_level = 0u  }, /* 8  */
     { "All girls lose" ,    "1 item of ",   "clothing"                    , .counter = 0u,  .nude_level = 2u, .sexy_level = 0u, .gay_level = 0u  }, /* 8  */
     { "Girls who are",      "former virgins",       "drink 2x"            , .counter = 0u,  .nude_level = 0u, .sexy_level = 1u, .gay_level = 0u  }, /* 14 */
     { "Girls must purr",    "like a kitten",        "after drinking!"     , .counter = 0u,  .nude_level = 0u, .sexy_level = 1u, .gay_level = 0u  }, /* 15 */
     { "All girls who" ,     "kissed a girl",        "today drink 2x"      , .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 1u  }, /* 17 */
};

/* Medium tasks. */
Private Task_T priv_TextArrayGuysLevel2[] =
{
 /* Casual tasks : These should not contain any extra criteria. */
     {  "The guy with the"      , "biggest balls",            "drinks vodka"        ,  .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u  },  /* 1  */
     {  "Guys"                  , "Never have I ever",          NULL                ,  .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u  },  /* 2  */
     {  "All guys whose"        , "name starts with",         "A drinks 2x"         ,  .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u  },  /* 4  */
     {  "All couples  "         , "drink 2x",                   NULL                ,  .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u  },  /* 5  */
     {  "All male"              , "organisers",               "drink 2x"            ,  .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u  },  /* 6  */
     {  "The guy with"          , "the biggest beer",         "belly drinks 2x"     ,  .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u  },  /* 7  */
     {  "All guys with"         , "beards",                   "drink 2x"            ,  .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u  },  /* 8  */
     {  "All former senors",      "drink vodka",               NULL                 ,  .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u  },  /* 9  */
     {  "Guys must do",           "10 squats",                "before drinking"     ,  .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u  },  /* 13 */
     {  "Guys that love",         "heavy metal",              "drink vodka"         ,  .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u  },  /* 14 */

 /* Tasks that need criteria. */
     {  "All guys must come"    , " up with a pickup ",       "line in 60 seconds"  ,  .counter = 0u,  .nude_level = 0u, .sexy_level = 1u, .gay_level = 0u  },  /* 3  */
     {  "All guys lose"         , "One Item of Clothing",       NULL                ,  .counter = 0u,  .nude_level = 2u, .sexy_level = 0u, .gay_level = 0u  },  /* 3  */
     {  "All guys lose"         , "One Item of Clothing",       NULL                ,  .counter = 0u,  .nude_level = 2u, .sexy_level = 0u, .gay_level = 0u  },  /* 3  */
     {  "All guys who are",       "virgins drink 2x",         "and best of luck!"   ,  .counter = 0u,  .nude_level = 0u, .sexy_level = 1u, .gay_level = 0u  },  /* 10 */
     {  "Guys who have",          "kissed a dude",            "drink vodka!"        ,  .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 1u  },  /* 11 */
     {  "Guys must",              "say meow",                 "after drinking!"     ,  .counter = 0u,  .nude_level = 0u, .sexy_level = 1u, .gay_level = 0u  },  /* 12 */
     {  "Guys say No Homo!",      "Look each other in",       "the eyes and drink"  ,  .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 1u  },  /* 15 */
};


Private Task_T priv_TextArrayAllLevel2[] =
{
 /* Casual tasks : These should not contain any extra criteria. */
     {  NULL                    , "Everybody drink 2x",            NULL       , .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u     }, /* 1  */
     {  NULL                    , "Everybody drink 3x",            NULL       , .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u     }, /* 2  */
     {  "Everybody drinks"      , "1 shot of vodka",     "and 1 shot of beer" , .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u     }, /* 3  */
     {  "Whoever is closest to" , "the machine must", "distribute 6 beershots", .counter = 0u,  .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u     }, /* 3  */


 /* Tasks that need criteria. */
     /* TODO */
     {  "Everybody takes off"   , "One Item of Clothing", "or drinks vodka"   ,  .counter = 0u,  .nude_level = 1u, .sexy_level = 0u, .gay_level = 0u  },  /* 3  */
     {  "Everybody takes off"   , "One Item of Clothing",          NULL       ,  .counter = 0u,  .nude_level = 2u, .sexy_level = 0u, .gay_level = 0u  },  /* 3  */

     {  "Everybody drinks"      , "while slapping",       "their own ass"     ,  .counter = 0u,  .nude_level = 0u, .sexy_level = 2u, .gay_level = 0u  },  /* 3  */
     {  "Everybody drinks"      , "while sitting on",     "their hands"       ,  .counter = 0u,  .nude_level = 0u, .sexy_level = 1u, .gay_level = 0u  },  /* 3  */

     {  "Everybody who has"     , "gotten an orgasm",     "today drinks 3x"   ,  .counter = 0u,  .nude_level = 0u, .sexy_level = 2u, .gay_level = 0u  },  /* 3  */
     {  "Every who has ever"    , "had a penis in their", "mouth drink 3x"    ,  .counter = 0u,  .nude_level = 0u, .sexy_level = 1u, .gay_level = 0u  },  /* 3  */

};

/* TODO : Placeholder. */
Private Task_T priv_TextArrayKaisaLevel2[] =
{
 /* Casual tasks : These should not contain any extra criteria. */
     {  NULL                    , "Kaisa drinks 2x",               NULL       , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u  }, /* 1  */
     {  NULL                    , "Kaisa drinks 2x",               NULL       , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u  }, /* 2  */
     {  NULL                    , "Kaisa drinks 2x",               NULL       , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u  }, /* 3  */
 /* Tasks that need criteria. */
     /* TODO */
};

/* Hard tasks.  */
Private Task_T priv_TextArrayGirlsLevel3[] =
{
 /* Casual tasks : These should not contain any extra criteria. */
     {  "Vodka round!!!",          "for girls!!!"   ,                   NULL   , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u  }, /* 2  */
     {  "Girl showing the"     ,   "least cleavage"  ,           "drinks 3x"   , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u  }, /* 9  */
     {  "Girls can"            ,   "slap one of"  ,              "the guys"    , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u  }, /* 12 */
     {  "All girls must",         "drink under",         "the table"           , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u  }, /* 18 */

 /* Tasks that need criteria. */
     {  "Girls must"             , "do a ",                      "sexy dance"  , .counter = 0u, .nude_level = 0u, .sexy_level = 2u, .gay_level = 0u  }, /* 1  */
     {  "Most naked"           ,   "girl drinks ",               "2x"          , .counter = 0u, .nude_level = 1u, .sexy_level = 0u, .gay_level = 0u  }, /* 3  */
     {  "All girls lose"       ,   "two items of ",              "clothing"    , .counter = 0u, .nude_level = 2u, .sexy_level = 0u, .gay_level = 0u  }, /* 4  */
     {  "Girls with"           ,   ">5 flags&numbers",           "drink 3x"    , .counter = 0u, .nude_level = 0u, .sexy_level = 1u, .gay_level = 0u  }, /* 5  */
     {  "Girls who've had"     ,   "sex with 1 of the",  "players drink 2x"    , .counter = 0u, .nude_level = 0u, .sexy_level = 1u, .gay_level = 0u  }, /* 6  */
     {  "Girls do"             ,   "bodyshots!!!",                     NULL    , .counter = 0u, .nude_level = 0u, .sexy_level = 2u, .gay_level = 0u  }, /* 7  */
     {  "The last girl"        ,   "to finish shot"  ,    "loses 1 clothing"   , .counter = 0u, .nude_level = 1u, .sexy_level = 0u, .gay_level = 0u  }, /* 10 */
     {  "Girl showing the"     ,   "most cleavage"  ,           "drinks 3x"    , .counter = 0u, .nude_level = 1u, .sexy_level = 1u, .gay_level = 0u  }, /* 11 */
     {  "Girls: 1 shot for"    ,  "each guy they slept"  , "with this year"    , .counter = 0u, .nude_level = 0u, .sexy_level = 2u, .gay_level = 0u  }, /* 14 */
     {  "All girls with"       ,  "black underwear"      , "drink vodka"       , .counter = 0u, .nude_level = 0u, .sexy_level = 1u, .gay_level = 0u  }, /* 15 */
     { " Girls must make"      ,  "a naughty"   ,                "toast"       , .counter = 0u, .nude_level = 0u, .sexy_level = 1u, .gay_level = 0u  }, /* 16 */
     { " Girls must make"      ,  "a kinky"   ,                  "toast"       , .counter = 0u, .nude_level = 0u, .sexy_level = 1u, .gay_level = 0u  }, /* 17 */
     {  "All girls must",         "drink with hands",    "on their butts"      , .counter = 0u, .nude_level = 0u, .sexy_level = 2u, .gay_level = 0u  }, /* 19 */
};


/* Hard tasks.  */
Private Task_T priv_TextArrayGuysLevel3[] =
{
 /* Casual tasks : These should not contain any extra criteria. */
     {  "Vodka round!!!",        "for guys!!!"   ,                   NULL    , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u  }, /* 2  */
     {  "1 guy must",            "drink while"     ,    "upside down"        , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u  }, /* 12 */
     {  "All guys must",         "drink under",         "the table"          , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u  }, /* 16 */
     {  "Army guys",             "do 20 pushups,", "civilians 10 pushups"    , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u  }, /* 17 */

 /* Tasks that need criteria. */
     {  "Guys must"            , "do a ",                      "sexy dance"  , .counter = 0u, .nude_level = 0u, .sexy_level = 2u, .gay_level = 0u  }, /* 1  */
     {  "Most naked"           , "guy drinks ",                "2x"          , .counter = 0u, .nude_level = 1u, .sexy_level = 0u, .gay_level = 0u  }, /* 3  */
     {  "All guys lose"        , "two items of ",         "clothing"         , .counter = 0u, .nude_level = 2u, .sexy_level = 0u, .gay_level = 0u  }, /* 4  */
     {  "Guys with"            , ">5 flags&numbers",      "drink 3x"         , .counter = 0u, .nude_level = 0u, .sexy_level = 1u, .gay_level = 0u  }, /* 5  */
     {  "Guys who want to"     , "sleep with 1 of",       "the players drink", .counter = 0u, .nude_level = 0u, .sexy_level = 1u, .gay_level = 0u  }, /* 6  */
     {  "Guys do"              , "bodyshots!!!",                     NULL    , .counter = 0u, .nude_level = 0u, .sexy_level = 2u, .gay_level = 0u  }, /* 7  */
     {  "All Guys"             , "with a boner",          "drink 3x"         , .counter = 0u, .nude_level = 0u, .sexy_level = 2u, .gay_level = 0u  }, /* 8  */
     {  "The last guy"         , "to finish shot"  ,    "loses 2 clothing"   , .counter = 0u, .nude_level = 2u, .sexy_level = 0u, .gay_level = 0u  }, /* 9  */
     {  "All guys drink"       , "while doing"     ,    "pushups"            , .counter = 0u, .nude_level = 0u, .sexy_level = 1u, .gay_level = 0u  }, /* 10 */
     {  "For each inch",         "of his D length",     "guys drink 1 shot"  , .counter = 0u, .nude_level = 0u, .sexy_level = 1u, .gay_level = 0u  }, /* 13 */
     {  "All guys that",         "saw boobs today",     "drink vodka"        , .counter = 0u, .nude_level = 0u, .sexy_level = 1u, .gay_level = 0u  }, /* 14 */
     {  "All guys lose",         "their shirts",        NULL                 , .counter = 0u, .nude_level = 2u, .sexy_level = 0u, .gay_level = 0u  }, /* 15 */
};


/* TODO : Placeholder. */
Private Task_T priv_TextArrayAllLevel3[] =
{
 /* Casual tasks : These should not contain any extra criteria. */
     {  NULL                    , "Everybody drink 2x",            NULL       , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u    }, /* 1  */
     {  NULL                    , "Everybody drink 2x",            NULL       , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u    }, /* 2  */
     {  NULL                    , "Everybody drink 2x",            NULL       , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u    }, /* 3  */
/* Tasks that need criteria. */
/* TODO */
};

/* TODO : Placeholder. */
Private Task_T priv_TextArrayKaisaLevel3[] =
{
 /* Casual tasks : These should not contain any extra criteria. */
     {  NULL                    , "Kaisa drinks 2x",               NULL       , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u      }, /* 1  */
     {  NULL                    , "Kaisa drinks 2x",               NULL       , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u      }, /* 2  */
     {  NULL                    , "Kaisa drinks 2x",               NULL       , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u      }, /* 3  */
 /* Tasks that need criteria. */
 /* TODO */
};

/* Hardcore tasks.  */
Private Task_T priv_TextArrayGirlsLevel4[] =
{
 /* Casual tasks : These should not contain any extra criteria. (Although considering the difficulty level, they might still be pretty hardcore... ) */
     { "Vodka round!!!" ,   "for girls!!!"        , NULL                , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u      }, /*  4  */
     { "Guys give a",       "dare for girls"      , "to do next round"  , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u      }, /*  12 */
     { "Girls give a",      "dare for guys"       , "to do next round"  , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u      }, /*  12 */
     { "Girl wearing the"  ,"most clothes"        , "drinks 3x"         , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u      },  /* 17 */
     { "Girls that still"  ,"have shirts on"      , "must drink 3x"     , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u      },  /* 20 */
     { "Girls who are "    ,"wearing black lingerie" ,"must drink 3x"   , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u      },  /* 20 */

 /* Tasks that need criteria. */

     { "1 girl must",       "do a lapdance"       , "to one guy"        , .counter = 0u, .nude_level = 0u, .sexy_level = 3u, .gay_level = 0u      }, /*  1  */
     { "All girls",         "lose 1 item"         , "of clothing"       , .counter = 0u, .nude_level = 2u, .sexy_level = 0u, .gay_level = 0u      }, /*  2  */
     { "2 girls",           "make out"            , "or drink 3x"       , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 2u      }, /*  3  */
     { "All clean",         "shaven girls"        , "drink 1x"          , .counter = 0u, .nude_level = 0u, .sexy_level = 1u, .gay_level = 0u      }, /*  5  */
     { "All girls",         "who masturbated"     , "today drink 2x"    , .counter = 0u, .nude_level = 0u, .sexy_level = 1u, .gay_level = 0u      }, /*  6  */
     { "Girls must",        "fake an orgasm"      , "or drink 3x"       , .counter = 0u, .nude_level = 0u, .sexy_level = 2u, .gay_level = 0u      }, /*  7  */
     { "Girls must",        "take off shirt"      , "or drink 3x"       , .counter = 0u, .nude_level = 2u, .sexy_level = 0u, .gay_level = 0u      }, /*  8  */
     { "Girls  sit",        "on guy's laps"       , "for next round"    , .counter = 0u, .nude_level = 0u, .sexy_level = 2u, .gay_level = 0u      }, /*  9  */
     { "Everybody who",     "isnt't wearing a"    , "bra drinks vodka"  , .counter = 0u, .nude_level = 1u, .sexy_level = 1u, .gay_level = 0u      }, /*  10 */
     { "2 Girls must",      "French-kiss"         , "or drink vodka"    , .counter = 0u, .nude_level = 0u, .sexy_level = 1u, .gay_level = 2u      }, /*  13 */
     { "Girls who swallow", "drink 1x, others"    , "drink vodka"       , .counter = 0u, .nude_level = 0u, .sexy_level = 1u, .gay_level = 0u      }, /*  14 */
     { "Girls must",        "take off bra or"     , "drink vodka"       , .counter = 0u, .nude_level = 3u, .sexy_level = 0u, .gay_level = 0u      }, /*  15 */
     { "All girls squeeze", "their boobs"         , "while drinking"    , .counter = 0u, .nude_level = 0u, .sexy_level = 2u, .gay_level = 0u      }, /*  16 */ /* God I love this machine :D */
     { "Only the girl"     ,"with the biggest"    , "boobs drinks"      , .counter = 0u, .nude_level = 0u, .sexy_level = 1u, .gay_level = 0u      },  /* 18 */
     { "Girls must make"   ,"a toast with"   ,      "a kinky voice"     , .counter = 0u, .nude_level = 0u, .sexy_level = 1u, .gay_level = 0u      },  /* 19 */
     { "Girls drink 1x for", "each day since they", "last masturbated",   .counter = 0u, .nude_level = 0u, .sexy_level = 3u, .gay_level = 0u      },  /* 21 */
     { "All girls who",      "have been in",        "handcuffs drink 3x", .counter = 0u, .nude_level = 0u, .sexy_level = 1u, .gay_level = 0u      },  /* 22 */
     { "All girls who",      "swallow drink 1x",    "who spit drink 3x",  .counter = 0u, .nude_level = 0u, .sexy_level = 1u, .gay_level = 0u      },  /* 23 */
};

/* Hardcore tasks */
Private Task_T priv_TextArrayGuysLevel4[] =
{
 /* Casual tasks : These should not contain any extra criteria. (Although considering the difficulty level, they might still be pretty hardcore... ) */
     { "Vodka round!!!" ,  "for guys!!!"       ,   NULL                , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u        },  /*  2  */
     { "Girls give a",     "dare for guys"     ,   "to do next round"  , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u        },  /*  7  */
     { "Guys",             "drink without"     ,   "using hands"       , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u        },  /*  11 */
     { "Guy wearing the"  ,"least clothes"        ,"drinks 3x"         , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u        },  /*  13 */
     { "Guy who is",       "most wasted"       ,   "does a vodka"      , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u        },  /*  9  */

 /* Tasks that need criteria. */
     { "2 guys",           "make out"          ,   "or drink 3x"       , .counter = 0u, .nude_level = 0u, .sexy_level = 1u, .gay_level = 2u        },  /*  1  */
     { "All guys",         "who wanked"        ,   "today drink 2x"    , .counter = 0u, .nude_level = 0u, .sexy_level = 2u, .gay_level = 0u        },  /*  3  */
     { "Guys must",        "fake an orgasm"    ,   "or drink vodka"    , .counter = 0u, .nude_level = 0u, .sexy_level = 2u, .gay_level = 0u        },  /*  4  */
     { "Guys must name",    "10 sex positions" ,   "or drink vodka"    , .counter = 0u, .nude_level = 0u, .sexy_level = 1u, .gay_level = 0u        },  /*  5  */
     { "Next round guys",   "do bodyshots"     ,   "from the girls"    , .counter = 0u, .nude_level = 0u, .sexy_level = 3u, .gay_level = 0u        },  /*  6  */
     { "All guys",         "get a spanking"    ,   "from the girls"    , .counter = 0u, .nude_level = 0u, .sexy_level = 2u, .gay_level = 0u        },  /*  8  */
     { "All guys",         "lose"              ,   "their pants"       , .counter = 0u, .nude_level = 2u, .sexy_level = 0u, .gay_level = 0u        },  /*  10 */
     { "Guys that haven't","eaten pussy in 30"    ,"days drink vodka"  , .counter = 0u, .nude_level = 0u, .sexy_level = 1u, .gay_level = 0u        },  /*  12 */
     { "2 guys must"      ,"make out or"          ,"drink vodka"       , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 3u        },  /*  14 */
     { "Guys drink 1x for", "each day since they", "last masturbated",   .counter = 0u, .nude_level = 0u, .sexy_level = 2u, .gay_level = 0u        },  /*  15 */
};


/* TODO : Placeholder. */
Private Task_T priv_TextArrayAllLevel4[] =
{
 /* Casual tasks : These should not contain any extra criteria. (Although considering the difficulty level, they might still be pretty hardcore... ) */

     {  NULL                    , "Everybody drink 2x",            NULL       , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u          }, /* 1  */
     {  NULL                    , "Everybody drink 2x",            NULL       , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u          }, /* 2  */
     {  NULL                    , "Everybody drink 2x",            NULL       , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u          }, /* 3  */
 /* Tasks that need criteria. */
 /* TODO */
};

/* TODO : Placeholder. */
Private Task_T priv_TextArrayKaisaLevel4[] =
{
 /* Casual tasks : These should not contain any extra criteria. (Although considering the difficulty level, they might still be pretty hardcore... ) */

     {  NULL                    , "Kaisa drinks 2x",               NULL       , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u            }, /* 1  */
     {  NULL                    , "Kaisa drinks 2x",               NULL       , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u            }, /* 2  */
     {  NULL                    , "Kaisa drinks 2x",               NULL       , .counter = 0u, .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u            }, /* 3  */
 /* Tasks that need criteria. */
 /* TODO */
};


Private const Task_Array_Descriptor priv_task_array_desc[NUMBER_OF_TASK_TYPES][4] =
{
     {
        {.task_array = priv_TextArrayGirlsLevel1 , .number_of_array_items = NUMBER_OF_ITEMS(priv_TextArrayGirlsLevel1)},
        {.task_array = priv_TextArrayGirlsLevel2 , .number_of_array_items = NUMBER_OF_ITEMS(priv_TextArrayGirlsLevel2)},
        {.task_array = priv_TextArrayGirlsLevel3 , .number_of_array_items = NUMBER_OF_ITEMS(priv_TextArrayGirlsLevel3)},
        {.task_array = priv_TextArrayGirlsLevel4 , .number_of_array_items = NUMBER_OF_ITEMS(priv_TextArrayGirlsLevel4)},
     },

     {
        {.task_array = priv_TextArrayGuysLevel1  , .number_of_array_items = NUMBER_OF_ITEMS(priv_TextArrayGuysLevel1)},
        {.task_array = priv_TextArrayGuysLevel2  , .number_of_array_items = NUMBER_OF_ITEMS(priv_TextArrayGuysLevel2)},
        {.task_array = priv_TextArrayGuysLevel3  , .number_of_array_items = NUMBER_OF_ITEMS(priv_TextArrayGuysLevel3)},
        {.task_array = priv_TextArrayGuysLevel4  , .number_of_array_items = NUMBER_OF_ITEMS(priv_TextArrayGuysLevel4)},
     },

     {
        {.task_array = priv_TextArrayAllLevel1  , .number_of_array_items = NUMBER_OF_ITEMS(priv_TextArrayAllLevel1)},
        {.task_array = priv_TextArrayAllLevel2  , .number_of_array_items = NUMBER_OF_ITEMS(priv_TextArrayAllLevel2)},
        {.task_array = priv_TextArrayAllLevel3  , .number_of_array_items = NUMBER_OF_ITEMS(priv_TextArrayAllLevel3)},
        {.task_array = priv_TextArrayAllLevel4  , .number_of_array_items = NUMBER_OF_ITEMS(priv_TextArrayAllLevel4)},
     },

     {
        {.task_array = priv_TextArrayKaisaLevel1  , .number_of_array_items = NUMBER_OF_ITEMS(priv_TextArrayKaisaLevel1)},
        {.task_array = priv_TextArrayKaisaLevel2  , .number_of_array_items = NUMBER_OF_ITEMS(priv_TextArrayKaisaLevel2)},
        {.task_array = priv_TextArrayKaisaLevel3  , .number_of_array_items = NUMBER_OF_ITEMS(priv_TextArrayKaisaLevel3)},
        {.task_array = priv_TextArrayKaisaLevel4  , .number_of_array_items = NUMBER_OF_ITEMS(priv_TextArrayKaisaLevel4)},
     },
};


/*****************************************************************************************************
 *
 * Private variable declarations
 *
 *****************************************************************************************************/
Private const Task_T * priv_task_str_ptr;
Private char priv_str_buf[64];
Private SpecialTaskFunc priv_selected_task_ptr;


/*****************************************************************/

/*****************************************************************************************************
 *
 * Private function definitions
 *
 *****************************************************************************************************/

Public Boolean girlsSpecialTask(U8 sec)
{
    Boolean res = FALSE;
    static U8 test_counter = 0u;

    /* If sec is 0, then we have just begun. */
    if (sec == 0u)
    {
        priv_selected_task_ptr = priv_special_tasks_girls_array[test_counter];
        test_counter++;
        if(test_counter >= NUMBER_OF_ITEMS(priv_special_tasks_girls_array))
        {
            test_counter = 0u;
        }
    }

    res = priv_selected_task_ptr(sec, TASK_FOR_GIRLS);
    return res;
}


Public Boolean guysSpecialTask(U8 sec)
{
    Boolean res = FALSE;
    static U8 test_counter = 0u;

    if (sec == 0u)
    {
        priv_selected_task_ptr = priv_special_tasks_guys_array[test_counter];
        test_counter++;
        if(test_counter >= NUMBER_OF_ITEMS(priv_special_tasks_guys_array))
        {
            test_counter = 0u;
        }
    }

    res = priv_selected_task_ptr(sec, TASK_FOR_GUYS);
    return res;
}

Public Boolean everybodySpecialTask(U8 sec)
{
    Boolean res = FALSE;
    static U8 test_counter = 0u;

    if (sec == 0u)
    {
        priv_selected_task_ptr = priv_special_tasks_everybody_array[test_counter];
        test_counter++;
        if (test_counter >= NUMBER_OF_ITEMS(priv_special_tasks_everybody_array))
        {
            test_counter = 0u;
        }
    }

    res = priv_selected_task_ptr(sec, TASK_FOR_EVERYONE);
    return res;
}

Public Boolean kaisaSpecialTask(U8 sec)
{
    Boolean res = FALSE;
    static U8 test_counter = 0u;

    if (sec == 0u)
    {
        priv_selected_task_ptr = priv_special_tasks_kaisa_array[test_counter];
        test_counter++;
        if (test_counter >= NUMBER_OF_ITEMS(priv_special_tasks_kaisa_array))
        {
            test_counter = 0u;
        }
    }

    res = priv_selected_task_ptr(sec, TASK_FOR_KAISA);
    return res;
}


/*****************************************************************************************************
 *
 * Private function definitions
 *
 *****************************************************************************************************/
Private Boolean DrinkTwiceTask(U8 sec, SpecialTaskType type)
{
    Boolean res = FALSE;

    switch(sec)
    {
    case(1u):
       display_clear();
       switch(type)
       {
           case TASK_FOR_GIRLS:
               strcpy(priv_str_buf, "Girls");
               break;
           case TASK_FOR_GUYS:
               strcpy(priv_str_buf, "Guys");
               break;
           case TASK_FOR_EVERYONE:
               strcpy(priv_str_buf, "Everyone");
               break;
           case TASK_FOR_KAISA:
               strcpy(priv_str_buf, "Kaisa");
               break;
           default:
               break;
       }

       strcat(priv_str_buf, " drink");
       display_drawStringCenter(priv_str_buf, 64u, 2u, FONT_LARGE_FONT, FALSE);
       display_drawStringCenter("2x", 64u ,20u, FONT_LARGE_FONT, FALSE);
       break;
    case (2u):
       display_drawBitmapCenter(&small_shot_bitmap, 64 - SMALL_SHOT_INTERVAL, SMALL_SHOT_Y, FALSE);
       break;
    case (3u):
        display_drawBitmapCenter(&small_shot_bitmap, 64 + SMALL_SHOT_INTERVAL, SMALL_SHOT_Y, FALSE);
       break;
    case(10u):
       res = TRUE;
       break;
    default:
        break;
    }

    return res;
}


/* The sec parameter specifies the current second from the beginning of the task.
 * This function is called cyclically after every second. */
Private Boolean SpecialTaskWithRandomText(U8 sec, SpecialTaskType type)
{
    Boolean res = FALSE;

    int taskLevel;


    switch(type)
    {
        case TASK_FOR_GIRLS:
            taskLevel = pot_getSelectedRange(POTENTIOMETER_GIRLS);
            break;
        case TASK_FOR_GUYS:
            taskLevel = pot_getSelectedRange(POTENTIOMETER_GUYS);
            break;
        case TASK_FOR_EVERYONE:
            taskLevel = (pot_getSelectedRange(POTENTIOMETER_GIRLS) + pot_getSelectedRange(POTENTIOMETER_GUYS)) / 2; /* We just take the average here... */
            break;
        case TASK_FOR_KAISA:
            taskLevel = pot_getSelectedRange(POTENTIOMETER_KAISA);
            break;
        default:
            /* Should not happen. */
            taskLevel = -1;
            break;
    }


    if (taskLevel < 0)
    {
        taskLevel = 0;
    }

    Task_T * SelectedTaskArray;
    U16 number_of_items;

    SelectedTaskArray = priv_task_array_desc[type][taskLevel].task_array;
    number_of_items = priv_task_array_desc[type][taskLevel].number_of_array_items;

    switch(sec)
    {
        case(1u):
            {
                priv_task_str_ptr = getRandomTaskFromArray(SelectedTaskArray, number_of_items);
                break;
            }
        case (2u):
            {
                display_clear();
                display_drawStringCenter(priv_task_str_ptr->upper_text, 64u, 4u, FONT_MEDIUM_FONT, FALSE);
            }
            break;
        case (3u):
            {
                display_drawStringCenter(priv_task_str_ptr->middle_text, 64u, 23u, FONT_MEDIUM_FONT, FALSE);
            }
            break;
        case(4u):
            {
                display_drawStringCenter(priv_task_str_ptr->lower_text, 64u, 43u, FONT_MEDIUM_FONT, FALSE);
            }
            break;
        case(12u):
            {
                res = TRUE;
            }
            break;
        default:
            break;
    }

    return res;
}

Private const Task_T priv_default_task = {  "Error selecting task!" , "Everybody drink 2x", "just in case", .counter = 0u  , .nude_level = 0u, .sexy_level = 0u, .gay_level = 0u };


/* NOTE : Each array must contain at least one task with a 0,0,0 for extra requirements. Otherwise there will be no tasks here. */
Private const Task_T * getRandomTaskFromArray(Task_T * array, U8 array_size)
{
    U8 ix;
    U8 min_count = 0xffu;

    U8 * index_array;
    U8 available = 0u;
    U16 result_index;

    U8 sexy_level = pot_getSelectedRange(POTENTIOMETER_SEXY_LEVEL);
    U8 nude_level = pot_getSelectedRange(POTENTIOMETER_NUDE_LEVEL);
    U8 gay_level = pot_getSelectedRange(POTENTIOMETER_RAINBOW_LEVEL);

    index_array = (U8 *)malloc(sizeof(U8) * array_size);

    if (index_array == NULL)
    {
        /* TODO : Review error handling. Currently will be stuck in infinite loop. */
        while(1);
    }

    for (ix = 0u; ix < array_size; ix++)
    {
        if (isTaskEnabled(&array[ix], sexy_level, nude_level, gay_level))
        {
            if (array[ix].counter < min_count)
            {
                min_count = array[ix].counter;
            }
        }
    }

    for (ix = 0u; ix < array_size; ix++)
    {
        if (isTaskEnabled(&array[ix], sexy_level, nude_level, gay_level))
        {
            if (array[ix].counter <= min_count)
            {
                /* We can use this item. */
                index_array[available] = ix;
                available++;
            }
        }
    }

    if (available > 0u)
    {
        /* So now index_array should contain all the unused indexes. */
        result_index = index_array[generate_random_number(available - 1u)];
    }
    else
    {
        /* Something has gone wrong. */
        free (index_array);
        return &priv_default_task;
    }

    free(index_array);
    array[result_index].counter++;
    return &array[result_index];
}


Private Boolean isTaskEnabled(const Task_T * task_ptr, U8 sexy_level, U8 nude_level, U8 gay_level)
{
    Boolean res = TRUE;

    if (task_ptr->sexy_level > sexy_level)
    {
        res = FALSE;
    }

    if (task_ptr->nude_level > nude_level)
    {
        res = FALSE;
    }

    if (task_ptr->gay_level > gay_level)
    {
        res = FALSE;
    }

    return res;
}
