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
#include "cdbblks.h"
#include <stdint.h>					// Required for int32_t, uint32_t, ...
#include "../common.h" 				// Required for element_struct
#include "../common/dxdatl.h" 		// Required for ProcessKeyDxdatl
#include "blkatl.h" 				// Required for ProcessKeyBlkatl
#include "../common/property.h" 	// Required for ProcessProperty
#include "../common/textdata.h"		// Required for ProcessTextdata
#include "joint.h"					// Required for ProcessJoint
#include "segment.h"				// Required for ProcessSegment
#include "../common/label.h"		// Required for ProcessLabel
#include "sheet.h"					// Required for ProcessPage
#include "../common/arc.h" 			// Required for ProcessArc
#include "../common/circle.h" 		// Required for ProcessCircle
#include "../common/line.h" 		// Required for ProcessLine
#include "../common/rectangle.h"	// Required for ProcessRectangle
#include "../common/text.h"			// Required for ProcessText
#include "net.h"					// Required for ProcessNet
#include "bus.h"					// Required for ProcessBus

/*
******************************************************************
* Global Variables
******************************************************************
*/
element_struct cdbblks_arc = { 0, NULL };
element_struct cdbblks_circle = { 0, NULL };
element_struct cdbblks_label = { 0, NULL };
element_struct cdbblks_line = { 0, NULL };
element_struct cdbblks_property = { 0, NULL };
element_struct cdbblks_rectangle = { 0, NULL };
element_struct cdbblks_text = { 0, NULL };
element_struct cdbblks_textdata = { 0, NULL };
element_struct cdbblks_bus = { 0, NULL };
element_struct cdbblks_joint = { 0, NULL };
element_struct cdbblks_net = { 0, NULL };
element_struct cdbblks_segment = { 0, NULL };
element_struct cdbblks_sheet = { 0, NULL };

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	parseCdbblks()
*
* - description: 	Cdbblks Parser code
*
* - parameter: 		source path string, string length
*
* - return value: 	error code
******************************************************************
*/
int parseCdbblks(char* path, uint32_t pathlength)
{
	initCdbblks(); // Prevent memory leak
	int errorcode = 0;
	errorcode |= parseFile(path, pathlength, PATH_DXDATL, sizeof(PATH_DXDATL), ProcessKeyDxdatl);
	errorcode |= parseFile(path, pathlength, PATH_BLKATL, sizeof(PATH_BLKATL), ProcessKeyBlkatl);
	
	if(!errorcode)
	{
		ProcessProperty(&cdbblks_property);					// Must be done before arc, circle, line & rect
		ProcessTextdata(&cdbblks_textdata);					// Must be done before text and label
		ProcessJoint(&cdbblks_joint); 						// Must be done before segment
		ProcessSegment(&cdbblks_segment); 					// Must be done before nets
		ProcessLabel(&cdbblks_label, &cdbblks_textdata);	// Must be done before nets
		ProcessSheet(&cdbblks_sheet);
		ProcessArc(&cdbblks_property, &cdbblks_arc);
		ProcessCircle(&cdbblks_property, &cdbblks_circle);
		ProcessLine(&cdbblks_property, &cdbblks_line);
		ProcessRectangle(&cdbblks_property, &cdbblks_rectangle);
		ProcessText(&cdbblks_text, &cdbblks_textdata);
		ProcessNet(&cdbblks_net);
		ProcessBus(&cdbblks_bus);
	}

	InitDxdatl();
	InitBlkatl();
	return errorcode;
}
/*
******************************************************************
* - function name:	initCdbblks()
*
* - description: 	Cdbblks Parser init code
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void initCdbblks(void)
{

	InitProperty(&cdbblks_property);
	InitTextdata(&cdbblks_textdata);
	InitJoint(&cdbblks_joint);
	InitSegment(&cdbblks_segment);
	InitLabel(&cdbblks_label);
	InitSheet(&cdbblks_sheet);
	InitArc(&cdbblks_arc);
	InitCircle(&cdbblks_circle);
	InitLine(&cdbblks_line);
	InitRectangle(&cdbblks_rectangle);
	InitText(&cdbblks_text);
	InitNet(&cdbblks_net);
	InitBus(&cdbblks_bus);
}