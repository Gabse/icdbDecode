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
#include "pages.h"
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
unsigned int page_num = 0;
page_struct* pages = NULL;

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	ProcessPages()
*
* - description: 	Processes pages
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void ProcessPages(void)
{
	// This is probably wrong, but it seams to work good enough for now..., but still:
	// ToDo: Rework pages
	if (Catlgatl_BlkUID != NULL && 
		Catlgatl_MdlNam != NULL && 
		(*Catlgatl_BlkUID).LengthCalc == (*Catlgatl_MdlNam).LengthCalc &&
		page_num == 0 
		)
	{
		page_num = (*Catlgatl_BlkUID).LengthCalc;
		pages = calloc(page_num, sizeof(page_struct));
		if (pages == NULL)
		{
			return;
		}
		for (unsigned int i = 0; i < page_num; i++)
		{
			// Name
			(pages[i]).Name = CopyString(((string_struct*)(*Catlgatl_MdlNam).Data)[i]);

			// UID
			memcpy(&(pages[i]).UID, &(((char*)(*Catlgatl_BlkUID).Data)[i * 8]), 8);
		}
	}
	else
	{
		myPrint("Error processing pages!\n");
	}
}

/*
******************************************************************
* - function name:	InitPages()
*
* - description: 	Initializes pages
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void InitPages(void)
{
	if (page_num != 0 && pages != NULL)
	{
		for(unsigned int i = 0; i< page_num; i++)
		if ((pages[i]).Name.Text != NULL)
		{
			free((pages[i]).Name.Text);
		}
		free(pages);
		page_num = 0;
	}
}

/*
******************************************************************
* - function name:	GetNumPages()
*
* - description: 	Returns the number of pages
*
* - parameter: 		-
*
* - return value: 	number of graphical pages
******************************************************************
*/
unsigned int GetNumPages(void)
{
	return page_num;
}

/*
******************************************************************
* - function name:	GetPage()
*
* - description: 	Returns the selected page
*
* - parameter: 		page index
*
* - return value: 	page struct
******************************************************************
*/
page_struct GetPage(unsigned int idx)
{
	page_struct page = { 0 };
	if (page_num > idx)
	{
		page = pages[idx];
	}
	return page;
}