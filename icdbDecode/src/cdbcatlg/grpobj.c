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
#include "grpobj.h"
#include <stdint.h>					// Required for int32_t, uint32_t, ...
#include <stdlib.h>					// Required for calloc to work properly
#include "catlgatl.h"				// Required for keys
#include "../common.h" 				// Required for uid struct

/*
******************************************************************
* Global Variables
******************************************************************
*/
unsigned int grpobj_num = 0;
grpobj_struct* grpobj = NULL;

/*
******************************************************************
* - function name:	ParseGrpobj()
*
* - description: 	Processes Groupobjects
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
int ParseGrpobj(char* path, uint32_t pathlength, char* file, uint32_t filelength)
{
	FILE* sourceFile;
	
	// Open file
	sourceFile = myfopen("rb", path, pathlength, file, filelength, DIR_SEPARATOR);
	if (sourceFile != 0 && grpobj_num == 0)
	{
		fseek(sourceFile, 0, SEEK_END);
		uint32_t FileEnd = ftell(sourceFile);
		fseek(sourceFile, 0x2000, SEEK_SET);
		
		uint32_t unknown;
		fread(&unknown, sizeof(uint32_t), 1, sourceFile); // Might be something interesting
		fread(&grpobj_num, sizeof(uint32_t), 1, sourceFile); // Number of entry�s

		grpobj = calloc(sizeof(grpobj_struct), grpobj_num);
		if (grpobj == NULL)
		{
			return -1;
		}

		for (unsigned int i = 0; i < grpobj_num; i++)
		{
			// Simple Safety
			if ((ftell(sourceFile) + sizeof(uid_struct)) + sizeof(unsigned int) > FileEnd)
			{
				break;
			}

			// UID
			fread(&grpobj[i].UID.UID[0], sizeof(uid_struct), 1, sourceFile);

			// Number of groups
			fread(&grpobj[i].numGroups, sizeof(unsigned int), 1, sourceFile);
			grpobj[i].group = calloc(grpobj[i].numGroups, sizeof(unsigned int));
			if (grpobj[i].group == NULL)
			{
				return -1;
			}

			// Simple Safety
			if ((ftell(sourceFile) + sizeof(unsigned int) * grpobj[i].numGroups) > FileEnd)
			{
				break;
			}
			// Read groups
			fread(grpobj[i].group, sizeof(unsigned int), grpobj[i].numGroups, sourceFile);
		}
		return 0;
	}
	else
	{
		return -1;
	}
}

/*
******************************************************************
* - function name:	InitGrpobj()
*
* - description: 	Initializes Groupobjects
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void InitGrpobj(void)
{
	if (grpobj_num != 0 && grpobj != NULL)
	{
		for (unsigned int i = 0; i < grpobj_num; i++)
		{
			if ((grpobj[i]).group != NULL)
			{
				free((grpobj[i]).group);
			}
		}
		free(grpobj);
		grpobj_num = 0;
	}
}

/*
******************************************************************
* - function name:	GetNumGrpobj()
*
* - description: 	Returns the number of group objects
*
* - parameter: 		-
*
* - return value: 	number of group objects
******************************************************************
*/
unsigned int GetNumGrpobj(void)
{
	return grpobj_num;
}

/*
******************************************************************
* - function name:	GetGrpobj()
*
* - description: 	Returns the selected group object
*
* - parameter: 		group object index
*
* - return value: 	group object struct
******************************************************************
*/
grpobj_struct GetGrpobj(unsigned int idx)
{
	grpobj_struct obj = { 0 };
	if (grpobj_num > idx)
	{
		obj = grpobj[idx];
	}
	return obj;
}

/*
******************************************************************
* - function name:	InsideGroup()
*
* - description: 	Returns true if a UID is in a specified group
*
* - parameter: 		UID; group to check for
*
* - return value: 	0 or 1
******************************************************************
*/
unsigned int InsideGroup(uid_struct UID, unsigned int group)
{
	grpobj_struct obj;
	for(unsigned int i = 0; i < grpobj_num; i++)
	{
		obj = grpobj[i];
		if(	obj.UID.UID[0] == UID.UID[0] && 
			obj.UID.UID[1] == UID.UID[1] && 
			obj.UID.UID[2] == UID.UID[2] && 
			obj.UID.UID[3] == UID.UID[3] && 
			obj.UID.UID[4] == UID.UID[4] && 
			obj.UID.UID[5] == UID.UID[5] && 
			obj.UID.UID[6] == UID.UID[6] &&
			obj.UID.UID[7] == UID.UID[7]
			)
		{
			for (unsigned int j = 0; j < obj.numGroups; j++)
			{
				if (obj.group[j] == group)
				{
					return 1;
				}
			}
		}
	}
	return 0;
}

