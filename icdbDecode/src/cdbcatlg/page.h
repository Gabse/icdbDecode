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
#ifndef _PAGE_H
#define _PAGE_H

/*
******************************************************************
* Global Includes
******************************************************************
*/
#include <stdint.h>			// Required for int32_t, uint32_t, ...
#include "../common.h" 		// Required for element struct
#include "../uid.h"			// Required for uid_union

/*
******************************************************************
* Structures
******************************************************************
*/
typedef struct page_struct
{
	string_struct Name;
	uid_union UID;
} page_struct;

/*
******************************************************************
* Global Functions
******************************************************************
*/
extern void ProcessPage(element_struct*);
extern void InitPage(element_struct*);
extern page_struct GetPage(element_struct*, unsigned int);

#endif //_PAGE_H