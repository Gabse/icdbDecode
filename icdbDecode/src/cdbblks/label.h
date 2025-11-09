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
#ifndef _LABEL_H
#define _LABEL_H

/*
******************************************************************
* Global Includes
******************************************************************
*/
#include <stdint.h>			// Required for int32_t, uint32_t, ...
#include "textdata.h" 		// Required for textdata struct

/*
******************************************************************
* Enums
******************************************************************
*/
#if C23
typedef enum visibility_type : uint8_t
{
#else
typedef enum visibility_type
{
#endif
	visibility_invissible	= 1,
	visibility_vissible		= 4,
	visibility_default		= 255,
}visibility_type;

/*
******************************************************************
* Structures
******************************************************************
*/
typedef struct sublabel_struct
{
	textdata_struct TextData;
	int Inverted;
	int Scope;
	int SegmentNum;
	visibility_type Visibility;
} sublabel_struct;

typedef struct label_struct
{
	int IDDXD;
	sublabel_struct* Sublable;
	int SublableNum;
	uid_struct IndexDxDNet;
} label_struct;

/*
******************************************************************
* Global Functions
******************************************************************
*/
extern void ProcessLabels(void);
extern void InitLabels(void);
extern label_struct GetLabel(int);
extern unsigned int GetNumLabels(void);

#endif //_LABEL_H