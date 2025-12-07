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
#ifndef _KICAD_SHARED_H
#define _KICAD_SHARED_H

/*
******************************************************************
* Global Includes
******************************************************************
*/
#include <stdint.h>				// Required for int32_t, uint32_t, ...
#include "../common.h"			// Required for element_struct
#include "../uid.h"				// Required for uid_union
#include "../common/label.h"	// Required for label_struct
#include "../common/textdata.h"	// Required for textdata_struct
#include "../cdbcatlg/page.h"	// Required for page
#include "../common/property.h" // Required for property

/*
******************************************************************
* Global Defines
******************************************************************
*/
#define UserCoordinateOffsetX 0
#define UserCoordinateOffsetY 0
#define UserCoordinateScaleX 1.0
#define UserCoordinateScaleY 1.0
#define UserDefaultTextOrigin 3
#define UserFontScale 0.5  // Fonts in KiCad are rendered ~0.5 the size from DxD. Exact scaling differs between fonts.
#define UserBaseLineThickness 0.1
#define NewKiCad 0 // Create Kicad for 9.99 required for filling pattern
#define KiCadFileEnding ".kicad_sch" // File ending for exported KiCad schematic file

/*
******************************************************************
* Global Variables
******************************************************************
*/
extern int CoordinateOffsetX;
extern int CoordinateOffsetY;
extern int CoordinateScaleX;
extern int CoordinateScaleY;
extern int DefaultTextOrigin;
extern float FontScale;
extern float BaseLineThickness;

/*
******************************************************************
* Global Functions
******************************************************************
*/
extern void KiCadProperty(FILE*, property_struct, uint8_t);
extern void KiCadTextData(FILE*, textdata_struct);
extern void KiCadUID(FILE*, uid_union, uid_union);
extern void KiCadPrintString(FILE*, string_struct);
extern void KiCadLabel(FILE*, uid_union, label_struct, string_struct);
extern void KiCadArc(FILE*, uid_union, uint32_t);
extern void KiCadCircle(FILE*, uid_union, uint32_t);
extern void KiCadRectangle(FILE*, uid_union, uint32_t);
extern void KiCadText(FILE*, uid_union, uint32_t);
extern void KiCadLine(FILE*, uid_union, uint32_t);


#endif //_KICAD_SHARED_H