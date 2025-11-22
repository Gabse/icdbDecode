/*
******************************************************************
* Info
******************************************************************
* iCDBdecode
*
* This tool can be used to analyze and decompress Siemens EDA (former Mentor Graphics) icdb.dat files.
* It's intend is to gain understanding of the file format, in order to allow interoperability with other EDA packages.
*
* The tool is based on initial resegrouph done by Patrick Yeon (https://github.com/patrickyeon/icdb2fs) in 2011.
* The resegrouph was performed by analyzing various icdb.dat files (basically staring at the hex editor for hours),
* No static or dynamic code analysis of any proprietary executable files was used to gain information about the file format.
*
* This project uses the Zlib library (https://www.zlib.net/) for decompression.
*/

/*
******************************************************************
* Includes
******************************************************************
*/
#include "group.h"
#include <stdint.h>					// Required for int32_t, uint32_t, ...
#include <stdlib.h>					// Required for calloc to work properly
#include <stdio.h>					// Required for memcpy
#include <string.h>					// Required for memcpy
#include "catlgatl.h"				// Required for keys
#include "../common.h" 				// Required for uid struct

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	ProcessGroup()
*
* - description: 	Processes group
*
* - parameter: 		element_struct pointer
*
* - return value: 	-
******************************************************************
*/
void ProcessGroup(element_struct* group)
{
	// ToDo: Check what GroupDxDView is used for, it sometimes mismatches with other elements
	if (Catlgatl_GroupDescription != NULL &&
		//Catlgatl_GroupDxDView != NULL &&
		Catlgatl_GroupName != NULL &&
		Catlgatl_GroupOwnerUID != NULL &&
		Catlgatl_GroupType != NULL &&
		Catlgatl_GroupUID != NULL &&
		//(*Catlgatl_GroupDescription).LengthCalc == (*Catlgatl_GroupDxDView).LengthCalc &&
		(*Catlgatl_GroupDescription).LengthCalc == (*Catlgatl_GroupName).LengthCalc &&
		(*Catlgatl_GroupDescription).LengthCalc == (*Catlgatl_GroupOwnerUID).LengthCalc &&
		(*Catlgatl_GroupDescription).LengthCalc == (*Catlgatl_GroupType).LengthCalc &&
		(*Catlgatl_GroupDescription).LengthCalc == (*Catlgatl_GroupUID).LengthCalc &&
		group->Length == 0
		)
	{
		group->Length = (*Catlgatl_GroupDescription).LengthCalc;
		group->Data = calloc(group->Length, sizeof(group_struct));
		if (group->Data == NULL)
		{
			return;
		}
		for (unsigned int i = 0; i < group->Length; i++)
		{
			// Description
			(((group_struct*)(group->Data))[i]).Description = CopyString(((string_struct*)(*Catlgatl_GroupDescription).Data)[i]);

			// DXDView
			//(((group_struct*)(group->Data))[i]).DxDView = ((IntData*)(*Catlgatl_GroupDxDView).Data)[i].u32;
			
			// Name
			(((group_struct*)(group->Data))[i]).Name = CopyString(((string_struct*)(*Catlgatl_GroupName).Data)[i]);
			
			// Owner UID
			memcpy(&(((group_struct*)(group->Data))[i]).OwnerUID, &(((char*)(*Catlgatl_GroupOwnerUID).Data)[i * 8]), 8);
			
			// Type
			(((group_struct*)(group->Data))[i]).Type = ((IntData*)(*Catlgatl_GroupType).Data)[i].i32[0];
			
			// UID
			memcpy(&(((group_struct*)(group->Data))[i]).UID, &(((char*)(*Catlgatl_GroupUID).Data)[i * 8]), 8);
		}
	}
	else
	{
		myPrint("Error processing groups!\n");
	}
}

/*
******************************************************************
* - function name:	InitGroup()
*
* - description: 	Initializes group
*
* - parameter: 		element_struct pointer
*
* - return value: 	-
******************************************************************
*/
void InitGroup(element_struct* group)
{
	if (group->Length != 0 && group->Data != NULL)
	{
		for(unsigned int i = 0; i< group->Length; i++)
		{
			if ((((group_struct*)(group->Data))[i]).Description.Text != NULL)
			{
				free((((group_struct*)(group->Data))[i]).Description.Text);
			}
			if ((((group_struct*)(group->Data))[i]).Name.Text != NULL)
			{
				free((((group_struct*)(group->Data))[i]).Name.Text);
			}
		}
		free(group->Data);
		group->Data = NULL;
		group->Length = 0;
	}
}

/*
******************************************************************
* - function name:	GetGroup()
*
* - description: 	Returns the selected group
*
* - parameter: 		element_struct pointer, group index
*
* - return value: 	group struct
******************************************************************
*/
group_struct GetGroup(element_struct* group, unsigned int idx)
{
	if (group->Data != NULL && idx <= group->Length && idx >= 0)
	{
		return ((group_struct*)(group->Data))[idx];
	}
	return (group_struct) { 0 };
}