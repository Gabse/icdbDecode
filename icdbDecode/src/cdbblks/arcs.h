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
#ifndef _ARCS_H
#define _ARCS_H

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
typedef struct arc_struct
{
	coordinate_struct StartCoord;
	coordinate_struct EndCoord;
	coordinate_struct MidCoord;
	properties_struct Properties;
	uid_struct UID;
	int IDDxD;
} arc_struct;

/*
******************************************************************
* Global Functions
******************************************************************
*/
extern void ProcessArcs(void);
extern void InitArcs(void);
extern arc_struct GetArc(int);
extern unsigned int GetNumArcs(void);

#endif //_ARCS_H