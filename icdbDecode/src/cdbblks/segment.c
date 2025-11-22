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

/*
******************************************************************
* Includes
******************************************************************
*/
#include "segment.h"
#include <stdint.h>					// Required for int32_t, uint32_t, ...
#include <stdlib.h>					// Required for calloc to work properly
#include <stdio.h>					// Required for memcpy
#include <string.h>					// Required for memcpy
#include "cdbblks.h" 				// Required for cdbblks_joint
#include "../common/dxdatl.h"		// Required for keys
#include "joint.h" 					// Required for joint
#include "../common.h" 				// Required for min

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	ProcessSegment()
*
* - description: 	Processes segment
*
* - parameter: 		element_struct pointer
*
* - return value: 	-
******************************************************************
*/
void ProcessSegment(element_struct* segment)
{
	// ToDo: Check what ID is used for, it sometimes mismatches with other elements
	if (Dxdatl_Segment2Joints != NULL &&
		//Dxdatl_SegmentID != NULL &&
		Dxdatl_Segments != NULL &&
		//(*Dxdatl_Segment2Joints).LengthCalc == (*Dxdatl_SegmentID).LengthCalc &&
		segment->Length == 0
		)
	{
		segment->Length = min(min((*Dxdatl_Segment2Joints).LengthCalc, (*Dxdatl_SegmentID).LengthCalc), (*Dxdatl_Segments).LengthCalc);
		segment->Data = calloc(segment->Length, sizeof(segment_struct));
		if (segment == NULL)
		{
			return;
		}
		for (unsigned int i = 0; i < segment->Length; i++)
		{
			((segment_struct*)(segment->Data))[i].numSegment = ((int_array_struct*)(*Dxdatl_Segments).Data)[i].Length;
			((segment_struct*)(segment->Data))[i].Segment = calloc(((segment_struct*)(segment->Data))[i].numSegment, sizeof(segment_section_struct));
			if (((segment_struct*)(segment->Data))[i].Segment == NULL)
			{
				return;
			}
			int idx_acc = -1;

			for (unsigned int j = 0; j < ((segment_struct*)(segment->Data))[i].numSegment; j++)
			{
				unsigned int idx = (((int_array_struct*)(*Dxdatl_Segments).Data)[i].Data)[j].i32[0];
				idx_acc += idx + 1;
				int addr;

				// Simple safety
				if (idx_acc > ((key_struct*)(Dxdatl_Segment2Joints))->LengthCalc)
				{
					myPrint("Error processing segments!\n");
					return;
				}

				// Start Joint
				addr = ((LongData*)(((key_struct*)(Dxdatl_Segment2Joints))->Data))[idx_acc - 1].i32[0];
				((segment_struct*)(segment->Data))[i].Segment[j].StartJoint = GetJoint(&cdbblks_joint, addr - 1);

				// End Joint
				addr = ((LongData*)(((key_struct*)(Dxdatl_Segment2Joints))->Data))[idx_acc - 1].i32[1];
				((segment_struct*)(segment->Data))[i].Segment[j].EndJoint = GetJoint(&cdbblks_joint, addr - 1);

				// ID
				//((segment_struct*)(segment->Data))[i].Segment[j].ID = ((int*)(*Dxdatl_SegmentID).Data)[i];
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
* - function name:	InitSegment()
*
* - description: 	Initializes Segment
*
* - parameter: 		element_struct pointer
*
* - return value: 	-
******************************************************************
*/
void InitSegment(element_struct* segment)
{
	if (segment->Length != 0 && segment->Data != NULL)
	{
		for (unsigned int i = 0; i < segment->Length; i++)
		{
			free(((segment_struct*)(segment->Data))[i].Segment);
		}
		free(segment->Data);
		segment->Data = NULL;
		segment->Length = 0;
	}
}

/*
******************************************************************
* - function name:	GetSegment()
*
* - description: 	Returns the selected segment
*
* - parameter: 		element_struct pointer, segment index in dxd format (Starting from 1)
*
* - return value: 	segment struct
******************************************************************
*/
segment_struct GetSegment(element_struct* segment, int idx)
{
	if (segment->Data != NULL && idx <= segment->Length && idx >= 0)
	{
		return ((segment_struct*)(segment->Data))[idx];
	}
	return (segment_struct){0};
}