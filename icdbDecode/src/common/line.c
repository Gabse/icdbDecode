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
#include "line.h"
#include <stdint.h>					// Required for int32_t, uint32_t, ...
#include <stdlib.h>					// Required for calloc to work properly
#include <stdio.h>					// Required for memcpy
#include <string.h>					// Required for memcpy
#include "../common.h"				// Required for element_struct
#include "dxdatl.h"					// Required for keys
#include "../cdbblks/cdbblks.h"		// Required for cdbblks
#include "property.h"				// Required for styles

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	ProcessLine()
*
* - description: 	Processes line
*
* - parameter: 		element_struct pointer
*
* - return value: 	-
******************************************************************
*/
void ProcessLine(element_struct* property, element_struct* line)
{
	if (
		Dxdatl_LinePointX != NULL &&
		Dxdatl_LinePointY != NULL &&
		Dxdatl_Line2Style != NULL &&
		Dxdatl_LineUID != NULL &&
		Dxdatl_LineIDDxD != NULL &&
		(*Dxdatl_LinePointX).LengthCalc == (*Dxdatl_LinePointY).LengthCalc &&
		(*Dxdatl_LinePointX).LengthCalc == (*Dxdatl_Line2Style).LengthCalc &&
		(*Dxdatl_LinePointX).LengthCalc == (*Dxdatl_LineUID).LengthCalc &&
		(*Dxdatl_LinePointX).LengthCalc == (*Dxdatl_LineIDDxD).LengthCalc &&
		line->Length == 0
		)
	{
		line->Length = (*Dxdatl_LinePointX).LengthCalc;
		line->Data = calloc(line->Length, sizeof(line_struct));
		if (line->Data == NULL)
		{
			return;
		}
		unsigned int cnt = 0;
		for (unsigned int i = 0; i < line->Length; i++)
		{
			// Line Start/Stop Coordinate
			if (((int_array_struct*)(*Dxdatl_LinePointX).Data)[i].Length == ((int_array_struct*)(*Dxdatl_LinePointY).Data)[i].Length)
			{
				// From list of points with coordinate offset to line segment with start + end coordinate
				((line_struct*)(line->Data))[i].numSegment = ((int_array_struct*)(*Dxdatl_LinePointX).Data)[i].Length - 1;
				((line_struct*)(line->Data))[i].Segment = calloc(((line_struct*)(line->Data))[i].numSegment, sizeof(line_segment_struct));
				if (((line_struct*)(line->Data))[i].Segment == NULL)
				{
					return;
				}
				int X;
				int Y;
				for (unsigned int j = 0; j < ((line_struct*)(line->Data))[i].numSegment; j++)
				{
					if (j == 0) // First Element inits accumulator
					{
						X = ((int_array_struct*)(*Dxdatl_LinePointX).Data)[i].Data[j].i32[0];
						Y = ((int_array_struct*)(*Dxdatl_LinePointY).Data)[i].Data[j].i32[0];
						((((line_struct*)(line->Data))[i]).Segment)[j].Start.X = X;
						((((line_struct*)(line->Data))[i]).Segment)[j].Start.Y = Y;
					}
					else
					{
						((((line_struct*)(line->Data))[i]).Segment)[j].Start = ((((line_struct*)(line->Data))[i]).Segment)[j - 1].End;
					}
					// Accumulate
					X += ((int_array_struct*)(*Dxdatl_LinePointX).Data)[i].Data[j + 1].i32[0] + 1;
					Y += ((int_array_struct*)(*Dxdatl_LinePointY).Data)[i].Data[j + 1].i32[0] + 1;
					((((line_struct*)(line->Data))[i]).Segment)[j].End.X = X;
					((((line_struct*)(line->Data))[i]).Segment)[j].End.Y = Y;

					// Line Endings
					if (
						Dxdatl_LineStartingCap != NULL &&
						Dxdatl_LineEndingCap != NULL &&
						Dxdatl_LineArrowSize != NULL &&
						(*Dxdatl_LineStartingCap).LengthCalc == (*Dxdatl_LineEndingCap).LengthCalc &&
						(*Dxdatl_LineStartingCap).LengthCalc == (*Dxdatl_LineArrowSize).LengthCalc &&
						(*Dxdatl_LineStartingCap).LengthCalc > cnt
						)
					{
						((((line_struct*)(line->Data))[i]).Segment)[j].EndingStart = ((lineending_type*)(*Dxdatl_LineStartingCap).Data)[cnt];
						((((line_struct*)(line->Data))[i]).Segment)[j].EndingEnd = ((lineending_type*)(*Dxdatl_LineEndingCap).Data)[cnt];
						((((line_struct*)(line->Data))[i]).Segment)[j].EndingSize = ((lineending_type*)(*Dxdatl_LineArrowSize).Data)[cnt];
						cnt++;
					}
				}
			}

			// UID
			memcpy(&(((line_struct*)(line->Data))[i]).UID, &(((char*)(*Dxdatl_LineUID).Data)[i*8]), 8);

			// IDDXD
			(((line_struct*)(line->Data))[i]).IDDxD = ((int*)(*Dxdatl_LineIDDxD).Data)[i];

			// Line Property
			(((line_struct*)(line->Data))[i]).Property = GetProperty(property, ((int*)(*Dxdatl_Line2Style).Data)[i] - 1);
		
		}
	}
	else
	{
		myPrint("Error processing lines!\n");
	}
}

/*
******************************************************************
* - function name:	InitLine()
*
* - description: 	Initializes line
*
* - parameter: 		element_struct pointer
*
* - return value: 	-
******************************************************************
*/
void InitLine(element_struct* line)
{
	if (line->Length != 0 && line->Data != NULL)
	{
		for (unsigned int i = 0; i < line->Length; i++)
		{
			free(((line_struct*)(line->Data))[i].Segment);
		}
		free(line->Data);
		line->Data = NULL;
		line->Length = 0;
	}
}

/*
******************************************************************
* - function name:	GetLine()
*
* - description: 	Returns the selected line
*
* - parameter: 		element_struct pointer, line index
*
* - return value: 	line struct
******************************************************************
*/
line_struct GetLine(element_struct* line, int idx)
{
	if (line->Data != NULL && idx <= line->Length && idx >= 0)
	{
		return ((line_struct*)(line->Data))[idx];
	}
	return (line_struct){0};
}