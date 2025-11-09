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
#ifndef _TEXTDATA_H
#define _TEXTDATA_H

/*
******************************************************************
* Global Includes
******************************************************************
*/
#include <stdint.h>			// Required for int32_t, uint32_t, ...
#include "cdbblks.h" 		// Required for coordinate struct and color struct

/*
******************************************************************
* Enums
******************************************************************
*/
#if C23
typedef enum font_type : uint8_t
{
#else
typedef enum font_type
{
#endif
	font_Fixed				= 0,
	font_Roman				= 1,
	font_RomanItalic		= 2,
	font_RomanBold			= 3,
	font_RomanBoldItalic	= 4,
	font_SansSerif			= 5,
	font_Script				= 6,
	font_SansSerifBold		= 7,
	font_ScriptBold			= 8,
	font_Gothic				= 9,
	font_OldEnglish			= 10,
	font_Kanji				= 11,
	font_Plot				= 12,
	font_None				= 127,
	font_Custom				= 255,
}font_type;

#if C23
typedef enum textrotation_type : int32_t
{
#else
typedef enum textrotation_type
{
	textrotation_hack = (int32_t)(-2147483647), // Hack to force size to 32bit signed
#endif
	textrotation_0Degree		= 0,
	textrotation_90Degree		= 1,
	textrotation_180Degree		= 2,
	textrotation_270Degree		= 3,
}textrotation_type;

#if C23
typedef enum textorigin_type : int32_t
{
#else
typedef enum textorigin_type
{
	textorigin_hack = (int32_t)(-2147483647), // Hack to force size to 32bit signed
#endif
	textorigin_Default			= 0,
	textorigin_UpperLeft		= 1,
	textorigin_MiddleLeft		= 2,
	textorigin_LowerLeft		= 3,
	textorigin_UpperCenter		= 4,
	textorigin_MiddleCenter		= 5,
	textorigin_LowerCenter		= 6,
	textorigin_UpperRight		= 7,
	textorigin_MiddleRight		= 8,
	textorigin_LowerRight		= 9,
}textorigin_type;

#if C23
typedef enum option_type : uint8_t
{
#else
typedef enum option_type
{
	option_hack = (int8_t)(288), // Hack to force size to 8bit signed
#endif
	option_true		= 1,
	option_false	= 0,
}option_type;

/*
******************************************************************
* Structures
******************************************************************
*/
typedef struct font_struct
{
	font_type Font;
	int FontNameLen;
	char* CustomFont;
	option_type Underline;
	option_type Strikeout;
	option_type Bold;
	option_type unknown1;
	option_type	unknown2;
	option_type unknown3;
	option_type unknown4;
	option_type unknown5;
} font_struct;

typedef struct textdata_struct
{
	color_struct LineColor;
	color_struct FillColor;
	font_struct Font;
	int IDDxD;
	textrotation_type Orientation;
	textorigin_type Origin;
	coordinate_struct Position;
	int Size;
} textdata_struct;

/*
******************************************************************
* Global Functions
******************************************************************
*/
extern void ProcessTextdatas(void);
extern void InitTextdatas(void);
extern textdata_struct GetTextdata(int);
extern unsigned int GetNumTextdatas(void);

#endif //_TEXTDATA_H