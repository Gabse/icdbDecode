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
#include "arc.h"
#include <stdint.h>				// Required for int32_t, uint32_t, ...
#include <stdlib.h>				// Required for calloc to work properly
#include <stdio.h>				// Required for memcpy
#include <string.h>				// Required for memcpy
#include "../common.h"			// Required for element_struct
#include "dxdatl.h"				// Required for keys
#include "../cdbblks/cdbblks.h"	// Required for cdbblks
#include "property.h"			// Required for styles

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	ProcessArc()
*
* - description: 	Processes arc
*
* - parameter: 		element_struct pointer
*
* - return value: 	-
******************************************************************
*/
void ProcessArc(element_struct* property, element_struct* arc)
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
		arc->Length == 0
		)
	{
		arc->Length = (*Dxdatl_Arc2Style).LengthCalc;
		arc->Data = calloc(arc->Length, sizeof(arc_struct));
		if (arc->Data == NULL)
		{
			return;
		}
		for (unsigned int i = 0; i < arc->Length; i++)
		{
			// Line Property
			(((arc_struct*)(arc->Data))[i]).Property = GetProperty(property, ((int*)(*Dxdatl_Arc2Style).Data)[i] - 1);

			// Start/End/Mid point
			(((arc_struct*)(arc->Data))[i]).EndCoord = ((coordinate_struct*)(*Dxdatl_ArcEnd).Data)[i];
			(((arc_struct*)(arc->Data))[i]).StartCoord = ((coordinate_struct*)(*Dxdatl_ArcStart).Data)[i];
			(((arc_struct*)(arc->Data))[i]).MidCoord = ((coordinate_struct*)(*Dxdatl_ArcMid).Data)[i];

			// ID
			(((arc_struct*)(arc->Data))[i]).IDDxD = ((int*)(*Dxdatl_ArcIDDxD).Data)[i];

			// UID
			memcpy(&(((arc_struct*)(arc->Data))[i]).UID, &(((char*)(*Dxdatl_ArcUID).Data)[i*8]), 8);
		}
	}
	else
	{
		myPrint("Error processing arcs!\n");
	}
}

/*
******************************************************************
* - function name:	InitArc()
*
* - description: 	Initializes arc
*
* - parameter: 		element_struct pointer
*
* - return value: 	-
******************************************************************
*/
void InitArc(element_struct* arc)
{
	if (arc->Length != 0 && ((arc_struct*)(arc->Data)) != NULL)
	{
		free(arc->Data);
		arc->Data = NULL;
		arc->Length = 0;
	}
}

/*
******************************************************************
* - function name:	GetArc()
*
* - description: 	Returns the selected arc
*
* - parameter: 		element_struct pointer, arc index
*
* - return value: 	arc struct
******************************************************************
*/
arc_struct GetArc(element_struct* arc, int idx)
{
	if (arc->Data != NULL && idx <= arc->Length && idx >= 0)
	{
		return ((arc_struct*)(arc->Data))[idx];
	}
	return (arc_struct){ 0 };
}