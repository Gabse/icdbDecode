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
#include "circle.h"
#include <stdint.h>				// Required for int32_t, uint32_t, ...
#include <stdlib.h>				// Required for calloc to work properly
#include <stdio.h>				// Required for memcpy
#include <string.h>				// Required for memcpy
#include "../common.h"			// Required for element_struct
#include "../cdbblks/cdbblks.h"	// Required for cdbblks
#include "dxdatl.h"				// Required for keys
#include "property.h"			// Required for styles

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	ProcessCircle
*
* - description: 	Processes Circle
*
* - parameter: 		element_struct pointer
*
* - return value: 	-
******************************************************************
*/
void ProcessCircle(element_struct* property, element_struct* circle)
{
	if (Dxdatl_Circle2Style != NULL &&
		Dxdatl_CircleIDDxD != NULL &&
		Dxdatl_CirclePos != NULL &&
		Dxdatl_CircleRadius != NULL &&
		Dxdatl_CircleUID != NULL &&
		(*Dxdatl_Circle2Style).LengthCalc == (*Dxdatl_CircleIDDxD).LengthCalc &&
		(*Dxdatl_Circle2Style).LengthCalc == (*Dxdatl_CirclePos).LengthCalc &&
		(*Dxdatl_Circle2Style).LengthCalc == (*Dxdatl_CircleRadius).LengthCalc &&
		(*Dxdatl_Circle2Style).LengthCalc == (*Dxdatl_CircleUID).LengthCalc &&
		circle->Length == 0
		)
	{
		circle->Length = (*Dxdatl_Circle2Style).LengthCalc;
		circle->Data = calloc(circle->Length, sizeof(circle_struct));
		if (circle->Data == NULL)
		{
			return;
		}
		for (unsigned int i = 0; i < circle->Length; i++)
		{
			// Line Property
			(((circle_struct*)(circle->Data))[i]).Property = GetProperty(property, ((int*)(*Dxdatl_Circle2Style).Data)[i] - 1);

			// IDDXD
			(((circle_struct*)(circle->Data))[i]).IDDxD = ((int*)(*Dxdatl_CircleIDDxD).Data)[i];

			// Coordinates
			(((circle_struct*)(circle->Data))[i]).CenterCoord = ((coordinate_struct*)(*Dxdatl_CirclePos).Data)[i];
			(((circle_struct*)(circle->Data))[i]).Radius = ((int*)(*Dxdatl_CircleRadius).Data)[i];

			// UID
			memcpy(&(((circle_struct*)(circle->Data))[i]).UID, &(((char*)(*Dxdatl_CircleUID).Data)[i*8]), 8);
		}
	}
	else
	{
		myPrint("Error processing circles!\n");
	}
}

/*
******************************************************************
* - function name:	InitCircle()
*
* - description: 	Initializes Circle
*
* - parameter: 		element_struct pointer
*
* - return value: 	-
******************************************************************
*/
void InitCircle(element_struct* circle)
{
	if (circle->Length != 0 && circle->Data != NULL)
	{
		free(circle->Data);
		circle->Data = NULL;
		circle->Length = 0;
	}
}

/*
******************************************************************
* - function name:	GetCircle()
*
* - description: 	Returns the selected circle
*
* - parameter: 		element_struct pointer, circle index
*
* - return value: 	circle struct
******************************************************************
*/
circle_struct GetCircle(element_struct* circle, int idx)
{
	if (circle->Data != NULL && idx <= circle->Length && idx >= 0)
	{
		return ((circle_struct*)(circle->Data))[idx];
	}
	return (circle_struct){0};
}