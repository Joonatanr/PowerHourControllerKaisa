/*
 * parser.c
 *
 *  Created on: Aug 21, 2017
 *      Author: Joonatan
 */

#include <misc.h>
#include <stdlib.h>

//General use text buffer.
Public char sY[TXT_BUF_LEN];

//We pass information with R:XX,YY,HH,WW --
Public Boolean parseRectangle(char * src, Rectangle * dest)
{
    Boolean res = FALSE;
    //U32 x,y,height,length = 0u;
    U32 values[4u];
    char * ps = src;

    if (*ps == 'R')
    {
        //We skip the : character.
        ps += 2;
        if (parseCommaSeparatedU32Array(values, ps, 4u))
        {
            res = TRUE;
            dest->location.x = values[0];
            dest->location.y = values[1];
            dest->size.height = values[2];
            dest->size.width = values[3];
        }
    }

    return res;
}


/* Returns a rectangle around a central point defined by location.*/
Public inline Rectangle CreateRectangleAroundCenter(Point location, Size size)
{
    Rectangle res;
    res.location.x = GET_X_FROM_CENTER(location.x, size.width);
    res.location.y = GET_Y_FROM_CENTER(location.y, size.height);

    res.size.height = size.height;
    res.size.width = size.width;
    return res;
}

//Returns the pointer to the next character.
Public char * parseU32FromString(U32 * dest, char * src)
{
    char * ps = src;
    U32 res = 0u;
    U32 dec = 1u;
    U32 temp;

    if(!IS_DIGIT(*ps))
    {
        ps++;
    }

    while (IS_DIGIT(*ps))
    {
        temp = (U32)*ps - (U32)'0';
        res += temp * dec;
        dec *= 10u;
        ps++;
    }
    return ps;
}


Public Boolean parseCommaSeparatedU32Array(U32 * dest, char * src, U32 dest_len)
{
    U8 ix = 0u;
    char * ps = src;

    //We need to parse the first member separately.
    dest[ix] = atoi(ps);
    ix++;
    ps++;

    while(*ps)
    {
        if ((*ps == ',') && (*(ps + 1) != 0))
        {
            dest[ix] = atoi(ps + 1);
            ix++;
            if (ix > dest_len)
            {
                return FALSE;
            }
        }
        ps++;
    }
    return TRUE;
}


Public U8 addchar (char * str, char c)
{
    U8 pos = 0u;
    char *ps = str;
    while (ps)
    {
        if (ps[0] == 0)
        {
            break;
        }
        ps++;
        pos++;
    }
    str[pos] = c;
    str[++pos] = 0;
    return pos;
}


Public U8 addstr (char * str1, const char * str2)
{
    U8 pos1 = 0u;

    char * ps1 = str1;
    const char * ps2 = str2;

    while (ps1)
    {
        if (ps1[0] == 0)
        {
            break;
        }
        ps1++;
        pos1++;
    }

    while (ps2)
    {
        if (ps2[0] == 0)
        {
            break;
        }
        str1[pos1] = ps2[0];
        ps2++;
        pos1++;
    }
    str1[pos1] = 0;
    return pos1;
}

//Returns length of the string.
Public U8 long2string (long nr, char *dest)
{
  U8 c, i, imax, imax2, i2;

  i2 = 0; i = 0;
  if (nr == 0) dest[i++] = '0';
  if (nr < 0) {i2 = 1; nr = 0 - nr; }

  while (nr > 0)
  {
    c = nr % 10;
    nr = nr / 10;
    dest[i++] = '0' + c;
  }

  if (i2) dest[i++] = '-';
  dest[i] = 0;

  imax = i;
  imax2 = imax / 2;

  for (i = 0; i < imax2; ++i)
  {
    i2 = imax-i-1;
    c = dest[i];
    dest[i] = dest[i2];
    dest[i2] = c;
  }

  return imax;
}

Public U8 hex2string(unsigned int hex, char *dest)
{
    char c;
    char *ps = dest;
    unsigned char nibble;

    do
    {
        nibble = hex & 0x0000000fu;
        if (nibble <= 9u)
        {
            c = '0' + nibble;
        }
        else
        {
            c = 'A' + (nibble - 10);
        }
        *ps = c;
        ps++;
        hex = hex >> 4u;
    }
    while(hex > 0);

    return 0;
}





