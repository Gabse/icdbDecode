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
* No static or dynamic code analysis of any proprietary executable files was used to gain information’s about the file format.
*
* This project uses the Zlib library (https://www.zlib.net/) for decompression.
*/

/*
******************************************************************
* Includes
******************************************************************
*/
#include "groups.h"
#include <stdint.h>					// Required for int32_t, uint32_t, ...
#include <stdlib.h>					// Required for calloc to work properly
#include <stdio.h>					// Required for memcpy
#include <string.h>					// Required for memcpy
#include "catlgatl.h"				// Required for keys
#include "../common.h" 				// Required for uid struct

/*
******************************************************************
* Global Variables
******************************************************************
*/
unsigned int group_num = 0;
group_struct* groups = NULL;

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	ProcessGroups()
*
* - description: 	Processes groups
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void ProcessGroups(void)
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
		group_num == 0 
		)
	{
		group_num = (*Catlgatl_GroupDescription).LengthCalc;
		groups = calloc(group_num, sizeof(group_struct));
		if (groups == NULL)
		{
			return;
		}
		for (unsigned int i = 0; i < group_num; i++)
		{
			// Description
			(groups[i]).Description = CopyString(((string_struct*)(*Catlgatl_GroupDescription).Data)[i]);

			// DXDView
			//(groups[i]).DxDView = ((IntData*)(*Catlgatl_GroupDxDView).Data)[i].u32;
			
			// Name
			(groups[i]).Name = CopyString(((string_struct*)(*Catlgatl_GroupName).Data)[i]);
			
			// Owner UID
			memcpy(&(groups[i]).OwnerUID, &(((char*)(*Catlgatl_GroupOwnerUID).Data)[i * 8]), 8);
			
			// Type
			(groups[i]).Type = ((IntData*)(*Catlgatl_GroupType).Data)[i].i32[0];
			
			// UID
			memcpy(&(groups[i]).UID, &(((char*)(*Catlgatl_GroupUID).Data)[i * 8]), 8);
		}
	}
	else
	{
		myPrint("Error processing groups!\n");
	}
}

/*
******************************************************************
* - function name:	InitGroups()
*
* - description: 	Initializes groups
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void InitGroups(void)
{
	if (group_num != 0 && groups != NULL)
	{
		for(unsigned int i = 0; i< group_num; i++)
		{
			if ((groups[i]).Description.Text != NULL)
			{
				free((groups[i]).Description.Text);
			}
			if ((groups[i]).Name.Text != NULL)
			{
				free((groups[i]).Name.Text);
			}
		}
		free(groups);
		group_num = 0;
	}
}

/*
******************************************************************
* - function name:	GetNumGroups()
*
* - description: 	Returns the number of groups
*
* - parameter: 		-
*
* - return value: 	number of graphical groups
******************************************************************
*/
unsigned int GetNumGroups(void)
{
	return group_num;
}

/*
******************************************************************
* - function name:	GetGroup()
*
* - description: 	Returns the selected group
*
* - parameter: 		group index
*
* - return value: 	group struct
******************************************************************
*/
group_struct GetGroup(unsigned int idx)
{
	group_struct group = { 0 };
	if (group_num > idx)
	{
		group = groups[idx];
	}
	return group;
}