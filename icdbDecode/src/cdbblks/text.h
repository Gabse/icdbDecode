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
* No static or dynamic code analysis of any proprietary executable files was used to gain informationís about the file format.
*
* This project uses the Zlib library (https://www.zlib.net/) for decompression.
*/
#ifndef _TEXT_H
#define _TEXT_H

/*
******************************************************************
* Global Includes
******************************************************************
*/
#include <stdint.h>			// Required for int32_t, uint32_t, ...
#include "textdata.h" 		// Required for textdata struct

/*
******************************************************************
* Structures
******************************************************************
*/
typedef struct text_struct
{
	string_struct String;
	textdata_struct TextData;
	uid_struct UID;
} text_struct;

/*
******************************************************************
* Global Functions
******************************************************************
*/
extern void ProcessTexts(void);
extern void InitTexts(void);
extern text_struct GetText(int);
extern unsigned int GetNumTexts(void);

#endif //_TEXT_H