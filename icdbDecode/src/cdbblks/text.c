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
#include <stdint.h>					// Required for int32_t, uint32_t, ...
#include <stdlib.h>					// Required for calloc to work properly
#include <stdio.h>					// Required for memcpy
#include <string.h>					// Required for memcpy
#include "dxdatl.h"					// Required for keys
#include "textdata.h" 				// Required for textdata struct

/*
******************************************************************
* Global Variables
******************************************************************
*/
unsigned int text_num = 0;
text_struct* texts = NULL;

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	ProcessTexts()
*
* - description: 	Processes texts
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void ProcessTexts(void)
{
	if (Dxdatl_Text2TextData != NULL && 
		Dxdatl_TextString != NULL &&
		Dxdatl_TextUID != NULL &&
		(*Dxdatl_Text2TextData).LengthCalc == (*Dxdatl_TextString).LengthCalc &&
		(*Dxdatl_Text2TextData).LengthCalc == (*Dxdatl_TextUID).LengthCalc &&
		text_num == 0 
		)
	{
		text_num = (*Dxdatl_Text2TextData).LengthCalc;
		texts = calloc(text_num, sizeof(text_struct));
		if (texts == NULL)
		{
			return;
		}
		for (unsigned int i = 0; i < text_num; i++)
		{
			// String
			(texts[i]).String = CopyString(((string_struct*)(*Dxdatl_TextString).Data)[i]);

			// UID
			memcpy(&(texts[i]).UID, &(((char*)(*Dxdatl_TextUID).Data)[i*8]), 8);

			// Text formating
			(texts[i]).TextData = GetTextdata(((int*)(*Dxdatl_Text2TextData).Data)[i] - 1);
		}
	}
	else
	{
		myPrint("Error processing texts!\n");
	}
}

/*
******************************************************************
* - function name:	InitTexts()
*
* - description: 	Initializes texts
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void InitTexts(void)
{
	if (text_num != 0 && texts != NULL)
	{
		for(unsigned int i = 0; i< text_num; i++)
		{
			free((texts[i]).String.Text);
		}
		free(texts);
		text_num = 0;
	}
}

/*
******************************************************************
* - function name:	GetNumTexts()
*
* - description: 	Returns the number of texts in this file
*
* - parameter: 		-
*
* - return value: 	number of texts in this file
******************************************************************
*/
unsigned int GetNumTexts(void)
{
	return text_num;
}

/*
******************************************************************
* - function name:	GetText()
*
* - description: 	Returns the selected text
*
* - parameter: 		text index
*
* - return value: 	text struct
******************************************************************
*/
text_struct GetText(int idx)
{
	if (texts != NULL && idx <= text_num && idx >= 0)	
	{
		return texts[idx];
	}
	return (text_struct){0};
}