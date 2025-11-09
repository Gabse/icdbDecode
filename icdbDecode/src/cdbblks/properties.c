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
#include "properties.h"
#include <stdint.h>					// Required for int32_t, uint32_t, ...
#include <stdlib.h>					// Required for calloc to work properly
#include <stdio.h>					// Required for memcpy
#include <string.h>					// Required for memcpy
#include "dxdatl.h"					// Required for keys

/*
******************************************************************
* Defines
******************************************************************
*/

/*
******************************************************************
* Global Variables
******************************************************************
*/
unsigned int property_num = 0;
properties_struct* properties = NULL;

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	ProcessProperties()
*
* - description: 	Processes properties
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void ProcessProperties(void)
{
	if (Dxdatl_Style != NULL &&
		Dxdatl_Fill != NULL &&
		Dxdatl_Thickness != NULL &&
		Dxdatl_Color != NULL &&
		(*Dxdatl_Style).LengthCalc == (*Dxdatl_Fill).LengthCalc &&
		(*Dxdatl_Style).LengthCalc == (*Dxdatl_Thickness).LengthCalc &&
		(*Dxdatl_Style).LengthCalc == (*Dxdatl_Color).LengthCalc &&
		
		property_num == 0		
		)
	{
		property_num = (*Dxdatl_Style).LengthCalc;
		properties = calloc(property_num, sizeof(properties_struct));
		if (properties == NULL)
		{
			return;
		}
		for (unsigned int i = 0; i < property_num; i++)
		{
			// Line Color
			(properties[i]).LineColor = ((color_struct*)(*Dxdatl_Color).Data)[i];

			// Line Style
			(properties[i]).LineType = ((style_type*)(*Dxdatl_Style).Data)[i];

			// Line Thikness
			(properties[i]).Thickness = ((thikness_type*)(*Dxdatl_Thickness).Data)[i];
			
			// Fill Style
			(properties[i]).Fill = ((fill_type*)(*Dxdatl_Fill).Data)[i];

			// Fill Color
			if (Dxdatl_ColorExt != NULL && property_num == (*Dxdatl_ColorExt).LengthCalc) // Newer Versions have separate fill and border color
			{
				(properties[i]).FillColor = ((color_struct*)(*Dxdatl_ColorExt).Data)[i];
			}
			else
			{
				(properties[i]).FillColor = (properties[i]).LineColor;
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
* - function name:	InitProperties()
*
* - description: 	Initializes Styles
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void InitProperties(void)
{
	if (property_num != 0 && properties != NULL)
	{
		free(properties);
		property_num = 0;
	}
}
/*
******************************************************************
* - function name:	GetNumProperties()
*
* - description: 	Returns the number of style lines in this file
*
* - parameter: 		-
*
* - return value: 	number of properties lines in this file
******************************************************************
*/
unsigned int GetNumProperties(void)
{
	return property_num;
}

/*
******************************************************************
* - function name:	GetProperty()
*
* - description: 	Returns the selected style
*
* - parameter: 		style index in dxd format (Starting from 1)
*
* - return value: 	style struct
******************************************************************
*/
properties_struct GetProperty(int idx)
{
	if (properties != NULL && idx <= property_num && idx >= 0)
	{
		return properties[idx];
	}
	return (properties_struct){0};
}