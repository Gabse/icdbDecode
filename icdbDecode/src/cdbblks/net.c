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
#include "net.h"
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
* - function name:	ProcessNet()
*
* - description: 	Processes net
*
* - parameter: 		element_struct pointer
*
* - return value: 	-
******************************************************************
*/
void ProcessNet(element_struct* net)
{
	// NetDxD => Unique Nets (Not graphically but electrically connected nets have one entry per unique graphical net)
	// Net => Glogal Nets (Not graphically but electrically connected nets have one single entry)
	// NetsDXD => Page Nets (Not graphically but electrically connected nets have one single entry per page they appear on)

	if (
		Dxdatl_Net2GrpSegments != NULL &&
		Dxdatl_Net2GrpStyles != NULL &&
		Dxdatl_NetDxD2Net != NULL &&
		Dxdatl_NetDxDUID != NULL &&
		Dxdatl_NetID != NULL &&
		Dxdatl_NetLabel != NULL &&
		Dxdatl_Net2GrpLabels != NULL &&
		(*Dxdatl_Net2GrpSegments).LengthCalc == (*Dxdatl_Net2GrpStyles).LengthCalc &&
		(*Dxdatl_Net2GrpSegments).LengthCalc == (*Dxdatl_NetDxD2Net).LengthCalc &&
		(*Dxdatl_Net2GrpSegments).LengthCalc == (*Dxdatl_NetDxDUID).LengthCalc &&
		(*Dxdatl_Net2GrpSegments).LengthCalc == (*Dxdatl_NetID).LengthCalc &&
		(*Dxdatl_Net2GrpSegments).LengthCalc == (*Dxdatl_NetLabel).LengthCalc &&
		(*Dxdatl_Net2GrpSegments).LengthCalc == (*Dxdatl_Net2GrpLabels).LengthCalc &&
		net->Length == 0
		)
	{
		net->Length = (*Dxdatl_Net2GrpSegments).LengthCalc;

		net->Data = calloc(net->Length, sizeof(net_struct));
		if (net->Data == NULL)
		{
			return;
		}
		for (unsigned int i = 0; i < net->Length; i++)
		{
			// Net
			(((net_struct*)(net->Data))[i]).Net = ((int*)(*Dxdatl_NetDxD2Net).Data)[i];

			// UID
			memcpy(&(((net_struct*)(net->Data))[i]).UID, &(((char*)(*Dxdatl_NetDxDUID).Data)[i * 8]), 8);

			// Net ID
			(((net_struct*)(net->Data))[i]).NetID = ((int*)(*Dxdatl_NetID).Data)[i];

			// Net name
			(((net_struct*)(net->Data))[i]).Name = CopyString(((string_struct*)(*Dxdatl_NetLabel).Data)[i]);

			// Segment
			if (
			(((int_array_struct*)(*Dxdatl_Net2GrpLabels).Data)[i].Length == ((int_array_struct*)(*Dxdatl_Net2GrpSegments).Data)[i].Length) &&
			(((int_array_struct*)(*Dxdatl_Net2GrpLabels).Data)[i].Length == ((int_array_struct*)(*Dxdatl_Net2GrpStyles).Data)[i].Length)
			)
			{
				((net_struct*)(net->Data))[i].NumNetSegment = ((int_array_struct*)(*Dxdatl_Net2GrpLabels).Data)[i].Length;
				((net_struct*)(net->Data))[i].NetSegment = calloc(((net_struct*)(net->Data))[i].NumNetSegment, sizeof(net_segment_struct));
				if (((net_struct*)(net->Data))[i].NetSegment == NULL)
				{
					return;
				}
				for (unsigned int j = 0; j < ((net_struct*)(net->Data))[i].NumNetSegment; j++)
				{
					// Label
					((((net_struct*)(net->Data))[i]).NetSegment)[j].Label = GetLabel(&cdbblks_label, (((int_array_struct*)(*Dxdatl_Net2GrpLabels).Data)[i].Data[j]).u32[0] - 1);

					// Segment
					((((net_struct*)(net->Data))[i]).NetSegment)[j].Segment = GetSegment(&cdbblks_segment, (((int_array_struct*)(*Dxdatl_Net2GrpSegments).Data)[i].Data[j]).u32[0] - 1);

					// Line Property
					((((net_struct*)(net->Data))[i]).NetSegment)[j].Property = GetProperty(&cdbblks_property, (((int_array_struct*)(*Dxdatl_Net2GrpStyles).Data)[i].Data[j]).u32[0] - 1);
				}
			}
		}
	}
	else
	{
		myPrint("Error processing nets!\n");
	}
}

/*
******************************************************************
* - function name:	InitNet()
*
* - description: 	Initializes net
*
* - parameter: 		element_struct pointer
*
* - return value: 	-
******************************************************************
*/
void InitNet(element_struct* net)
{
	if (net->Length != 0 && net->Data != NULL)
	{
		for (unsigned int i = 0; i < net->Length; i++)
		{
			free(((net_struct*)(net->Data))[i].Name.Text);
			free(((net_struct*)(net->Data))[i].NetSegment);
		}
		free(net->Data);
		net->Data = NULL;
		net->Length = 0;
	}
}

/*
******************************************************************
* - function name:	GetNet()
*
* - description: 	Returns the selected net
*
* - parameter: 		element_struct pointer, net index
*
* - return value: 	net struct
******************************************************************
*/
net_struct GetNet(element_struct* net, int idx)
{
	if (net->Data != NULL && idx <= net->Length && idx >= 0)
	{
		return ((net_struct*)(net->Data))[idx];
	}
	return (net_struct){0};
}