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
#include "net.h"
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
unsigned int net_num = 0;
net_struct* net = NULL;

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	ProcessNet()
*
* - description: 	Processes nets
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void ProcessNet(void)
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
		net_num == 0
		)
	{
		net_num = (*Dxdatl_Net2GrpSegments).LengthCalc;

		net = calloc(net_num, sizeof(net_struct));
		if (net == NULL)
		{
			return;
		}
		for (unsigned int i = 0; i < net_num; i++)
		{
			// Net
			(net[i]).Net = ((int*)(*Dxdatl_NetDxD2Net).Data)[i];

			// UID
			memcpy(&(net[i]).UID, &(((char*)(*Dxdatl_NetDxDUID).Data)[i * 8]), 8);

			// Net ID
			(net[i]).NetID = ((int*)(*Dxdatl_NetID).Data)[i];

			// Net name
			(net[i]).Name = CopyString(((string_struct*)(*Dxdatl_NetLabel).Data)[i]);

			// Segments
			if (
			(((int_array_struct*)(*Dxdatl_Net2GrpLabels).Data)[i].Length == ((int_array_struct*)(*Dxdatl_Net2GrpSegments).Data)[i].Length) &&
			(((int_array_struct*)(*Dxdatl_Net2GrpLabels).Data)[i].Length == ((int_array_struct*)(*Dxdatl_Net2GrpStyles).Data)[i].Length)
			)
			{
				net[i].NumNetSegment = ((int_array_struct*)(*Dxdatl_Net2GrpLabels).Data)[i].Length;
				net[i].NetSegment = calloc(net[i].NumNetSegment, sizeof(net_segment_struct));
				if (net[i].NetSegment == NULL)
				{
					return;
				}
				for (unsigned int j = 0; j < net[i].NumNetSegment; j++)
				{
					// Label
					((net[i]).NetSegment)[j].Label = GetLabel((((int_array_struct*)(*Dxdatl_Net2GrpLabels).Data)[i].Data[j]).u32[0] - 1);

					// Segments
					((net[i]).NetSegment)[j].Segments = GetSegment((((int_array_struct*)(*Dxdatl_Net2GrpSegments).Data)[i].Data[j]).u32[0] - 1);

					// Line Properties
					((net[i]).NetSegment)[j].Properties = GetProperty((((int_array_struct*)(*Dxdatl_Net2GrpStyles).Data)[i].Data[j]).u32[0] - 1);
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
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void InitNet(void)
{
	if (net_num != 0 && net != NULL)
	{
		for (unsigned int i = 0; i < net_num; i++)
		{
			free(net[i].Name.Text);
			free(net[i].NetSegment);
		}
		free(net);
		net_num = 0;
	}
}

/*
******************************************************************
* - function name:	GetNumNet()
*
* - description: 	Returns the number of graphical nets in this file
*
* - parameter: 		-
*
* - return value: 	number of graphical nets in this file
******************************************************************
*/
unsigned int GetNumNet(void)
{
	return net_num;
}

/*
******************************************************************
* - function name:	GetNet()
*
* - description: 	Returns the selected net
*
* - parameter: 		net index
*
* - return value: 	net struct
******************************************************************
*/
net_struct GetNet(int idx)
{
	if (net != NULL && idx <= net_num && idx >= 0)
	{
		return net[idx];
	}
	return (net_struct){0};
}