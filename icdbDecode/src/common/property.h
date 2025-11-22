/*
******************************************************************
* Info
******************************************************************
* iCDBdecode
*
* This tool can be used to analyze and decompress Siemens EDA (former Mentor Graphics) icdb.dat files.
* It's intend is to gain understanding of the file format, in order to allow interoperability with other EDA packages.
*
* The tool is based on initial research done by Patrick Yeon (https://github.com/patrickyeon/icdb2fs) in 2011.
* The research was performed by analyzing various icdb.dat files (basically staring at the hex editor for hours),
* No static or dynamic code analysis of any proprietary executable files was used to gain information about the file format.
*
* This project uses the Zlib library (https://www.zlib.net/) for decompression.
*/
#ifndef _PROPERTY_H
#define _PROPERTY_H

/*
******************************************************************
* Global Includes
******************************************************************
*/
#include <stdint.h>		// Required for int32_t, uint32_t, ...
#include "../common.h"	// Required for string_struct

/*
******************************************************************
* Enums
******************************************************************
*/
#if C23
typedef enum style_type : int32_t
{
#else
typedef enum style_type
{
	style_hack = (int32_t)(-2147483647), // Hack to force size to 32bit signed
#endif
	style_AutoSolid		= -1,
	style_Solid			= 0,
	style_Dash			= 1,
	style_Center		= 2,
	style_Phantom		= 3,
	style_Bigdash		= 4,
	style_Dot			= 5,
	style_DashDot		= 6,
	style_Mediumdash	= 7,
}style_type;

#if C23
typedef enum fill_type : int32_t
{
#else
typedef enum fill_type
{
	fill_hack = (int32_t)(-2147483647), // Hack to force size to 32bit signed
#endif
	fill_AutoHollow		= -1,
	fill_Hollow			= 0,
	fill_Solid			= 1,
	fill_Diagdn1		= 2,
	fill_Diagup2		= 3,
	fill_Grey08			= 4,
	fill_Diagdn2		= 5,
	fill_Diagup1		= 6,
	fill_Horiz			= 7,
	fill_Vert			= 8,
	fill_Grid2			= 9,
	fill_Grid1			= 10,
	fill_X2				= 11,
	fill_X1				= 12,
	fill_Grey50			= 13,
	fill_Grey92			= 14,
	fill_Grey04			= 15,
}fill_type;

#if C23
typedef enum thikness_type : int32_t
{
#else
typedef enum thikness_type
{
	thikness_hack = (int32_t)(-2147483647), // Hack to force size to 32bit signed
#endif
	thikness_Auto	= -1,
	thikness_1		= 1,
	thikness_2		= 2,
	thikness_3		= 3,
	thikness_4		= 4,
	thikness_5		= 5,
	thikness_6		= 6,
	thikness_7		= 7,
	thikness_8		= 8,
	thikness_9		= 9,
	thikness_10		= 10,

}thikness_type;

#if C23
typedef enum colorkey_type : uint8_t
{
#else
typedef enum colorkey_type
{
#endif
	colorkey_custom_F		= 0,
	colorkey_custom_L		= 1,
	colorkey_default		= 255,
}colorkey_type;

/*
******************************************************************
* Structures
******************************************************************
*/

typedef struct coordinate_struct
{
	int32_t X;
	int32_t Y;
} coordinate_struct;

typedef struct color_struct
{
	uint8_t Red;
	uint8_t Green;
	uint8_t Blue;
	uint8_t Key;
} color_struct;

typedef struct property_struct
{
	color_struct LineColor;
	color_struct FillColor;
	style_type LineType;
	thikness_type Thickness;
	fill_type Fill;
} property_struct;

/*
******************************************************************
* Global Functions
******************************************************************
*/
extern void ProcessProperty(element_struct*);
extern void InitProperty(element_struct*);
extern property_struct GetProperty(element_struct*, int);

#endif //_PROPERTY_H