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
#include "sheets.h"
#include <stdint.h>					// Required for int32_t, uint32_t, ...
#include <stdlib.h>					// Required for calloc to work properly
#include <stdio.h>					// Required for memcpy
#include <string.h>					// Required for memcpy
#include "dxdatl.h"					// Required for keys
#include "rectangles.h"				// Required for rectcoord struct

/*
******************************************************************
* Defines
******************************************************************
*/

/*
******************************************************************
* Global Variables
******************************************************************
*/
unsigned int sheet_num = 0;
sheet_struct* sheets = NULL;

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	ProcessSheets()
*
* - description: 	Processes sheets
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void ProcessSheets(void)
{
	if (Dxdatl_Block2Grp != NULL &&
		Dxdatl_BlockBBox != NULL &&
		Dxdatl_BlockSize != NULL &&
		(*Dxdatl_Block2Grp).LengthCalc == (*Dxdatl_BlockBBox).LengthCalc &&
		(*Dxdatl_Block2Grp).LengthCalc == (*Dxdatl_BlockSize).LengthCalc &&
		sheet_num == 0		
		)
	{
		sheet_num = (*Dxdatl_Block2Grp).LengthCalc;
		sheets = calloc(sheet_num, sizeof(sheet_struct));
		if (sheets == NULL)
		{
			return;
		}
		for (unsigned int i = 0; i < sheet_num; i++)
		{
			// Sheet size custom
			(sheets[i]).Size.X = ((rectcoord_struct*)(*Dxdatl_BlockBBox).Data)[i].EndCoord.X - ((rectcoord_struct*)(*Dxdatl_BlockBBox).Data)[i].StartCoord.X;
			(sheets[i]).Size.Y = ((rectcoord_struct*)(*Dxdatl_BlockBBox).Data)[i].EndCoord.Y - ((rectcoord_struct*)(*Dxdatl_BlockBBox).Data)[i].StartCoord.Y;

			// Sheet Size presets
			(sheets[i]).SizeCode = ((sheetsize_type*)(*Dxdatl_BlockSize).Data)[i];

			// Sheet Group
			(sheets[i]).Group = ((int*)(*Dxdatl_Block2Grp).Data)[i];

			// Landscape/Portrait
			if((sheets[i]).SizeCode == sheetsize_Custom || (sheets[i]).Size.X == (sheets[i]).Size.Y)
			{
				(sheets[i]).Orientation = sheetorientation_Custom;
			}
			else if ((sheets[i]).Size.X > (sheets[i]).Size.Y)
			{
				(sheets[i]).Orientation = sheetorientation_Landscape;
			}
			else
			{
				(sheets[i]).Orientation = sheetorientation_Portrait;
			}
		}
	}
	else
	{
		myPrint("Error processing sheets!\n");
	}
}

/*
******************************************************************
* - function name:	InitSheets()
*
* - description: 	Initializes sheets
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void InitSheets(void)
{
	if (sheet_num != 0 && sheets != NULL)
	{
		free(sheets);
		sheet_num = 0;
	}
}
/*
******************************************************************
* - function name:	GetNumSheets()
*
* - description: 	Returns the number of sheets in this file
*
* - parameter: 		-
*
* - return value: 	number of sheets in this file
******************************************************************
*/
unsigned int GetNumSheets(void)
{
	return sheet_num;
}

/*
******************************************************************
* - function name:	GetSheet()
*
* - description: 	Returns the selected sheet
*
* - parameter: 		sheet index in dxd format (Starting from 1)
*
* - return value: 	sheet struct
******************************************************************
*/
sheet_struct GetSheet(int idx)
{
	if (sheets != NULL && idx <= sheet_num && idx >= 0)
	{
		return sheets[idx];
	}
	return (sheet_struct){0};
}