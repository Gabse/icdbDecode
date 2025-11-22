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
#include "rectangle.h"
#include <stdint.h>				// Required for int32_t, uint32_t, ...
#include <stdlib.h>				// Required for calloc to work properly
#include <stdio.h>				// Required for memcpy
#include <string.h>				// Required for memcpy
#include "../common.h"			// Required for element_struct
#include "../cdbblks/cdbblks.h"	// Required for cdbblks
#include "dxdatl.h"				// Required for keys
#include "property.h" 			// Required for property

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	ProcessRectangle()
*
* - description: 	Processes rectangle
*
* - parameter: 		element_struct pointer
*
* - return value: 	-
******************************************************************
*/
void ProcessRectangle(element_struct* property, element_struct* rectangle)
{
	if (Dxdatl_Rect2Style != NULL &&
		Dxdatl_RectCoord != NULL &&
		Dxdatl_RectIDDxD != NULL &&
		Dxdatl_RectUID != NULL &&
		(*Dxdatl_RectCoord).LengthCalc == (*Dxdatl_Rect2Style).LengthCalc &&
		(*Dxdatl_RectCoord).LengthCalc == (*Dxdatl_RectIDDxD).LengthCalc &&
		(*Dxdatl_RectCoord).LengthCalc == (*Dxdatl_RectUID).LengthCalc &&
		rectangle->Length == 0
		)
	{
		rectangle->Length = (*Dxdatl_RectCoord).LengthCalc;
		rectangle->Data = calloc(rectangle->Length, sizeof(rectangle_struct));
		if (rectangle->Data == NULL)
		{
			return;
		}
		for (unsigned int i = 0; i < rectangle->Length; i++)
		{
			// Line Property
			(((rectangle_struct*)(rectangle->Data))[i]).Property = GetProperty(property, ((int*)(*Dxdatl_Rect2Style).Data)[i] - 1);

			// Start/End Coordinate
			(((rectangle_struct*)(rectangle->Data))[i]).EndCoord = ((rectcoord_struct*)(*Dxdatl_RectCoord).Data)[i].EndCoord;
			(((rectangle_struct*)(rectangle->Data))[i]).StartCoord = ((rectcoord_struct*)(*Dxdatl_RectCoord).Data)[i].StartCoord;

			// IDDXD
			(((rectangle_struct*)(rectangle->Data))[i]).IDDxD = ((int*)(*Dxdatl_RectIDDxD).Data)[i];

			// UID
			memcpy(&(((rectangle_struct*)(rectangle->Data))[i]).UID, &(((char*)(*Dxdatl_RectUID).Data)[i*8]), 8);
		}
	}
	else
	{
		myPrint("Error processing rectangles!\n");
	}
}

/*
******************************************************************
* - function name:	InitRectangle()
*
* - description: 	Initializes rectangle
*
* - parameter: 		element_struct pointer
*
* - return value: 	-
******************************************************************
*/
void InitRectangle(element_struct* rectangle)
{
	if (rectangle->Length != 0 && rectangle->Data != NULL)
	{
		free(rectangle->Data);
		rectangle->Data = NULL;
		rectangle->Length = 0;
	}
}

/*
******************************************************************
* - function name:	GetRectangle()
*
* - description: 	Returns the selected rectangle
*
* - parameter: 		element_struct pointer, rectangle index
*
* - return value: 	rectangle struct
******************************************************************
*/
rectangle_struct GetRectangle(element_struct* rectangle, int idx)
{
	if (rectangle->Data != NULL && idx <= rectangle->Length && idx >= 0)
	{
		return ((rectangle_struct*)(rectangle->Data))[idx];
	}
	return (rectangle_struct){0};
}