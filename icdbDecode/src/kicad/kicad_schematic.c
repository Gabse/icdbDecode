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
#include "kicad_schematic.h"
#include <stdio.h>					// Required for fprint, fopen, ...
#include <stdint.h>					// Required for int32_t, uint32_t, ...
#include <stdlib.h>					// Required for calloc to work properly
#include "../common.h"				// Required for myfopen
#include "../stringutil.h"			// Required for string manipulation
#include "../cdbcatlg/cdbcatlg.h"	// Required for cdbcatlg
#include "../cdbcatlg/page.h"		// Required for page
#include "../cdbcatlg/group.h"		// Required for group
#include "../cdbcatlg/grpobj.h"		// Required for group
#include "../cdbblks/cdbblks.h"		// Required for cdbblks
#include "../cdbblks/sheet.h"		// Required for sheet
#include "../cdbblks/net.h"			// Required for net
#include "../cdbblks/bus.h"			// Required for bus
#include "../common/property.h" 	// Required for property
#include "../common/label.h"		// Required for label
#include "../common/arc.h"			// Required for arc
#include "../common/circle.h"		// Required for circle
#include "../common/rectangle.h"	// Required for rectangle
#include "../common/text.h"			// Required for text
#include "../common/line.h"			// Required for line


/*
******************************************************************
* Defines
******************************************************************
*/
#define _CRT_SECURE_NO_DEPRECATE

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
* Local Function Prototype
******************************************************************
*/
void KiCadProperty(FILE*, property_struct, uint8_t);
void KiCadSheetProp(FILE*, sheet_struct);
void KiCadTextData(FILE*, textdata_struct);
void KiCadUID(FILE*, uid_struct, uid_struct);
void KiCadPrintString(FILE*, string_struct);
void KiCadLabel(FILE*, uid_struct, label_struct, string_struct);
void KiCadArc(FILE*, uid_struct, uint32_t);
void KiCadCircle(FILE*, uid_struct, uint32_t);
void KiCadRectangle(FILE*, uid_struct, uint32_t);
void KiCadText(FILE*, uid_struct, uint32_t);
void KiCadLine(FILE*, uid_struct, uint32_t);
void KiCadNets(FILE*, uid_struct, uint32_t);
void KiCadBusses(FILE*, uid_struct, uint32_t);

/*
******************************************************************
* Global Variables
******************************************************************
*/
int CoordinateOffsetX	= UserCoordinateOffsetX;
int CoordinateOffsetY	= UserCoordinateOffsetY;
int CoordinateScaleX	= UserCoordinateScaleX;
int CoordinateScaleY	= UserCoordinateScaleY;
int DefaultTextOrigin	= UserDefaultTextOrigin;
float FontScale			= UserFontScale;
float BaseLineThickness = UserBaseLineThickness;

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	StoreAsKicadSchematic()
*
* - description: 	Stores the parsed data as KiCad file
*
* - parameter: 		source path string; string length; file name string; file name length
*
* - return value: 	errorcode
******************************************************************
*/
int StoreAsKicadSchematic(char* path, uint32_t pathlength, page_struct page)
{
	char* destination = NULL;
	uid_struct temp = { 0 };
	for (unsigned int i = 0; i < cdbblks_sheet.Length; i++)
	{
		sheet_struct Sheet = GetSheet(&cdbblks_sheet, i);
		uint32_t completePathLength;

		if (cdbblks_sheet.Length > 1) // Schematic name + sheet number + sheet name
		{
			group_struct group = GetGroup(&cdbcatlg_group, i);
			char* TempDest1 = NULL;
			char* TempDest2 = malloc(10);
			if (TempDest2 == NULL)
			{
				return -1;
			}
			sprintf(TempDest2, "%d", i + 1);
			uint32_t completePathLengthTemp = addStrings(&TempDest1, page.Name.Text, page.Name.Length, TempDest2, stringLen(TempDest2, 10), '_');
			completePathLengthTemp = addStrings(&TempDest2, TempDest1, completePathLengthTemp, group.Name.Text, group.Name.Length, '_');
			completePathLength = addStrings(&destination, TempDest2, completePathLengthTemp, KiCadFileEnding, sizeof(KiCadFileEnding), '\0');
			free(TempDest1);
			free(TempDest2);
		}
		else // Schematic name only
		{
			completePathLength = addStrings(&destination, page.Name.Text, page.Name.Length, KiCadFileEnding, sizeof(KiCadFileEnding), '\0');
		}

		// Open file
		FILE* KiCadFile = myfopen("w", path, pathlength, destination, completePathLength, DIR_SEPARATOR);
		if (KiCadFile != 0)
		{
			myPrint("\n--------------- Writing KiCad file ---------------\n");
			myPrint("File: [%s]\n", destination);
			// File Header
			fprintf(KiCadFile, "(kicad_sch\n");
#if NewKiCad
#pragma message ("Building for KiCad 9.99")
			fprintf(KiCadFile, "\t(version 20251012)\n");
			fprintf(KiCadFile, "\t(generator \"eeschema\")\n");
			fprintf(KiCadFile, "\t(generator_version \"9.99\")\n");
#else
#pragma message ("Building for KiCad 9.0")
			fprintf(KiCadFile, "\t(version 20250114)\n");
			fprintf(KiCadFile, "\t(generator \"eeschema\")\n");
			fprintf(KiCadFile, "\t(generator_version \"9.0\")\n");
#endif
			fprintf(KiCadFile, "\t");
			KiCadUID(KiCadFile, page.UID, temp);
			KiCadSheetProp(KiCadFile, Sheet);
			fprintf(KiCadFile, "\t(lib_symbols)\n");
			myPrint("\n");

			// Elements
			KiCadArc(KiCadFile, page.UID, Sheet.Group);
			KiCadCircle(KiCadFile, page.UID, Sheet.Group);
			KiCadRectangle(KiCadFile, page.UID, Sheet.Group);
			KiCadText(KiCadFile, page.UID, Sheet.Group);
			KiCadLine(KiCadFile, page.UID, Sheet.Group);
			KiCadNets(KiCadFile, page.UID, Sheet.Group);
			KiCadBusses(KiCadFile, page.UID, Sheet.Group);

			// File Leader
			fprintf(KiCadFile, "\t(sheet_instances\n");
			fprintf(KiCadFile, "\t\t(path \"/\"\n");
			fprintf(KiCadFile, "\t\t\t(page \"1\")\n");
			fprintf(KiCadFile, "\t\t)\n");
			fprintf(KiCadFile, "\t)\n");

			fprintf(KiCadFile, ")\n");
			fclose(KiCadFile);
			free(destination);

		}
		else
		{
			myPrint("Error Writing [%s] !\n", destination);
			free(destination);
			return -1;
		}
	}
	return 0;
}

/*
******************************************************************
* Local Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	KiCadProperty()
*
* - description: 	Stores property in KiCad
*
* - parameter: 		Pointer to KiCad file; Index of Property; 1 for filled shape (Rect, Circle, ..), 0 for not filled shape (Line, ...)
*
* - return value: 	-
******************************************************************
*/
void KiCadProperty(FILE* KiCadFile, property_struct Property, uint8_t Filling)
{
	fprintf(KiCadFile, "\t\t(stroke\n");

	// Linewidth
	if (Property.Thickness == thikness_Auto)
	{
		fprintf(KiCadFile, "\t\t\t(width 0)\n");
		myPrint("\tLinewidth: Default\n");
	}
	else
	{
		fprintf(KiCadFile, "\t\t\t(width %f)\n", (float)Property.Thickness * BaseLineThickness);
		myPrint("\tLinewidth: %f\n", (float)Property.Thickness * BaseLineThickness);
	}

	// Linestyle
	switch (Property.LineType)
	{
	case style_AutoSolid: // Solid (Automatic)
		myPrint("\tLinestyle: Default\n");
		fprintf(KiCadFile, "\t\t\t(type default)\n");
		break;
	case style_Solid: // Solid
		myPrint("\tLinestyle: Solid\n");
		fprintf(KiCadFile, "\t\t\t(type solid)\n");
		break;
	case style_Dash: // Dash
	case style_Mediumdash: // Medium dash
	case style_Bigdash: // Big dash
		myPrint("\tLinestyle: Dash\n");
		fprintf(KiCadFile, "\t\t\t(type dash)\n");
		break;
	case style_Center: // Center
	case style_DashDot: // Dash-Dot
		myPrint("\tLinestyle: Dash-Dot\n");
		fprintf(KiCadFile, "\t\t\t(type dash_dot)\n");
		break;
	case style_Phantom: // Phantom
		myPrint("\tLinestyle: Dash-Dot-Dot\n");
		fprintf(KiCadFile, "\t\t\t(type dash_dot_dot)\n");
		break;
	case style_Dot: // Dot
		myPrint("\tLinestyle: Dot\n");
		fprintf(KiCadFile, "\t\t\t(type dot)\n");
		break;
	default:
		break;
	}

	// Color
	if (Property.LineColor.Key == colorkey_default)
	{ // Default Color
		myPrint("\tColor: Default\n");
	}
	else
	{ // Custom Color
		fprintf(KiCadFile, "\t\t\t(color %d %d %d 1)\n", Property.LineColor.Red, Property.LineColor.Green, Property.LineColor.Blue);
		myPrint("\tColor: R:%d G:%d B:%d\n", Property.LineColor.Red, Property.LineColor.Green, Property.LineColor.Blue);
	}
	fprintf(KiCadFile, "\t\t)\n");

	// Filling style
	if (Filling)
	{
		int8_t opacity = -1;
#if NewKiCad
		fprintf(KiCadFile, "\t\t(fill\n");
		switch (Property.Fill)
		{
		case fill_AutoHollow: // Hollow (Automatic)
		case fill_Hollow: // Hollow
			fprintf(KiCadFile, "\t\t\t(type none)\n");
			myPrint("\tFill : None\n");
			break;

		case fill_Solid: // Solid
			fprintf(KiCadFile, "\t\t\t(type color)\n");
			myPrint("\tFill: Solid\n");
			opacity = 100;
			break;

		case fill_Diagdn1: // Diagdn1
		case fill_Diagdn2: // Diagdn2
			fprintf(KiCadFile, "\t\t\t(type reverse_hatch)\n");
			myPrint("\tFill: Reverse Hatch\n");
			opacity = 100;
			break;

		case fill_Diagup1: // Diagup1
		case fill_Diagup2: // Diagup2
			fprintf(KiCadFile, "\t\t\t(type hatch)\n");
			myPrint("\tFill: Hatch\n");
			opacity = 100;
			break;
			
		case fill_X1: // X1
		case fill_X2: // X2
			fprintf(KiCadFile, "\t\t\t(type cross_hatch)\n");
			myPrint("\tFill: Cross Hatch\n");
			opacity = 100;
			break;

		case fill_Horiz: // Horiz
		case fill_Vert: // Vert
			fprintf(KiCadFile, "\t\t\t(type color)\n");
			myPrint("\tFill: Solid\n");
			opacity = 8;
			break;

		case fill_Grid1: // Grid1
		case fill_Grid2: // Grid2
			fprintf(KiCadFile, "\t\t\t(type color)\n");
			myPrint("\tFill: Solid\n");
			opacity = 16;
			break;

		case fill_Grey04: // Grey04
			fprintf(KiCadFile, "\t\t\t(type color)\n");
			myPrint("\tFill: Solid\n");
			opacity = 4;
			break;

		case fill_Grey08: // Grey08
			fprintf(KiCadFile, "\t\t\t(type color)\n");
			myPrint("\tFill: Solid\n");
			opacity = 8;
			break;

		case fill_Grey50: // Grey50
			fprintf(KiCadFile, "\t\t\t(type color)\n");
			myPrint("\tFill: Solid\n");
			opacity = 50;
			break;

		case fill_Grey92: // Grey92
			fprintf(KiCadFile, "\t\t\t(type color)\n");
			myPrint("\tFill: Solid\n");
			opacity = 92;
			break;
		default:
			break;
		}
#else
		// Old KiCad has no fill pattern. Translate into fill opacity
		fprintf(KiCadFile, "\t\t(fill\n");
		switch (Property.Fill)
		{
		case fill_AutoHollow: // Hollow (Automatic)
		case fill_Hollow: // Hollow
			break;
		case fill_Solid: // Solid
			opacity = 100;
			break;
		case fill_Diagup1: // Diagup1
		case fill_Diagup2: // Diagup2
		case fill_Diagdn1: // Diagdn1
		case fill_Diagdn2: // Diagdn2
		case fill_Horiz: // Horiz
		case fill_Vert: // Vert
		case fill_Grey08: // Grey08
			opacity = 8;
			break;
		case fill_Grid1: // Grid1
		case fill_Grid2: // Grid2
		case fill_X1: // X1
		case fill_X2: // X2
			opacity = 16;
			break;
		case fill_Grey04: // Grey04
			opacity = 4;
			break;
		case fill_Grey50: // Grey50
			opacity = 50;
			break;
		case fill_Grey92: // Grey92
			opacity = 92;
			break;
		default:
			break;
		}
#endif
		if (opacity == -1)
		{
			fprintf(KiCadFile, "\t\t\t(type none)\n");
			myPrint("\tFill: None\n");
		}
		else
		{
			fprintf(KiCadFile, "\t\t\t(type color)\n");
			if (Property.FillColor.Key == colorkey_default)
			{ // Default Color
				myPrint("\tFill: %d%c\n", opacity, 0x25);
				myPrint("\tColor: Default\n");
				fprintf(KiCadFile, "\t\t\t(color 0 0 0 %.2f)\n", ((float)opacity / 100.0));
			}
			else
			{ // Custom Color
				fprintf(KiCadFile, "\t\t\t(color %d %d %d %.2f)\n", Property.FillColor.Red, Property.FillColor.Green, Property.FillColor.Blue, ((float)opacity / 100.0));
				myPrint("\tFill: %d%c\n", opacity, 0x25);
				myPrint("\tColor: R:%d G:%d B:%d\n", Property.FillColor.Red, Property.FillColor.Green, Property.FillColor.Blue);
			}
		}
		fprintf(KiCadFile, "\t\t)\n");
	}
}

/*
******************************************************************
* - function name:	KiCadPageProp()
*
* - description: 	Stores page property in KiCad
*
* - parameter: 		Pointer to KiCad file; Index of Sheet Property
*
* - return value: 	-
******************************************************************
*/
void KiCadSheetProp(FILE* KiCadFile, sheet_struct Sheet)
{
	fprintf(KiCadFile, "\t(paper \"");
	myPrint("Paper Size: ");
	switch (Sheet.SizeCode)
	{
	case sheetsize_A:
		fprintf(KiCadFile, "A\"");
		myPrint("A");
		break;
	case sheetsize_B:
		fprintf(KiCadFile, "B\"");
		myPrint("B");
		break;
	case sheetsize_C:
		fprintf(KiCadFile, "C\"");
		myPrint("C");
		break;
	case sheetsize_D:
		fprintf(KiCadFile, "D\"");
		myPrint("D");
		break;
	case sheetsize_E:
		fprintf(KiCadFile, "E\"");
		myPrint("E");
		break;
	case sheetsize_F:
		fprintf(KiCadFile, "F\"");
		myPrint("F");
		break;
	case sheetsize_A0:
		fprintf(KiCadFile, "A0\"");
		myPrint("A0");
		break;
	case sheetsize_A1:
		fprintf(KiCadFile, "A1\"");
		myPrint("A1");
		break;
	case sheetsize_A2:
		fprintf(KiCadFile, "A2\"");
		myPrint("A2");
		break;
	case sheetsize_A3:
		fprintf(KiCadFile, "A3\"");
		myPrint("A3");
		break;
	case sheetsize_A4:
		fprintf(KiCadFile, "A4\"");
		myPrint("A4");
		break;
	default: // Custom
		fprintf(KiCadFile, "User\"");
		num_struct X = numProcess(Sheet.Size.X, CoordinateScaleX, 0);
		num_struct Y = numProcess(Sheet.Size.Y, CoordinateScaleY, CoordinateOffsetY);
		fprintf(KiCadFile, " %d.%05d %d.%05d", X.Integ, X.Frac, Y.Integ, Y.Frac);
		myPrint("%d.%05dx%d.%05d", X.Integ, X.Frac, Y.Integ, Y.Frac);
		break;
	}

	// Modify Ancor
	CoordinateOffsetY = UserCoordinateOffsetY + Sheet.Size.Y;
	CoordinateScaleY = UserCoordinateScaleY * -1;

	if (Sheet.Orientation == sheetorientation_Portrait)
	{
		fprintf(KiCadFile, " portrait");
		myPrint(" Portrait");
	}
	else
	{
		myPrint(" Landscape");
	}
	myPrint("\n");
	fprintf(KiCadFile, ")\n");
}

/*
******************************************************************
* - function name:	KiCadTextData()
*
* - description: 	Stores text data in KiCad
*
* - parameter: 		Pointer to KiCad file; Index of Property; 1 for filled shape (Rect, Circle, ..), 0 for not filled shape (Line, ...)
*
* - return value: 	-
******************************************************************
*/
void KiCadTextData(FILE* KiCadFile, textdata_struct textdata)
{
	// Position + Rotation
	num_struct X = numProcess(textdata.Position.X, CoordinateScaleX, CoordinateOffsetX);
	num_struct Y = numProcess(textdata.Position.Y, CoordinateScaleY, CoordinateOffsetY);
	myPrint("\tX: %d.%05d, Y: %d.%05d\n", X.Integ, X.Frac, Y.Integ, Y.Frac);
	fprintf(KiCadFile, "\t\t(at %d.%05d %d.%05d %d)\n", X.Integ, X.Frac, Y.Integ, Y.Frac, textdata.Orientation * 90);
	myPrint("\tRotation: %d Degree\n", textdata.Orientation * 90);
	
	fprintf(KiCadFile, "\t\t(effects\n");
	
	// Font
	fprintf(KiCadFile, "\t\t\t(font \n");
	switch(textdata.Font.Font)
	{
		case font_Fixed:
			break;
		case font_Roman:
			myPrint("\tFont: Default\n");
			break;
		case font_RomanItalic:
			myPrint("\tFont: Default\n");
			fprintf(KiCadFile, "\t\t\t\t(italic yes)\n");
			myPrint("\tItalic\n");
			break;
		case font_RomanBold:
			myPrint("\tFont: Default\n");
			fprintf(KiCadFile, "\t\t\t\t(bold yes)\n");
			myPrint("\tBold\n");
			break;
		case font_RomanBoldItalic:
			myPrint("\tFont: Default\n");
			fprintf(KiCadFile, "\t\t\t\t(bold yes)\n");
			fprintf(KiCadFile, "\t\t\t\t(italic yes)\n");
			myPrint("\tBold & Italic\n");
			break;
		case font_SansSerif:
			fprintf(KiCadFile, "\t\t\t\t(face %s)\n", "SansSerif");
			myPrint("\tFont: SansSerif\n");
			break;
		case font_Script:
			fprintf(KiCadFile, "\t\t\t\t(face %s)\n", "ScriptS");
			myPrint("\tFont: ScriptS\n");
			break;
		case font_SansSerifBold:
			fprintf(KiCadFile, "\t\t\t\t(face %s)\n", "SansSerif");
			myPrint("\tFont: SansSerif\n");
			fprintf(KiCadFile, "\t\t\t\t(bold yes)\n");
			myPrint("\tBold\n");
			break;
		case font_ScriptBold:
			fprintf(KiCadFile, "\t\t\t\t(face %s)\n", "ScriptS");
			myPrint("\tFont: ScriptS\n");
			fprintf(KiCadFile, "\t\t\t\t(bold yes)\n");
			myPrint("\tBold\n");
			break;
		case font_Gothic:
			fprintf(KiCadFile, "\t\t\t\t(face %s)\n", "GothicE");
			myPrint("\tFont: GothicE\n");
			break;
		case font_OldEnglish:
			fprintf(KiCadFile, "\t\t\t\t(face %s)\n", "Old English Text MT");
			myPrint("\tFont: Old English Text MT\n");
			break;
		case font_Kanji:
			myPrint("\tFont: Kanji => Not supported in KiCad!\n");
			fprintf(KiCadFile, "\t\t\t\t(face %s)\n", "Kanji");
			break;
		case font_Plot:
			myPrint("\tFont: Plot => Not supported in KiCad!\n");
			fprintf(KiCadFile, "\t\t\t\t(face %s)\n", "Plot");
			break;
		case font_Custom:
			fprintf(KiCadFile, "\t\t\t\t(face %s)\n", textdata.Font.CustomFont);
			myPrint("\tFont: %s\n", textdata.Font.CustomFont);
			if (textdata.Font.Bold == option_true)
			{
				fprintf(KiCadFile, "\t\t\t\t(bold yes)\n");
				myPrint("\tBold\n");
			}
			break;
		default:
			break;
	}
	
	// Text Size
	num_struct Size = numProcess(textdata.Size * FontScale, 1, 0);
	fprintf(KiCadFile, "\t\t\t\t(size %d.%05d %d.%05d)\n", Size.Integ, Size.Frac, Size.Integ, Size.Frac);
	myPrint("\tSize: %d.%05d\n", Size.Integ, Size.Frac);
	
	// Text Color
	if (textdata.LineColor.Key == colorkey_default)
	{ // Default Color
		fprintf(KiCadFile, "\t\t\t\t(color 0 0 0 0)\n");
		myPrint("\tColor: Default\n");
	}
	else
	{ // Custom Color
		fprintf(KiCadFile, "\t\t\t\t(color %d %d %d 1)\n", textdata.LineColor.Red, textdata.LineColor.Green, textdata.LineColor.Blue);
		myPrint("\tColor: R:%d G:%d B:%d\n", textdata.LineColor.Red, textdata.LineColor.Green, textdata.LineColor.Blue);
	}
	fprintf(KiCadFile, "\t\t\t)\n");
	
	// Text Orientation
	fprintf(KiCadFile, "\t\t\t(justify");
	
	/*
	Orientation codes:
	Default = 0 (Lower left)
	UpperLeft = 1
	MiddleLeft = 2
	LowerLeft = 3
	UpperCenter = 4
	MiddleCenter = 5
	LowerCenter = 6
	UpperRight = 7
	MiddleRight = 8
	LowerRight = 9
	*/
	uint32_t tempOrigin = textdata.Origin;
	if (tempOrigin == textorigin_Default)
	{
		tempOrigin = DefaultTextOrigin;
	}
	else if (tempOrigin > textorigin_LowerRight)
	{
		myPrint("\tWarning, unknown orientation Code! %d\n", tempOrigin);
		tempOrigin = DefaultTextOrigin;
	}
	
	myPrint("\tOrientation: ");
	if (!(tempOrigin % 3))
	{
		fprintf(KiCadFile, " bottom");
		myPrint("Upper");
	}
	else if (!((tempOrigin + 2) % 3))
	{
		fprintf(KiCadFile, " top");
		myPrint("Lower");
	}
	else
	{
		myPrint("Center");
	}
	
	if (tempOrigin <= 3)
	{
		fprintf(KiCadFile, " left");
		myPrint("Left\n");
	}
	else if (tempOrigin >= 7)
	{
		fprintf(KiCadFile, " right");
		myPrint("Right\n");
	}
	else
	{
		myPrint("Center\n");
	}
	fprintf(KiCadFile, ")\n");
	fprintf(KiCadFile, "\t\t)\n");
}

/*
******************************************************************
* - function name:	KiCadPrintString()
*
* - description: 	Prints strings to KiCad file
*
* - parameter: 		Pointer to KiCad file; String to print
*
* - return value: 	-
******************************************************************
*/
void KiCadPrintString(FILE* KiCadFile, string_struct String)
{
	myPrint("\t[");
	char Overbar = 0;
	// Print characters one by one to check for newline and overbar
	for (uint32_t j = 0; j < String.Length; j++)
	{
		if (*((String.Text) + j) == '\n') // Check for line feed
		{
			fprintf(KiCadFile, "\\n"); // print \n, not linefeed character
			myPrint("]\n\t[");
		}
		else if (*((String.Text) + j) == '"')
		{
			fprintf(KiCadFile, "\\\""); // print \n, not linefeed character
			myPrint("\"");
		}
		else if (*((String.Text) + j) == '~') // Check for overbar
		{
			if (Overbar) // Overbar open
			{
				Overbar = 0;
				fprintf(KiCadFile, "}"); // Close Overbar
			}
			else // Overbar closed
			{
				Overbar = 1;
				fprintf(KiCadFile, "~{"); // Open Overbar
			}
			myPrint("~");
		}
		else
		{
			fprintf(KiCadFile, "%c", *((String.Text) + j));
			myPrint("%c", *((String.Text) + j));
		}
	}
	if (Overbar) // Close Overbar if open
	{
		fprintf(KiCadFile, "}");
	}
	fprintf(KiCadFile, "\"\n");
	myPrint("]\n");
}

/*
******************************************************************
* - function name:	KiCadUID()
*
* - description: 	Stores UIDs
*
* - parameter: 		Pointer to KiCad file; UID of page; UID of element
*
* - return value: 	-
******************************************************************
*/
void KiCadUID(FILE* KiCadFile, uid_struct pageUID, uid_struct elementUID)
{
	uint8_t UIDs[16];
	for (int i = 0; i < 8; i++)
	{
		UIDs[i] = swpnib(pageUID.UID[i]);
	}
	for (int i = 0; i < 8; i++)
	{
		UIDs[i + 8] = swpnib(elementUID.UID[i]);
	}

	myPrint("UID: %02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x\n",
			UIDs[0],
			UIDs[1],
			UIDs[2],
			UIDs[3],
			UIDs[4],
			UIDs[5],
			UIDs[6],
			UIDs[7],
			UIDs[8],
			UIDs[9],
			UIDs[10],
			UIDs[11],
			UIDs[12],
			UIDs[13],
			UIDs[14],
			UIDs[15]);
	fprintf(KiCadFile, "(uuid \"%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x\")\n",
			UIDs[0],
			UIDs[1],
			UIDs[2],
			UIDs[3],
			UIDs[4],
			UIDs[5],
			UIDs[6],
			UIDs[7],
			UIDs[8],
			UIDs[9],
			UIDs[10],
			UIDs[11],
			UIDs[12],
			UIDs[13],
			UIDs[14],
			UIDs[15]);
}

/*
******************************************************************
* - function name:	KiCadLabel()
*
* - description: 	Stores label in KiCad
*
* - parameter: 		Pointer to KiCad file; UID of page; label to print; label text length; label text
*
* - return value: 	-
******************************************************************
*/
void KiCadLabel(FILE* KiCadFile, uid_struct UID, label_struct label, string_struct Name)
{
	if (
		label.IndexDxDNet.UID[0] == 0 &&
		label.IndexDxDNet.UID[1] == 0 &&
		label.IndexDxDNet.UID[2] == 0 &&
		label.IndexDxDNet.UID[3] == 0 &&
		label.IndexDxDNet.UID[4] == 0 &&
		label.IndexDxDNet.UID[5] == 0 &&
		label.IndexDxDNet.UID[6] == 0 &&
		label.IndexDxDNet.UID[7] == 0 )
	{
		// No actual Label, just net name
	}
	else // Label
	{
		for (uint32_t i = 0; i < label.SublableNum; i++)
		{
			if ((label.Sublable)[i].Visibility == visibility_vissible)
			{
				fprintf(KiCadFile, "\t(label \"");
				myPrint("Label %d:\n", i + 1);
				KiCadPrintString(KiCadFile, Name);

				KiCadTextData(KiCadFile, (label.Sublable)[i].TextData);
				fprintf(KiCadFile, "\t\t");
				myPrint("\t");
				KiCadUID(KiCadFile, UID, label.IndexDxDNet);
				fprintf(KiCadFile, "\t)\n");
			}
		}
		myPrint("\n");
	}
}

/*
******************************************************************
* - function name:	KiCadArc()
*
* - description: 	Stores arc in KiCad
*
* - parameter: 		Pointer to KiCad file, UID of page, page group
*
* - return value: 	-
******************************************************************
*/
void KiCadArc(FILE* KiCadFile, uid_struct UID, uint32_t page)
{
	if (cdbblks_arc.Length > 0)
	{
		for (uint32_t i = 0; i < cdbblks_arc.Length; i++)
		{
			arc_struct Arc = GetArc((&cdbblks_arc), i);
			if (InsideGroup(&cdbcatlg_grpobj, Arc.UID, page))
			{
				fprintf(KiCadFile, "\t(arc\n");

				num_struct XStart = numProcess(Arc.StartCoord.X, CoordinateScaleX, CoordinateOffsetX);
				num_struct YStart = numProcess(Arc.StartCoord.Y, CoordinateScaleY, CoordinateOffsetY);
				num_struct XMid = numProcess(Arc.MidCoord.X, CoordinateScaleX, CoordinateOffsetX);
				num_struct YMid = numProcess(Arc.MidCoord.Y, CoordinateScaleY, CoordinateOffsetY);
				num_struct XEnd = numProcess(Arc.EndCoord.X, CoordinateScaleX, CoordinateOffsetX);
				num_struct YEnd = numProcess(Arc.EndCoord.Y, CoordinateScaleY, CoordinateOffsetY);

				fprintf(KiCadFile, "\t\t(start %d.%05d %d.%05d)\n", XStart.Integ, XStart.Frac, YStart.Integ, YStart.Frac);
				fprintf(KiCadFile, "\t\t(mid %d.%05d %d.%05d)\n", XMid.Integ, XMid.Frac, YMid.Integ, YMid.Frac);
				fprintf(KiCadFile, "\t\t(end %d.%05d %d.%05d)\n", XEnd.Integ, XEnd.Frac, YEnd.Integ, YEnd.Frac);

				myPrint("Arc %d:\n", i + 1);
				myPrint("\tX Start: %d.%05d, X Mid: %d.%05d X End: %d.%05d\n", XStart.Integ, XStart.Frac, XMid.Integ, XMid.Frac, XEnd.Integ, XEnd.Frac);
				myPrint("\tY Start: %d.%05d, Y Mid: %d.%05d Y End: %d.%05d\n", YStart.Integ, YStart.Frac, YMid.Integ, YMid.Frac, YEnd.Integ, YEnd.Frac);

				KiCadProperty(KiCadFile, Arc.Property, 0);
				fprintf(KiCadFile, "\t\t");
				myPrint("\t");
				KiCadUID(KiCadFile, UID, Arc.UID);
				fprintf(KiCadFile, "\t)\n");
			}
		}
		myPrint("\n");
	}
}

/*
******************************************************************
* - function name:	KiCadCircle()
*
* - description: 	Stores circle in KiCad
*
* - parameter: 		Pointer to KiCad file, UID of page, page group
*
* - return value: 	-
******************************************************************
*/
void KiCadCircle(FILE* KiCadFile, uid_struct UID, uint32_t page)
{
	if (cdbblks_circle.Length > 0)
	{
		for (uint32_t i = 0; i < cdbblks_circle.Length; i++)
		{
			circle_struct Circle = GetCircle(&cdbblks_circle, i);
			if (InsideGroup(&cdbcatlg_grpobj, Circle.UID, page))
			{
				fprintf(KiCadFile, "\t(circle\n");
				num_struct X = numProcess(Circle.CenterCoord.X, CoordinateScaleX, CoordinateOffsetX);
				num_struct Y = numProcess(Circle.CenterCoord.Y, CoordinateScaleY, CoordinateOffsetY);
				num_struct R = numProcess(Circle.Radius, 1, 0);

				fprintf(KiCadFile, "\t\t(center %d.%05d %d.%05d)\n", X.Integ, X.Frac, Y.Integ, Y.Frac);
				fprintf(KiCadFile, "\t\t(radius %d.%05d)\n", R.Integ, R.Frac);

				myPrint("Circle %d:\n", i + 1);
				myPrint("\tX: %d.%05d, Y: %d.%05d, Radius: %d.%05d\n", X.Integ, X.Frac, Y.Integ, Y.Frac, R.Integ, R.Frac);

				KiCadProperty(KiCadFile, Circle.Property, 1);
				fprintf(KiCadFile, "\t\t");
				myPrint("\t");
				KiCadUID(KiCadFile, UID, Circle.UID);
				fprintf(KiCadFile, "\t)\n");
			}
		}
		myPrint("\n");
	}
}

/*
******************************************************************
* - function name:	KiCadRectangle()
*
* - description: 	Stores rectangle in KiCad
*
* - parameter: 		Pointer to KiCad file, UID of page, page group
*
* - return value: 	-
******************************************************************
*/
void KiCadRectangle(FILE* KiCadFile, uid_struct UID, uint32_t page)
{
	if (cdbblks_rectangle.Length > 0)
	{
		for (uint32_t i = 0; i < cdbblks_rectangle.Length; i++)
		{
			rectangle_struct Rectangle = GetRectangle(&cdbblks_rectangle, i);

			if (InsideGroup(&cdbcatlg_grpobj, Rectangle.UID, page))
			{
				fprintf(KiCadFile, "\t(rectangle\n");

				num_struct XStart = numProcess(Rectangle.StartCoord.X, CoordinateScaleX, CoordinateOffsetX);
				num_struct YStart = numProcess(Rectangle.StartCoord.Y, CoordinateScaleY, CoordinateOffsetY);
				num_struct XEnd = numProcess(Rectangle.EndCoord.X, CoordinateScaleX, CoordinateOffsetX);
				num_struct YEnd = numProcess(Rectangle.EndCoord.Y, CoordinateScaleY, CoordinateOffsetY);

				fprintf(KiCadFile, "\t\t(start %d.%05d %d.%05d)\n", XStart.Integ, XStart.Frac, YStart.Integ, YStart.Frac);
				fprintf(KiCadFile, "\t\t(end %d.%05d %d.%05d)\n", XEnd.Integ, XEnd.Frac, YEnd.Integ, YEnd.Frac);

				myPrint("Rectangle %d:\n", i + 1);
				myPrint("\tX Start: %d.%05d, X End: %d.%05d\n", XStart.Integ, XStart.Frac, XEnd.Integ, XEnd.Frac);
				myPrint("\tY Start: %d.%05d, Y End: %d.%05d\n", YStart.Integ, YStart.Frac, YEnd.Integ, YEnd.Frac);
				KiCadProperty(KiCadFile, Rectangle.Property, 1);
				fprintf(KiCadFile, "\t\t");
				myPrint("\t");
				KiCadUID(KiCadFile, UID, Rectangle.UID);
				fprintf(KiCadFile, "\t)\n");
			}
		}
		myPrint("\n");
	}
}

/*
******************************************************************
* - function name:	KiCadText()
*
* - description: 	Stores text in KiCad
*
* - parameter: 		Pointer to KiCad file, UID of page, page group
*
* - return value: 	-
******************************************************************
*/
void KiCadText(FILE* KiCadFile, uid_struct UID, uint32_t page)
{
	if (cdbblks_text.Length > 0)
	{
		for (uint32_t i = 0; i < cdbblks_text.Length; i++)
		{
			text_struct Text = GetText(&cdbblks_text, i);
			if (InsideGroup(&cdbcatlg_grpobj, Text.UID, page))
			{

				fprintf(KiCadFile, "\t(text \"");
				myPrint("Text %d:\n", i + 1);
				KiCadPrintString(KiCadFile, Text.String);

				fprintf(KiCadFile, "\t\t(exclude_from_sim no)\n");
				KiCadTextData(KiCadFile, Text.TextData);
				fprintf(KiCadFile, "\t\t");
				myPrint("\t");
				KiCadUID(KiCadFile, UID, Text.UID);
				fprintf(KiCadFile, "\t)\n");
			}
		}
		myPrint("\n");
	}
}

/*
******************************************************************
* - function name:	KiCadLine()
*
* - description: 	Stores line in KiCad
*
* - parameter: 		Pointer to KiCad file, UID of page, page group
*
* - return value: 	-
******************************************************************
*/
void KiCadLine(FILE* KiCadFile, uid_struct UID, uint32_t page)
{
	if (cdbblks_line.Length > 0)
	{
		for (uint32_t i = 0; i < cdbblks_line.Length; i++)
		{
			line_struct Line = GetLine(&cdbblks_line, i);
			if (InsideGroup(&cdbcatlg_grpobj, Line.UID, page))
			{
				fprintf(KiCadFile, "\t(polyline\n");
				fprintf(KiCadFile, "\t\t(pts\n");
				myPrint("Line %d:\n", i + 1);
				for (unsigned int j = 0; j < Line.numSegment; j++)
				{
					num_struct XStart = numProcess(Line.Segment[j].Start.X, CoordinateScaleX, CoordinateOffsetX);
					num_struct YStart = numProcess(Line.Segment[j].Start.Y, CoordinateScaleY, CoordinateOffsetY);
					num_struct XEnd = numProcess(Line.Segment[j].End.X, CoordinateScaleX, CoordinateOffsetX);
					num_struct YEnd = numProcess(Line.Segment[j].End.Y, CoordinateScaleY, CoordinateOffsetY);

					fprintf(KiCadFile, "\t\t\t(xy %d.%05d %d.%05d) (xy %d.%05d %d.%05d)\n", XStart.Integ, XStart.Frac, YStart.Integ, YStart.Frac, XEnd.Integ, XEnd.Frac, YEnd.Integ, YEnd.Frac);

					myPrint("\tSegment %d:\n", j + 1);
					myPrint("\t\tX Start: %d.%05d, X End: %d.%05d\n", XStart.Integ, XStart.Frac, XEnd.Integ, XEnd.Frac);
					myPrint("\t\tY Start: %d.%05d, Y End: %d.%05d\n", YStart.Integ, YStart.Frac, YEnd.Integ, YEnd.Frac);
				}
				fprintf(KiCadFile, "\t\t)\n");
				KiCadProperty(KiCadFile, Line.Property, 0);
				fprintf(KiCadFile, "\t\t");
				myPrint("\t");
				KiCadUID(KiCadFile, UID, Line.UID);
				fprintf(KiCadFile, "\t)\n");
			}
		}
		myPrint("\n");
	}
}

/*
******************************************************************
* - function name:	KiCadNets()
*
* - description: 	Stores nets in KiCad
*
* - parameter: 		Pointer to KiCad file, UID of page, page group
*
* - return value: 	-
******************************************************************
*/
void KiCadNets(FILE* KiCadFile, uid_struct UID, uint32_t page)
{
	if (cdbblks_net.Length > 0)
	{
		for (uint32_t i = 0; i < cdbblks_net.Length; i++)
		{
			net_struct Net = GetNet(&cdbblks_net, i);
			if (InsideGroup(&cdbcatlg_grpobj, Net.UID, page))
			{
				for (unsigned int j = 0; j < Net.NumNetSegment; j++)
				{
					for (unsigned int k = 0; k < (Net.NetSegment)[j].Segment.numSegment; k++)
					{
						if ((Net.NetSegment)[j].Segment.Segment == NULL)
						{
							return;
						}
						fprintf(KiCadFile, "\t(wire\n");
						fprintf(KiCadFile, "\t\t(pts\n");
						myPrint("Net %d:\n", i + 1);
						segment_section_struct Segment = (Net.NetSegment)[j].Segment.Segment[k];

						num_struct XStart = numProcess(Segment.StartJoint.Coord.X, CoordinateScaleX, CoordinateOffsetX);
						num_struct YStart = numProcess(Segment.StartJoint.Coord.Y, CoordinateScaleY, CoordinateOffsetY);
						num_struct XEnd = numProcess(Segment.EndJoint.Coord.X, CoordinateScaleX, CoordinateOffsetX);
						num_struct YEnd = numProcess(Segment.EndJoint.Coord.Y, CoordinateScaleY, CoordinateOffsetY);

						fprintf(KiCadFile, "\t\t\t(xy %d.%05d %d.%05d) (xy %d.%05d %d.%05d)\n", XStart.Integ, XStart.Frac, YStart.Integ, YStart.Frac, XEnd.Integ, XEnd.Frac, YEnd.Integ, YEnd.Frac);

						myPrint("\tSegment %d:\n", j + 1);
						myPrint("\t\tX Start: %d.%05d, X End: %d.%05d\n", XStart.Integ, XStart.Frac, XEnd.Integ, XEnd.Frac);
						myPrint("\t\tY Start: %d.%05d, Y End: %d.%05d\n", YStart.Integ, YStart.Frac, YEnd.Integ, YEnd.Frac);
						fprintf(KiCadFile, "\t\t)\n");
						KiCadProperty(KiCadFile, (Net.NetSegment)[j].Property, 0);

						fprintf(KiCadFile, "\t\t");
						myPrint("\t");
						KiCadUID(KiCadFile, UID, Net.UID);
						fprintf(KiCadFile, "\t)\n");
					}
					KiCadLabel(KiCadFile, UID, (Net.NetSegment)[j].Label, Net.Name);
				}
			}
		}
		myPrint("\n");
	}
}

/*
******************************************************************
* - function name:	KiCadBusses()
*
* - description: 	Stores buses in KiCad
*
* - parameter: 		Pointer to KiCad file, UID of page, page group
*
* - return value: 	-
******************************************************************
*/
void KiCadBusses(FILE* KiCadFile, uid_struct UID, uint32_t page)
{
	if (cdbblks_bus.Length > 0)
	{
		for (uint32_t i = 0; i < cdbblks_bus.Length; i++)
		{
			bus_struct Bus = GetBus(&cdbblks_bus, i);
			if (InsideGroup(&cdbcatlg_grpobj, Bus.UID, page))
			{
				for (unsigned int j = 0; j < Bus.BusSegmentLen; j++)
				{
					for (unsigned int k = 0; k < (Bus.BusSegment)[j].Segment.numSegment; k++)
					{
						if ((Bus.BusSegment)[j].Segment.Segment == NULL)
						{
							return;
						}
						fprintf(KiCadFile, "\t(bus\n");
						fprintf(KiCadFile, "\t\t(pts\n");
						myPrint("Bus %d:\n", i + 1);
						segment_section_struct Segment = (Bus.BusSegment)[j].Segment.Segment[k];

						num_struct XStart = numProcess(Segment.StartJoint.Coord.X, CoordinateScaleX, CoordinateOffsetX);
						num_struct YStart = numProcess(Segment.StartJoint.Coord.Y, CoordinateScaleY, CoordinateOffsetY);
						num_struct XEnd = numProcess(Segment.EndJoint.Coord.X, CoordinateScaleX, CoordinateOffsetX);
						num_struct YEnd = numProcess(Segment.EndJoint.Coord.Y, CoordinateScaleY, CoordinateOffsetY);

						fprintf(KiCadFile, "\t\t\t(xy %d.%05d %d.%05d) (xy %d.%05d %d.%05d)\n", XStart.Integ, XStart.Frac, YStart.Integ, YStart.Frac, XEnd.Integ, XEnd.Frac, YEnd.Integ, YEnd.Frac);

						myPrint("\tSegment %d:\n", j + 1);
						myPrint("\t\tX Start: %d.%05d, X End: %d.%05d\n", XStart.Integ, XStart.Frac, XEnd.Integ, XEnd.Frac);
						myPrint("\t\tY Start: %d.%05d, Y End: %d.%05d\n", YStart.Integ, YStart.Frac, YEnd.Integ, YEnd.Frac);
						fprintf(KiCadFile, "\t\t)\n");
						KiCadProperty(KiCadFile, (Bus.BusSegment)[j].Property, 0);

						fprintf(KiCadFile, "\t\t");
						myPrint("\t");
						KiCadUID(KiCadFile, UID, Bus.UID);
						fprintf(KiCadFile, "\t)\n");
					}
					KiCadLabel(KiCadFile, UID, (Bus.BusSegment)[j].Label, Bus.Name);
				}
			}
		}
		myPrint("\n");
	}
}
