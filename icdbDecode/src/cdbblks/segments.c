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

/*
******************************************************************
* Includes
******************************************************************
*/
#include "segments.h"
#include <stdint.h>					// Required for int32_t, uint32_t, ...
#include <stdlib.h>					// Required for calloc to work properly
#include <stdio.h>					// Required for memcpy
#include <string.h>					// Required for memcpy
#include "dxdatl.h"					// Required for keys
#include "joints.h" 				// Required for joints

/*
******************************************************************
* Defines
******************************************************************
*/

/*
******************************************************************
* Global Variables
******************************************************************
*/
unsigned int segment_num = 0;
segment_struct* segment = NULL;

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	ProcessSegments()
*
* - description: 	Processes segments
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void ProcessSegments(void)
{
	// ToDo: Check what ID is used for, it sometimes mismatches with other elements
	if (Dxdatl_Segment2Joints != NULL &&
		//Dxdatl_SegmentID != NULL &&
		Dxdatl_Segments != NULL &&
		//(*Dxdatl_Segment2Joints).LengthCalc == (*Dxdatl_SegmentID).LengthCalc &&
		segment_num == 0
		)
	{
		segment_num = (*Dxdatl_Segment2Joints).LengthCalc;
		segment = calloc(segment_num, sizeof(segment_struct));
		if (segment == NULL)
		{
			return;
		}
		for (unsigned int i = 0; i < segment_num; i++)
		{
			segment[i].numSegments = ((int_array_struct*)(*Dxdatl_Segments).Data)[i].Length;
			segment[i].Segments = calloc(segment[i].numSegments, sizeof(segment_section_struct));
			if (segment[i].Segments == NULL)
			{
				return;
			}
			int idx_acc = -1;

			for (unsigned int j = 0; j < segment[i].numSegments; j++)
			{
				unsigned int idx = (((int_array_struct*)(*Dxdatl_Segments).Data)[i].Data)[j].i32[0];
				idx_acc += idx + 1;
				int addr;

				// Simple safety
				if (idx_acc > ((key_struct*)(Dxdatl_Segment2Joints))->LengthCalc)
				{
					return;
					myPrint("Error processing segments!\n");
				}

				// Start Joint
				addr = ((LongData*)(((key_struct*)(Dxdatl_Segment2Joints))->Data))[idx_acc - 1].i32[0];
				segment[i].Segments[j].StartJoint = GetJoint(addr - 1);

				// End Joint
				addr = ((LongData*)(((key_struct*)(Dxdatl_Segment2Joints))->Data))[idx_acc - 1].i32[1];
				segment[i].Segments[j].EndJoint = GetJoint(addr - 1);

				// ID
				//segment[i].Segments[j].ID = ((int*)(*Dxdatl_SegmentID).Data)[i];
			}
		}
	}
	else
	{
		myPrint("Error processing segments!\n");
	}
}

/*
******************************************************************
* - function name:	InitSegments()
*
* - description: 	Initializes Segments
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void InitSegments(void)
{
	if (segment_num != 0 && segment != NULL)
	{
		for (unsigned int i = 0; i < segment_num; i++)
		{
			free(segment[i].Segments);
		}
		free(segment);
		segment_num = 0;
	}
}
/*
******************************************************************
* - function name:	GetNumSegments()
*
* - description: 	Returns the number of segments in this file
*
* - parameter: 		-
*
* - return value: 	number of segments in this file
******************************************************************
*/
unsigned int GetNumSegments(void)
{
	return segment_num;
}

/*
******************************************************************
* - function name:	GetSegment()
*
* - description: 	Returns the selected segment
*
* - parameter: 		segment index in dxd format (Starting from 1)
*
* - return value: 	segment struct
******************************************************************
*/
segment_struct GetSegment(int idx)
{
	if (segment != NULL && idx <= segment_num && idx >= 0)
	{
		return segment[idx];
	}
	return (segment_struct){0};
}