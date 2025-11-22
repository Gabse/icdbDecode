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
#ifndef _SEGMENT_H
#define _SEGMENT_H

/*
******************************************************************
* Global Includes
******************************************************************
*/
#include <stdint.h>			// Required for int32_t, uint32_t, ...
#include "../common.h"		// Required for element_struct
#include "joint.h" 		// Required for joint

/*
******************************************************************
* Structures
******************************************************************
*/
typedef struct segment_section_struct
{
	joint_struct StartJoint;
	joint_struct EndJoint;
	int ID;
} segment_section_struct;

typedef struct segment_struct
{
	unsigned int numSegment;
	segment_section_struct* Segment;
} segment_struct;

/*
******************************************************************
* Global Functions
******************************************************************
*/
extern void ProcessSegment(element_struct*);
extern void InitSegment(element_struct*);
extern segment_struct GetSegment(element_struct*, int);

#endif //_SEGMENT_H