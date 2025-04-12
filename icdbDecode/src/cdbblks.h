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
* No static or dynamic code analysis of any proprietary executable files was used to gain information’s about the file format.
*
* This project uses the Zlib library (https://www.zlib.net/) for decompression.
*/
#ifndef _CDBBLKS_H
#define _CDBBLKS_H

/*
******************************************************************
* Global Includes
******************************************************************
*/
#include <stdint.h>		// Required for int32_t, uint32_t, ...
#include "common.h"		// Required for text_struct

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
typedef enum font_type : uint8_t
{
#else
typedef enum font_type
{
#endif
	font_Custom				= 0,
	font_Default			= 1,
	fontRomanItalic			= 2,
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
}font_type;

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

typedef struct segment2joint_struct
{
	uint32_t StartJoint;
	uint32_t EndJoint;
} segment2joint_struct;

typedef struct color_struct
{
	uint8_t Red;
	uint8_t Green;
	uint8_t Blue;
	uint8_t Key;
} color_struct;

typedef struct segment_struct
{
	uint32_t Len;
	uint32_t* Data;
} segment_struct;

typedef struct linepoint_struct
{
	uint32_t Count;
	int32_t StartPoint;
	int32_t Offset;
} linepoint_struct;

typedef struct rectangle_struct
{
	coordinate_struct Start;
	coordinate_struct End;
} rectangle_struct;

typedef struct style_struct
{
	uint32_t index;
} style_struct;

typedef struct bus_id_struct
{
	uint32_t index;
	uint32_t ID;
} bus_id_struct;

typedef struct net_id_struct
{
	uint32_t ID;
} net_id_struct;

typedef struct group_struct
{
	uint32_t index;
	uint32_t ID;
} group_struct;

/*
******************************************************************
* Global Variables
******************************************************************
*/

FILE* logFile;
int logError;
char* source;
int sourceLenth;
char* destination;
int destinationLenth;
int quietMode;

extern FILE* sourceFile;
extern uint32_t filesize;
extern uint32_t NameLenth;
extern char* Name;

extern int32_t NumJoint;
extern coordinate_struct* JointPoss;
extern uint32_t* JointIDs;

extern int32_t NumSegment2Joints;
extern segment2joint_struct* Segment2Joints;
extern uint32_t* SegmentNets;

extern int32_t NumStyles;
extern color_struct* Color;
extern color_struct* ColorExt;
extern style_type* Style;
extern int32_t* Thickness;
extern fill_type* Fill;

extern int32_t NumSegments;
extern segment_struct* Segments;

extern int32_t NumLines;
extern linepoint_struct* LinePointX;
extern linepoint_struct* LinePointY;
extern style_struct* Line2Style;

extern int32_t NumLineEnds;
extern uint32_t* LineEndingCap;
extern uint32_t* LineStartingCap;
extern uint32_t* LineArrowSize;

extern int32_t NumRectangle;
extern rectangle_struct* RectCoord;
extern style_struct* Rect2Style;

extern int32_t NumCircle;
extern coordinate_struct* CirclesPos;
extern uint32_t* CircleRadius;
extern style_struct* Circle2Style;

extern int32_t NumArc;
extern coordinate_struct* ArcEnd;
extern coordinate_struct* ArcStart;
extern coordinate_struct* ArcMid;
extern style_struct* Arc2Style;

extern int32_t NumTextData;
extern text_struct* TextFont;
extern coordinate_struct* TextPos;
extern textorigin_type* TextOrigin;
extern uint32_t* TextSize;
extern textrotation_type* TextOrient;
extern color_struct* TextColor;
extern uint32_t* TextIDDxD;

extern int32_t NumText;
extern text_struct* TextString;
extern uint32_t* Text2TextData;

extern int32_t NumNetLabel;
extern text_struct* NetLabel;
extern group_struct* Label2TextData;

extern int32_t NumBus;
extern bus_id_struct* BusID;
extern group_struct* Bus2GrpSegments;
extern group_struct* Bus2GrpStyles;

extern int32_t NumNet;
extern net_id_struct* NetID;
extern group_struct* Net2GrpSegments;
extern group_struct* Net2GrpStyles;
extern group_struct* Net2NetDxD;

/*
******************************************************************
* Global Functions
******************************************************************
*/

#endif //_CDBBLKS_H