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
#ifndef _RECTANGLE_H
#define _RECTANGLE_H

/*
******************************************************************
* Global Includes
******************************************************************
*/
#include <stdint.h>		// Required for int32_t, uint32_t, ...
#include "../common.h"	// Required for element_struct
#include "../uid.h"		// Required for uid_union
#include "property.h"	// Required for property struct

/*
******************************************************************
* Structures
******************************************************************
*/

typedef struct rectcoord_struct
{
	coordinate_struct StartCoord;
	coordinate_struct EndCoord;
} rectcoord_struct;


typedef struct rectangle_struct
{
	coordinate_struct StartCoord;
	coordinate_struct EndCoord;
	property_struct Property;
	uid_union UID;
	int IDDxD;
} rectangle_struct;

/*
******************************************************************
* Global Functions
******************************************************************
*/
extern void ProcessRectangle(element_struct*, element_struct*);
extern void InitRectangle(element_struct*);
extern rectangle_struct GetRectangle(element_struct*, int);

#endif //_RECTANGLE_H