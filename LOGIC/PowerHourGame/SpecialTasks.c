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
    U8 counter; /* Number of times, this task has been selected. */
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
     { "The girl with ",     "the fanciest clothes"  , "drinks 2x"         , .counter = 0u  }, /* 1  */
     { NULL,                 "Only girls drink"      , NULL                , .counter = 0u  }, /* 2  */
     { "Girls drink",        "without "      ,         "using hands"       , .counter = 0u  }, /* 3  */
     { "Choose one girl",    "who drinks 3x ",         NULL                , .counter = 0u  }, /* 4  */
     { "The girl with",      "the longest hair ",      "drinks a shot"     , .counter = 0u  }, /* 5  */
     { "The girl with",      "the highest voice",      "drinks a shot"     , .counter = 0u  }, /* 6  */
     { "All blondes",        "drink 2x ",              NULL                , .counter = 0u  }, /* 7  */
     { "All brunettes",      "drink 2x ",              NULL                , .counter = 0u  }, /* 8  */
     { "The tallest girl",   "drinks 2x ",             NULL                , .counter = 0u  }, /* 9  */
     { "The youngest girl",  "drinks 2x ",             NULL                , .counter = 0u  }, /* 10 */
     { "All girl BESTies",   "drink  2x ",             NULL                , .counter = 0u  }, /* 11 */
     { "The shortest girl",  "drinks 2x ",             NULL                , .counter = 0u  }, /* 12 */
     { "Girls propose",      "the next toast ",        NULL                , .counter = 0u  }, /* 13 */
     { "All redheads",       "drink 2x ",              NULL                , .counter = 0u  }, /* 14 */
     { "All girls",          "with purple hair ",      "drink 2x"          , .counter = 0u  }, /* 15 */
     { "All girls",          "with glasses ",          "drink 2x"          , .counter = 0u  }, /* 16 */
     { "Choose 1 girl",      "that drinks 2x",        NULL                 , .counter = 0u  }, /* 17 */
     { "All girls",          "wearing black",         "drink 2x"           , .counter = 0u  }, /* 18 */
};

/* Easy tasks. */
Private Task_T priv_TextArrayGuysLevel1[] =
{
     {  NULL                    , "Only guys drink",            NULL          , .counter = 0u  }, /* 1  */
     {  "Guys drink"            , "without",                 "using hands"    , .counter = 0u  }, /* 2  */
     {  "The toughest guy"      , "drinks 3x",                  NULL          , .counter = 0u  }, /* 3  */
     {  "The biggest playboy"   , "drinks 3x",                  NULL          , .counter = 0u  }, /* 4  */
     {  NULL                    , "Guys must sing",         "a song together" , .counter = 0u  }, /* 5  */
     {  "Last guy to put his"   , "finger on his nose",        "drinks 2x"    , .counter = 0u  }, /* 6  */
     {  "Choose one guy"        , "who drinks 3x ",             NULL          , .counter = 0u  }, /* 7  */
     {  "All guys"              , "drop and do 10  ",        "pushups"        , .counter = 0u  }, /* 8  */
     {  "All guys with"         , "a six-pack ",             "drink 3x"       , .counter = 0u  }, /* 9  */
     {  "The most wasted"       , "guy drinks",              "water-shot"     , .counter = 0u  }, /* 10 */
     {  "Guys with"             , "hair gel",                "drink 3x"       , .counter = 0u  }, /* 11 */
     {  "Single Guys "          , "drink vodka",             NULL             , .counter = 0u  }, /* 12 */
     {  "The youngest guy"      , "drinks 2x",               NULL             , .counter = 0u  }, /* 14 */
     {  "All guys that"         , "are in the army",         "drink 3x"       , .counter = 0u  }, /* 15 */
     {  "All guys",               "with glasses ",           "drink 2x"       , .counter = 0u  }, /* 16 */
     {  "One guy",                "must say a toast",        "that rhymes"    , .counter = 0u  }, /* 17 */
     { "All guys whose" ,         "name starts with",     "S drinks 2x"       , .counter = 0u  }, /* 18 */
};

/* TODO : Placeholder. */
Private Task_T priv_TextArrayAllLevel1[] =
{
     {  NULL                    , "Everybody drink 2x",            NULL       , .counter = 0u  }, /* 1  */
     {  NULL                    , "Everybody drink 2x",            NULL       , .counter = 0u  }, /* 2  */
     {  NULL                    , "Everybody drink 2x",            NULL       , .counter = 0u  }, /* 3  */
};

/* TODO : Placeholder. */
Private Task_T priv_TextArrayKaisaLevel1[] =
{
     {  NULL                    , "Kaisa drinks 2x",               NULL       , .counter = 0u  }, /* 1  */
     {  NULL                    , "Kaisa drinks 2x",               NULL       , .counter = 0u  }, /* 2  */
     {  NULL                    , "Kaisa drinks 2x",               NULL       , .counter = 0u  }, /* 3  */
};

/* Medium tasks */
Private Task_T priv_TextArrayGirlsLevel2[] =
{
     { "The girl with ",     "the sexiest voice"     , "drinks 2x "        , .counter = 0u  }, /* 1  */
     { "Girls",              "I have never ever"     , NULL                , .counter = 0u  }, /* 2  */
     { "The girl with  ",    "the largest boobs"     , "drinks 2x"         , .counter = 0u  }, /* 3  */
     { "All couples  ",      "drink 2x"              , NULL                , .counter = 0u  }, /* 4  */
     { "All girls whose" ,   "name starts with",     "S drinks 2x"         , .counter = 0u  }, /* 5  */
     { "All bad girls",      "drink 2x ",              NULL                , .counter = 0u  }, /* 6  */
     { "All good girls",     "drink 2x ",              NULL                , .counter = 0u  }, /* 7  */
     { "The girls with" ,    "the shortest skirt",   "drinks 2x"           , .counter = 0u  }, /* 8  */
     { "All female" ,        "organisers",           "drink 2x"            , .counter = 0u  }, /* 9  */
     { "Last girl to" ,      "put finger on",        "nose drinks 2x"      , .counter = 0u  }, /* 10 */
     { "All girls that",     "have boyfriends ",     "drink 2x"            , .counter = 0u  }, /* 11 */
     { "All blondes",        "drink vodka ",              NULL             , .counter = 0u  }, /* 12 */
     { "All brunettes",      "drink vodka ",              NULL             , .counter = 0u  }, /* 13 */
     { "Girls who are",      "former virgins",       "drink 2x"            , .counter = 0u  }, /* 14 */
     { "Girls must purr",    "like a kitten",        "after drinking!"     , .counter = 0u  }, /* 15 */
     { "All girls whose" ,   "name starts with",     "L drinks 2x"         , .counter = 0u  }, /* 16 */
     { "All girls who" ,     "kissed a girl",        "today drink 2x"      , .counter = 0u  }, /* 17 */
     { "All girls with" ,    "blue eyes",            "drink 2x"            , .counter = 0u  }, /* 18 */
     { "All girls with" ,    "a pony tail",          "drink 2x"            , .counter = 0u  }, /* 19 */
};

/* Medium tasks. */
Private Task_T priv_TextArrayGuysLevel2[] =
{
     {  "The guy with the"      , "biggest balls",            "drinks vodka"        ,  .counter = 0u  },  /* 1  */
     {  "Guys"                  , "Never have I ever",          NULL                ,  .counter = 0u  },  /* 2  */
     {  "All guys lose"         , "One Item of Clothing",       NULL                ,  .counter = 0u  },  /* 3  */
     {  "All guys whose"        , "name starts with",         "A drinks 2x"         ,  .counter = 0u  },  /* 4  */
     {  "All couples  "         , "drink 2x",                   NULL                ,  .counter = 0u  },  /* 5  */
     {  "All male"              , "organisers",               "drink 2x"            ,  .counter = 0u  },  /* 6  */
     {  "The guy with"          , "the biggest beer",         "belly drinks 2x"     ,  .counter = 0u  },  /* 7  */
     {  "All guys with"         , "beards",                   "drink 2x"            ,  .counter = 0u  },  /* 8  */
     {  "All former senors",      "drink vodka",               NULL                 ,  .counter = 0u  },  /* 9  */
     {  "All guys who are",       "virgins drink 2x",         "and best of luck!"   ,  .counter = 0u  },  /* 10 */
     {  "Guys who have",          "kissed a dude",            "drink vodka!"        ,  .counter = 0u  },  /* 11 */
     {  "Guys must",              "say meow",                 "after drinking!"     ,  .counter = 0u  },  /* 12 */
     {  "Guys must do",           "10 squats",                "before drinking"     ,  .counter = 0u  },  /* 13 */
     {  "Guys that love",         "heavy metal",              "drink vodka"         ,  .counter = 0u  },  /* 14 */
     {  "Guys say No Homo!",      "Look each other in",       "the eyes and drink"  ,  .counter = 0u  },  /* 15 */
};


/* TODO : Placeholder. */
Private Task_T priv_TextArrayAllLevel2[] =
{
     {  NULL                    , "Everybody drink 2x",            NULL       , .counter = 0u  }, /* 1  */
     {  NULL                    , "Everybody drink 2x",            NULL       , .counter = 0u  }, /* 2  */
     {  NULL                    , "Everybody drink 2x",            NULL       , .counter = 0u  }, /* 3  */
};

/* TODO : Placeholder. */
Private Task_T priv_TextArrayKaisaLevel2[] =
{
     {  NULL                    , "Kaisa drinks 2x",               NULL       , .counter = 0u  }, /* 1  */
     {  NULL                    , "Kaisa drinks 2x",               NULL       , .counter = 0u  }, /* 2  */
     {  NULL                    , "Kaisa drinks 2x",               NULL       , .counter = 0u  }, /* 3  */
};

/* Hard tasks.  */
Private Task_T priv_TextArrayGirlsLevel3[] =
{
     {  "Girls must"             , "do a ",                      "sexy dance"  , .counter = 0u  }, /* 1  */
     {  "Vodka round!!!",          "for girls!!!"   ,                   NULL   , .counter = 0u  }, /* 2  */
     {  "Most naked"           ,   "girl drinks ",               "2x"          , .counter = 0u  }, /* 3  */
     {  "All girls lose"       ,   "two items of ",              "clothing"    , .counter = 0u  }, /* 4  */
     {  "Girls with"           ,   ">5 flags&numbers",           "drink 3x"    , .counter = 0u  }, /* 5  */
     {  "Girls who've had"     ,   "sex with 1 of the",  "players drink 2x"    , .counter = 0u  }, /* 6  */
     {  "Girls do"             ,   "bodyshots!!!",                     NULL    , .counter = 0u  }, /* 7  */
     {  "Girl showing the"     ,   "least cleavage"  ,           "drinks 3x"   , .counter = 0u  }, /* 9  */
     {  "The last girl"        ,   "to finish shot"  ,    "loses 1 clothing"   , .counter = 0u  }, /* 10 */
     {  "Girl showing the"     ,   "most cleavage"  ,           "drinks 3x"    , .counter = 0u  }, /* 11 */
     {  "Girls can"            ,   "slap one of"  ,              "the guys"    , .counter = 0u  }, /* 12 */
     {  "Girls- 1 shot for"    ,  "each guy they slept"  , "with this year"    , .counter = 0u  }, /* 14 */
     {  "All girls with"       ,  "black underwear"      , "drink vodka"       , .counter = 0u  }, /* 15 */
     { " Girls must make"      ,  "a naughty"   ,                "toast"       , .counter = 0u  }, /* 16 */
     { " Girls must make"      ,  "a kinky"   ,                  "toast"       , .counter = 0u  }, /* 17 */
     {  "All girls must",         "drink under",         "the table"           , .counter = 0u  }, /* 18 */
     {  "All girls must",         "drink with hands",    "on their butts"      , .counter = 0u  }, /* 19 */
};


/* Hard tasks.  */
Private Task_T priv_TextArrayGuysLevel3[] =
{
     {  "Guys must"            , "do a ",                      "sexy dance"  , .counter = 0u  }, /* 1  */
     {  "Vodka round!!!",        "for guys!!!"   ,                   NULL    , .counter = 0u  }, /* 2  */
     {  "Most naked"           , "guy drinks ",                "2x"          , .counter = 0u  }, /* 3  */
     {  "All guys lose"        , "two items of ",         "clothing"         , .counter = 0u  }, /* 4  */
     {  "Guys with"            , ">5 flags&numbers",      "drink 3x"         , .counter = 0u  }, /* 5  */
     {  "Guys who want to"     , "sleep with 1 of",       "the players drink", .counter = 0u  }, /* 6  */
     {  "Guys do"              , "bodyshots!!!",                     NULL    , .counter = 0u  }, /* 7  */
     {  "All Guys"             , "with a boner",          "drink 3x"         , .counter = 0u  }, /* 8  */
     {  "The last guy"         , "to finish shot"  ,    "loses 2 clothing"   , .counter = 0u  }, /* 9  */
     {  "All guys drink"       , "while doing"     ,    "pushups"            , .counter = 0u  }, /* 10 */
     {  "1 guy must",            "drink while"     ,    "upside down"        , .counter = 0u  }, /* 12 */
     {  "For each inch",         "of his D length",     "guys drink 1 shot"  , .counter = 0u  }, /* 13 */
     {  "All guys that",         "saw boobs today",     "drink vodka"        , .counter = 0u  }, /* 14 */
     {  "All guys lose",         "their shirts",        NULL                 , .counter = 0u  }, /* 15 */
     {  "All guys must",         "drink under",         "the table"          , .counter = 0u  }, /* 16 */
     {  "Army guys",             "do 20 pushups",       NULL                 , .counter = 0u  }, /* 17 */
};


/* TODO : Placeholder. */
Private Task_T priv_TextArrayAllLevel3[] =
{
     {  NULL                    , "Everybody drink 2x",            NULL       , .counter = 0u  }, /* 1  */
     {  NULL                    , "Everybody drink 2x",            NULL       , .counter = 0u  }, /* 2  */
     {  NULL                    , "Everybody drink 2x",            NULL       , .counter = 0u  }, /* 3  */
};

/* TODO : Placeholder. */
Private Task_T priv_TextArrayKaisaLevel3[] =
{
     {  NULL                    , "Kaisa drinks 2x",               NULL       , .counter = 0u  }, /* 1  */
     {  NULL                    , "Kaisa drinks 2x",               NULL       , .counter = 0u  }, /* 2  */
     {  NULL                    , "Kaisa drinks 2x",               NULL       , .counter = 0u  }, /* 3  */
};

/* Hardcore tasks.  */
Private Task_T priv_TextArrayGirlsLevel4[] =
{
     { "1 girl must",       "do a lapdance"       , "to one guy"        , .counter = 0u  }, /*  1  */
     { "All girls",         "lose 1 item"         , "of clothing"       , .counter = 0u  }, /*  2  */
     { "2 girls",           "make out"            , "or drink 3x"       , .counter = 0u  }, /*  3  */
     { "Vodka round!!!" ,   "for girls!!!"        , NULL                , .counter = 0u  }, /*  4  */
     { "All clean",         "shaven girls"        , "drink 1x"          , .counter = 0u  }, /*  5  */
     { "All girls",         "who masturbated"     , "today drink 2x"    , .counter = 0u  }, /*  6  */
     { "Girls must",        "fake an orgasm"      , "or drink 3x"       , .counter = 0u  }, /*  7  */
     { "Girls must",        "take off shirt"      , "or drink 3x"       , .counter = 0u  }, /*  8  */
     { "Girls  sit",        "on guy's laps"       , "for next round"    , .counter = 0u  }, /*  9  */
     { "Everybody who",     "isnt't wearing a"    , "bra drinks vodka"  , .counter = 0u  }, /*  10 */
     { "Guys give a",       "dare for girls"      , "to do next round"  , .counter = 0u  }, /*  12 */
     { "2 Girls must",      "French-kiss"         , "or drink vodka"    , .counter = 0u  }, /*  13 */
     { "Girls who swallow", "drink 1x, others"    , "drink vodka"       , .counter = 0u  }, /*  14 */
     { "Girls must",        "take off bra or"     , "drink vodka"       , .counter = 0u  }, /*  15 */
     { "All girls squeeze", "their boobs"         , "while drinking"    , .counter = 0u  }, /*  16 */ /* God I love this machine :D */
     { "Girl wearing the"  ,"most clothes"        , "drinks 3x"         , .counter = 0u  },  /* 17 */
     { "Only the girl"     ,"with the biggest"    , "boobs drinks"      , .counter = 0u  },  /* 18 */
     { "Girls must make"   ,"a toast with"   ,      "a kinky voice"     , .counter = 0u  },  /* 19 */
     { "Girls that still"  ,"have shirts on" ,      "must drink 3x"     , .counter = 0u  },  /* 20 */
     { "Girls drink 1x for", "each day since they", "last masturbated",   .counter = 0u  },  /* 21 */
     { "All girls who",      "have been in",        "handcuffs drink 3x", .counter = 0u  },  /* 22 */
     { "All girls who",      "swallow drink 1x",    "who spit drink 3x",  .counter = 0u  },  /* 23 */
};

/* Hardcore tasks */
Private Task_T priv_TextArrayGuysLevel4[] =
{
     { "2 guys",           "make out"          ,   "or drink 3x"       , .counter = 0u  },  /*  1  */
     { "Vodka round!!!" ,  "for girls!!!"      ,   NULL                , .counter = 0u  },  /*  2  */
     { "All guys",         "who wanked"        ,   "today drink 2x"    , .counter = 0u  },  /*  3  */
     { "Guys must",        "fake an orgasm"    ,   "or drink vodka"    , .counter = 0u  },  /*  4  */
     { "Guys must name",    "10 sex positions" ,   "or drink vodka"    , .counter = 0u  },  /*  5  */
     { "Next round guys",   "do bodyshots"     ,   "from the girls"    , .counter = 0u  },  /*  6  */
     { "Girls give a",     "dare for guys"     ,   "to do next round"  , .counter = 0u  },  /*  7  */
     { "All guys",         "get a spanking"    ,   "from the girls"    , .counter = 0u  },  /*  8  */
     { "Guy who is",       "most wasted"       ,   "does a vodka"      , .counter = 0u  },  /*  9  */
     { "All guys",         "lose"              ,   "their pants"       , .counter = 0u  },  /*  10 */
     { "Guys",             "drink without"     ,   "using hands"       , .counter = 0u  },  /*  11 */
     { "Guys that haven't","eaten pussy in 30"    ,"days drink vodka"  , .counter = 0u  },  /*  12 */
     { "Guy wearing the"  ,"least clothes"        ,"drinks 3x"         , .counter = 0u  },  /*  13 */
     { "2 guys must"      ,"make out or"          ,"drink vodka"       , .counter = 0u  },  /*  14 */
     { "Guys drink 1x for", "each day since they", "last masturbated",   .counter = 0u  },  /*  15 */
};


/* TODO : Placeholder. */
Private Task_T priv_TextArrayAllLevel4[] =
{
     {  NULL                    , "Everybody drink 2x",            NULL       , .counter = 0u  }, /* 1  */
     {  NULL                    , "Everybody drink 2x",            NULL       , .counter = 0u  }, /* 2  */
     {  NULL                    , "Everybody drink 2x",            NULL       , .counter = 0u  }, /* 3  */
};

/* TODO : Placeholder. */
Private Task_T priv_TextArrayKaisaLevel4[] =
{
     {  NULL                    , "Kaisa drinks 2x",               NULL       , .counter = 0u  }, /* 1  */
     {  NULL                    , "Kaisa drinks 2x",               NULL       , .counter = 0u  }, /* 2  */
     {  NULL                    , "Kaisa drinks 2x",               NULL       , .counter = 0u  }, /* 3  */
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

    /* TODO : Placeholder. */
    int taskLevel = pot_getSelectedRange(POTENTIOMETER_ONE);

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


Private const Task_T * getRandomTaskFromArray(Task_T * array, U8 array_size)
{
    U8 ix;
    U8 min_count = 0xffu;

    U8 * index_array;
    U8 unused = 0u;
    U16 result_index;

    index_array = (U8 *)malloc(sizeof(U8) * array_size);

    if (index_array == NULL)
    {
        /* TODO : Review error handling. Currently will be stuck in infinite loop. */
        while(1);
    }

    for (ix = 0u; ix < array_size; ix++)
    {

        if (array[ix].counter < min_count)
        {
            min_count = array[ix].counter;
        }
    }

    for (ix = 0u; ix < array_size; ix++)
    {
        if (array[ix].counter <= min_count)
        {
            /* We can use this item. */
            index_array[unused] = ix;
            unused++;
        }
    }

    if (unused > 0u)
    {
        /* So now index_array should contain all the unused indexes. */
        result_index = index_array[generate_random_number(unused - 1u)];
    }
    else
    {
        /* TODO : Review this case. something has gone seriously wrong... */
        result_index = 0u;
    }

    free(index_array);
    array[result_index].counter++;
    return &array[result_index];
}

