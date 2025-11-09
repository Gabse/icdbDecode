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
#ifndef _NET_H
#define _NET_H

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
typedef struct net_segment_struct
{
	label_struct Label;
	segment_struct Segments;
	properties_struct Properties;
} net_segment_struct;

typedef struct net_struct
{
	int Net;
	uid_struct UID;
	int NetID;
	string_struct Name;
	int NumNetSegment;
	net_segment_struct* NetSegment;
} net_struct;


/*
******************************************************************
* Global Functions
******************************************************************
*/
extern void ProcessNet(void);
extern void InitNet(void);
extern net_struct GetNet(int);
extern unsigned int GetNumNet(void);

#endif //_NET_H