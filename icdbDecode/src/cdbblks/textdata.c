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
#include <stdint.h>					// Required for int32_t, uint32_t, ...
#include <stdlib.h>					// Required for calloc to work properly
#include <stdio.h>					// Required for memcpy
#include <string.h>					// Required for memcpy
#include "dxdatl.h"					// Required for keys
#include "properties.h" 			// Required for properties
#include "cdbblks.h" 				// Required for coordinate struct
#include "../common.h"				// Required for parseFile

/*
******************************************************************
* Global Variables
******************************************************************
*/
unsigned int textdata_num = 0;
textdata_struct* textdatas = NULL;

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	ProcessTextdatas()
*
* - description: 	Processes textdatas
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void ProcessTextdatas(void)
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
		textdata_num == 0 
		)
	{
		textdata_num = (*Dxdatl_TextColor).LengthCalc;
		textdatas = calloc(textdata_num, sizeof(textdata_struct));
		if (textdatas == NULL)
		{
			return;
		}
		for (unsigned int i = 0; i < textdata_num; i++)
		{
			// Line Color
			(textdatas[i]).LineColor = ((color_struct*)(*Dxdatl_TextColor).Data)[i];

			// Fill Color
			if (Dxdatl_TextColorExt != NULL && textdata_num == (*Dxdatl_TextColorExt).LengthCalc) // Newer Versions have separate fill and border color
			{
				(textdatas[i]).FillColor = ((color_struct*)(*Dxdatl_TextColorExt).Data)[i];
			}
			else
			{
				(textdatas[i]).FillColor = (textdatas[i]).LineColor;
			}

			// Font
			if (((string_struct*)(*Dxdatl_TextFont).Data)[i].Length == 0) // not visible
			{
				(textdatas[i]).Font.Font = font_None;
			}
			else if (((string_struct*)(*Dxdatl_TextFont).Data)[i].Length == 1) // 1 digit font code
			{
				(textdatas[i]).Font.Font = ((string_struct*)(*Dxdatl_TextFont).Data)[i].Text[0] - '0';
			}
			else if (((string_struct*)(*Dxdatl_TextFont).Data)[i].Length == 2) // 2 digit font code
			{
				(textdatas[i]).Font.Font = (((string_struct*)(*Dxdatl_TextFont).Data)[i].Text[0] - '0') * 10;
				(textdatas[i]).Font.Font += ((string_struct*)(*Dxdatl_TextFont).Data)[i].Text[1] - '0';
			}
			else // custom font
			{
				(textdatas[i]).Font.Font = font_Custom;
				uint32_t FontNameStart = strchr(((string_struct*)(*Dxdatl_TextFont).Data)[i].Text, '|') - ((string_struct*)(*Dxdatl_TextFont).Data)[i].Text + 1; // get index of Font name
				(textdatas[i]).Font.FontNameLen = strchr(((string_struct*)(*Dxdatl_TextFont).Data)[i].Text + FontNameStart, '|') - ((string_struct*)(*Dxdatl_TextFont).Data)[i].Text - FontNameStart;
				(textdatas[i]).Font.CustomFont = calloc((textdatas[i]).Font.FontNameLen + 1, sizeof(char));
				if ((textdatas[i]).Font.CustomFont == NULL) // Custom Font
				{
					return;
				}
				memcpy((textdatas[i]).Font.CustomFont, ((string_struct*)(*Dxdatl_TextFont).Data)[i].Text + FontNameStart, (textdatas[i]).Font.FontNameLen);
				(textdatas[i]).Font.CustomFont[(textdatas[i]).Font.FontNameLen] = '\0'; // Zero terminate
				(textdatas[i]).Font.unknown2 = ((string_struct*)(*Dxdatl_TextFont).Data)[i].Text[(textdatas[i]).Font.FontNameLen + 3] - '0';
				(textdatas[i]).Font.unknown3 = ((string_struct*)(*Dxdatl_TextFont).Data)[i].Text[(textdatas[i]).Font.FontNameLen + 5] - '0';
				(textdatas[i]).Font.Strikeout = ((string_struct*)(*Dxdatl_TextFont).Data)[i].Text[(textdatas[i]).Font.FontNameLen + 7] - '0';
				(textdatas[i]).Font.Underline = ((string_struct*)(*Dxdatl_TextFont).Data)[i].Text[(textdatas[i]).Font.FontNameLen + 9] - '0';
				(textdatas[i]).Font.Bold = ((string_struct*)(*Dxdatl_TextFont).Data)[i].Text[(textdatas[i]).Font.FontNameLen + 11] - '0';
				(textdatas[i]).Font.unknown4 = ((string_struct*)(*Dxdatl_TextFont).Data)[i].Text[(textdatas[i]).Font.FontNameLen + 13] - '0';
				(textdatas[i]).Font.unknown5 = ((string_struct*)(*Dxdatl_TextFont).Data)[i].Text[(textdatas[i]).Font.FontNameLen + 15] - '0';
			}

			// IDDXD
			//(textdatas[i]).IDDxD = ((int*)(*Dxdatl_TextIDDxD).Data)[i];

			// Orientation
			(textdatas[i]).Orientation = ((textrotation_type*)(*Dxdatl_TextOrient).Data)[i];

			// Origin
			(textdatas[i]).Origin = ((textorigin_type*)(*Dxdatl_TextOrigin).Data)[i];

			// Coordinate
			(textdatas[i]).Position = ((coordinate_struct*)(*Dxdatl_TextPos).Data)[i];

			// Size
			(textdatas[i]).Size = ((int*)(*Dxdatl_TextSize).Data)[i];			
		}
	}
	else
	{
		myPrint("Error processing text data!\n");
	}
}

/*
******************************************************************
* - function name:	InitTextdatas()
*
* - description: 	Initializes textdatas
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void InitTextdatas(void)
{
	if (textdata_num != 0 && textdatas != NULL)
	{
		for(unsigned int i = 0; i< textdata_num; i++)
		if ((textdatas[i]).Font.CustomFont != NULL)
		{
			free((textdatas[i]).Font.CustomFont);
		}
		free(textdatas);
		textdata_num = 0;
	}
}

/*
******************************************************************
* - function name:	GetNumTextdatas()
*
* - description: 	Returns the number of textdatas in this file
*
* - parameter: 		-
*
* - return value: 	number of textdatas in this file
******************************************************************
*/
unsigned int GetNumTextdatas(void)
{
	return textdata_num;
}

/*
******************************************************************
* - function name:	GetTextdata()
*
* - description: 	Returns the selected textdata
*
* - parameter: 		textdata index
*
* - return value: 	textdata struct
******************************************************************
*/
textdata_struct GetTextdata(int idx)
{
	if (textdatas != NULL && idx <= textdata_num && idx >= 0)
	{
		return textdatas[idx];
	}
	return (textdata_struct){0};
}