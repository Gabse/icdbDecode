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
#include "joint.h"
#include <stdint.h>					// Required for int32_t, uint32_t, ...
#include <stdlib.h>					// Required for calloc to work properly
#include <stdio.h>					// Required for memcpy
#include <string.h>					// Required for memcpy
#include "../common.h"				// Required for element_struct
#include "../common/dxdatl.h"		// Required for keys
#include "../common/property.h"		// Required for keys

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	ProcessJoint()
*
* - description: 	Processes joint
*
* - parameter: 		element_struct pointer
*
* - return value: 	-
******************************************************************
*/
void ProcessJoint(element_struct* joint)
{
	// ToDo: Check what ID is used for, it sometimes mismatches with other elements
	if (Dxdatl_Joint2UID != NULL &&
	//	Dxdatl_JointID != NULL &&
		Dxdatl_JointPos != NULL &&
	//	(*Dxdatl_Joint2UID).LengthCalc == (*Dxdatl_JointID).LengthCalc &&
		(*Dxdatl_Joint2UID).LengthCalc == (*Dxdatl_JointPos).LengthCalc &&
		joint->Length == 0		
		)
	{
		joint->Length = (*Dxdatl_Joint2UID).LengthCalc;
		joint->Data = calloc(joint->Length, sizeof(joint_struct));
		if (joint->Data == NULL)
		{
			return;
		}
		for (unsigned int i = 0; i < joint->Length; i++)
		{
			// Coordinate
			(((joint_struct*)(joint->Data))[i]).Coord = ((coordinate_struct*)(*Dxdatl_JointPos).Data)[i];

			// UID
			(((joint_struct*)(joint->Data))[i]).UID = ((uid_struct*)(*Dxdatl_Joint2UID).Data)[i];

			// ID
			//(((joint_struct*)(joint->Data))[i]).ID = ((int*)(*Dxdatl_JointID).Data)[i];
		}
	}
	else
	{
		myPrint("Error processing joints!\n");
	}
}

/*
******************************************************************
* - function name:	InitJoint()
*
* - description: 	Initializes joint
*
* - parameter: 		element_struct pointer
*
* - return value: 	-
******************************************************************
*/
void InitJoint(element_struct* joint)
{
	if (joint->Length != 0 && joint->Data != NULL)
	{
		free(joint->Data);
		joint->Data = NULL;
		joint->Length = 0;
	}
}

/*
******************************************************************
* - function name:	GetJoint()
*
* - description: 	Returns the selected joint
*
* - parameter: 		element_struct pointer, index in dxd format (Starting from 1)
*
* - return value: 	joint struct
******************************************************************
*/
joint_struct GetJoint(element_struct* joint, int idx)
{
	if (joint->Data != NULL && idx <= joint->Length && idx >= 0)
	{
		return ((joint_struct*)(joint->Data))[idx];
	}
	return (joint_struct){0};
}