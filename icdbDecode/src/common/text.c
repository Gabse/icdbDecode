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
#include "text.h"
#include <stdint.h>		// Required for int32_t, uint32_t, ...
#include <stdlib.h>		// Required for calloc to work properly
#include <stdio.h>		// Required for memcpy
#include <string.h>		// Required for memcpy
#include "../common.h"	// Required for element_struct
#include "dxdatl.h"		// Required for keys
#include "textdata.h" 	// Required for textdata struct

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	ProcessText()
*
* - description: 	Processes text
*
* - parameter: 		element_struct pointer
*
* - return value: 	-
******************************************************************
*/
void ProcessText(element_struct* text, element_struct* textdata)
{
	if (Dxdatl_Text2TextData != NULL &&
		Dxdatl_TextString != NULL &&
		Dxdatl_TextUID != NULL &&
		(*Dxdatl_Text2TextData).LengthCalc == (*Dxdatl_TextString).LengthCalc &&
		(*Dxdatl_Text2TextData).LengthCalc == (*Dxdatl_TextUID).LengthCalc &&
		text->Length == 0
		)
	{
		text->Length = (*Dxdatl_Text2TextData).LengthCalc;
		text->Data = calloc(text->Length, sizeof(text_struct));
		if (text->Data == NULL)
		{
			return;
		}
		for (unsigned int i = 0; i < text->Length; i++)
		{
			// String
			(((text_struct*)(text->Data))[i]).String = CopyString(((string_struct*)(*Dxdatl_TextString).Data)[i]);

			// UID
			memcpy(&(((text_struct*)(text->Data))[i]).UID, &(((char*)(*Dxdatl_TextUID).Data)[i*8]), 8);

			// Text formating
			(((text_struct*)(text->Data))[i]).TextData = GetTextdata(textdata, ((int*)(*Dxdatl_Text2TextData).Data)[i] - 1);
		}
	}
	else
	{
		myPrint("Error processing texts!\n");
	}
}

/*
******************************************************************
* - function name:	InitText()
*
* - description: 	Initializes text
*
* - parameter: 		element_struct pointer
*
* - return value: 	-
******************************************************************
*/
void InitText(element_struct* text)
{
	if (text->Length != 0 && text->Data != NULL)
	{
		for(unsigned int i = 0; i< text->Length; i++)
		{
			free((((text_struct*)(text->Data))[i]).String.Text);
		}
		free(text->Data);
		text->Data = NULL;
		text->Length = 0;
	}
}

/*
******************************************************************
* - function name:	GetText()
*
* - description: 	Returns the selected text
*
* - parameter: 		element_struct pointer, text index
*
* - return value: 	text struct
******************************************************************
*/
text_struct GetText(element_struct* text, int idx)
{
	if (text->Data != NULL && idx <= text->Length && idx >= 0)	
	{
		return ((text_struct*)(text->Data))[idx];
	}
	return (text_struct){0};
}