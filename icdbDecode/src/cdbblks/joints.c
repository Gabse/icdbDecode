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
#include "joints.h"
#include <stdint.h>					// Required for int32_t, uint32_t, ...
#include <stdlib.h>					// Required for calloc to work properly
#include <stdio.h>					// Required for memcpy
#include <string.h>					// Required for memcpy
#include "dxdatl.h"					// Required for keys

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
unsigned int joint_num = 0;
joint_struct* joints = NULL;

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	ProcessJoints()
*
* - description: 	Processes joints
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void ProcessJoints(void)
{
	// ToDo: Check what ID is used for, it sometimes mismatches with other elements
	if (Dxdatl_Joint2UID != NULL &&
	//	Dxdatl_JointID != NULL &&
		Dxdatl_JointPos != NULL &&
	//	(*Dxdatl_Joint2UID).LengthCalc == (*Dxdatl_JointID).LengthCalc &&
		(*Dxdatl_Joint2UID).LengthCalc == (*Dxdatl_JointPos).LengthCalc &&
		joint_num == 0		
		)
	{
		joint_num = (*Dxdatl_Joint2UID).LengthCalc;
		joints = calloc(joint_num, sizeof(joint_struct));
		if (joints == NULL)
		{
			return;
		}
		for (unsigned int i = 0; i < joint_num; i++)
		{
			// Coordinate
			(joints[i]).Coord = ((coordinate_struct*)(*Dxdatl_JointPos).Data)[i];

			// UID
			(joints[i]).UID = ((uid_struct*)(*Dxdatl_Joint2UID).Data)[i];

			// ID
			//(joints[i]).ID = ((int*)(*Dxdatl_JointID).Data)[i];
		}
	}
	else
	{
		myPrint("Error processing joints!\n");
	}
}

/*
******************************************************************
* - function name:	InitJoints()
*
* - description: 	Initializes joints
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void InitJoints(void)
{
	if (joint_num != 0 && joints != NULL)
	{
		free(joints);
		joint_num = 0;
	}
}
/*
******************************************************************
* - function name:	GetNumJoints()
*
* - description: 	Returns the number of joints in this file
*
* - parameter: 		-
*
* - return value: 	number of joints in this file
******************************************************************
*/
unsigned int GetNumJoints(void)
{
	return joint_num;
}

/*
******************************************************************
* - function name:	GetJoint()
*
* - description: 	Returns the selected joint
*
* - parameter: 		joint index in dxd format (Starting from 1)
*
* - return value: 	joint struct
******************************************************************
*/
joint_struct GetJoint(int idx)
{
	if (joints != NULL && idx <= joint_num && idx >= 0)
	{
		return joints[idx];
	}
	return (joint_struct){0};
}