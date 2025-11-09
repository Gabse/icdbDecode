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
#ifndef _RECTANGLES_H
#define _RECTANGLES_H

/*
******************************************************************
* Global Includes
******************************************************************
*/
#include <stdint.h>			// Required for int32_t, uint32_t, ...
#include "cdbblks.h" 		// Required for coordinate struct
#include "properties.h" 	// Required for properties struct

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
	properties_struct Properties;
	uid_struct UID;
	int IDDxD;
} rectangle_struct;

/*
******************************************************************
* Global Functions
******************************************************************
*/
extern void ProcessRectangles(void);
extern void InitRectangles(void);
extern rectangle_struct GetRectangle(int);
extern unsigned int GetNumRectangles(void);

#endif //_RECTANGLES_H