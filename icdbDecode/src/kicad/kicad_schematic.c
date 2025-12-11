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
#include <stdlib.h>					// Required for calloc to work properly#
#include "kicad_shared.h"			// Required for shared kicad code
#include "../common.h"				// Required for myfopen
#include "../uid.h"					// Required for uid_union
#include "../stringutil.h"			// Required for string manipulation
#include "../cdbcatlg/cdbcatlg.h"	// Required for cdbcatlg
#include "../cdbcatlg/page.h"		// Required for page
#include "../cdbcatlg/group.h"		// Required for group
#include "../cdbcatlg/grpobj.h"		// Required for group
#include "../cdbblks/cdbblks.h"		// Required for cdbblks
#include "../cdbblks/sheet.h"		// Required for sheet
#include "../cdbblks/net.h"			// Required for net
#include "../cdbblks/bus.h"			// Required for bus

/*
******************************************************************
* Defines
******************************************************************
*/
#define _CRT_SECURE_NO_DEPRECATE

/*
******************************************************************
* Local Function Prototype
******************************************************************
*/
void KiCadSheetProp(FILE*, sheet_struct);
void KiCadNets(FILE*, uid_union, uint32_t);
void KiCadBusses(FILE*, uid_union, uint32_t);

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	StoreAsKicadSchematic()
*
* - description: 	Stores the parsed data as KiCad Schematic file
*
* - parameter: 		source path string; string length; file name string; file name length
*
* - return value: 	errorcode
******************************************************************
*/
int StoreAsKicadSchematic(char* path, uint32_t pathlength, page_struct page)
{
	char* destination = NULL;
	uid_union temp = { 0 };
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
			completePathLength = addStrings(&destination, TempDest2, completePathLengthTemp, KiCadSchematicFileEnding, sizeof(KiCadSchematicFileEnding), '\0');
			free(TempDest1);
			free(TempDest2);
		}
		else // Schematic name only
		{
			completePathLength = addStrings(&destination, page.Name.Text, page.Name.Length, KiCadSchematicFileEnding, sizeof(KiCadSchematicFileEnding), '\0');
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
			fprintf(KiCadFile, "\t(version 20251012)\n");
			fprintf(KiCadFile, "\t(generator \"eeschema\")\n");
			fprintf(KiCadFile, "\t(generator_version \"9.99\")\n");
#else
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
			KiCadArc(KiCadFile, cdbblks_arc, page.UID, Sheet.Group);
			KiCadCircle(KiCadFile, cdbblks_circle, page.UID, Sheet.Group);
			KiCadRectangle(KiCadFile, cdbblks_rectangle, page.UID, Sheet.Group);
			KiCadText(KiCadFile, cdbblks_text, page.UID, Sheet.Group);
			KiCadLine(KiCadFile, cdbblks_line, page.UID, Sheet.Group);
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
		char X[13]; // 10 char + sign + point + zero termination
		char Y[13]; // 10 char + sign + point + zero termination
		numPrint(&X[0], Sheet.Size.Y, CoordinateScaleY, CoordinateOffsetY);
		numPrint(&Y[0], Sheet.Size.X, CoordinateScaleX, 0);
		fprintf(KiCadFile, " %s %s", X, Y);
		myPrint("%s %s", X, Y);
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
* - function name:	KiCadNets()
*
* - description: 	Stores nets in KiCad
*
* - parameter: 		Pointer to KiCad file, UID of page, page group
*
* - return value: 	-
******************************************************************
*/
void KiCadNets(FILE* KiCadFile, uid_union UID, uint32_t page)
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

						char XStart[13]; // 10 char + sign + point + zero termination
						char YStart[13]; // 10 char + sign + point + zero termination
						char XEnd[13]; // 10 char + sign + point + zero termination
						char YEnd[13]; // 10 char + sign + point + zero termination
						numPrint(&XStart[0], Segment.StartJoint.Coord.X, CoordinateScaleX, CoordinateOffsetX);
						numPrint(&YStart[0], Segment.StartJoint.Coord.Y, CoordinateScaleY, CoordinateOffsetY);
						numPrint(&XEnd[0], Segment.EndJoint.Coord.X, CoordinateScaleX, CoordinateOffsetX);
						numPrint(&YEnd[0], Segment.EndJoint.Coord.Y, CoordinateScaleY, CoordinateOffsetY);

						fprintf(KiCadFile, "\t\t\t(xy %s %s) (xy %s %s)\n", XStart, YStart, XEnd, YEnd);

						myPrint("\tSegment %d:\n", j + 1);
						myPrint("\t\tX Start: %s, X End: %s\n", XStart, XEnd);
						myPrint("\t\tY Start: %s, Y End: %s\n", YStart, YEnd);
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
void KiCadBusses(FILE* KiCadFile, uid_union UID, uint32_t page)
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
						
						char XStart[13]; // 10 char + sign + point + zero termination
						char YStart[13]; // 10 char + sign + point + zero termination
						char XEnd[13]; // 10 char + sign + point + zero termination
						char YEnd[13]; // 10 char + sign + point + zero termination
						numPrint(&XStart[0], Segment.StartJoint.Coord.X, CoordinateScaleX, CoordinateOffsetX);
						numPrint(&YStart[0], Segment.StartJoint.Coord.Y, CoordinateScaleY, CoordinateOffsetY);
						numPrint(&XEnd[0], Segment.EndJoint.Coord.X, CoordinateScaleX, CoordinateOffsetX);
						numPrint(&YEnd[0], Segment.EndJoint.Coord.Y, CoordinateScaleY, CoordinateOffsetY);

						fprintf(KiCadFile, "\t\t\t(xy %s %s) (xy %s %s)\n", XStart, YStart, XEnd, YEnd);

						myPrint("\tSegment %d:\n", j + 1);
						myPrint("\t\tX Start: %s, X End: %s\n", XStart, XEnd);
						myPrint("\t\tY Start: %s, Y End: %s\n", YStart, YEnd);
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
