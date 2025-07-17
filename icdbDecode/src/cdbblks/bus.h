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
* No static or dynamic code analysis of any proprietary executable files was used to gain information’s about the file format.
*
* This project uses the Zlib library (https://www.zlib.bus/) for decompression.
*/
#ifndef _BUS_H
#define _BUS_H

/*
******************************************************************
* Global Includes
******************************************************************
*/
#include <stdint.h>			// Required for int32_t, uint32_t, ...
#include "label.h"			// Required for label struct
#include "segments.h"		// Required for segment struct
#include "properties.h"		// Required for properties struct

/*
******************************************************************
* Structures
******************************************************************
*/
typedef struct bus_segment_struct
{
	label_struct Label;
	segment_struct Segments;
	properties_struct Properties;
	int Groups;
	int BusID;
} bus_segment_struct;

typedef struct bus_struct
{
	string_struct Name;
	uid_struct UID;
	bus_segment_struct* BusSegment;
	int BusSegmentLen;
	int BusType;
} bus_struct;

/*
******************************************************************
* Global Functions
******************************************************************
*/
extern void ProcessBus(void);
extern void InitBus(void);
extern bus_struct GetBus(int);
extern unsigned int GetNumBus(void);

#endif //_BUS_H