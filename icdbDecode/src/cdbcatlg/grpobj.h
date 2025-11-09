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
#ifndef _GRPOBJ_H
#define _GRPOBJ_H

/*
******************************************************************
* Global Includes
******************************************************************
*/
#include <stdint.h>			// Required for int32_t, uint32_t, ...
#include "../common.h" 		// Required for uid struct

/*
******************************************************************
* Global Defines
******************************************************************
*/
#define PATH_GRPOBJ "grpobj"

/*
******************************************************************
* Structures
******************************************************************
*/
typedef struct grpobj_struct
{
	uid_struct UID;
	unsigned int numGroups; 
	unsigned int* group;
} grpobj_struct;

/*
******************************************************************
* Global Functions
******************************************************************
*/
extern int ParseGrpobj(char*, uint32_t, char*, uint32_t);
extern void InitGrpobj(void);
extern unsigned int GetNumGrpobj(void);
extern grpobj_struct GetGrpobj(unsigned int);
extern unsigned int InsideGroup(uid_struct, unsigned int);


#endif //_GRPOBJ_H