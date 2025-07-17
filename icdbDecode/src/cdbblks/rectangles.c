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
* No static or dynamic code analysis of any proprietary executable files was used to gain information’s about the file format.
*
* This project uses the Zlib library (https://www.zlib.net/) for decompression.
*/

/*
******************************************************************
* Includes
******************************************************************
*/
#include "rectangles.h"
#include <stdint.h>					// Required for int32_t, uint32_t, ...
#include <stdlib.h>					// Required for calloc to work properly
#include <stdio.h>					// Required for memcpy
#include <string.h>					// Required for memcpy
#include "dxdatl.h"					// Required for keys
#include "properties.h" 			// Required for properties
#include "cdbblks.h" 				// Required for coordinate struct


/*
******************************************************************
* Global Variables
******************************************************************
*/
unsigned int rectangle_num = 0;
rectangle_struct* rectangles = NULL;

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	ProcessRectangles()
*
* - description: 	Processes rectangles
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void ProcessRectangles(void)
{
	if (Dxdatl_Rect2Style != NULL && 
		Dxdatl_RectCoord != NULL &&
		Dxdatl_RectIDDxD != NULL &&
		Dxdatl_RectUID != NULL &&
		(*Dxdatl_RectCoord).LengthCalc == (*Dxdatl_Rect2Style).LengthCalc &&
		(*Dxdatl_RectCoord).LengthCalc == (*Dxdatl_RectIDDxD).LengthCalc &&
		(*Dxdatl_RectCoord).LengthCalc == (*Dxdatl_RectUID).LengthCalc &&
		rectangle_num == 0 
		)
	{
		rectangle_num = (*Dxdatl_RectCoord).LengthCalc;
		rectangles = calloc(rectangle_num, sizeof(rectangle_struct));
		if (rectangles == NULL)
		{
			return;
		}
		for (unsigned int i = 0; i < rectangle_num; i++)
		{
			// Line Properties
			(rectangles[i]).Properties = GetProperty(((int*)(*Dxdatl_Rect2Style).Data)[i] - 1);

			// Start/End Coordinate
			(rectangles[i]).EndCoord = ((rectcoord_struct*)(*Dxdatl_RectCoord).Data)[i].EndCoord;
			(rectangles[i]).StartCoord = ((rectcoord_struct*)(*Dxdatl_RectCoord).Data)[i].StartCoord;

			// IDDXD
			(rectangles[i]).IDDxD = ((int*)(*Dxdatl_RectIDDxD).Data)[i];

			// UID
			memcpy(&(rectangles[i]).UID, &(((char*)(*Dxdatl_RectUID).Data)[i*8]), 8);
		}
	}
	else
	{
		myPrint("Error processing rectangles!\n");
	}
}

/*
******************************************************************
* - function name:	InitRectangles()
*
* - description: 	Initializes rectangles
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void InitRectangles(void)
{
	if (rectangle_num != 0 && rectangles != NULL)
	{
		free(rectangles);
		rectangle_num = 0;
	}
}

/*
******************************************************************
* - function name:	GetNumRectangles()
*
* - description: 	Returns the number of rectangles in this file
*
* - parameter: 		-
*
* - return value: 	number of rectangles in this file
******************************************************************
*/
unsigned int GetNumRectangles(void)
{
	return rectangle_num;
}

/*
******************************************************************
* - function name:	GetRectangle()
*
* - description: 	Returns the selected rectangle
*
* - parameter: 		rectangle index
*
* - return value: 	rectangle struct
******************************************************************
*/
rectangle_struct GetRectangle(int idx)
{
	if (rectangles != NULL && idx <= rectangle_num && idx >= 0)
	{
		return rectangles[idx];
	}
	return (rectangle_struct){0};
}