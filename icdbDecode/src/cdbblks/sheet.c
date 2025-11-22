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
#include "sheet.h"
#include <stdint.h>					// Required for int32_t, uint32_t, ...
#include <stdlib.h>					// Required for calloc to work properly
#include <stdio.h>					// Required for memcpy
#include <string.h>					// Required for memcpy
#include "../common.h"				// Required for element_struct
#include "../common/dxdatl.h"		// Required for keys
#include "../common/rectangle.h"	// Required for rectcoord struct

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	ProcessSheet()
*
* - description: 	Processes sheet
*
* - parameter: 		element_struct pointer
*
* - return value: 	-
******************************************************************
*/
void ProcessSheet(element_struct* sheet)
{
	if (Dxdatl_Block2Grp != NULL &&
		Dxdatl_BlockBBox != NULL &&
		Dxdatl_BlockSize != NULL &&
		(*Dxdatl_Block2Grp).LengthCalc == (*Dxdatl_BlockBBox).LengthCalc &&
		(*Dxdatl_Block2Grp).LengthCalc == (*Dxdatl_BlockSize).LengthCalc &&
		sheet->Length == 0		
		)
	{
		sheet->Length = (*Dxdatl_Block2Grp).LengthCalc;
		sheet->Data = calloc(sheet->Length, sizeof(sheet_struct));
		if (sheet->Data == NULL)
		{
			return;
		}
		for (unsigned int i = 0; i < sheet->Length; i++)
		{
			// Sheet size custom
			(((sheet_struct*)(sheet->Data))[i]).Size.X = ((rectcoord_struct*)(*Dxdatl_BlockBBox).Data)[i].EndCoord.X - ((rectcoord_struct*)(*Dxdatl_BlockBBox).Data)[i].StartCoord.X;
			(((sheet_struct*)(sheet->Data))[i]).Size.Y = ((rectcoord_struct*)(*Dxdatl_BlockBBox).Data)[i].EndCoord.Y - ((rectcoord_struct*)(*Dxdatl_BlockBBox).Data)[i].StartCoord.Y;

			// Sheet Size presets
			(((sheet_struct*)(sheet->Data))[i]).SizeCode = ((sheetsize_type*)(*Dxdatl_BlockSize).Data)[i];

			// Sheet Group
			(((sheet_struct*)(sheet->Data))[i]).Group = ((int*)(*Dxdatl_Block2Grp).Data)[i];

			// Landscape/Portrait
			if((((sheet_struct*)(sheet->Data))[i]).SizeCode == sheetsize_Custom || (((sheet_struct*)(sheet->Data))[i]).Size.X == (((sheet_struct*)(sheet->Data))[i]).Size.Y)
			{
				(((sheet_struct*)(sheet->Data))[i]).Orientation = sheetorientation_Custom;
			}
			else if ((((sheet_struct*)(sheet->Data))[i]).Size.X > (((sheet_struct*)(sheet->Data))[i]).Size.Y)
			{
				(((sheet_struct*)(sheet->Data))[i]).Orientation = sheetorientation_Landscape;
			}
			else
			{
				(((sheet_struct*)(sheet->Data))[i]).Orientation = sheetorientation_Portrait;
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
* - function name:	InitSheet()
*
* - description: 	Initializes sheet
*
* - parameter: 		element_struct pointer
*
* - return value: 	-
******************************************************************
*/
void InitSheet(element_struct* sheet)
{
	if (sheet->Length != 0 && sheet->Data != NULL)
	{
		free(sheet->Data);
		sheet->Data = NULL;
		sheet->Length = 0;
	}
}

/*
******************************************************************
* - function name:	GetSheet()
*
* - description: 	Returns the selected sheet
*
* - parameter: 		element_struct pointer, sheet index in dxd format (Starting from 1)
*
* - return value: 	sheet struct
******************************************************************
*/
sheet_struct GetSheet(element_struct* sheet, int idx)
{
	if (sheet->Data != NULL && idx <= sheet->Length && idx >= 0)
	{
		return ((sheet_struct*)(sheet->Data))[idx];
	}
	return (sheet_struct){0};
}