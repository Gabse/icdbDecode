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
#include <stdint.h>					// Required for int32_t, uint32_t, ...
#include <stdlib.h>					// Required for calloc to work properly
#include <stdio.h>					// Required for memcpy
#include <string.h>					// Required for memcpy
#include "dxdatl.h"					// Required for keys
#include "textdata.h" 				// Required for textdata struct

/*
******************************************************************
* Global Variables
******************************************************************
*/
unsigned int label_num = 0;
label_struct* labels = NULL;

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	ProcessLabels()
*
* - description: 	Processes labels
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void ProcessLabels(void)
{
	// ToDo:
	// - Check what IDDxD is used for, it sometimes mismatches with other elements
	// - Check why IndexDxDNet sometimes mismatches label_num. Might need a accumulator of something
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
		label_num == 0 
		)
	{
		label_num = (*Dxdatl_Label2TextData).LengthCalc;
		labels = calloc(label_num, sizeof(label_struct));
		if (labels == NULL)
		{
			return;
		}
		for (unsigned int i = 0; i < label_num; i++)
		{
			if (
				((int_array_struct*)(*Dxdatl_LabelInverted).Data)[i].Length != ((int_array_struct*)(*Dxdatl_LabelScope).Data)[i].Length ||
				((int_array_struct*)(*Dxdatl_LabelInverted).Data)[i].Length != ((int_array_struct*)(*Dxdatl_LabelSegmentNum).Data)[i].Length ||
				((int_array_struct*)(*Dxdatl_LabelInverted).Data)[i].Length != ((int_array_struct*)(*Dxdatl_LabelVisibility).Data)[i].Length
				)
			{
				return;
			}
			(labels[i]).SublableNum = ((int_array_struct*)(*Dxdatl_LabelInverted).Data)[i].Length;
			(labels[i]).Sublable = calloc((labels[i]).SublableNum, sizeof(sublabel_struct));
			if ((labels[i]).Sublable == NULL)
			{
				return;
			}
			for (unsigned int j = 0; j < (labels[i]).SublableNum; j++)
			{
				// Inverted. Unknown what this does
				(labels[i]).Sublable[j].Inverted = ((int_array_struct*)(*Dxdatl_LabelInverted).Data)[i].Data[j].i32[0];

				// Scope. Unknown what this does
				(labels[i]).Sublable[j].Scope = ((int_array_struct*)(*Dxdatl_LabelScope).Data)[i].Data[j].i32[0];

				// Segment
				(labels[i]).Sublable[j].SegmentNum = ((int_array_struct*)(*Dxdatl_LabelSegmentNum).Data)[i].Data[j].i32[0];

				// Visibility
				(labels[i]).Sublable[j].Visibility = ((int_array_struct*)(*Dxdatl_LabelVisibility).Data)[i].Data[j].i32[0];

				// Text Formating
				(labels[i]).Sublable[j].TextData = GetTextdata(((int_array_struct*)(*Dxdatl_Label2TextData).Data)[i].Data[j].i32[0] - 1);
			}

			if (Dxdatl_LabelIDDxD != NULL && label_num == (*Dxdatl_LabelIndexDxDNet).LengthCalc)
			{
				// IndexDXD. UID
				memcpy(&(labels[i]).IndexDxDNet, &(((char*)(*Dxdatl_LabelIndexDxDNet).Data)[i * 8]), 8);
			}

			// IDDXD
			//(labels[i]).IDDXD = ((int*)(*Dxdatl_LabelIDDxD).Data)[i];
		}
	}
	else
	{
		myPrint("Error processing labels!\n");
	}
}

/*
******************************************************************
* - function name:	InitLabels()
*
* - description: 	Initializes labels
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void InitLabels(void)
{
	if (label_num != 0 && labels != NULL)
	{
		
		for (unsigned int i = 0; i < label_num; i++)
		{
			free((labels[i]).Sublable);
		}
		free(labels);
		label_num = 0;
	}
}

/*
******************************************************************
* - function name:	GetNumLabels()
*
* - description: 	Returns the number of labels in this file
*
* - parameter: 		-
*
* - return value: 	number of labels in this file
******************************************************************
*/
unsigned int GetNumLabels(void)
{
	return label_num;
}

/*
******************************************************************
* - function name:	GetLabel()
*
* - description: 	Returns the selected label
*
* - parameter: 		label index
*
* - return value: 	label struct
******************************************************************
*/
label_struct GetLabel(int idx)
{
	if (labels != NULL && idx <= label_num && idx >= 0)
	{
		return labels[idx];
	}
	return (label_struct){0};
}