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
#include <stdint.h>			// Required for int32_t, uint32_t, ...
#include "dxdatl.h" 		// Required for ProcessKeyDxdatl 
#include "blkatl.h" 		// Required for ProcessKeyBlkatl
#include "properties.h" 	// Required for ProcessProperties 
#include "textdata.h"		// Required for ProcessTextdatas
#include "joints.h"			// Required for ProcessJoints
#include "segments.h"		// Required for ProcessSegments
#include "label.h"			// Required for ProcessLabel
#include "sheets.h"			// Required for ProcessPages
#include "arcs.h" 			// Required for ProcessArcs
#include "circles.h" 		// Required for ProcessCircles
#include "lines.h" 			// Required for ProcessLines
#include "rectangles.h"		// Required for ProcessRectangles
#include "text.h"			// Required for ProcessTexts
#include "net.h"			// Required for ProcessNet
#include "bus.h"			// Required for ProcessBus

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

	ProcessProperties();	// Must be done before arcs, circles, lines & rect
	ProcessTextdatas();		// Must be done before texts and labels
	ProcessJoints(); 		// Must be done before segments
	ProcessSegments(); 		// Must be done before nets
	ProcessLabels();		// Must be done before nets
	ProcessSheets();
	ProcessArcs();
	ProcessCircles();
	ProcessLines();
	ProcessRectangles();
	ProcessTexts();
	ProcessNet();
	ProcessBus();

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
	InitDxdatl();
	InitBlkatl();

	InitProperties();
	InitTextdatas();
	InitJoints();
	InitSegments();
	InitLabels();
	InitSheets();
	InitArcs();
	InitCircles();
	InitLines();
	InitRectangles();
	InitTexts();
	InitNet();
	InitBus();
}