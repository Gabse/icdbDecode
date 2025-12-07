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
element_struct cdbcmpcach_circle = { 0, NULL };
element_struct cdbcmpcach_label = { 0, NULL };
element_struct cdbcmpcach_line = { 0, NULL };
element_struct cdbcmpcach_property = { 0, NULL };
element_struct cdbcmpcach_rectangle = { 0, NULL };
element_struct cdbcmpcach_text = { 0, NULL };
element_struct cdbcmpcach_textdata = { 0, NULL };

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
		ProcessProperty(&cdbcmpcach_property);	// Must be done before arc, circle, line & rect
		ProcessTextdata(&cdbcmpcach_textdata);		// Must be done before text and label
		ProcessLabel(&cdbcmpcach_label, &cdbcmpcach_textdata);
		ProcessArc(&cdbcmpcach_property, &cdbcmpcache_arc);
		ProcessCircle(&cdbcmpcach_property, &cdbcmpcach_circle);
		ProcessLine(&cdbcmpcach_property, &cdbcmpcach_line);
		ProcessRectangle(&cdbcmpcach_property, &cdbcmpcach_rectangle);
		ProcessText(&cdbcmpcach_text, &cdbcmpcach_textdata);
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
	InitProperty(&cdbcmpcach_property);
	InitTextdata(&cdbcmpcach_textdata);
	InitLabel(&cdbcmpcach_label);
	InitArc(&cdbcmpcache_arc);
	InitCircle(&cdbcmpcach_circle);
	InitLine(&cdbcmpcach_line);
	InitRectangle(&cdbcmpcach_rectangle);
	InitText(&cdbcmpcach_text);
}