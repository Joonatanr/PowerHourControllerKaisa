/*
 * font.h
 *
 *  Created on: Aug 25, 2017
 *      Author: Joonatan
 */

#ifndef HW_FONT_H_
#define HW_FONT_H_


#include "typedefs.h"
#include "bitmaps.h"

typedef enum
{
    FONT_ARIAL_TINY,
    FONT_SMALL_FONT,
    FONT_MEDIUM_FONT,
    FONT_LARGE_FONT,
    FONT_NUMBERS_LARGE,
    FONT_NUMBERS_HUGE,
    NUMBER_OF_FONTS
} FontType;

extern const Bitmap large_font_bitmap;

extern void font_getFontChar(char asc, Bitmap * dest);
extern void font_setFont(FontType font);
extern U16  font_getFontHeight(FontType font);
extern U8   font_getCharWidth(char asc, FontType font);


#endif /* HW_FONT_H_ */
