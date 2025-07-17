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
#ifndef _LINES_H
#define _LINES_H

/*
******************************************************************
* Global Includes
******************************************************************
*/
#include <stdint.h>			// Required for int32_t, uint32_t, ...
#include "cdbblks.h" 		// Required for coordinate struct
#include "properties.h" 	// Required for properties struct


/*
******************************************************************
* Enums
******************************************************************
*/
#if C23
typedef enum lineending_type : int32_t
{
#else
typedef enum lineending_type
{
	lineending_hack = (int32_t)(-2147483647), // Hack to force size to 32bit signed
#endif
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
	int numSegments;
	line_segment_struct* Segments;
	properties_struct Properties;
	uid_struct UID;
	int IDDxD;
} line_struct;

/*
******************************************************************
* Global Functions
******************************************************************
*/
extern void ProcessLines(void);
extern void InitLines(void);
extern line_struct GetLine(int);
extern unsigned int GetNumLines(void);

#endif //_LINES_H