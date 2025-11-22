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
#ifndef _ARC_H
#define _ARC_H

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
typedef struct arc_struct
{
	coordinate_struct StartCoord;
	coordinate_struct EndCoord;
	coordinate_struct MidCoord;
	property_struct Property;
	uid_struct UID;
	int IDDxD;
} arc_struct;

/*
******************************************************************
* Global Functions
******************************************************************
*/
extern void ProcessArc(element_struct*);
extern void InitArc(element_struct*);
extern arc_struct GetArc(element_struct*, int);

#endif //_ARC_H