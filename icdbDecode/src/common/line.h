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
#ifndef _LINE_H
#define _LINE_H

/*
******************************************************************
* Global Includes
******************************************************************
*/
#include <stdint.h>		// Required for int32_t, uint32_t, ...
#include "../common.h"	// Required for element_struct
#include "property.h" // Required for property struct


/*
******************************************************************
* Enums
******************************************************************
*/
typedef enum lineending_type : int32_t
{
	lineending_None					= 0,
	lineending_SimpleArrow			= 1,
	lineending_SimpleArrowFilled	= 2,
	lineending_SharpArrow			= 3,
	lineending_SharpArrowFilled		= 4,
	lineending_ComplexArrow			= 5,
	lineending_ComplexArrowFilled	= 6,
}lineending_type;

/*
******************************************************************
* Structures
******************************************************************
*/

typedef struct line_segment_struct
{
	coordinate_struct Start;
	coordinate_struct End;
	lineending_type EndingStart;
	lineending_type EndingEnd;
	uint32_t EndingSize;
} line_segment_struct;

typedef struct line_struct
{
	int numSegment;
	line_segment_struct* Segment;
	property_struct Property;
	uid_struct UID;
	int IDDxD;
} line_struct;

/*
******************************************************************
* Global Functions
******************************************************************
*/
extern void ProcessLine(element_struct*, element_struct*);
extern void InitLine(element_struct*);
extern line_struct GetLine(element_struct*, int);

#endif //_LINE_H