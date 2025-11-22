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
#ifndef _CIRCLE_H
#define _CIRCLE_H

/*
******************************************************************
* Global Includes
******************************************************************
*/
#include <stdint.h>		// Required for int32_t, uint32_t, ...
#include "../common.h"	// Required for element_struct
#include "property.h"	// Required for property struct

/*
******************************************************************
* Structures
******************************************************************
*/

typedef struct circle_struct
{
	coordinate_struct CenterCoord;
	int Radius;
	property_struct Property;
	uid_struct UID;
	int IDDxD;
} circle_struct;

/*
******************************************************************
* Global Functions
******************************************************************
*/
extern void ProcessCircle(element_struct*, element_struct*);
extern void InitCircle(element_struct*);
extern circle_struct GetCircle(element_struct*, int);

#endif //_CIRCLE_H