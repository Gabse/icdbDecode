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
#include "property.h"
#include <stdint.h>		// Required for int32_t, uint32_t, ...
#include <stdlib.h>		// Required for calloc to work properly
#include <stdio.h>		// Required for memcpy
#include <string.h>		// Required for memcpy
#include "../common.h"	// Required for element_struct
#include "dxdatl.h"		// Required for keys

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	ProcessProperty()
*
* - description: 	Processes property
*
* - parameter: 		element_struct pointer
*
* - return value: 	-
******************************************************************
*/
void ProcessProperty(element_struct* property)
{
	if (Dxdatl_Style != NULL &&
		Dxdatl_Fill != NULL &&
		Dxdatl_Thickness != NULL &&
		Dxdatl_Color != NULL &&
		(*Dxdatl_Style).LengthCalc == (*Dxdatl_Fill).LengthCalc &&
		(*Dxdatl_Style).LengthCalc == (*Dxdatl_Thickness).LengthCalc &&
		(*Dxdatl_Style).LengthCalc == (*Dxdatl_Color).LengthCalc &&
		
		property->Length == 0		
		)
	{
		property->Length = (*Dxdatl_Style).LengthCalc;
		property->Data = calloc(property->Length, sizeof(property_struct));
		if (property->Data == NULL)
		{
			return;
		}
		for (unsigned int i = 0; i < property->Length; i++)
		{
			// Line Color
			(((property_struct*)(property->Data))[i]).LineColor = ((color_struct*)(*Dxdatl_Color).Data)[i];

			// Line Style
			(((property_struct*)(property->Data))[i]).LineType = ((style_type*)(*Dxdatl_Style).Data)[i];

			// Line Thikness
			(((property_struct*)(property->Data))[i]).Thickness = ((thikness_type*)(*Dxdatl_Thickness).Data)[i];
			
			// Fill Style
			(((property_struct*)(property->Data))[i]).Fill = ((fill_type*)(*Dxdatl_Fill).Data)[i];

			// Fill Color
			if (Dxdatl_ColorExt != NULL && property->Length == (*Dxdatl_ColorExt).LengthCalc) // Newer Versions have separate fill and border color
			{
				(((property_struct*)(property->Data))[i]).FillColor = ((color_struct*)(*Dxdatl_ColorExt).Data)[i];
			}
			else
			{
				(((property_struct*)(property->Data))[i]).FillColor = (((property_struct*)(property->Data))[i]).LineColor;
			}
				
		}
	}
	else
	{
		myPrint("Error processing properties!\n");
	}
}

/*
******************************************************************
* - function name:	InitProperty()
*
* - description: 	Initializes property
*
* - parameter: 		element_struct pointer
*
* - return value: 	-
******************************************************************
*/
void InitProperty(element_struct* property)
{
	if (property->Length != 0 && property->Data != NULL)
	{
		free(property->Data);
		property->Data = NULL;
		property->Length = 0;
	}
}

/*
******************************************************************
* - function name:	GetProperty()
*
* - description: 	Returns the selected property
*
* - parameter: 		property index in dxd format (Starting from 1)
*
* - return value: 	property struct
******************************************************************
*/
property_struct GetProperty(element_struct* property, int idx)
{
	if (property->Data != NULL && idx <= property->Length && idx >= 0)
	{
		return ((property_struct*)(property->Data))[idx];
	}
	return (property_struct){0};
}