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
#include "label.h"
#include <stdint.h>		// Required for int32_t, uint32_t, ...
#include <stdlib.h>		// Required for calloc to work properly
#include <stdio.h>		// Required for memcpy
#include <string.h>		// Required for memcpy
#include "../common.h"	// Required for element_struct
#include "dxdatl.h"		// Required for keys
#include "textdata.h" 	// Required for textdata struct

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	ProcessLabel()
*
* - description: 	Processes label
*
* - parameter: 		element_struct pointer
*
* - return value: 	-
******************************************************************
*/
void ProcessLabel(element_struct* label, element_struct* textdata)
{
	// ToDo:
	// - Check what IDDxD is used for, it sometimes mismatches with other elements
	// - Check why IndexDxDNet sometimes mismatches label->Length. Might need a accumulator of something
	if (Dxdatl_Label2TextData != NULL &&
		Dxdatl_LabelIndexDxDNet != NULL &&
		Dxdatl_LabelInverted != NULL &&
		Dxdatl_LabelScope != NULL &&
		Dxdatl_LabelSegmentNum != NULL &&
		Dxdatl_LabelVisibility != NULL &&
		//(*Dxdatl_Label2TextData).LengthCalc == (*Dxdatl_LabelIDDxD).LengthCalc &&
		(*Dxdatl_Label2TextData).LengthCalc == (*Dxdatl_LabelInverted).LengthCalc &&
		(*Dxdatl_Label2TextData).LengthCalc == (*Dxdatl_LabelScope).LengthCalc &&
		(*Dxdatl_Label2TextData).LengthCalc == (*Dxdatl_LabelSegmentNum).LengthCalc &&
		(*Dxdatl_Label2TextData).LengthCalc == (*Dxdatl_LabelVisibility).LengthCalc &&
		label->Length == 0
		)
	{
		label->Length = (*Dxdatl_Label2TextData).LengthCalc;
		label->Data = calloc(label->Length, sizeof(label_struct));
		if (label->Data == NULL)
		{
			return;
		}
		for (unsigned int i = 0; i < label->Length; i++)
		{
			if (
				((int_array_struct*)(*Dxdatl_LabelInverted).Data)[i].Length != ((int_array_struct*)(*Dxdatl_LabelScope).Data)[i].Length ||
				((int_array_struct*)(*Dxdatl_LabelInverted).Data)[i].Length != ((int_array_struct*)(*Dxdatl_LabelSegmentNum).Data)[i].Length ||
				((int_array_struct*)(*Dxdatl_LabelInverted).Data)[i].Length != ((int_array_struct*)(*Dxdatl_LabelVisibility).Data)[i].Length
				)
			{
				return;
			}
			(((label_struct*)(label->Data))[i]).SublableNum = ((int_array_struct*)(*Dxdatl_LabelInverted).Data)[i].Length;
			(((label_struct*)(label->Data))[i]).Sublable = calloc((((label_struct*)(label->Data))[i]).SublableNum, sizeof(sublabel_struct));
			if ((((label_struct*)(label->Data))[i]).Sublable == NULL)
			{
				return;
			}
			for (unsigned int j = 0; j < (((label_struct*)(label->Data))[i]).SublableNum; j++)
			{
				// Inverted. Unknown what this does
				(((label_struct*)(label->Data))[i]).Sublable[j].Inverted = ((int_array_struct*)(*Dxdatl_LabelInverted).Data)[i].Data[j].i32[0];

				// Scope. Unknown what this does
				(((label_struct*)(label->Data))[i]).Sublable[j].Scope = ((int_array_struct*)(*Dxdatl_LabelScope).Data)[i].Data[j].i32[0];

				// Segment
				(((label_struct*)(label->Data))[i]).Sublable[j].SegmentNum = ((int_array_struct*)(*Dxdatl_LabelSegmentNum).Data)[i].Data[j].i32[0];

				// Visibility
				(((label_struct*)(label->Data))[i]).Sublable[j].Visibility = ((int_array_struct*)(*Dxdatl_LabelVisibility).Data)[i].Data[j].i32[0];

				// Text Formating
				(((label_struct*)(label->Data))[i]).Sublable[j].TextData = GetTextdata(textdata, ((int_array_struct*)(*Dxdatl_Label2TextData).Data)[i].Data[j].i32[0] - 1);
			}

			if (Dxdatl_LabelIDDxD != NULL && label->Length == (*Dxdatl_LabelIndexDxDNet).LengthCalc)
			{
				// IndexDXD. UID
				memcpy(&(((label_struct*)(label->Data))[i]).IndexDxDNet, &(((char*)(*Dxdatl_LabelIndexDxDNet).Data)[i * 8]), 8);
			}

			// IDDXD
			//(((label_struct*)(label->Data))[i]).IDDXD = ((int*)(*Dxdatl_LabelIDDxD).Data)[i];
		}
	}
	else
	{
		myPrint("Error processing labels!\n");
	}
}

/*
******************************************************************
* - function name:	InitLabel()
*
* - description: 	Initializes label
*
* - parameter: 		element_struct pointer
*
* - return value: 	-
******************************************************************
*/
void InitLabel(element_struct* label)
{
	if (label->Length != 0 && label->Data != NULL)
	{
		
		for (unsigned int i = 0; i < label->Length; i++)
		{
			free((((label_struct*)(label->Data))[i]).Sublable);
		}
		free(label->Data);
		label->Data = NULL;
		label->Length = 0;
	}
}

/*
******************************************************************
* - function name:	GetLabel()
*
* - description: 	Returns the selected label
*
* - parameter: 		element_struct pointer, label index
*
* - return value: 	label struct
******************************************************************
*/
label_struct GetLabel(element_struct* label, int idx)
{
	if (label->Data != NULL && idx <= label->Length && idx >= 0)
	{
		return ((label_struct*)(label->Data))[idx];
	}
	return (label_struct){0};
}