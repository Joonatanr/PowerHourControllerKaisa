/*
 * display_drv.c
 *
 *  Created on: Aug 13, 2017
 *      Author: Joonatan
 */

#include "display_drv.h"
#include <driverlib.h>
#include "spi_drv.h"
#include "bitmaps.h"
#include "register.h"
#include "misc.h"

//#define ROTATE_180_DEGREES


/*****************************************************************************************************
 *
 * Type definitions.
 *
 *****************************************************************************************************/
typedef struct
{
    FillPatternType id;
    U8 number_of_segments;
    const U8 * pattern;
} FillPattern;



/*****************************************************************************************************
 *
 * Private function prototypes
 *
 *****************************************************************************************************/

Private void drawImage(Point * p, Size * s, const Bitmap * bmp, Boolean isInverted);
Private void drawPattern(Point * p, Size * s, const FillPattern * patternType);
Private void disp_command(U8 cmd, Boolean reg_select);
Private void set_page_address(U8 page);
Private void set_column(U8 column);
Private void write_data(U8 data);
Private void display_reset(void);
Private void display_empty(void);




/*****************************************************************************************************
 *
 * Private variable declarations.
 *
 *****************************************************************************************************/

Private U8 priv_display_buffer[DISPLAY_NUMBER_OF_COLUMNS][DISPLAY_NUMBER_OF_PAGES];
Private U8 priv_split_buffer[DISPLAY_NUMBER_OF_COLUMNS]; //Used for splitting image data between pages.

Private U16 priv_page_validity_bits[DISPLAY_NUMBER_OF_PAGES]; //Each page is divided into 16 virtual segments.
Private Boolean priv_is_locked = FALSE;

//Display patterns.


Private const U8 priv_white_pattern = 0x00u;
Private const U8 priv_black_pattern = 0xffu;
Private const U8 priv_gray_pattern[2u] = {0xAAu, 0x55u };

Private const FillPattern priv_fill_patterns[NUMBER_OF_PATTERNS] =
{
     {PATTERN_WHITE, 1u, &priv_white_pattern    },
     {PATTERN_BLACK, 1u, &priv_black_pattern    },
     {PATTERN_GRAY,  2u, priv_gray_pattern      }
};




volatile Boolean isDisplayReady = FALSE;

/* Set up A0 pin for display. */
Public void display_init(void)
{
    //Set the chip select pin as high initially.
    memset(priv_display_buffer, 0x00u, sizeof(priv_display_buffer));
    memset(priv_page_validity_bits, 0x0000u, sizeof(priv_page_validity_bits));

    display_reset();
    set_cs_pin(1u);
}

//Should be called after display_init.
Public void display_start(void)
{
    //Low CS, means that it is active.
    set_cs_pin(0u);

    //Display start line set.
    disp_command(0x40u, FALSE);

#ifdef ROTATE_180_DEGREES
    //Set ADC in normal
    disp_command(0xA0u, FALSE);
    //Common output mode select
    disp_command(0xC8u, FALSE);
#else
    //Set ADC in reverse
    disp_command(0xA1u, FALSE);
    //Common output mode select
    disp_command(0xC0u, FALSE);
#endif

    //display normal
    disp_command(0xA6u, FALSE);

    //LCD bias set
    disp_command(0xA3u, FALSE);

    //Power control set
    disp_command(0x2Fu, FALSE);

    //Booster ratio set.
    disp_command(0xF8u, FALSE);
    disp_command(0x00u, FALSE);

    //Contrast set.
    disp_command(0x27u, FALSE);
    disp_command(0x81u, FALSE);
    disp_command(0x10u, FALSE);

    //Static indicator set.
    disp_command(0xACu, FALSE);

    //Turn on display.
    disp_command(0x00u, FALSE);
    disp_command(0xAFu, FALSE);

    delay_msec(200);
    //Turn on all points.
    disp_command((0xA4u | 0x01u), FALSE);
    display_empty();
    delay_msec(2000);
    //Turn off all points.
    disp_command(0xA4u, FALSE);

    //TODO : Probably should remove this delay from here.
    delay_msec(200);

    display_clear();

    isDisplayReady = TRUE;
}


Public void display_cyclic_50msec(void)
{
    //Redraw display.
    U8 page, column, seg, prev_column;
    if (isDisplayReady && !priv_is_locked)
    {
        for (page = 0u; page < DISPLAY_NUMBER_OF_PAGES; page++)
        {
            if (priv_page_validity_bits[page] > 0u)
            {
                //Redraw page.
                set_page_address(page);
                prev_column = 0xffu;

                for (seg = 0u; seg < 16u; seg++) //Iterate over each segment.
                {
                    if ((priv_page_validity_bits[page] >> seg) & (U16)0x01u)
                    {
                        column = GET_FIRST_COLUMN(seg);
                        if (column != prev_column)
                        {
                            set_column(column);
                        }
                        for (; column < GET_FIRST_COLUMN(seg + 1u); column++)
                        {
                            write_data(priv_display_buffer[column][page]);
                            //write_data(~priv_display_buffer[column][page]);
                        }
                        prev_column = column;
                    }
                }

                priv_page_validity_bits[page] = 0x0000u;
            }
        }
    }
}


/* Return true, if successful, false, otherwise. */
Public Boolean display_drawTextBox(const Rectangle * box, const char * str, FontType font)
{
    U8 box_center_x;
    U8 box_center_y;
    U8 str_height = font_getFontHeight(font);
    /* First we clear the previous value. */
    display_fillRectangle(box->location.x, box->location.y, box->size.height, box->size.width, PATTERN_WHITE);

    /* Check if string will fit in the box. */
    if ((str_height > box->size.height) || (display_getStringWidth(str, font) > box->size.width))
    {
        /* This should not normally happen, so we use the gray pattern to indicate that the box is too small. */
        display_fillRectangle(box->location.x, box->location.y, box->size.height, box->size.width, PATTERN_GRAY);
        return FALSE;
    }
    else
    {
        /* We draw the string to the center. */
        box_center_x = box->location.x + (box->size.width / 2);
        box_center_y = box->location.y + (box->size.height / 2);


        //display_drawStringCenter(str, box_center_x, box_center_y - (str_height / 2) , font, FALSE);
        /* Currently we draw a string to the center of the box. */
        display_drawStringCenter(str, box_center_x, GET_Y_FROM_CENTER(box_center_y, str_height), font, FALSE);
        //display_fillRectangle(box->location.x, box->location.y, box->size.height, box->size.width, PATTERN_GRAY);
        return TRUE;
    }
}


//Draws string around the centerPoint.
Public void display_drawStringCenter(const char * str, U8 centerPoint, U8 yloc, FontType font, Boolean isInverted)
{
    U16 str_width; //String width in bits.
    U8 begin_point;
    if(str != NULL)
    {
        str_width = display_getStringWidth(str, font);
        str_width = str_width >> 1u; //Divide with 2.

        if (str_width > centerPoint)
        {
            //String is too large to fit to display anyway.
            begin_point = 0u;
        }
        else
        {
            begin_point = centerPoint - str_width;
        }

        display_drawString(str, begin_point, yloc, font, isInverted);
    }
}


Public void display_drawString(const char * str, U8 xloc, U8 yloc, FontType font, Boolean isInverted)
{
    const char * ps = str;
    U8 x = xloc;
    U8 y = yloc;
    Size char_size;

    if(str != NULL)
    {
        font_setFont(font);
        char_size.height = font_getFontHeight(font);

        while (*ps)
        {
            if (*ps == '\n')
            {
                x = xloc;
                y += char_size.height;
            }
            else
            {
                display_drawChar(*ps, x, y, &char_size, isInverted);
                x += char_size.width;
                //Draw a blank line in between the characters.
                if(isInverted)
                {
                    display_fillRectangle(x, y, char_size.height, 1u, PATTERN_BLACK);
                }
                else
                {
                    display_fillRectangle(x, y, char_size.height, 1u, PATTERN_WHITE);
                }
                x += 1u;
            }
            ps++;
        }
    }
}


Public void display_drawChar(char c, U8 xloc, U8 yloc, Size * destSize, Boolean isInverted)
{
    Bitmap myBitMap;

    font_getFontChar(c, &myBitMap);
    display_drawBitmap(&myBitMap, xloc, yloc, isInverted);

    destSize->height = myBitMap.height;
    destSize->width = myBitMap.width;
}


Public void display_drawBitmapCenter(const Bitmap * bmp, U16 centerPoint, U16 y, Boolean isInverted)
{
    Size mySize;
    Point myPoint;
    U8 halfWidth;

    mySize.height = bmp->height;
    mySize.width = bmp->width;
    myPoint.y = y;

    if (bmp != NULL)
    {
        halfWidth = bmp->width >> 1u;
        if (halfWidth <= centerPoint)
        {
            myPoint.x = centerPoint - halfWidth;
        }
        else
        {
            myPoint.x = 0u;
        }

        drawImage(&myPoint, &mySize, bmp, isInverted);
    }
}


Public void display_drawBitmap(const Bitmap * bmp, U16 x, U16 y, Boolean isInverted)
{
    Size mySize;
    Point myPoint;

    mySize.height = bmp->height;
    mySize.width = bmp->width;
    myPoint.x = x;
    myPoint.y = y;

    if (bmp != NULL)
    {
        drawImage(&myPoint, &mySize, bmp, isInverted);
    }
}


//Draw rectangle into frame buffer and invalidate pages that contain the rectangle.
Public void display_fillRectangle(U16 x, U16 y, U16 height, U16 width, FillPatternType patternType)
{
    Size mySize;
    Point myPoint;

    mySize.height = height;
    mySize.width = width;

    myPoint.x = x;
    myPoint.y = y;

    if (patternType < NUMBER_OF_PATTERNS)
    {
        drawPattern(&myPoint, &mySize, &priv_fill_patterns[patternType]);
    }
}


Public void display_drawRectangle(U16 x, U16 y, U16 height, U16 width, U8 borderWidth)
{
    /* 1. Sanity check */
    if ((x > DISPLAY_NUMBER_OF_COLUMNS) || (y > DISPLAY_NUMBER_OF_ROWS))
    {
        return;
    }

    if ((height == 0) || (width == 0u) || (borderWidth == 0u))
    {
        return;
    }

    /* 2. Check if we have a border width of 1. */

    if (borderWidth == 1u)
    {
        Point xx = {.x = x,                .y = y                };
        Point xy = {.x = x,                .y = y + height - 1u  };
        Point yx = {.x = x +  width - 1u , .y = y                };
        Point yy = {.x = x +  width - 1u , .y = y + height - 1u  };

        display_drawLine(xx, xy, TRUE);
        display_drawLine(xx, yx, TRUE);
        display_drawLine(yx, yy, TRUE);
        display_drawLine(xy, yy, TRUE);
    }
    else
    {
        /* We solve this by drawing 4 filled rectangles (the borders) */

        /* TOP side */
        display_fillRectangle(x,
                              y,
                              borderWidth,
                              width,
                              PATTERN_BLACK);
        /* LEFT side*/
        display_fillRectangle(x,
                              y,
                              height,
                              borderWidth,
                              PATTERN_BLACK);
        /* RIGHT side */
        display_fillRectangle(x + (width - borderWidth),
                              y,
                              height,
                              borderWidth,
                              PATTERN_BLACK);
        /* BOTTOM side */
        display_fillRectangle(x,
                              y + (height - borderWidth),
                              borderWidth,
                              width,
                              PATTERN_BLACK);
    }
}


Public void display_clear(void)
{
    U8 x, y;
    for (x = 0u; x < DISPLAY_NUMBER_OF_PAGES; x++)
    {
        priv_page_validity_bits[x] = (U16)0xffffu; //Invalidate whole display.
        for(y = 0u; y < DISPLAY_NUMBER_OF_COLUMNS; y++)
        {
            priv_display_buffer[y][x] = 0x00u;
        }
    }
}


/* Sets or clears a single pixel. */
Public void display_setPixel(U8 x, U8 y, Boolean val)
{
    if ((x < DISPLAY_NUMBER_OF_COLUMNS) && (y < DISPLAY_NUMBER_OF_ROWS))
    {
        U8 page = y >> 3u;
        U8 bit = y % 8u;
        U8 segment = GET_SEGMENT(x);

        if (val)
        {
            SETBIT(priv_display_buffer[x][page], (U8)(1u << bit));
        }
        else
        {
            CLRBIT(priv_display_buffer[x][page], (U8)(1u << bit));
        }

        //Invalidate segment.
        SETBIT(priv_page_validity_bits[page], (U16)(1u << segment));
    }
}


/* Line width is fixed at 1 pixel. If we want thicker lines, should implement rotating rectangles... */
Public void display_drawLine(Point begin, Point end, Boolean isBlack)
{

    if (begin.x >= DISPLAY_NUMBER_OF_COLUMNS ||
        begin.y >= DISPLAY_NUMBER_OF_ROWS    ||
        end.x >= DISPLAY_NUMBER_OF_COLUMNS   ||
        end.y >= DISPLAY_NUMBER_OF_ROWS)
    {
        return;
    }

    /* Eventually should be able to draw any line between 2 points.
     * Lets begin with just simple straight line implementation... */
    if (begin.x == end.x)
    {
        /* Straight vertical line. */
        U8 y;
        for (y = MIN(begin.y, end.y); y <= MAX(begin.y, end.y); y++)
        {
            display_setPixel(begin.x, y, isBlack);
        }
    }
    else if(begin.y == end.y)
    {
        /* Straight horizontal line */
        U8 x;
        for (x = MIN(begin.x, end.x); x <= MAX(begin.x, end.x); x++)
        {
            display_setPixel(x, begin.y, isBlack);
        }
    }
    else
    {
        /* TODO : Implement Bresenham's algorithm here. */
    }
}


Public U16 display_getStringWidth(const char * str, FontType font)
{
    const char *ps = str;
    U8 width = 0u;

    while(*ps)
    {
        if (*ps == '\n')
        {
            break;
        }

        width += font_getCharWidth(*ps, font);
        width++; //Account for one bit in between chars.
        ps++;
    }

    return width;
}


Public void display_lock(Boolean isLocked)
{
    priv_is_locked = isLocked;
}

#if 0
/* This is still under test. */
Public void display_getDisplayedImage(Rectangle * rect, U8 * dest)
{
    U8 x;
    U8 page, last_page;
    U8 * px = dest;
    U8 y_offset = rect->location.y % 8u;

    page = rect->location.y >> 3u;
    last_page = page + (rect->size.height >> 3u);

    for (x = rect->location.x; x < (rect->location.x + rect->size.width); x++)
    {
        for (; page <= last_page; page++)
        {
            *px = (U8)priv_display_buffer[x][page] << y_offset;
            *px |= (U8)priv_display_buffer[x][page + 1u] << y_offset;
            px++;
        }
    }
}
#endif

/*****************************************************************************************************
 *
 * Private Functions
 *
 *****************************************************************************************************/

Private void drawPattern(Point * p, Size * s, const FillPattern * pattern_ptr)
{
    U8 curr_page, bottom_page, top_page;
    U8 column, right_column, bottom_row;
    U8 mask, y_offset;

    U8 left_segment, right_segment, ix;
    U8 pattern_segment = 0u;

    const U8 * data;
    U8 value;

    if (pattern_ptr->id >= NUMBER_OF_PATTERNS)
    {
        return;
    }

    data = pattern_ptr->pattern;

    if ((p->x < DISPLAY_NUMBER_OF_COLUMNS) && (p->y < DISPLAY_NUMBER_OF_ROWS))
    {
        bottom_row = p->y + s->height - 1;
        right_column = p->x + s->width - 1;

        bottom_page = bottom_row >> 3u; //Divide with 8
        top_page = p->y >> 3u; //Divide with 8

        if (bottom_page >= DISPLAY_NUMBER_OF_PAGES )
        {
            bottom_page = DISPLAY_NUMBER_OF_PAGES - 1;
        }

        if (right_column >= DISPLAY_NUMBER_OF_COLUMNS)
        {
            right_column = DISPLAY_NUMBER_OF_COLUMNS - 1;
        }

        left_segment = GET_SEGMENT(p->x);
        right_segment = GET_SEGMENT(right_column);

        y_offset = p->y % 8u;

        for (curr_page = top_page; curr_page <= bottom_page; curr_page++)
        {
            mask = 0xffu;
            if (curr_page == top_page)
            {
                mask = mask << y_offset;
            }

            if (curr_page == bottom_page)
            {
                mask &= (0xffu >> (7u - (bottom_row % 8u)));
            }

            for (column = p->x; column <= right_column; column++)
            {
                //We are drawing a pattern.
                pattern_segment = column % pattern_ptr->number_of_segments;
                data = pattern_ptr->pattern + pattern_segment;
                value = *data;
                //priv_display_buffer[column][curr_page] |= (mask & value);
                priv_display_buffer[column][curr_page] &= (~mask);
                priv_display_buffer[column][curr_page] |= (mask & value);
            }

            //Invalidate page segments.
            for (ix = left_segment; ix <= right_segment; ix++)
            {
                priv_page_validity_bits[curr_page] |= (U16)((U16)1u << ix);
            }
        }
    }
}


//Draw an image in a rectangular area.
Private void drawImage(Point * p, Size * s, const Bitmap * bmp, Boolean isInverted)
{
    U8 curr_page, bottom_page, top_page;
    U8 column, right_column, bottom_row;
    U8 mask, y_offset;

    U8 left_segment, right_segment, ix;

    const U8 * data;
    U8 value;

    //We start drawing a bitmap.
    data = bmp->bmp_data;

    if ((p->x < DISPLAY_NUMBER_OF_COLUMNS) && (p->y < DISPLAY_NUMBER_OF_ROWS))
    {
        bottom_row = p->y + s->height - 1;
        right_column = p->x + s->width - 1;

        bottom_page = bottom_row >> 3u; //Divide with 8
        top_page = p->y >> 3u; //Divide with 8

        if (bottom_page >= DISPLAY_NUMBER_OF_PAGES )
        {
            bottom_page = DISPLAY_NUMBER_OF_PAGES - 1;
        }

        if (right_column >= DISPLAY_NUMBER_OF_COLUMNS)
        {
            right_column = DISPLAY_NUMBER_OF_COLUMNS - 1;
        }

        left_segment = GET_SEGMENT(p->x);
        right_segment = GET_SEGMENT(right_column);

        y_offset = p->y % 8u;

        for (curr_page = top_page; curr_page <= bottom_page; curr_page++)
        {
            mask = 0xffu;
            if (curr_page == top_page)
            {
                mask = mask << y_offset;
            }

            if (curr_page == bottom_page)
            {
                //We do not need to mask, if the bitmap is out of bounds.
                if (bottom_row < DISPLAY_NUMBER_OF_ROWS)
                {
                    mask &= (0xffu >> (7u - (bottom_row % 8u)));
                }
            }

            for (column = p->x; column <= right_column; column++)
            {
                //We are drawing a bitmap.
                //We need to extract the value to be written to the display.
                value = (isInverted ? (~(*data)) : (*data)) << y_offset;

                if(curr_page == top_page)
                {
                   priv_split_buffer[column] = 0x00u; //TODO : Must get rid of this split buffer somehow.
                }
                else
                {
                    value |= priv_split_buffer[column]; //Should contain the top bits for the bitmap image. If it is top page, then these bits are ignored anyway.
                }

                if (y_offset > 0)
                {
                    priv_split_buffer[column] = (isInverted ? (U8)(~((U8)*data)) : (*data)) >> (8u - y_offset);
                }

                priv_display_buffer[column][curr_page] &= (~mask);
                priv_display_buffer[column][curr_page] |= (mask & value);
                data++;
            }

            //Invalidate page segments.
            for (ix = left_segment; ix <= right_segment; ix++)
            {
                priv_page_validity_bits[curr_page] |= (U16)((U16)1u << ix);
            }
        }
    }
}


Private inline void disp_command(U8 cmd, Boolean reg_select)
{
    spi_transmit_byte(cmd, reg_select);
}


Private inline void set_page_address(U8 page)
{
    disp_command(0xB0u | (page & 0x0fu), FALSE);
}


Private inline void write_data(U8 data)
{
    disp_command(data, TRUE);
}


Private inline void set_column(U8 column)
{
#ifdef ROTATE_180_DEGREES
    column += 4u;
#endif
    //Set column MSB.
    disp_command(0x10u | (column >> 4u),  FALSE);
    //Set column LSB.
    disp_command(column & 0x0fu, FALSE);
}


Private void display_reset(void)
{
    set_disp_reset_pin(0u);
    delay_msec(100);
    set_disp_reset_pin(1u);
    delay_msec(100);
}

//This is more of a HW function, maybe it should still remain, but not be used when wanting to clear the framebuf instead.
Private void display_empty(void)
{
    U8 ix;
    U8 yx;

    for (ix = 0u; ix < DISPLAY_NUMBER_OF_PAGES; ix++)
    {
        set_page_address(ix);
        set_column(0);
        for (yx = 0u; yx < DISPLAY_NUMBER_OF_COLUMNS; yx++)
        {
            write_data(0xffu);
        }
    }
}
