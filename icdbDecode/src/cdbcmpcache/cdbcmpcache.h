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
#ifndef _CDBCMPCACHE_H
#define _CDBCMPCACHE_H

/*
******************************************************************
* Includes
******************************************************************
*/
#include <stdint.h>		// Required for int32_t, uint32_t, ...
#include "../common.h"	// Required for string_struct

/*
******************************************************************
* Global Variables
******************************************************************
*/
extern element_struct cdbcmpcache_arc;
extern element_struct cdbcmpcach_circle;
extern element_struct cdbcmpcach_label;
extern element_struct cdbcmpcach_line;
extern element_struct cdbcmpcach_property;
extern element_struct cdbcmpcach_rectangle;
extern element_struct cdbcmpcach_text;
extern element_struct cdbcmpcach_textdata;

/*
******************************************************************
* Global Functions
******************************************************************
*/
int parseCdbcmpcache(char*, uint32_t);
void initCdbcmpcache(void);

#endif //_CDBCMPCACHE_H