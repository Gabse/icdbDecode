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
* This project uses the Zlib library (https://www.zlib.net/) for decompression.
*/
#ifndef _STRINGUTIL_H
#define _STRINGUTIL_H

/*
******************************************************************
* Global Includes
******************************************************************
*/
#include <stdint.h>		// Required for int32_t, uint32_t, ...

/*
******************************************************************
* Global Defines
******************************************************************
*/

/*
******************************************************************
* Global Functions
******************************************************************
*/
extern uint32_t addStrings(char**, char*, uint32_t, char*, uint32_t, char);
extern uint32_t assemblePath(char**, char*, uint32_t, char*, uint32_t, char);
extern void removeFileEnding(char* , uint32_t*);
extern unsigned int removeFilePath(char*, unsigned int, char**);
extern uint32_t createPath(char**, char*, uint32_t, char*, uint32_t, char);
extern char* stringSmall(char*, unsigned int);
extern char* stringBig(char*, unsigned int);

#endif //_STRINGUTIL_H