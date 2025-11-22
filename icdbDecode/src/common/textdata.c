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
#include "textdata.h"
#include <stdint.h>		// Required for int32_t, uint32_t, ...
#include <stdlib.h>		// Required for calloc to work properly
#include <stdio.h>		// Required for memcpy
#include <string.h>		// Required for memcpy
#include "dxdatl.h"		// Required for keys
#include "property.h"	// Required for property
#include "../common.h"	// Required for parseFile

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	ProcessTextdata()
*
* - description: 	Processes textdata
*
* - parameter: 		element_struct pointer
*
* - return value: 	-
******************************************************************
*/
void ProcessTextdata(element_struct* textdata)
{
	// ToDo: Check what IDDXD is used for, it sometimes mismatches with other elements

	if (Dxdatl_TextColor != NULL &&
		Dxdatl_TextFont != NULL &&
		//Dxdatl_TextIDDxD != NULL &&
		Dxdatl_TextOrient != NULL &&
		Dxdatl_TextOrigin != NULL &&
		Dxdatl_TextPos != NULL &&
		Dxdatl_TextSize != NULL &&
		(*Dxdatl_TextColor).LengthCalc == (*Dxdatl_TextFont).LengthCalc &&
		//(*Dxdatl_TextColor).LengthCalc == (*Dxdatl_TextIDDxD).LengthCalc &&
		(*Dxdatl_TextColor).LengthCalc == (*Dxdatl_TextOrient).LengthCalc &&
		(*Dxdatl_TextColor).LengthCalc == (*Dxdatl_TextOrigin).LengthCalc &&
		(*Dxdatl_TextColor).LengthCalc == (*Dxdatl_TextPos).LengthCalc &&
		(*Dxdatl_TextColor).LengthCalc == (*Dxdatl_TextSize).LengthCalc &&
		textdata->Length == 0
		)
	{
		textdata->Length = (*Dxdatl_TextColor).LengthCalc;
		textdata->Data = calloc(textdata->Length, sizeof(textdata_struct));
		if (textdata == NULL)
		{
			return;
		}
		for (unsigned int i = 0; i < textdata->Length; i++)
		{
			// Line Color
			(((textdata_struct*)(textdata->Data))[i]).LineColor = ((color_struct*)(*Dxdatl_TextColor).Data)[i];

			// Fill Color
			if (Dxdatl_TextColorExt != NULL && textdata->Length == (*Dxdatl_TextColorExt).LengthCalc) // Newer Versions have separate fill and border color
			{
				(((textdata_struct*)(textdata->Data))[i]).FillColor = ((color_struct*)(*Dxdatl_TextColorExt).Data)[i];
			}
			else
			{
				(((textdata_struct*)(textdata->Data))[i]).FillColor = (((textdata_struct*)(textdata->Data))[i]).LineColor;
			}

			// Font
			if (((string_struct*)(*Dxdatl_TextFont).Data)[i].Length == 0) // not visible
			{
				(((textdata_struct*)(textdata->Data))[i]).Font.Font = font_None;
			}
			else if (((string_struct*)(*Dxdatl_TextFont).Data)[i].Length == 1) // 1 digit font code
			{
				(((textdata_struct*)(textdata->Data))[i]).Font.Font = ((string_struct*)(*Dxdatl_TextFont).Data)[i].Text[0] - '0';
			}
			else if (((string_struct*)(*Dxdatl_TextFont).Data)[i].Length == 2) // 2 digit font code
			{
				(((textdata_struct*)(textdata->Data))[i]).Font.Font = (((string_struct*)(*Dxdatl_TextFont).Data)[i].Text[0] - '0') * 10;
				(((textdata_struct*)(textdata->Data))[i]).Font.Font += ((string_struct*)(*Dxdatl_TextFont).Data)[i].Text[1] - '0';
			}
			else // custom font
			{
				(((textdata_struct*)(textdata->Data))[i]).Font.Font = font_Custom;
				uint32_t FontNameStart = strchr(((string_struct*)(*Dxdatl_TextFont).Data)[i].Text, '|') - ((string_struct*)(*Dxdatl_TextFont).Data)[i].Text + 1; // get index of Font name
				(((textdata_struct*)(textdata->Data))[i]).Font.FontNameLen = strchr(((string_struct*)(*Dxdatl_TextFont).Data)[i].Text + FontNameStart, '|') - ((string_struct*)(*Dxdatl_TextFont).Data)[i].Text - FontNameStart;
				(((textdata_struct*)(textdata->Data))[i]).Font.CustomFont = calloc((((textdata_struct*)(textdata->Data))[i]).Font.FontNameLen + 1, sizeof(char));
				if ((((textdata_struct*)(textdata->Data))[i]).Font.CustomFont == NULL) // Custom Font
				{
					return;
				}
				memcpy((((textdata_struct*)(textdata->Data))[i]).Font.CustomFont, ((string_struct*)(*Dxdatl_TextFont).Data)[i].Text + FontNameStart, (((textdata_struct*)(textdata->Data))[i]).Font.FontNameLen);
				(((textdata_struct*)(textdata->Data))[i]).Font.CustomFont[(((textdata_struct*)(textdata->Data))[i]).Font.FontNameLen] = '\0'; // Zero terminate
				(((textdata_struct*)(textdata->Data))[i]).Font.unknown2 = ((string_struct*)(*Dxdatl_TextFont).Data)[i].Text[(((textdata_struct*)(textdata->Data))[i]).Font.FontNameLen + 3] - '0';
				(((textdata_struct*)(textdata->Data))[i]).Font.unknown3 = ((string_struct*)(*Dxdatl_TextFont).Data)[i].Text[(((textdata_struct*)(textdata->Data))[i]).Font.FontNameLen + 5] - '0';
				(((textdata_struct*)(textdata->Data))[i]).Font.Strikeout = ((string_struct*)(*Dxdatl_TextFont).Data)[i].Text[(((textdata_struct*)(textdata->Data))[i]).Font.FontNameLen + 7] - '0';
				(((textdata_struct*)(textdata->Data))[i]).Font.Underline = ((string_struct*)(*Dxdatl_TextFont).Data)[i].Text[(((textdata_struct*)(textdata->Data))[i]).Font.FontNameLen + 9] - '0';
				(((textdata_struct*)(textdata->Data))[i]).Font.Bold = ((string_struct*)(*Dxdatl_TextFont).Data)[i].Text[(((textdata_struct*)(textdata->Data))[i]).Font.FontNameLen + 11] - '0';
				(((textdata_struct*)(textdata->Data))[i]).Font.unknown4 = ((string_struct*)(*Dxdatl_TextFont).Data)[i].Text[(((textdata_struct*)(textdata->Data))[i]).Font.FontNameLen + 13] - '0';
				(((textdata_struct*)(textdata->Data))[i]).Font.unknown5 = ((string_struct*)(*Dxdatl_TextFont).Data)[i].Text[(((textdata_struct*)(textdata->Data))[i]).Font.FontNameLen + 15] - '0';
			}

			// IDDXD
			//(((textdata_struct*)(textdata->Data))[i]).IDDxD = ((int*)(*Dxdatl_TextIDDxD).Data)[i];

			// Orientation
			(((textdata_struct*)(textdata->Data))[i]).Orientation = ((textrotation_type*)(*Dxdatl_TextOrient).Data)[i];

			// Origin
			(((textdata_struct*)(textdata->Data))[i]).Origin = ((textorigin_type*)(*Dxdatl_TextOrigin).Data)[i];

			// Coordinate
			(((textdata_struct*)(textdata->Data))[i]).Position = ((coordinate_struct*)(*Dxdatl_TextPos).Data)[i];

			// Size
			(((textdata_struct*)(textdata->Data))[i]).Size = ((int*)(*Dxdatl_TextSize).Data)[i];			
		}
	}
	else
	{
		myPrint("Error processing text data!\n");
	}
}

/*
******************************************************************
* - function name:	InitTextdata()
*
* - description: 	Initializes textdata
*
* - parameter: 		element_struct pointer
*
* - return value: 	-
******************************************************************
*/
void InitTextdata(element_struct* textdata)
{
	if (textdata->Length != 0 && textdata->Data != NULL)
	{
		for(unsigned int i = 0; i< textdata->Length; i++)
		if ((((textdata_struct*)(textdata->Data))[i]).Font.CustomFont != NULL)
		{
			free((((textdata_struct*)(textdata->Data))[i]).Font.CustomFont);
		}
		free(textdata->Data);
		textdata->Data = NULL;
		textdata->Length = 0;
	}
}

/*
******************************************************************
* - function name:	GetTextdata()
*
* - description: 	Returns the selected textdata
*
* - parameter: 		element_struct pointer, textdata index
*
* - return value: 	textdata struct
******************************************************************
*/
textdata_struct GetTextdata(element_struct* textdata, int idx)
{
	if (textdata->Data != NULL && idx <= textdata->Length && idx >= 0)
	{
		return ((textdata_struct*)(textdata->Data))[idx];
	}
	return (textdata_struct){0};
}