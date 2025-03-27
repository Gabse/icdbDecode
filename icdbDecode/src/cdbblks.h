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
extern uint32_t* Style;
extern uint32_t* Thickness;
extern uint32_t* Fill;

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
extern uint32_t* TextOrigin;
extern uint32_t* TextSize;
extern uint32_t* TextOrient;
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
extern int ProcessKeyCdbblks(char*, uint32_t);

#endif //_CDBBLKS_H