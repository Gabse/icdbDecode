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
#include "lines.h"
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
unsigned int line_num = 0;
line_struct* lines = NULL;

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	ProcessLines()
*
* - description: 	Processes lines
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void ProcessLines(void)
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
		line_num == 0 
		)
	{
		line_num = (*Dxdatl_LinePointX).LengthCalc;
		lines = calloc(line_num, sizeof(line_struct));
		if (lines == NULL)
		{
			return;
		}
		unsigned int cnt = 0;
		for (unsigned int i = 0; i < line_num; i++)
		{
			// Line Start/Stop Coordinate
			if (((int_array_struct*)(*Dxdatl_LinePointX).Data)[i].Length == ((int_array_struct*)(*Dxdatl_LinePointY).Data)[i].Length)
			{
				// From list of points with coordinate offset to line segments with start + end coordinate
				lines[i].numSegments = ((int_array_struct*)(*Dxdatl_LinePointX).Data)[i].Length - 1;
				lines[i].Segments = calloc(lines[i].numSegments, sizeof(line_segment_struct));
				if (lines[i].Segments == NULL)
				{
					return;
				}
				int X;
				int Y;
				for (unsigned int j = 0; j < lines[i].numSegments; j++)
				{
					if (j == 0) // First Element inits accumulator
					{
						X = ((int_array_struct*)(*Dxdatl_LinePointX).Data)[i].Data[j].i32[0];
						Y = ((int_array_struct*)(*Dxdatl_LinePointY).Data)[i].Data[j].i32[0];
						((lines[i]).Segments)[j].Start.X = X;
						((lines[i]).Segments)[j].Start.Y = Y;
					}
					else
					{
						((lines[i]).Segments)[j].Start = ((lines[i]).Segments)[j - 1].End;
					}
					// Accumulate
					X += ((int_array_struct*)(*Dxdatl_LinePointX).Data)[i].Data[j + 1].i32[0] + 1;
					Y += ((int_array_struct*)(*Dxdatl_LinePointY).Data)[i].Data[j + 1].i32[0] + 1;
					((lines[i]).Segments)[j].End.X = X;
					((lines[i]).Segments)[j].End.Y = Y;

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
						((lines[i]).Segments)[j].EndingStart = ((lineending_type*)(*Dxdatl_LineStartingCap).Data)[cnt];
						((lines[i]).Segments)[j].EndingEnd = ((lineending_type*)(*Dxdatl_LineEndingCap).Data)[cnt];
						((lines[i]).Segments)[j].EndingSize = ((lineending_type*)(*Dxdatl_LineArrowSize).Data)[cnt];
						cnt++;
					}
				}
			}

			// UID
			memcpy(&(lines[i]).UID, &(((char*)(*Dxdatl_LineUID).Data)[i*8]), 8);

			// IDDXD
			(lines[i]).IDDxD = ((int*)(*Dxdatl_LineIDDxD).Data)[i];

			// Line Properties
			(lines[i]).Properties = GetProperty(((int*)(*Dxdatl_Line2Style).Data)[i] - 1);
		
		}
	}
	else
	{
		myPrint("Error processing lines!\n");
	}
}

/*
******************************************************************
* - function name:	InitLines()
*
* - description: 	Initializes lines
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void InitLines(void)
{
	if (line_num != 0 && lines != NULL)
	{
		for (unsigned int i = 0; i < line_num; i++)
		{
			free(lines[i].Segments);
		}
		free(lines);
		line_num = 0;
	}
}

/*
******************************************************************
* - function name:	GetNumLines()
*
* - description: 	Returns the number of graphical lines in this file
*
* - parameter: 		-
*
* - return value: 	number of graphical lines in this file
******************************************************************
*/
unsigned int GetNumLines(void)
{
	return line_num;
}

/*
******************************************************************
* - function name:	GetLine()
*
* - description: 	Returns the selected line
*
* - parameter: 		line index
*
* - return value: 	line struct
******************************************************************
*/
line_struct GetLine(int idx)
{
	if (lines != NULL && idx <= line_num && idx >= 0)
	{
		return lines[idx];
	}
	return (line_struct){0};
}