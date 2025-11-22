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
#include "page.h"
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
* - function name:	ProcessPage()
*
* - description: 	Processes page
*
* - parameter: 		element_struct pointer
*
* - return value: 	-
******************************************************************
*/
void ProcessPage(element_struct* page)
{
	// This is probably wrong, but it seams to work good enough for now..., but still:
	// ToDo: Rework page->Data
	if (Catlgatl_BlkUID != NULL &&
		Catlgatl_MdlNam != NULL &&
		(*Catlgatl_BlkUID).LengthCalc == (*Catlgatl_MdlNam).LengthCalc &&
		page->Length == 0
		)
	{
		page->Length = (*Catlgatl_BlkUID).LengthCalc;
		page->Data = calloc(page->Length, sizeof(page_struct));
		if (page->Data == NULL)
		{
			return;
		}
		for (unsigned int i = 0; i < page->Length; i++)
		{
			// Name
			(((page_struct*)(page->Data))[i]).Name = CopyString(((string_struct*)(*Catlgatl_MdlNam).Data)[i]);

			// UID
			memcpy(&(((page_struct*)(page->Data))[i]).UID, &(((char*)(*Catlgatl_BlkUID).Data)[i * 8]), 8);
		}
	}
	else
	{
		myPrint("Error processing pages!\n");
	}
}

/*
******************************************************************
* - function name:	InitPage()
*
* - description: 	Initializes page
*
* - parameter: 		element_struct pointer
*
* - return value: 	-
******************************************************************
*/
void InitPage(element_struct* page)
{
	if (page->Length != 0 && page->Data != NULL)
	{
		for(unsigned int i = 0; i< page->Length; i++)
		if ((((page_struct*)(page->Data))[i]).Name.Text != NULL)
		{
			free((((page_struct*)(page->Data))[i]).Name.Text);
		}
		free(page->Data);
		page->Data = NULL;
		page->Length = 0;
	}
}

/*
******************************************************************
* - function name:	GetPage()
*
* - description: 	Returns the selected page
*
* - parameter: 		element_struct pointer, page index
*
* - return value: 	page struct
******************************************************************
*/
page_struct GetPage(element_struct* page, unsigned int idx)
{
	if (page->Data != NULL && idx <= page->Length && idx >= 0)
	{
		return ((page_struct*)(page->Data))[idx];
	}
	return (page_struct) { 0 };
}

