/*
 * parser.h
 *
 *  Created on: Aug 21, 2017
 *      Author: Joonatan
 */

#ifndef LOGIC_PARSER_H_
#define LOGIC_PARSER_H_

#include "typedefs.h"

#define TXT_BUF_LEN 32
#define IS_DIGIT(a) (((a) >= '0') && ((a) <= '9'))

#define GET_X_FROM_CENTER(center,height) (((center) >= ((height) / 2)) ? (center) - ((height) / 2) : 0u)
#define GET_Y_FROM_CENTER(center,width)  (((center) >= ((width) / 2)) ? (center) -  ((width)  / 2) : 0u)

Public inline Rectangle CreateRectangleAroundCenter(Point location, Size size);

Public char * parseU32FromString(U32 * dest, char * src);
Public Boolean parseCommaSeparatedU32Array(U32 * dest, char * src, U32 dest_len);
Public Boolean parseRectangle(char * src, Rectangle * dest);

Public U8 addchar (char * str, char c);
Public U8 addstr (char * str1, const char * str2);
Public U8 long2string (long nr, char *s);
Public U8 hex2string(unsigned int hex, char *dest);

extern char sY[TXT_BUF_LEN];

#endif /* LOGIC_PARSER_H_ */
