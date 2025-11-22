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
#include "bus.h"
#include <stdint.h>					// Required for int32_t, uint32_t, ...
#include <stdlib.h>					// Required for calloc to work properly
#include <stdio.h>					// Required for memcpy
#include <string.h>					// Required for memcpy
#include "../common.h"				// Required for element_struct
#include "../common/dxdatl.h"		// Required for keys
#include "blkatl.h"					// Required for keys
#include "../common/property.h" 	// Required for styles
#include "cdbblks.h" 				// Required for coordinate struct
#include "../common/label.h"		// Required for label struct
#include "segment.h"				// Required for segment struct

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	ProcessBus()
*
* - description: 	Processes bus->Datases
*
* - parameter: 		element_struct pointer
*
* - return value: 	-
******************************************************************
*/
void ProcessBus(element_struct* bus)
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
		bus->Length == 0
		)
	{
		bus->Length = (*Blkatl_BusNam).LengthCalc;
		bus->Data = calloc(bus->Length, sizeof(bus_struct));
		if (bus->Data == NULL)
		{
			return;
		}
		unsigned int cnt = 0;
		for (unsigned int i = 0; i < bus->Length; i++)
		{
			// Bus name
			(((bus_struct*)(bus->Data))[i]).Name = CopyString(((string_struct*)(*Blkatl_BusNam).Data)[i]);

			// UID
			memcpy(&(((bus_struct*)(bus->Data))[i]).UID, &(((char*)(*Blkatl_BusUID).Data)[i * 8]), 8);

			// Type code
			if (Blkatl_BusType != NULL && (*Blkatl_BusType).LengthCalc == bus->Length) // Not present on older versions
			{
				(((bus_struct*)(bus->Data))[i]).BusType = ((int*)(*Blkatl_BusType).Data)[i];
			}
			else
			{
				(((bus_struct*)(bus->Data))[i]).BusType = 0;
			}

			if (
				(((int_array_struct*)(*Dxdatl_Bus2GrpSegments).Data)[i].Length == ((int_array_struct*)(*Dxdatl_Bus2GrpStyles).Data)[i].Length) &&
				(((int_array_struct*)(*Dxdatl_Bus2GrpSegments).Data)[i].Length == ((int_array_struct*)(*Dxdatl_Bus2Grps).Data)[i].Length) &&
				(((int_array_struct*)(*Dxdatl_Bus2GrpSegments).Data)[i].Length == ((int_array_struct*)(*Dxdatl_BusID).Data)[i].Length)
				)
			{
				((bus_struct*)(bus->Data))[i].BusSegmentLen = ((int_array_struct*)(*Dxdatl_Bus2GrpSegments).Data)[i].Length;
				((bus_struct*)(bus->Data))[i].BusSegment = calloc(((bus_struct*)(bus->Data))[i].BusSegmentLen, sizeof(bus_segment_struct));
				if (((bus_struct*)(bus->Data))[i].BusSegment == NULL)
				{
					return;
				}
				for (unsigned int j = 0; j < ((bus_struct*)(bus->Data))[i].BusSegmentLen; j++)
				{
					// Buses without custom names don't have a label entry, leading to a mismatch between Bus2GrpSegments and the other bus->Data data. Detect bus->Dataes without name ("$" Prefix) and skip them
					if ((((bus_struct*)(bus->Data))[i]).Name.Text[0] != '$')
					{
						if (((*Dxdatl_Bus2GrpLabels).LengthCalc > cnt) && (((int_array_struct*)(*Dxdatl_Bus2GrpSegments).Data)[i].Length == ((int_array_struct*)(*Dxdatl_Bus2GrpLabels).Data)[cnt].Length))
						{
							((((bus_struct*)(bus->Data))[i]).BusSegment)[j].Label = GetLabel(&cdbblks_label, (((int_array_struct*)(*Dxdatl_Bus2GrpLabels).Data)[cnt].Data[j]).u32[0] - 1);
						}
						cnt++;
					}

					// Bus Segment
					((((bus_struct*)(bus->Data))[i]).BusSegment)[j].Segment = GetSegment(&cdbblks_segment, (((int_array_struct*)(*Dxdatl_Bus2GrpSegments).Data)[i].Data[j]).u32[0] - 1);

					// Bus Property
					((((bus_struct*)(bus->Data))[i]).BusSegment)[j].Property = GetProperty(&cdbblks_property, (((int_array_struct*)(*Dxdatl_Bus2GrpStyles).Data)[i].Data[j]).u32[0] - 1);

					// Group
					((((bus_struct*)(bus->Data))[i]).BusSegment)[j].Group = (((int_array_struct*)(*Dxdatl_Bus2Grps).Data)[i].Data[j]).u32[0];

					// BusDXD
					((((bus_struct*)(bus->Data))[i]).BusSegment)[j].BusID = (((int_array_struct*)(*Dxdatl_BusID).Data)[i].Data[j]).u32[0];
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
* - parameter: 		element_struct pointer
*
* - return value: 	-
******************************************************************
*/
void InitBus(element_struct* bus)
{
	if (bus->Length != 0 && bus->Data != NULL)
	{
		for (unsigned int i = 0; i < bus->Length; i++)
		{
			free(((bus_struct*)(bus->Data))[i].BusSegment);
			free(((bus_struct*)(bus->Data))[i].Name.Text);
		}
		free(bus->Data);
		bus->Data = NULL;
		bus->Length = 0;
	}
}

/*
******************************************************************
* - function name:	GetBus()
*
* - description: 	Returns the selected bus
*
* - parameter: 		element_struct pointer, bus index
*
* - return value: 	bus struct
******************************************************************
*/
bus_struct GetBus(element_struct* bus, int idx)
{
	if (bus->Data != NULL && idx <= bus->Length && idx >= 0)
	{
		return ((bus_struct*)(bus->Data))[idx];
	}
	return (bus_struct){0};
}