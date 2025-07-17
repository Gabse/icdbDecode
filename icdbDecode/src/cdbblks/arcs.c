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
#include "arcs.h"
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
unsigned int arc_num = 0;
arc_struct* arcs = NULL;

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	ProcessArcs()
*
* - description: 	Processes arcs
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void ProcessArcs(void)
{
	if (Dxdatl_Arc2Style != NULL && 
		Dxdatl_ArcEnd != NULL && 
		Dxdatl_ArcIDDxD != NULL &&
		Dxdatl_ArcMid != NULL &&
		Dxdatl_ArcStart != NULL &&
		Dxdatl_ArcUID != NULL &&
		(*Dxdatl_Arc2Style).LengthCalc == (*Dxdatl_ArcEnd).LengthCalc &&
		(*Dxdatl_Arc2Style).LengthCalc == (*Dxdatl_ArcIDDxD).LengthCalc &&
		(*Dxdatl_Arc2Style).LengthCalc == (*Dxdatl_ArcMid).LengthCalc &&
		(*Dxdatl_Arc2Style).LengthCalc == (*Dxdatl_ArcStart).LengthCalc &&
		(*Dxdatl_Arc2Style).LengthCalc == (*Dxdatl_ArcUID).LengthCalc &&
		arc_num == 0 
		)
	{
		arc_num = (*Dxdatl_Arc2Style).LengthCalc;
		arcs = calloc(arc_num, sizeof(arc_struct));
		if (arcs == NULL)
		{
			return;
		}
		for (unsigned int i = 0; i < arc_num; i++)
		{
			// Line Properties
			(arcs[i]).Properties = GetProperty(((int*)(*Dxdatl_Arc2Style).Data)[i] - 1);

			// Start/End/Mid point
			(arcs[i]).EndCoord = ((coordinate_struct*)(*Dxdatl_ArcEnd).Data)[i];
			(arcs[i]).StartCoord = ((coordinate_struct*)(*Dxdatl_ArcStart).Data)[i];
			(arcs[i]).MidCoord = ((coordinate_struct*)(*Dxdatl_ArcMid).Data)[i];

			// ID
			(arcs[i]).IDDxD = ((int*)(*Dxdatl_ArcIDDxD).Data)[i];

			// UID
			memcpy(&(arcs[i]).UID, &(((char*)(*Dxdatl_ArcUID).Data)[i*8]), 8);
		}
	}
	else
	{
		myPrint("Error processing arcs!\n");
	}
}

/*
******************************************************************
* - function name:	InitArcs()
*
* - description: 	Initializes arcs
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void InitArcs(void)
{
	if (arc_num != 0 && arcs != NULL)
	{
		free(arcs);
		arc_num = 0;
	}
}

/*
******************************************************************
* - function name:	GetNumArcs()
*
* - description: 	Returns the number of graphical arcs in this file
*
* - parameter: 		-
*
* - return value: 	number of graphical arcs in this file
******************************************************************
*/
unsigned int GetNumArcs(void)
{
	return arc_num;
}

/*
******************************************************************
* - function name:	GetArc()
*
* - description: 	Returns the selected arc
*
* - parameter: 		arc index
*
* - return value: 	arc struct
******************************************************************
*/
arc_struct GetArc(int idx)
{
	if (arcs != NULL && idx <= arc_num && idx >= 0)
	{
		return arcs[idx];
	}
	return (arc_struct){0};
}