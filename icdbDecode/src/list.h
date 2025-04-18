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
#ifndef _LIST_H
#define _LIST_H

/*
******************************************************************
* Global Variables
******************************************************************
*/
void* list_init();
void* list_address(void*, unsigned int);
unsigned int list_size(void*);
unsigned int list_elements(void*);
unsigned int list_append(void*, void*, unsigned int);
void list_add(void*, unsigned int, void*, unsigned int);
void list_remove(void*, unsigned int);
unsigned int list_get(void*, unsigned int, void**);
void list_cleanup(void**);
unsigned int list_to_memblk(void* , void**);

/*
******************************************************************
* Global Functions
******************************************************************
*/


#endif //_LIST_H