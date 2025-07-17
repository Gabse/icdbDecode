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
* This project uses the Zlib library (https://www.zlib.bus/) for decompression.
*/

/*
******************************************************************
* Includes
******************************************************************
*/
#include "bus.h"
#include <stdint.h>					// Required for int32_t, uint32_t, ...
#include <stdlib.h>					// Required for calloc to work properly
#include <stdio.h>					// Required for memcpy
#include <string.h>					// Required for memcpy
#include "dxdatl.h"					// Required for keys
#include "blkatl.h"					// Required for keys
#include "properties.h" 			// Required for styles
#include "cdbblks.h" 				// Required for coordinate struct
#include "label.h"					// Required for label struct
#include "segments.h"				// Required for segment struct

/*
******************************************************************
* Global Variables
******************************************************************
*/
unsigned int bus_num = 0;
bus_struct* bus = NULL;

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	ProcessBus()
*
* - description: 	Processes busses
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void ProcessBus(void)
{
	if (Blkatl_BusNam != NULL &&
		Blkatl_BusUID != NULL &&
		Blkatl_Bus2Prps != NULL &&
		Dxdatl_Bus2GUID != NULL &&
		Dxdatl_Bus2GrpLabels != NULL &&
		Dxdatl_Bus2GrpSegments != NULL &&
		Dxdatl_Bus2GrpStyles != NULL &&
		Dxdatl_Bus2Grps != NULL &&
		Dxdatl_BusID != NULL &&
		(*Blkatl_BusNam).LengthCalc == (*Blkatl_BusUID).LengthCalc &&
		(*Blkatl_BusNam).LengthCalc == (*Blkatl_Bus2Prps).LengthCalc &&
		// Deleting a Bus leaves behind elements in Dxdatl. Elements are overwritten as soon as they are used agian.
		(*Blkatl_BusNam).LengthCalc <= (*Dxdatl_Bus2GrpSegments).LengthCalc &&
		(*Blkatl_BusNam).LengthCalc <= (*Dxdatl_Bus2GrpStyles).LengthCalc &&
		(*Blkatl_BusNam).LengthCalc <= (*Dxdatl_Bus2Grps).LengthCalc &&
		(*Blkatl_BusNam).LengthCalc <= (*Dxdatl_BusID).LengthCalc &&
		bus_num == 0
		)
	{
		bus_num = (*Blkatl_BusNam).LengthCalc;
		bus = calloc(bus_num, sizeof(bus_struct));
		if (bus == NULL)
		{
			return;
		}
		unsigned int cnt = 0;
		for (unsigned int i = 0; i < bus_num; i++)
		{
			// Bus name
			(bus[i]).Name = CopyString(((string_struct*)(*Blkatl_BusNam).Data)[i]);

			// UID
			memcpy(&(bus[i]).UID, &(((char*)(*Blkatl_BusUID).Data)[i * 8]), 8);

			// Type code
			if (Blkatl_BusType != NULL && (*Blkatl_BusType).LengthCalc == bus_num) // Not present on older versions
			{
				(bus[i]).BusType = ((int*)(*Blkatl_BusType).Data)[i];
			}
			else
			{
				(bus[i]).BusType = 0;
			}

			if (
				(((int_array_struct*)(*Dxdatl_Bus2GrpSegments).Data)[i].Length == ((int_array_struct*)(*Dxdatl_Bus2GrpStyles).Data)[i].Length) &&
				(((int_array_struct*)(*Dxdatl_Bus2GrpSegments).Data)[i].Length == ((int_array_struct*)(*Dxdatl_Bus2Grps).Data)[i].Length) &&
				(((int_array_struct*)(*Dxdatl_Bus2GrpSegments).Data)[i].Length == ((int_array_struct*)(*Dxdatl_BusID).Data)[i].Length)
				)
			{
				bus[i].BusSegmentLen = ((int_array_struct*)(*Dxdatl_Bus2GrpSegments).Data)[i].Length;
				bus[i].BusSegment = calloc(bus[i].BusSegmentLen, sizeof(bus_segment_struct));
				if (bus[i].BusSegment == NULL)
				{
					return;
				}
				for (unsigned int j = 0; j < bus[i].BusSegmentLen; j++)
				{
					// Buses without custom names don't have a label entry, leading to a mismatch between Bus2GrpSegment and the other bus data. Detect buses without name ("$" Prefix) and skip them
					if ((bus[i]).Name.Text[0] != '$')
					{
						if (((*Dxdatl_Bus2GrpLabels).LengthCalc > cnt) && (((int_array_struct*)(*Dxdatl_Bus2GrpSegments).Data)[i].Length == ((int_array_struct*)(*Dxdatl_Bus2GrpLabels).Data)[cnt].Length))
						{
							((bus[i]).BusSegment)[j].Label = GetLabel((((int_array_struct*)(*Dxdatl_Bus2GrpLabels).Data)[cnt].Data[j]).u32[0] - 1);
						}
						cnt++;
					}

					// Bus Segments
					((bus[i]).BusSegment)[j].Segments = GetSegment((((int_array_struct*)(*Dxdatl_Bus2GrpSegments).Data)[i].Data[j]).u32[0] - 1);

					// Bus Properties
					((bus[i]).BusSegment)[j].Properties = GetProperty((((int_array_struct*)(*Dxdatl_Bus2GrpStyles).Data)[i].Data[j]).u32[0] - 1);

					// Groups
					((bus[i]).BusSegment)[j].Groups = (((int_array_struct*)(*Dxdatl_Bus2Grps).Data)[i].Data[j]).u32[0];

					// BusDXD
					((bus[i]).BusSegment)[j].BusID = (((int_array_struct*)(*Dxdatl_BusID).Data)[i].Data[j]).u32[0];
				}
			}
		}
	}
	else
	{
		myPrint("Error processing busses!\n");
	}
}

/*
******************************************************************
* - function name:	InitBus()
*
* - description: 	Initializes bus
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void InitBus(void)
{
	if (bus_num != 0 && bus != NULL)
	{
		for (unsigned int i = 0; i < bus_num; i++)
		{
			free(bus[i].BusSegment);
			free(bus[i].Name.Text);
		}
		free(bus);
		bus_num = 0;
	}
}

/*
******************************************************************
* - function name:	GetNumBus()
*
* - description: 	Returns the number of graphical buss in this file
*
* - parameter: 		-
*
* - return value: 	number of graphical buss in this file
******************************************************************
*/
unsigned int GetNumBus(void)
{
	return bus_num;
}

/*
******************************************************************
* - function name:	GetBus()
*
* - description: 	Returns the selected bus
*
* - parameter: 		bus index
*
* - return value: 	bus struct
******************************************************************
*/
bus_struct GetBus(int idx)
{
	if (bus != NULL && idx <= bus_num && idx >= 0)
	{
		return bus[idx];
	}
	return (bus_struct){0};
}