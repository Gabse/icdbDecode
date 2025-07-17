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
#include "circles.h"
#include <stdint.h>					// Required for int32_t, uint32_t, ...
#include <stdlib.h>					// Required for calloc to work properly
#include <stdio.h>					// Required for memcpy
#include <string.h>					// Required for memcpy
#include "dxdatl.h"					// Required for keys
#include "properties.h" 			// Required for styles
#include "cdbblks.h" 				// Required for coordinate struct

/*
******************************************************************
* Global Variables
******************************************************************
*/
unsigned int circle_num = 0;
circle_struct* circles = NULL;

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	ProcessCircles()
*
* - description: 	Processes circles
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void ProcessCircles(void)
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
		circle_num == 0 
		)
	{
		circle_num = (*Dxdatl_Circle2Style).LengthCalc;
		circles = calloc(circle_num, sizeof(circle_struct));
		if (circles == NULL)
		{
			return;
		}
		for (unsigned int i = 0; i < circle_num; i++)
		{
			// Line Properties
			(circles[i]).Properties = GetProperty(((int*)(*Dxdatl_Circle2Style).Data)[i] - 1);

			// IDDXD
			(circles[i]).IDDxD = ((int*)(*Dxdatl_CircleIDDxD).Data)[i];

			// Coordinates
			(circles[i]).CenterCoord = ((coordinate_struct*)(*Dxdatl_CirclePos).Data)[i];
			(circles[i]).Radius = ((int*)(*Dxdatl_CircleRadius).Data)[i];

			// UID
			memcpy(&(circles[i]).UID, &(((char*)(*Dxdatl_CircleUID).Data)[i*8]), 8);
		}
	}
	else
	{
		myPrint("Error processing circles!\n");
	}
}

/*
******************************************************************
* - function name:	InitCircles()
*
* - description: 	Initializes circles
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void InitCircles(void)
{
	if (circle_num != 0 && circles != NULL)
	{
		free(circles);
		circle_num = 0;
	}
}

/*
******************************************************************
* - function name:	GetNumCircles()
*
* - description: 	Returns the number of graphical circles in this file
*
* - parameter: 		-
*
* - return value: 	number of graphical circles in this file
******************************************************************
*/
unsigned int GetNumCircles(void)
{
	return circle_num;
}

/*
******************************************************************
* - function name:	GetCircle()
*
* - description: 	Returns the selected circle
*
* - parameter: 		circle index
*
* - return value: 	circle struct
******************************************************************
*/
circle_struct GetCircle(int idx)
{
	if (circles != NULL && idx <= circle_num && idx >= 0)
	{
		return circles[idx];
	}
	return (circle_struct){0};
}