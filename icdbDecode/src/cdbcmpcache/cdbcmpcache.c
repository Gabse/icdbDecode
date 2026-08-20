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

/*
******************************************************************
* Includes
******************************************************************
*/
#include "cdbcmpcache.h"
#include <stdint.h>					// Required for int32_t, uint32_t, ...
#include "../common/dxdatl.h" 		// Required for ProcessKeyDxdatl
#include "cmpatl.h" 				// Required for ProcessKeyCmpatl
#include "../common.h" 				// Required for element_struct
#include "../common/property.h" 	// Required for ProcessProperty
#include "../common/textdata.h"		// Required for ProcessTextdata
#include "../common/label.h"		// Required for ProcessLabel
#include "../common/arc.h" 			// Required for ProcessArc
#include "../common/circle.h" 		// Required for ProcessCircle
#include "../common/line.h" 		// Required for ProcessLine
#include "../common/rectangle.h"	// Required for ProcessRectangle
#include "../common/text.h"			// Required for ProcessText

/*
******************************************************************
* Global Variables
******************************************************************
*/
element_struct cdbcmpcache_arc = { 0, NULL };
element_struct cdbcmpcache_circle = { 0, NULL };
element_struct cdbcmpcache_label = { 0, NULL };
element_struct cdbcmpcache_line = { 0, NULL };
element_struct cdbcmpcache_property = { 0, NULL };
element_struct cdbcmpcache_rectangle = { 0, NULL };
element_struct cdbcmpcache_text = { 0, NULL };
element_struct cdbcmpcache_textdata = { 0, NULL };

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	parseCdbcmpcache()
*
* - description: 	Cdbcmpcache Parser code
*
* - parameter: 		source path string, string length
*
* - return value: 	error code
******************************************************************
*/
int parseCdbcmpcache(char* path, uint32_t pathlength)
{
	initCdbcmpcache(); // Prevent memory leak
	int errorcode = 0;
	errorcode |= parseFile(path, pathlength, PATH_DXDATL, sizeof(PATH_DXDATL), ProcessKeyDxdatl);
	errorcode |= parseFile(path, pathlength, PATH_CMPATL, sizeof(PATH_CMPATL), ProcessKeyCmpatl);

	if(!errorcode)
	{
		ProcessProperty(&cdbcmpcache_property);	// Must be done before arc, circle, line & rect
		ProcessTextdata(&cdbcmpcache_textdata);		// Must be done before text and label
		ProcessLabel(&cdbcmpcache_label, &cdbcmpcache_textdata);
		ProcessArc(&cdbcmpcache_property, &cdbcmpcache_arc);
		ProcessCircle(&cdbcmpcache_property, &cdbcmpcache_circle);
		ProcessLine(&cdbcmpcache_property, &cdbcmpcache_line);
		ProcessRectangle(&cdbcmpcache_property, &cdbcmpcache_rectangle);
		ProcessText(&cdbcmpcache_text, &cdbcmpcache_textdata);
	}

	InitDxdatl();
	InitCmpatl();
	return errorcode;
}

/*
******************************************************************
* - function name:	initCdbcmpcache()
*
* - description: 	Cdbcmpcache Parser init code
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void initCdbcmpcache(void)
{
	InitProperty(&cdbcmpcache_property);
	InitTextdata(&cdbcmpcache_textdata);
	InitLabel(&cdbcmpcache_label);
	InitArc(&cdbcmpcache_arc);
	InitCircle(&cdbcmpcache_circle);
	InitLine(&cdbcmpcache_line);
	InitRectangle(&cdbcmpcache_rectangle);
	InitText(&cdbcmpcache_text);
}