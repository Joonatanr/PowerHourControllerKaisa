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

#define SMALL_SHOT_X 20u
#define SMALL_SHOT_Y 32u
#define SMALL_SHOT_INTERVAL 20u

typedef struct
{
    const char * upper_text;
    const char * middle_text;
    const char * lower_text;
    U8 counter; /* Number of times, this task has been selected. */
} Task_T;


//Private Boolean DrinkTwiceTask(U8 sec, const char * headerWord);
Private Boolean DrinkTwiceTask(U8 sec, SpecialTaskType type);
Private Boolean SpecialTaskWithRandomText(U8 sec, SpecialTaskType type);
Private const Task_T * getRandomTaskFromArray(Task_T * array, U8 array_size);

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


Private char priv_str_buf[64];
Private SpecialTaskFunc priv_selected_task_ptr;

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


Private Task_T priv_TextArrayAlumniLevel1[] =
{
     { "Oldest alumni", 	"drinks 2x", 				NULL 			, .counter = 0u 	}, /* 1  */
     { "Youngest alumni", 	"drinks 2x", 				NULL 			, .counter = 0u		}, /* 2  */
     { "One alumni", 		"must tell", 				"a joke " 		, .counter = 0u 	}, /* 3  */
	 { "All alumni",        "drink 2x", 				NULL			, .counter = 0u		}, /* 4  */
	 { "Everybody over 35", "skip 1 round", 			"rest drink 2x"	, .counter = 0u     }, /* 5  */
	 { "Alumni with a",   	"young BEST girlfriend", 	"drink 3x" 		, .counter = 0u     }, /* 6  */
     { "All alumni who",    "remember GA 2016", 		"drink 3x"		, .counter = 0u 	}, /* 7  */
	 { "Alumni who are",    "already drunk",    		"drink 3x"      , .counter = 0u     }, /* 8  */
	 { "Alumni who have", 	"been to Ekaterinburg", 	"drink 2x"		, .counter = 0u		}, /* 9  */
     { "Alumni board",      "drinks 3x"				    , NULL          , .counter = 0u		}, /* 10 */
};

Private Task_T priv_TextArrayCoreTeamLevel1[] =
{
     { "All core team",     "members drink", "2x " ,                              .counter = 0u },  /* 1  */
     { "All core team",     "members drink", "3x " ,                              .counter = 0u },  /* 2  */
     { "Project MO",        "drinks 3x",      NULL ,                              .counter = 0u },  /* 3  */
     { "CT members who have", "followed ghantt chart", "skip this round",         .counter = 0u },  /* 4  */
     { "Core team member", "who was most wasted", "last night drink 2x",          .counter = 0u },  /* 5  */
     { "MO takes one shot", "for each CT member", "present",                      .counter = 0u },  /* 6  */
     { "Core team member", "who slept least", "skip this round",                  .counter = 0u },  /* 7  */
     { "Whoever made breakfast", "for everybody today", "skip this round",        .counter = 0u },  /* 8  */
     { "All core team", "girls drink 2x", NULL,                                   .counter = 0u },  /* 9  */
     { "All core team", "guys drink 2x", NULL,                                    .counter = 0u },  /* 10 */
};

Private Task_T priv_TextArraySocRespLevel1[] =
{
     { "Social responsible", "drinks 2x and", "does 10 pushups" 	, .counter = 0u },  /* 1  */
     { "Social responsible", "takes off 1 pieces", "of clothing " 	, .counter = 0u },  /* 2  */
     { "Social responsible", "tells", "\"your mama\" joke " 		, .counter = 0u },  /* 3  */
	 { "Social responsible", "drinks 3x", NULL 						, .counter = 0u },  /* 4  */
	 { "Social responsible", "must say the toast", "all drink " 	, .counter = 0u },  /* 5  */
	 { "Youngest", 			 "social responsible", "takes a shot "  , .counter = 0u },  /* 6  */
	 { "Social responsible", "tells an", 	"embarrassing story"    , .counter = 0u },  /* 7  */
	 { "Social responsible", "makes sure", "everyone has a drink"   , .counter = 0u },  /* 8  */
	 { "Social responsible", "drinks without", "using hands " 		, .counter = 0u },  /* 9  */
	 { "Social responsible", "decides", "who drinks " 				, .counter = 0u },  /* 10 */
};

Private Task_T priv_TextArrayKtLevel1[] =
{
     { "All KT members", "take a shot", "of vodka "                 , .counter = 0u },   /* 1  */
     { "Most wasted", "KT member takes" "1 vodka "                  , .counter = 0u },   /* 2  */
     { "KT president", "takes a shot", "of vodka "                  , .counter = 0u },   /* 3  */
     { "All KT members", "who have been senor", "skip this round "  , .counter = 0u },   /* 4  */
     { "If KT founders", "are present", "they drink vodka "         , .counter = 0u },   /* 5  */
     { "If KT founders", "are present", "they ship this round "     , .counter = 0u },   /* 6  */
     { "All KT members", "drink 3x OR", "drink vodka "              , .counter = 0u },   /* 7  */
     { "1 KT member", "must make toast", "and drink 2x "            , .counter = 0u },   /* 8  */
     { "Most wasted", "KT member" "skips round "                    , .counter = 0u },   /* 9  */
     { "All KT members", "drink water" "just kidding! vodka! "      , .counter = 0u },   /* 10 */
};


Private Task_T priv_TextArrayPaxLevel1[] =
{
     { "All participants", "from Russia", "drink vodka " ,              .counter = 0u }, /* 1   */
     { "All participants", "from Latvia", "drink 2x " ,                 .counter = 0u }, /* 2   */
     { "All participants", "from Poland", "drink vodka " ,              .counter = 0u }, /* 3   */
     { "All participants", "from France", "drink wine OR drink 3x " ,   .counter = 0u }, /* 4   */
     { "All participants", "drink 2x",      NULL ,                      .counter = 0u }, /* 5   */
     { "All male participants",     "drink 2x", NULL ,                  .counter = 0u }, /* 6   */
     { "All female participants",   "drink 2x", NULL ,                  .counter = 0u }, /* 7   */
     { "All participants", "from Spain", "drink 2x " ,                  .counter = 0u }, /* 8   */
     { "Most wasted PAX",  "drinks 2x", NULL ,                          .counter = 0u }, /* 9   */
     { "Tallest PAX",      "drinks 2x", NULL ,                          .counter = 0u }, /* 10  */
     { "Pax must distribute",      "8 drinks", "between them" ,         .counter = 0u }, /* 11  */
};


Private Task_T priv_TextArrayBoardLevel1[] =
{
     { "All boardies", 		    "drink 2x", 			NULL   , 				.counter = 0u },  /* 1   */
     { "Boardies have to", 	    "distribute 5 shots", 	"amongst each other" , 	.counter = 0u },  /* 2   */
     { "President of LBG",      "drinks vodka",         NULL , 	                .counter = 0u },  /* 3   */
     { "Boardies drink 2x",     "Ex-boardies can",    "skip this round" ,       .counter = 0u },  /* 4   */
     { "All female boardies",   "drink 2x",           NULL ,                    .counter = 0u },  /* 5   */
     { "All male boardies",     "drink 2x",             NULL ,                  .counter = 0u },  /* 6   */
     { "Corporate Relations",   "drinks 2x",          NULL ,                    .counter = 0u },  /* 7   */
     { "Human Resources",       "drinks 2x",          NULL ,                    .counter = 0u },  /* 8   */
     { "Treasurer",             "drinks 2x",          NULL ,                    .counter = 0u },  /* 9   */
     { "Public Relations",      "drinks 2x",          NULL ,                    .counter = 0u },  /* 10  */
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


Private Task_T priv_TextArrayAlumniLevel2[] =
{
     { "Alumni who lost", 	"virginity in BEST event", 	"drink 3x " 	, 		.counter = 0u }, /* 1  */
     { "All drunk alumni",	"lost 1 item", 		      	"of clothing " 	, 		.counter = 0u }, /* 2  */
     { "Alumni who were", 	"in GA Estonian", 			"Evening drink 3x ",	.counter = 0u }, /* 3  */
	 { "All alumni who", 	"have been to Moscow", 		"drink 2x", 			.counter = 0u }, /* 4  */
	 { "Alumni who have", 	"done a Death Round", 		"skip this round!", 	.counter = 0u }, /* 5  */
	 { "Alumni who have",   "been senors at",           "Cantus, drink 3x",     .counter = 0u }, /* 6  */
     { "Alumni with most",  "flags & numbers", 		 	"drinks 3x", 			.counter = 0u }, /* 7  */
	 { "Alumni drink 1x",   "for every coreteam",       "they have been in.",   .counter = 0u }, /* 8  */
     { "All alumni that",   "have children", 			"drink 1 vodka shot", 	.counter = 0u }, /* 9  */
	 { "One alumni", 		"must tell ", 				"a dirty joke", 		.counter = 0u }, /* 10 */
};


Private Task_T priv_TextArrayCoreTeamLevel2[] =
{
     { "Core team must",        "do waterfall",          "until next round",             .counter = 0u }, /* 1  */
     { "Core team member",      "who has hooked up",     "drinks vodka",                 .counter = 0u }, /* 2  */
     { "Core team member",      "with biggest boobs",    "drinks 3x " ,                  .counter = 0u }, /* 3  */
     { "Core team member",      "with most flags &",     "numbers drinks 3x " ,          .counter = 0u }, /* 4  */
     { "Every member of CT",    "on the gossip wall",    "drinks 3x " ,                  .counter = 0u }, /* 5  */
     { "Every male Core",       "team member",           "loses shirt " ,                .counter = 0u }, /* 6  */
     { "Every female ",         "coreteam member",       "loses 1 clothing item " ,      .counter = 0u }, /* 7  */
     { "One core team ",        "member must kiss",      "another on the cheek " ,       .counter = 0u }, /* 8  */
     { "The core team makes ",  "a toast with their",    "team motto " ,                 .counter = 0u }, /* 9  */
     { "All CT members",        "wearing black underwear", "drink 2x",                   .counter = 0u }, /* 10 */
};


Private Task_T priv_TextArraySocRespLevel2[] =
{
     { "Social Responsible", "distributes 3", "shots "          ,       .counter = 0u },          /* 1  */
     { "Social Responsible", "must name 10", "sex positions "   ,       .counter = 0u },          /* 2  */
     { "Social Responsible", "must make a toast", "with Borat voice " , .counter = 0u },          /* 3  */
     { "Social Responsible", "must drink", "vodka "             ,       .counter = 0u },          /* 4  */
     { "Soc. Resp. drinks 1x","for each girl", "kissed during event",   .counter = 0u },          /* 5  */
     { "Social Responsible", "makes a toast like", "James Bond"       , .counter = 0u },          /* 6  */
     { "Soc. Resp. drinks 1x", "for every inch", "of his penis"       , .counter = 0u },          /* 7  */
     { "Social Responsible", "can skip this", "round"                 , .counter = 0u },          /* 8  */
     { "Social Responsible", "must drink", "on his knees"             , .counter = 0u },          /* 9  */
     { "Soc. Resp. gets", "kissed on the cheek", "by all the girls"   , .counter = 0u },          /* 10 */
};


Private Task_T priv_TextArrayKtLevel2[] =
{
     { "All KT members", "do 10 pushups",         "before drink " ,          .counter = 0u },  /* 1 */
     { "KT members drink", "1x for each",         "Cantus experienced "  ,   .counter = 0u },  /* 2 */
     { "KT members drink", "1x for every party",  "organized this year " ,   .counter = 0u },  /* 3 */
     { "KT members ",      "divide 8 shots",      "amongst themselves",      .counter = 0u },  /* 4 */
     { "KT member with ",  "the largest penis",   "drinks vodka",            .counter = 0u },  /* 5 */
     { "KT members who ",  "are single skip this","others drink vodka",      .counter = 0u },  /* 6 */
     { "The fastest drinker",   "in KT",          "drinks 3x",               .counter = 0u },  /* 7 */
     { "All female ",      "KT members",          "drink 3x",                .counter = 0u },  /* 8 */
     { "KT members who have", "pulled the root",  "from the ground 3x",      .counter = 0u },  /* 9 */
     { "All KT members",      "drink 1x for every","other KT member here",   .counter = 0u },  /* 10 */
};


Private Task_T priv_TextArrayPaxLevel2[] =
{
     { "All participants", "from Russia", "drink vodka " ,              .counter = 0u }, /* 1  */
     { "All virgin", "participants", "drink x2 " ,                      .counter = 0u }, /* 2  */
     { "Oldest participant", "tells who", "drinks x2 " ,                .counter = 0u }, /* 3  */
     { "Youngest participant", NULL, "drinks x2 " ,                     .counter = 0u }, /* 4  */
     { "All participants", "drink", NULL ,                              .counter = 0u }, /* 5  */
     { "Who can speak German", "screams Heil Hitler", "and drinks " ,   .counter = 0u }, /* 6  */
     { "Who spent more than", "than 30 min on ML ", "drinks 2x" ,       .counter = 0u }, /* 7  */
     { "Oldest participant", "pours a ", "shot round" ,                 .counter = 0u }, /* 8  */
     { "Spanish speaking", "participant tells", "a toast" ,             .counter = 0u }, /* 9  */
     { "Shortest participant", "drinks", "1 shot of vodka " ,           .counter = 0u }, /* 10 */
};

Private Task_T priv_TextArrayBoardLevel2[] =
{
     { "All boardies",          "lose 1 item", "of clothing " ,                    .counter = 0u },  /* 1  */
     { "HR tells the toast",    "and drinks", "x2 " ,                              .counter = 0u },  /* 2  */
     { "President gives ",      "a quick speech", "and drinks x2 " ,               .counter = 0u },  /* 3  */
     { "Virgin Boardie",        "is the only", "who doesn't drink" ,               .counter = 0u },  /* 4  */
     { "Treasurer makes sure",  "who is least drunk", "and drinks with him " ,     .counter = 0u },  /* 5  */
     { "PR makes a promo", "    speech for", "the next alcohol " ,                 .counter = 0u },  /* 6  */
     { "CR has to make",        "someone drink", "x2 " ,                           .counter = 0u },  /* 7  */
     { "All boardies",          "spank ", "each other " ,                          .counter = 0u },  /* 8  */
     { "President makes",       "a long boring ", "1 min speech " ,                .counter = 0u },  /* 9  */
     { "All boardies",          "drink", "x2" ,                                    .counter = 0u },  /* 10 */
     { "Every boardie who",     "has had sex in" , "last 3 days drink 3x ",        .counter = 0u },  /* 11 */
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


Private Task_T priv_TextArrayAlumniLevel3[] =
{
     { "Alumni with", 		"biggest boobs", 	    "drinks 3x " , 		    .counter = 0u },  /* 1  */
     { "Everybody that"	  , "has met Kark",     	"drinks vodka ", 	    .counter = 0u },  /* 2  */
     { "All alumni",        "must share", 		    "a crazy story ",       .counter = 0u },  /* 3  */
	 { "Alumni with",       "the most exes",        "drinks vodka",         .counter = 0u },  /* 4  */
	 { "Alumni that went",  "to Tatra JAM",         "lose their shirt " ,   .counter = 0u },  /* 5  */
	 { "Alumni must teach", "BESTies a good ",      "sex trick ",           .counter = 0u },  /* 6  */
	 { "All alumni that",   "had a threesome",      "drink 3x",             .counter = 0u },  /* 7  */
	 { "Alumni must do",    "the Tunak beginning",  "Yodelling",            .counter = 0u },  /* 8  */
	 { "All married alumni","must drink 4x",        NULL,                   .counter = 0u },  /* 9  */
	 { "All alumni",        "take off",             "shirts ",              .counter = 0u },  /* 10 */
};

Private Task_T priv_TextArrayCoreTeamLevel3[] =
{
     { "Core team members",      "lose their", "pants ",                         .counter = 0u      }, /* 1  */
     { "Core team members",      "drink a ", "buffalo ",                         .counter = 0u      }, /* 2  */
     { "Main Organizer",         "must twerk", "while drinking " ,               .counter = 0u      }, /* 3  */
     { "All CT members must",    "name who they", "want to sleep with",          .counter = 0u      }, /* 4  */
     { "2 CT members must",      "kiss or everybody", "drinks vodka shot",       .counter = 0u      }, /* 5  */
     { "FR responsible must",    "make sales speech", "while removing shirt" ,   .counter = 0u      }, /* 6  */
     { "Pax responsible must",   "organize a", "bodyshot" ,                      .counter = 0u      }, /* 7  */
     { "The core team",          "drinks while on", "all fours",                 .counter = 0u      }, /* 8  */
     { "The core team",          "drinks a shot of", "vodka",                    .counter = 0u      }, /* 9  */
     { "If there is any",        "Soplica, then drink it!",  "Twoje zdrowie!",   .counter = 0u      }, /* 10 */
     { "All CT members",         "who have been in",  "handcuffs drink 3x",      .counter = 0u      }, /* 11 */
};

Private Task_T priv_TextArraySocRespLevel3[] =
{
     { "Social resp. who", "has slept with 1 of", "the players drinks" ,    .counter = 0u },   /* 1  */
     { "Social responsible", "does a buffalo", NULL ,                       .counter = 0u },   /* 2  */
     { "Social responsible", "takes", "pants off " ,                        .counter = 0u },   /* 3  */
     { "Social responsible", "takes a shot", "with straight hand " ,        .counter = 0u },   /* 4  */
     { "Social responsible", "takes a body shot", NULL ,                    .counter = 0u },   /* 5  */
     { "Social responsible", "licks the shot glass", "like she means it" ,  .counter = 0u },   /* 6  */
     { "Social responsible", "takes 2 shots", NULL ,                        .counter = 0u },   /* 7  */
     { "Social responsible", "tells a dirty", "personal fantacy" ,          .counter = 0u },   /* 8  */
     { "Social resp. tells", " a dirty toast", "for the next round" ,       .counter = 0u },   /* 9  */
     { "Social responsible", "drinks and ", "fakes an orgasm" ,             .counter = 0u },   /* 10 */
     { "Social responsible", "tells his/her", "favorite boob size" ,        .counter = 0u },   /* 11 */
};


Private Task_T priv_TextArrayKtLevel3[] =
{
     { "All the KT",        "members take",     "off shirts "               , .counter = 0u },   /* 1  */
     { "Female KT members", "distribute 3",     "vodka shots "              , .counter = 0u },   /* 2  */
     { "KT members do a",   "waterfall until",  "next shot "                , .counter = 0u },   /* 3  */
     { "KT members do a",   "buffalo",          NULL                        , .counter = 0u },   /* 4  */
     { "KT remove 2", "items of clothing", "or take 2 vodka shots"          , .counter = 0u },   /* 5  */
     { "Oldest KT member",  "must organize",   "a bodyshot"                 , .counter = 0u },   /* 6  */
     { "Most horny",  "KT member",             "drinks 3x"                  , .counter = 0u },   /* 7  */
     { "!Error code 0x00A45",  "Just kidding!","Drink 2x everybody"         , .counter = 0u },   /* 8  */
     { "All KT members who",  "performed oral sex","this week, drink 3x"    , .counter = 0u },   /* 9  */
     { "KT members who",  "want to try anal","skip this round"              , .counter = 0u },   /* 10 */
};


Private Task_T priv_TextArrayPaxLevel3[] =
{
     { "All Pax girls sit", "on somebody's lap", "while drinking "  ,           .counter = 0u },   /* 1  */
     { "Most horny",  "PAX",             "drinks 3x"                ,           .counter = 0u },   /* 2  */
     { "All PAX who performed",  "oral sex","this week, drink 3x"   ,           .counter = 0u },   /* 3  */
     { "!Error code 0x0311",  "Just kidding!","Drink 2x everybody"  ,           .counter = 0u },   /* 4  */
     { "All PAX who", "have tried 69", "drink 2x",                              .counter = 0u },   /* 5  */
     { "All PAX lose", "2 items of", "clothing",                                .counter = 0u },   /* 6  */
     { "All PAX guys lose", "pants", NULL,                                      .counter = 0u },   /* 7  */
     { "The most drunk", "participant", "sings a song",                         .counter = 0u },   /* 8  */
     { "All participants", "must", "drink 3x",                                  .counter = 0u },   /* 9  */
     { "Participants must", "feed each other", "their shot",                    .counter = 0u },   /* 10 */
     { "All participants", "must", "drink vodka",                               .counter = 0u },   /* 11 */
     { "All PAX who had", "sex last night", "shout BAZINGA! and drink",         .counter = 0u },   /* 12 */
     { "Pax drink 1x for", "each day since they", "last masturbated",           .counter = 0u },   /* 13 */ /* :D :D :D */
};


Private Task_T priv_TextArrayBoardLevel3[] =
{
     { "All boardies who",   "have tried 69", "drink 2x",                       .counter = 0u },  /* 1  */
     { "The president must", "drink 2x and describe",  "a sex fantasy" ,        .counter = 0u },  /* 2  */
     { "All boardies",       "must drink 3x",  NULL ,                           .counter = 0u },  /* 3  */
     { "The board can ",     "command who will",  "remove their shirt" ,        .counter = 0u },  /* 4  */
     { "All who want to",    "sleep with a boardie ", "drink 2x",               .counter = 0u },  /* 5  */
     { "Boardie with",       "the biggest boobs", "drinks 3x",                  .counter = 0u },  /* 6  */
     { "The boardie with",   "the craziest sex", "life drinks 3x",              .counter = 0u },  /* 7  */
     { "All ex-boardies",    "drink vodka", NULL,                               .counter = 0u },  /* 8  */
     { "The Treasurer",      "loses 1 item of",  "clothing" ,                   .counter = 0u },  /* 9  */
     { "The President",      "loses 1 item of",  "clothing" ,                   .counter = 0u },  /* 10 */
     { "The CR",             "loses 1 item of",  "clothing" ,                   .counter = 0u },  /* 11 */
     { "The PR",             "loses 1 item of",  "clothing" ,                   .counter = 0u },  /* 12 */
     { "The HR",             "loses 1 item of",  "clothing" ,                   .counter = 0u },  /* 13 */
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


Private Task_T priv_TextArrayAlumniLevel4[] =
{
     { "One alumni", 	"gets spanked", NULL 	 				        , .counter = 0u },    /*  1  */
     { "One alumni", 	"gets slapped",	"in the face" 			        , .counter = 0u },    /*  2  */
     { "Alumni select", "who loses", 	"2 items of clothing "          , .counter = 0u },    /*  3  */
     { "Alumni must teach", "and demonstrate a",    "new sex position " , .counter = 0u },    /*  4  */
     { "All alumni must", "do a buffalo",       NULL                    , .counter = 0u },    /*  5  */
     { "All alumni must", "name a BEST event",       "they had sex in"  , .counter = 0u },    /*  6  */
     { "All alumni lose", "1 item of",       "clothing"                 , .counter = 0u },    /*  7  */
     { "All alumni drink", "a round of ",    "vodka"                    , .counter = 0u },    /*  8  */
     { "Alumni must",      "organize ",      "a bodyshot"               , .counter = 0u },    /*  9  */
     { "Oldest alumni",    "distributes 3 ",      "vodka shots"         , .counter = 0u },    /*  10 */
     { "If Kark is present","he drinks 3x ",      NULL                  , .counter = 0u },    /*  11 */
};


Private Task_T priv_TextArrayCoreTeamLevel4[] =
{
     { "Coreteam girls must", "make out or", "drink vodka "             , .counter = 0u },      /*  1  */
     { "Coreteam guys must" , "make out or", "drink vodka "             , .counter = 0u },      /*  2  */
     { "Coreteam members", "chug a beer or", "cider "                   , .counter = 0u },      /*  3  */
     { "Coreteam girls", "squeeze their",    "boobs while drinking "    , .counter = 0u },      /*  4  */
     { "Main Organizer", "loses 2 items of", "clothing "                , .counter = 0u },      /*  5  */
     { "Core team must", "sacrifice 5 items", "of clothing total"       , .counter = 0u },      /*  6  */
     { "One CT member",  "must give another", "member a lapdance"       , .counter = 0u },      /*  7  */
     { "Core team decides", "who takes off", "1 item of clothing",        .counter = 0u },      /*  8  */
     { "Core team guys", "must do a", "sexy dance",                       .counter = 0u },      /*  9  */
     { "Core team girls", "must do a", "sexy dance",                      .counter = 0u },      /*  10 */
     { "Core team member", "with biggest boobs", "takes off shirt",       .counter = 0u },      /*  11 */
     { "CT members must", "make out with the", "glass while drinking",    .counter = 0u },      /*  12 */
};


Private Task_T priv_TextArraySocRespLevel4[] =
{
     { "Social responsible", "shotguns a can", "of beer / cider " ,     .counter = 0u },     /*  1  */
     { "Social responsible", "slaps MO and ", "and calls him Sally " ,  .counter = 0u },     /*  2  */
     { "Social responsible", "takes 3 shots", NULL ,                    .counter = 0u },     /*  3  */
     { "Social responsible", "shows his/her", "orgasm face " ,          .counter = 0u },     /*  4  */
     { "Social resp. takes", "of his/her shoe", "and drinks from it" ,  .counter = 0u },     /*  5  */
     { "Social responsible", "chugs a ", "can of beer/cider " ,         .counter = 0u },     /*  6  */
     { "Social responsible", "kisses player", "next to him/her " ,      .counter = 0u },     /*  7  */
     { "Social responsible", "drinks 2x", "and loses shirt!" ,          .counter = 0u },     /*  8  */
     { "Social responsible", "tells a dirty ", "personal secret! " ,    .counter = 0u },     /*  9  */
     { "Social responsible", "chugs a beer", "and does 20 pushups" ,    .counter = 0u },     /*  10 */
};


Private Task_T priv_TextArrayKtLevel4[] =
{
     { "All KT members", "lose 2", "items of clothing " ,               .counter = 0u },              /*  1  */
     { "KT drinks", "vodka", "for 2 rounds "            ,               .counter = 0u },              /*  2  */
     { "Most wasted", "KT member", "drinks water "      ,               .counter = 0u },              /*  3  */
     { "KT guys", "get a lapdance", NULL,                               .counter = 0u },              /*  4  */
     { "KT guys", "must do a lapdance", NULL,                           .counter = 0u },              /*  5  */
     { "KT members ", "must organize a", "bodyshot",                    .counter = 0u },              /*  6  */
     { "The KT member who", "likes paragraph 5", "the most BUFFALO",    .counter = 0u },              /*  7  */
     { "All KT members", "drink under table", "for 5 rounds",           .counter = 0u },              /*  8  */
     { "All KT members", "do a buffalo", NULL,                          .counter = 0u },              /*  9  */
     { "KT members who", "havent organized an", "event, drink vodka",   .counter = 0u },              /*  10 */
     { "All KT members", "must sing", "until the next round " ,         .counter = 0u },              /*  11 */
     { "KT members...",  "VODKA", "ROUND! " ,                           .counter = 0u },              /*  12 */
     { "KT members...",  "VODKA", "ROUND! " ,                           .counter = 0u },              /*  13 */
};


Private Task_T priv_TextArrayPaxLevel4[] =
{
     { "Greek participant", "tells who is", "the sexiest pax" ,         .counter = 0u },   /*  1  */
     { "All participants", "drink", "x3" ,                              .counter = 0u },   /*  2  */
     { "All pax who have", "had sex during event", "drink x2" ,         .counter = 0u },   /*  3  */
     { "Most sexually", "experienced pax", "drink x2" ,                 .counter = 0u },   /*  4  */
     { "All participants", "lose 2 pieces", "of clothing" ,             .counter = 0u },   /*  5  */
     { "French pax", "does", "limousin dance!" ,                        .counter = 0u },   /*  6  */
     { "Drunkest pax takes", "his sock off and", " drinks through it" , .counter = 0u },   /*  7  */
     { "Participant with", "best English", "drink x2" ,                 .counter = 0u },   /*  8  */
     { "All participants", "kiss the person ", "next to them" ,         .counter = 0u },   /*  9  */
     { "Pax who haven't",  "had sex during event", "drink x2" ,         .counter = 0u },   /*  10 */
     { "Pax drink like",   "somebody is sitting", "on their face "    , .counter = 0u },   /*  11 */
     { "All participant", "couples must kiss", "before drinking",       .counter = 0u },   /*  12 */
};

Private Task_T priv_TextArrayBoardLevel4[] =
{
     { "Boardies must", "re-enact a", "sex position" ,                  .counter = 0u },   /*  1   */
     { "Drunkest boardie", "drinks while", "twerking ",                 .counter = 0u },   /*  2   */
     { "The president", "must do a", "striptease ",                     .counter = 0u },   /*  3   */
     { "All boardie girls", "must make out", "or drink vodka",          .counter = 0u },   /*  4   */
     { "All Boardies", "lose their", "shirts" ,                         .counter = 0u },   /*  5   */
     { "The Boardie who", "lost their virginity", "last drinks 3x" ,    .counter = 0u },   /*  6   */
     { "The president", "drinks vodka", NULL,                           .counter = 0u },   /*  7   */
     { "The Treasurer", "drinks vodka", NULL,                           .counter = 0u },   /*  8   */
     { "The HR", "drinks vodka", NULL,                                  .counter = 0u },   /*  9   */
     { "The PR", "drinks vodka", NULL,                                  .counter = 0u },   /*  10  */
     { "The CR", "drinks vodka", NULL,                                  .counter = 0u },   /*  11  */
     { "One boardie", "must do a", "striptease ",                       .counter = 0u },   /*  12  */
     { "The president", "gets a", "lapdance ",                          .counter = 0u },   /*  13  */
};

/*****************************************************************/

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


Public Boolean alumniSpecialTask(U8 sec)
{
    return SpecialTaskWithRandomText(sec, TASK_FOR_ALUMNI);
}


Public Boolean boardSpecialTask(U8 sec)
{
    return SpecialTaskWithRandomText(sec, TASK_FOR_BOARD);
}


Public Boolean socRespSpecialTask(U8 sec)
{
    return SpecialTaskWithRandomText(sec, TASK_FOR_SOCRESP);
}


Public Boolean KtSpecialTask(U8 sec)
{
    return SpecialTaskWithRandomText(sec, TASK_FOR_KT);
}


Public Boolean PaxSpecialTask(U8 sec)
{
    return SpecialTaskWithRandomText(sec, TASK_FOR_PAX);
}


Public Boolean coreTeamSpecialTask(U8 sec)
{
    return SpecialTaskWithRandomText(sec, TASK_FOR_CORETEAM);
}



Private Boolean DrinkTwiceTask(U8 sec, SpecialTaskType type)
{
    Boolean res = FALSE;

    switch(sec)
    {
    case(1u):
       display_clear();
       if (type == TASK_FOR_GIRLS)
       {
           strcpy(priv_str_buf, "Girls");
       }
       else if(type == TASK_FOR_GUYS)
       {
           strcpy(priv_str_buf, "Guys");
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

Private const Task_T * priv_task_str_ptr;

/* The sec parameter specifies the current second from the beginning of the task.
 * This function is called cyclically after every second. */
Private Boolean SpecialTaskWithRandomText(U8 sec, SpecialTaskType type)
{
    //This is the simplest special task, currently no bitmaps, but we just display text on screen.
    //Text is three lines and randomly selected from tasks from previous PH controller :)

    Boolean res = FALSE;
    int taskLevel = pot_getSelectedRange();
    Task_T * girlsSelectedArray;
    Task_T * guysSelectedArray;
    Task_T * alumniSelectedArray;
    Task_T * boardSelectedArray;
    Task_T * ktSelectedArray;
    Task_T * paxSelectedArray;
    Task_T * SocRespSelectedArray;
    Task_T * coreTeamSelectedArray;

    U16 number_of_items_girls = 0u;
    U16 number_of_items_guys = 0u;
    U16 number_of_items_alumni = 0u;
    U16 number_of_items_board = 0u;
    U16 number_of_items_kt = 0u;
    U16 number_of_items_pax = 0u;
    U16 number_of_items_socresp = 0u;
    U16 number_of_items_coreteam = 0u;

    switch(taskLevel)
    {
        case 0:
            girlsSelectedArray = priv_TextArrayGirlsLevel1;
            number_of_items_girls = NUMBER_OF_ITEMS(priv_TextArrayGirlsLevel1);

            guysSelectedArray = priv_TextArrayGuysLevel1;
            number_of_items_guys = NUMBER_OF_ITEMS(priv_TextArrayGuysLevel1);

            alumniSelectedArray = priv_TextArrayAlumniLevel1;
            number_of_items_alumni = NUMBER_OF_ITEMS(priv_TextArrayAlumniLevel1);

            boardSelectedArray = priv_TextArrayBoardLevel1;
            number_of_items_board = NUMBER_OF_ITEMS(priv_TextArrayBoardLevel1);

            ktSelectedArray = priv_TextArrayKtLevel1;
            number_of_items_kt = NUMBER_OF_ITEMS(priv_TextArrayKtLevel1);

            paxSelectedArray = priv_TextArrayPaxLevel1;
            number_of_items_pax = NUMBER_OF_ITEMS(priv_TextArrayPaxLevel1);

            SocRespSelectedArray = priv_TextArraySocRespLevel1;
            number_of_items_socresp = NUMBER_OF_ITEMS(priv_TextArraySocRespLevel1);

            coreTeamSelectedArray = priv_TextArrayCoreTeamLevel1;
            number_of_items_coreteam = NUMBER_OF_ITEMS(priv_TextArrayCoreTeamLevel1);
            break;
        case 1:
            girlsSelectedArray = priv_TextArrayGirlsLevel2;
            number_of_items_girls = NUMBER_OF_ITEMS(priv_TextArrayGirlsLevel2);

            guysSelectedArray = priv_TextArrayGuysLevel2;
            number_of_items_guys = NUMBER_OF_ITEMS(priv_TextArrayGuysLevel2);

            alumniSelectedArray = priv_TextArrayAlumniLevel2;
            number_of_items_alumni = NUMBER_OF_ITEMS(priv_TextArrayAlumniLevel2);

            boardSelectedArray = priv_TextArrayBoardLevel2;
            number_of_items_board = NUMBER_OF_ITEMS(priv_TextArrayBoardLevel2);

            ktSelectedArray = priv_TextArrayKtLevel2;
            number_of_items_kt = NUMBER_OF_ITEMS(priv_TextArrayKtLevel2);

            paxSelectedArray = priv_TextArrayPaxLevel2;
            number_of_items_pax = NUMBER_OF_ITEMS(priv_TextArrayPaxLevel2);

            SocRespSelectedArray = priv_TextArraySocRespLevel2;
            number_of_items_socresp = NUMBER_OF_ITEMS(priv_TextArraySocRespLevel2);

            coreTeamSelectedArray = priv_TextArrayCoreTeamLevel2;
            number_of_items_coreteam = NUMBER_OF_ITEMS(priv_TextArrayCoreTeamLevel2);
            break;
        case 2:
            girlsSelectedArray = priv_TextArrayGirlsLevel3;
            number_of_items_girls = NUMBER_OF_ITEMS(priv_TextArrayGirlsLevel3);

            guysSelectedArray = priv_TextArrayGuysLevel3;
            number_of_items_guys = NUMBER_OF_ITEMS(priv_TextArrayGuysLevel3);

            alumniSelectedArray = priv_TextArrayAlumniLevel3;
            number_of_items_alumni = NUMBER_OF_ITEMS(priv_TextArrayAlumniLevel3);

            boardSelectedArray = priv_TextArrayBoardLevel3;
            number_of_items_board = NUMBER_OF_ITEMS(priv_TextArrayBoardLevel3);

            ktSelectedArray = priv_TextArrayKtLevel3;
            number_of_items_kt = NUMBER_OF_ITEMS(priv_TextArrayKtLevel3);

            paxSelectedArray = priv_TextArrayPaxLevel3;
            number_of_items_pax = NUMBER_OF_ITEMS(priv_TextArrayPaxLevel3);

            SocRespSelectedArray = priv_TextArraySocRespLevel3;
            number_of_items_socresp = NUMBER_OF_ITEMS(priv_TextArraySocRespLevel3);

            coreTeamSelectedArray = priv_TextArrayCoreTeamLevel3;
            number_of_items_coreteam = NUMBER_OF_ITEMS(priv_TextArrayCoreTeamLevel3);
            break;
        case 3:
            girlsSelectedArray = priv_TextArrayGirlsLevel4;
            number_of_items_girls = NUMBER_OF_ITEMS(priv_TextArrayGirlsLevel4);

            guysSelectedArray = priv_TextArrayGuysLevel4;
            number_of_items_guys = NUMBER_OF_ITEMS(priv_TextArrayGuysLevel4);

            alumniSelectedArray = priv_TextArrayAlumniLevel4;
            number_of_items_alumni = NUMBER_OF_ITEMS(priv_TextArrayAlumniLevel4);

            boardSelectedArray = priv_TextArrayBoardLevel4;
            number_of_items_board = NUMBER_OF_ITEMS(priv_TextArrayBoardLevel4);

            ktSelectedArray = priv_TextArrayKtLevel4;
            number_of_items_kt = NUMBER_OF_ITEMS(priv_TextArrayKtLevel4);

            paxSelectedArray = priv_TextArrayPaxLevel4;
            number_of_items_pax = NUMBER_OF_ITEMS(priv_TextArrayPaxLevel4);

            SocRespSelectedArray = priv_TextArraySocRespLevel4;
            number_of_items_socresp = NUMBER_OF_ITEMS(priv_TextArraySocRespLevel4);

            coreTeamSelectedArray = priv_TextArrayCoreTeamLevel4;
            number_of_items_coreteam = NUMBER_OF_ITEMS(priv_TextArrayCoreTeamLevel4);
            break;
        default:
            /* Should not happen. */
            girlsSelectedArray = priv_TextArrayGirlsLevel1;
            number_of_items_girls = NUMBER_OF_ITEMS(priv_TextArrayGirlsLevel1);

            guysSelectedArray = priv_TextArrayGuysLevel1;
            number_of_items_guys = NUMBER_OF_ITEMS(priv_TextArrayGuysLevel1);

            alumniSelectedArray = priv_TextArrayAlumniLevel1;
            number_of_items_alumni = NUMBER_OF_ITEMS(priv_TextArrayAlumniLevel1);

            boardSelectedArray = priv_TextArrayBoardLevel1;
            number_of_items_board = NUMBER_OF_ITEMS(priv_TextArrayBoardLevel1);

            ktSelectedArray = priv_TextArrayKtLevel1;
            number_of_items_kt = NUMBER_OF_ITEMS(priv_TextArrayKtLevel1);

            paxSelectedArray = priv_TextArrayPaxLevel1;
            number_of_items_pax = NUMBER_OF_ITEMS(priv_TextArrayPaxLevel1);

            SocRespSelectedArray = priv_TextArraySocRespLevel1;
            number_of_items_socresp = NUMBER_OF_ITEMS(priv_TextArraySocRespLevel1);

            coreTeamSelectedArray = priv_TextArrayCoreTeamLevel1;
            number_of_items_coreteam = NUMBER_OF_ITEMS(priv_TextArrayCoreTeamLevel1);
            break;
    }

    switch(sec)
    {
        case(1u):
            {
               switch(type)
               {
                   case TASK_FOR_GIRLS:
                       {
                           priv_task_str_ptr = getRandomTaskFromArray(girlsSelectedArray, number_of_items_girls);
                       }
                       break;
                   case TASK_FOR_GUYS:
                       {
                           priv_task_str_ptr = getRandomTaskFromArray(guysSelectedArray, number_of_items_guys);
                       }
                       break;
                   case TASK_FOR_ALUMNI:
                       {
                           priv_task_str_ptr = getRandomTaskFromArray(alumniSelectedArray, number_of_items_alumni);
                       }
                       break;
                   case TASK_FOR_BOARD:
                       {
                           priv_task_str_ptr = getRandomTaskFromArray(boardSelectedArray, number_of_items_board);
                       }
                       break;
                   case TASK_FOR_KT:
                       {
                           priv_task_str_ptr = getRandomTaskFromArray(ktSelectedArray, number_of_items_kt);
                       }
                       break;
                   case TASK_FOR_SOCRESP:
                       {
                           priv_task_str_ptr = getRandomTaskFromArray(SocRespSelectedArray, number_of_items_socresp);
                       }
                       break;
                   case TASK_FOR_PAX:
                       {
                           priv_task_str_ptr = getRandomTaskFromArray(paxSelectedArray, number_of_items_pax);
                       }
                       break;
                   case TASK_FOR_CORETEAM:
                       {
                           priv_task_str_ptr = getRandomTaskFromArray(coreTeamSelectedArray, number_of_items_coreteam);
                       }
                       break;
                   default:
                       /* Should not happen. */
                       break;
               }
            }
            break;
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

