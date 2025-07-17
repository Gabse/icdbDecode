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
* No static or dynamic code analysis of any proprietary executable files was used to gain information�s about the file format.
*
* This project uses the Zlib library (https://www.zlib.net/) for decompression.
*/

/*
******************************************************************
* Includes
******************************************************************
*/
#include "cdbcatlg.h"
#include <stdint.h>			// Required for int32_t, uint32_t, ...
#include "catlgatl.h" 		// Required for ProcessKeyCatlgatl 
#include "grpatl.h" 		// Required for ProcessKeyGrpatl
#include "grpobj.h" 		// Required for ParseGrpobj
#include "pages.h" 			// Required for ParsePages
#include "groups.h" 		// Required for ParseGroups

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	parseCdbcatlg()
*
* - description: 	Cdbcatlg Parser code
*
* - parameter: 		source path string, string length
*
* - return value: 	error code
******************************************************************
*/
int parseCdbcatlg(char* path, uint32_t pathlength)
{
	initCdbcatlg(); // Prevent memory leak
	int errorcode = 0;
	errorcode |= parseFile(path, pathlength, PATH_CATLGATL, sizeof(PATH_CATLGATL), ProcessKeyCatlgatl);
	errorcode |= parseFile(path, pathlength, PATH_GRPATL, sizeof(PATH_GRPATL), ProcessKeyGrpatl);
	errorcode |= ParseGrpobj(path, pathlength, PATH_GRPOBJ, sizeof(PATH_GRPOBJ));

	ProcessPages();
	ProcessGroups();
	return errorcode;
}
/*
******************************************************************
* - function name:	initCdbcatlg()
*
* - description: 	Cdbcatlg Parser init code
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void initCdbcatlg(void)
{
	InitCatlgatl();
	InitGrpatl();
	InitGrpobj();

	InitPages();
	InitGroups();
}