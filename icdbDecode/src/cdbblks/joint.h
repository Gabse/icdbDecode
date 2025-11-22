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
#ifndef _JOINT_H
#define _JOINT_H

/*
******************************************************************
* Global Includes
******************************************************************
*/
#include <stdint.h>					// Required for int32_t, uint32_t, ...
#include "../common.h"				// Required for element_struct
#include "../common/property.h" 	// Required for coordinate struct

/*
******************************************************************
* Structures
******************************************************************
*/

typedef struct joint_struct
{
	coordinate_struct Coord;
	uid_struct UID;
	int ID;
} joint_struct;

/*
******************************************************************
* Global Functions
******************************************************************
*/
extern void ProcessJoint(element_struct*);
extern void InitJoint(element_struct*);
extern joint_struct GetJoint(element_struct*, int);;

#endif //_JOINT_H