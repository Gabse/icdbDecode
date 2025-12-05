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
* - function name:	ParseGrpobj()
*
* - description: 	Processes Groupobjects
*
* - parameter: 		element_struct pointer
*
* - return value: 	-
******************************************************************
*/
int ParseGrpobj(element_struct* grpobj, char* path, uint32_t pathlength, char* file, uint32_t filelength)
{
	FILE* sourceFile;
	
	// Open file
	sourceFile = myfopen("rb", path, pathlength, file, filelength, DIR_SEPARATOR);
	if (sourceFile != 0 && grpobj->Length == 0)
	{
		fseek(sourceFile, 0, SEEK_END);
		uint32_t FileEnd = ftell(sourceFile);
		fseek(sourceFile, 0x2000, SEEK_SET);
		
		uint32_t unknown;
		(void)!fread(&unknown, sizeof(uint32_t), 1, sourceFile); // Might be something interesting
		(void)!fread(&grpobj->Length, sizeof(uint32_t), 1, sourceFile); // Number of entry�s

		grpobj->Data = calloc(sizeof(grpobj_struct), grpobj->Length);
		if (grpobj->Data == NULL)
		{
			return -1;
		}

		for (unsigned int i = 0; i < grpobj->Length; i++)
		{
			// Simple Safety
			if ((ftell(sourceFile) + sizeof(uid_struct)) + sizeof(unsigned int) > FileEnd)
			{
				break;
			}

			// UID
			(void)!fread(&((grpobj_struct*)(grpobj->Data))[i].UID.UID[0], sizeof(uid_struct), 1, sourceFile);

			// Number of group
			(void)!fread(&((grpobj_struct*)(grpobj->Data))[i].numGroup, sizeof(unsigned int), 1, sourceFile);
			((grpobj_struct*)(grpobj->Data))[i].group = calloc(((grpobj_struct*)(grpobj->Data))[i].numGroup, sizeof(unsigned int));
			if (((grpobj_struct*)(grpobj->Data))[i].group == NULL)
			{
				return -1;
			}

			// Simple Safety
			if ((ftell(sourceFile) + sizeof(unsigned int) * ((grpobj_struct*)(grpobj->Data))[i].numGroup) > FileEnd)
			{
				break;
			}
			// Read group
			(void)!fread(((grpobj_struct*)(grpobj->Data))[i].group, sizeof(unsigned int), ((grpobj_struct*)(grpobj->Data))[i].numGroup, sourceFile);
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
* - parameter: 		element_struct pointer
*
* - return value: 	-
******************************************************************
*/
void InitGrpobj(element_struct* grpobj)
{
	if (grpobj->Length != 0 && grpobj->Data != NULL)
	{
		for (unsigned int i = 0; i < grpobj->Length; i++)
		{
			if ((((grpobj_struct*)(grpobj->Data))[i]).group != NULL)
			{
				free((((grpobj_struct*)(grpobj->Data))[i]).group);
			}
		}
		free(grpobj->Data);
		grpobj->Data = NULL;
		grpobj->Length = 0;
	}
}

/*
******************************************************************
* - function name:	GetGrpobj()
*
* - description: 	Returns the selected grpobj
*
* - parameter: 		element_struct pointer, grpobj index
*
* - return value: 	grpobj object struct
******************************************************************
*/
grpobj_struct GetGrpobj(element_struct* grpobj, unsigned int idx)
{
	if (grpobj->Data != NULL && idx <= grpobj->Length && idx >= 0)
	{
		return ((grpobj_struct*)(grpobj->Data))[idx];
	}
	return (grpobj_struct) { 0 };
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
unsigned int InsideGroup(element_struct* grpobj, uid_struct UID, unsigned int group)
{
	grpobj_struct obj;
	for(unsigned int i = 0; i < grpobj->Length; i++)
	{
		obj = ((grpobj_struct*)(grpobj->Data))[i];
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
			for (unsigned int j = 0; j < obj.numGroup; j++)
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

