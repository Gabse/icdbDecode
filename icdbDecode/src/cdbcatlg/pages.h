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
#ifndef _PAGES_H
#define _PAGES_H

/*
******************************************************************
* Global Includes
******************************************************************
*/
#include <stdint.h>			// Required for int32_t, uint32_t, ...
#include "../common.h" 		// Required for uid struct

/*
******************************************************************
* Structures
******************************************************************
*/
typedef struct page_struct
{
	string_struct Name;
	uid_struct UID;
} page_struct;

/*
******************************************************************
* Global Functions
******************************************************************
*/
extern void ProcessPages(void);
extern void InitPages(void);
extern page_struct GetPage(unsigned int);
extern unsigned int GetNumPages(void);

#endif //_PAGES_H