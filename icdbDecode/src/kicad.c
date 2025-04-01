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

/*
******************************************************************
* Includes
******************************************************************
*/
#include "kicad.h"
#include <stdio.h>		// Required for fprint, fopen, ...
#include <stdint.h>		// Required for int32_t, uint32_t, ...
#include <stdlib.h>		// Required for calloc to work properly
#include "common.h"		// Required for myfopen
#include "cdbblks.h"	// Required for source data


/*
******************************************************************
* Defines
******************************************************************
*/
#define _CRT_SECURE_NO_DEPRECATE

#define CoordinateOffsetX 0
#define CoordinateOffsetY 21000000
#define CoordinateScaleX 1
#define CoordinateScaleY -1
#define DefaultTextOrigin 3
#define FontScale 0.5  // Fonts in KiCad are rendered about half the size than in DxD. Exact size TBD
#define BaseLineThickness 0.1524
#define NewKiCad 1 // Create Kicad 9.99 required for filling pattern
#define KiCadFileEnding ".kicad_sch" // File ending for exported KiCad schematic file



/*
******************************************************************
* Structures
******************************************************************
*/
typedef struct net_bus_segment_struct
{
	coordinate_struct StartCoord;
	coordinate_struct EndCoord;
	uint32_t ID;
	uint32_t StartID;
	uint32_t EndID;
} net_bus_segment_struct;

typedef struct net_bus_struct
{
	uint32_t IsBus;
	uint32_t Style;
	uint32_t NumSegments;
	net_bus_segment_struct* Segments;
	uint32_t ID;
	uint32_t NumJoints;
	coordinate_struct* Joints;
} net_bus_struct;

// Non dxd structs
int32_t NumNetstyles = -1;
style_struct* NetStyles = NULL;

int32_t NumNetsBus = -1;
net_bus_struct* NetBus = NULL;

/*
******************************************************************
* Local Function Protptypes
******************************************************************
*/
void KiCadProcessStyle(FILE*, uint32_t, uint8_t);
void KiCadProcessText(FILE*, uint8_t );
void Processing(void);
/*
******************************************************************
* Global Variables
******************************************************************
*/

/*
******************************************************************
* - function name:	StoreAsKicadFile()
*
* - description: 	Stores the parsed data as KiCad file
*
* - parameter: 		source path string; string length; file name string; file name length
*
* - return value: 	errorcode
******************************************************************
*/
int StoreAsKicadFile(char* path, uint32_t pathlenth, char* file, uint32_t filelenth)
{
	char* destination = NULL;
	uint32_t completePathLength = addStrings(&destination, file, filelenth, KiCadFileEnding, sizeof(KiCadFileEnding), '\0');
	Processing();
	// Open file
	FILE* KiCadFile = myfopen("w", path, pathlenth, destination, completePathLength, DIR_SEPERATOR);
	if (KiCadFile != 0)
	{
		myPrint("\n--------------- Writing KiCad file ---------------\n");
		myPrint("File: [%s]\n", destination);
		// Header
		fprintf(KiCadFile, "(kicad_sch\n");
#if NewKiCad
		fprintf(KiCadFile, "\t(version 20250222)\n");
		fprintf(KiCadFile, "\t(generator_version %c9.99%c)\n", 0x22, 0x22);
		fprintf(KiCadFile, "\t(generator %ceeschema%c)\n", 0x22, 0x22);
#else
		fprintf(KiCadFile, "\t(version 20250114)\n");
		fprintf(KiCadFile, "\t(generator_version %c9.0%c)\n", 0x22, 0x22);
		fprintf(KiCadFile, "\t(generator %ceeschema%c)\n", 0x22, 0x22);
#endif
		fprintf(KiCadFile, "\t(paper %cA4%c)\n", 0x22, 0x22);
		fprintf(KiCadFile, "\t(lib_symbols)\n");

		// Nets / Busses
		if (NumNetsBus > 0 && NetBus != 0)
		{
			myPrint("\nNets & Busses:\n");
			for (uint32_t i = 0; i < NumNetsBus; i++)
			{
				if (NetBus[i].IsBus)
				{
					myPrint("Bus %d Junctions:\n", i + 1);
				}
				else
				{
					myPrint("Net %d Junctions:\n", i + 1);
				}
				// Joints
				if (NetBus[i].NumJoints > 0 && NetBus[i].Joints != 0)
				{
					for (uint32_t j = 0; j < NetBus[i].NumJoints; j++)
					{
						fprintf(KiCadFile, "\t(junction\n");

						num_struct X = numProcess(NetBus[i].Joints[j].X, CoordinateScaleX, CoordinateOffsetX);
						num_struct Y = numProcess(NetBus[i].Joints[j].Y, CoordinateScaleY, CoordinateOffsetY);

						fprintf(KiCadFile, "\t\t(at %d.%05d %d.%05d)\n", X.Integ, X.Frac, Y.Integ, Y.Frac);
						myPrint("\tJunction at X: %d.%05d, Y %d.%05d\n", X.Integ, X.Frac, Y.Integ, Y.Frac);
						uint32_t index = NetBus[i].Style;
						if (Color[index].Key != 0xff)
						{
							fprintf(KiCadFile, "\t\t(color %d %d %d 1)\n", Color[index].Red, Color[index].Green, Color[index].Blue);
						}
						else
						{
							fprintf(KiCadFile, "\t\t(color 0 0 0 0)\n");
						}
						fprintf(KiCadFile, "\t)\n");
					}
				}
				// Nets
				if (NetBus[i].NumSegments > 0 && NetBus[i].Segments != 0)
				{
					for (uint32_t j = 0; j < NetBus[i].NumSegments; j++)
					{
						if (NetBus[i].IsBus)
						{
							fprintf(KiCadFile, "\t(bus\n");
							myPrint("Bus %d\n", i + 1);
						}
						else
						{
							fprintf(KiCadFile, "\t(wire\n");
							myPrint("Net %d\n", i + 1);
						}
						fprintf(KiCadFile, "\t\t(pts\n");
						num_struct XStart = numProcess(NetBus[i].Segments[j].StartCoord.X, CoordinateScaleX, CoordinateOffsetX);
						num_struct YStart = numProcess(NetBus[i].Segments[j].StartCoord.Y, CoordinateScaleY, CoordinateOffsetY);
						num_struct XEnd = numProcess(NetBus[i].Segments[j].EndCoord.X, CoordinateScaleX, CoordinateOffsetX);
						num_struct YEnd = numProcess(NetBus[i].Segments[j].EndCoord.Y, CoordinateScaleY, CoordinateOffsetY);

						fprintf(KiCadFile, "\t\t\t(xy %d.%05d %d.%05d) (xy %d.%05d %d.%05d)\n", XStart.Integ, XStart.Frac, YStart.Integ, YStart.Frac, XEnd.Integ, XEnd.Frac, YEnd.Integ, YEnd.Frac);

						myPrint("\tX Start: %d.%05d, X End: %d.%05d\n", XStart.Integ, XStart.Frac, XEnd.Integ, XEnd.Frac);
						myPrint("\tY Start: %d.%05d, Y End: %d.%05d\n", YStart.Integ, YStart.Frac, YEnd.Integ, YEnd.Frac);
						fprintf(KiCadFile, "\t\t)\n");

						KiCadProcessStyle(KiCadFile, NetBus[i].Style, 0);
					}
				}
			}
		}

		// Lines
		if (LinePointX != 0 && LinePointY != 0 && Line2Style != 0 && NumLines > 0)
		{
			myPrint("\nLines:\n");
			for (uint32_t i = 0; i < NumLines; i++)
			{
				fprintf(KiCadFile, "\t(polyline\n");
				fprintf(KiCadFile, "\t\t(pts\n");

				num_struct XStart = numProcess(LinePointX[i].StartPoint, CoordinateScaleX, CoordinateOffsetX);
				num_struct YStart = numProcess(LinePointY[i].StartPoint, CoordinateScaleY, CoordinateOffsetY);
				num_struct XEnd = numProcess(LinePointX[i].StartPoint + LinePointX[i].Offset + 1, CoordinateScaleX, CoordinateOffsetX);
				num_struct YEnd = numProcess(LinePointY[i].StartPoint + LinePointY[i].Offset + 1, CoordinateScaleY, CoordinateOffsetY);

				fprintf(KiCadFile, "\t\t\t(xy %d.%05d %d.%05d) (xy %d.%05d %d.%05d)\n", XStart.Integ, XStart.Frac, YStart.Integ, YStart.Frac, XEnd.Integ, XEnd.Frac, YEnd.Integ, YEnd.Frac);

				myPrint("Line %d\n", i + 1);
				myPrint("\tX Start: %d.%05d, X End: %d.%05d\n", XStart.Integ, XStart.Frac, XEnd.Integ, XEnd.Frac);
				myPrint("\tY Start: %d.%05d, Y End: %d.%05d\n", YStart.Integ, YStart.Frac, YEnd.Integ, YEnd.Frac);
				fprintf(KiCadFile, "\t\t)\n");

				KiCadProcessStyle(KiCadFile, Line2Style[i].index - 1, 0);
			}
		}

		// Rectangles
		if (RectCoord != 0 && NumRectangle > 0)
		{
			myPrint("\nRectangles:\n");
			for (uint32_t i = 0; i < NumRectangle; i++)
			{
				fprintf(KiCadFile, "\t(rectangle\n");

				num_struct XStart = numProcess(RectCoord[i].Start.X, CoordinateScaleX, CoordinateOffsetX);
				num_struct YStart = numProcess(RectCoord[i].Start.Y, CoordinateScaleY, CoordinateOffsetY);
				num_struct XEnd = numProcess(RectCoord[i].End.X, CoordinateScaleX, CoordinateOffsetX);
				num_struct YEnd = numProcess(RectCoord[i].End.Y, CoordinateScaleY, CoordinateOffsetY);

				fprintf(KiCadFile, "\t\t(start %d.%05d %d.%05d)\n", XStart.Integ, XStart.Frac, YStart.Integ, YStart.Frac);
				fprintf(KiCadFile, "\t\t(end %d.%05d %d.%05d)\n", XEnd.Integ, XEnd.Frac, YEnd.Integ, YEnd.Frac);

				myPrint("Rectangle %d\n", i + 1);
				myPrint("\tX Start: %d.%05d, X End: %d.%05d\n", XStart.Integ, XStart.Frac, XEnd.Integ, XEnd.Frac);
				myPrint("\tY Start: %d.%05d, Y End: %d.%05d\n", YStart.Integ, YStart.Frac, YEnd.Integ, YEnd.Frac);

				KiCadProcessStyle(KiCadFile, Rect2Style[i].index - 1, 1);

			}
		}

		// Circle
		if (CircleRadius != 0 && CirclesPos != 0 && NumCircle > 0)
		{
			myPrint("\nCircles:\n");
			for (uint32_t i = 0; i < NumCircle; i++)
			{
				fprintf(KiCadFile, "\t(circle\n");

				num_struct X = numProcess(CirclesPos[i].X, CoordinateScaleX, CoordinateOffsetX);
				num_struct Y = numProcess(CirclesPos[i].Y, CoordinateScaleY, CoordinateOffsetY);
				num_struct R = numProcess(CircleRadius[i], 1, 0);

				fprintf(KiCadFile, "\t\t(center %d.%05d %d.%05d)\n", X.Integ, X.Frac, Y.Integ, Y.Frac);
				fprintf(KiCadFile, "\t\t(radius %d.%05d)\n", R.Integ, R.Frac);

				myPrint("Circle %d\n", i + 1);
				myPrint("\tX: %d.%05d, Y: %d.%05d, Radius: %d.%05d\n", X.Integ, X.Frac, Y.Integ, Y.Frac, R.Integ, R.Frac);

				KiCadProcessStyle(KiCadFile, Circle2Style[i].index - 1, 1);
			}
		}

		// Arc
		if (ArcEnd != 0 && ArcStart != 0 && ArcMid != 0 && NumArc > 0)
		{
			myPrint("\nArcs:\n");
			for (uint32_t i = 0; i < NumArc; i++)
			{
				fprintf(KiCadFile, "\t(arc\n");

				num_struct XStart = numProcess(ArcStart[i].X, CoordinateScaleX, CoordinateOffsetX);
				num_struct YStart = numProcess(ArcStart[i].Y, CoordinateScaleY, CoordinateOffsetY);
				num_struct XMid = numProcess(ArcMid[i].X, CoordinateScaleX, CoordinateOffsetX);
				num_struct YMid = numProcess(ArcMid[i].Y, CoordinateScaleY, CoordinateOffsetY);
				num_struct XEnd = numProcess(ArcEnd[i].X, CoordinateScaleX, CoordinateOffsetX);
				num_struct YEnd = numProcess(ArcEnd[i].Y, CoordinateScaleY, CoordinateOffsetY);

				fprintf(KiCadFile, "\t\t(start %d.%05d %d.%05d)\n", XStart.Integ, XStart.Frac, YStart.Integ, YStart.Frac);
				fprintf(KiCadFile, "\t\t(mid %d.%05d %d.%05d)\n", XMid.Integ, XMid.Frac, YMid.Integ, YMid.Frac);
				fprintf(KiCadFile, "\t\t(end %d.%05d %d.%05d)\n", XEnd.Integ, XEnd.Frac, YEnd.Integ, YEnd.Frac);

				myPrint("Arc: %d\n", i + 1);
				myPrint("\tX Start: %d.%05d, X Mid: %d.%05d X End: %d.%05d\n", XStart.Integ, XStart.Frac, XMid.Integ, XMid.Frac, XEnd.Integ, XEnd.Frac);
				myPrint("\tY Start: %d.%05d, Y Mid: %d.%05d Y End: %d.%05d\n", YStart.Integ, YStart.Frac, YMid.Integ, YMid.Frac, YEnd.Integ, YEnd.Frac);

				KiCadProcessStyle(KiCadFile, Arc2Style[i].index - 1, 0);
			}
		}

		// Text
		KiCadProcessText(KiCadFile, 0);

		// NetLabel
		KiCadProcessText(KiCadFile, 1);

		// Leader
		fprintf(KiCadFile, "\t(sheet_instances\n");
		fprintf(KiCadFile, "\t\t(path %c/%c\n", 0x22, 0x22);
		fprintf(KiCadFile, "\t\t\t(page %c1%c)\n", 0x22, 0x22);
		fprintf(KiCadFile, "\t\t) \n");
		fprintf(KiCadFile, "\t) \n");
		fprintf(KiCadFile, ") \n");
		fclose(KiCadFile);
		return 0;
	}
	else
	{
		myPrint("Error Writing [%s] !\n", destination);
		return -1;
	}
}

/*
******************************************************************
* Local Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	KiCadProcessStyle()
*
* - description: 	Stores styles in KiCad
*
* - parameter: 		Pointer to KiCad file; Index of Style; 1 for filled shape (Rect, Dircle, ..), 0 for not filled shape (Lines, ...)
*
* - return value: 	-
******************************************************************
*/
void KiCadProcessStyle(FILE* KiCadFile, uint32_t index, uint8_t Filling)
{
	fprintf(KiCadFile, "\t\t(stroke\n");

	// Linewidth
	if (Thickness[index] == -1)
	{
		myPrint("\tLinewidth: Default\n");
	}
	else if (Thickness[index] >= 1 && Thickness[index] <= 10)
	{
		fprintf(KiCadFile, "\t\t\t(width %f)\n", (float)Thickness[index] * BaseLineThickness);
		myPrint("\tLinewidth: %f\n", (float)Thickness[index] * BaseLineThickness);
	}

	// Linestyle
	switch (Style[index])
	{
	case -1: // Solid (Automatic)
		myPrint("\tLinestyle: Default\n");
		fprintf(KiCadFile, "\t\t\t(type default)\n");
		break;
	case 0: // Solid
		myPrint("\tLinestyle: Solid\n");
		fprintf(KiCadFile, "\t\t\t(type solid)\n");
		break;
	case 1: // Dash
	case 4: // Big dash
	case 7: // Medium dash
		myPrint("\tLinestyle: Dash\n");
		fprintf(KiCadFile, "\t\t\t(type dash)\n");
		break;
	case 2: // Center
	case 6: // Dash-Dot
		myPrint("\tLinestyle: Dash-Dot\n");
		fprintf(KiCadFile, "\t\t\t(type dash_dot)\n");
		break;
	case 3: // Phantom
		myPrint("\tLinestyle: Dash-Dot-Dot\n");
		fprintf(KiCadFile, "\t\t\t(type dash_dot_dot)\n");
		break;
	case 5: // Dot
		myPrint("\tLinestyle: Dot\n");
		fprintf(KiCadFile, "\t\t\t(type dot)\n");
		break;
	}

	// Color
	if (Color[index].Key == 0xff)
	{ // Default Color
		myPrint("\tColor: Default\n");
		fprintf(KiCadFile, "\t\t\t(color 0 0 0 0)\n");
	}
	else
	{ // Custom Color
		fprintf(KiCadFile, "\t\t\t(color %d %d %d 1)\n", Color[index].Red, Color[index].Green, Color[index].Blue);
		myPrint("\tColor: R:%d G:%d B:%d\n", Color[index].Red, Color[index].Green, Color[index].Blue);
	}
	fprintf(KiCadFile, "\t\t)\n");

	// Filling style
	if (Filling)
	{
		int8_t opacity = -1;
#if NewKiCad
		fprintf(KiCadFile, "\t\t(fill\n");
		switch (Fill[index])
		{
		case -1: // Hollow (Automatic)
		case 0: // Hollow
			fprintf(KiCadFile, "\t\t\t(type none)\n");
			myPrint("\tFill : None\n");
			break;

		case 1: // Solid
			fprintf(KiCadFile, "\t\t\t(type outline)\n");
			myPrint("\tFill: Solid\n");
			opacity = 100;
			break;

		case 2: // Diagdn1
		case 5: // Diagdn2
			fprintf(KiCadFile, "\t\t\t(type reverse_hatch)\n");
			myPrint("\tFill: Reverse Hatch\n");
			opacity = 100;
			break;

		case 3: // Diagup2
		case 6: // Diagup1
			fprintf(KiCadFile, "\t\t\t(type hatch)\n");
			myPrint("\tFill: Hatch\n");
			opacity = 100;
			break;

		case 11: // X2
		case 12: // X1
			fprintf(KiCadFile, "\t\t\t(type cross_hatch)\n");
			myPrint("\tFill: Cross Hatch\n");
			opacity = 100;
			break;

		case 7: // Horiz
		case 8: // Vert
			fprintf(KiCadFile, "\t\t\t(type outline)\n");
			myPrint("\tFill: Solid\n");
			opacity = 8;
			break;

		case 9: // Grid2
		case 10: // Grid1
			fprintf(KiCadFile, "\t\t\t(type outline)\n");
			myPrint("\tFill: Solid\n");
			opacity = 16;
			break;

		case 4: // Grey08
			fprintf(KiCadFile, "\t\t\t(type outline)\n");
			myPrint("\tFill: Solid\n");
			opacity = 8;
			break;

		case 15: // Grey04
			fprintf(KiCadFile, "\t\t\t(type outline)\n");
			myPrint("\tFill: Solid\n");
			opacity = 4;
			break;

		case 13: // Grey50
			fprintf(KiCadFile, "\t\t\t(type outline)\n");
			myPrint("\tFill: Solid\n");
			opacity = 50;
			break;

		case 14: // Grey92
			fprintf(KiCadFile, "\t\t\t(type outline)\n");
			myPrint("\tFill: Solid\n");
			opacity = 92;
			break;

		}

		if (opacity != -1)
		{
			if (ColorExt[index].Key == 0xff)
			{ // Default Color
				myPrint("\tFill: %d%c\n", opacity, 0x25);
				myPrint("\tColor: Default\n");
				fprintf(KiCadFile, "\t\t\t(color 0 0 0 %.2f)\n", ((float)opacity / 100.0));
			}
			else
			{ // Custom Color
				fprintf(KiCadFile, "\t\t\t(color %d %d %d %.2f)\n", ColorExt[index].Red, ColorExt[index].Green, ColorExt[index].Blue, ((float)opacity / 100.0));
				myPrint("\tFill: %d%c\n", opacity, 0x25);
				myPrint("\tColor: R:%d G:%d B:%d\n", ColorExt[index].Red, ColorExt[index].Green, ColorExt[index].Blue);
			}
		}
		fprintf(KiCadFile, "\t\t)\n");
#else
		// Old KiCad has no fill patern. Translate into fill opacity
		fprintf(KiCadFile, "\t\t(fill\n");
		switch (Fill[index])
		{
		case -1: // Hollow (Automatic)
		case 0: // Hollow
			break;
		case 1: // Solid
			opacity = 100;
			break;
		case 2: // Diagdn1
		case 3: // Diagup2
		case 4: // Grey08
		case 5: // Diagdn2
		case 6: // Diagup1
		case 7: // Horiz
		case 8: // Vert
			opacity = 8;
			break;
		case 9: // Grid2
		case 10: // Grid1
		case 11: // X2
		case 12: // X1
			opacity = 16;
			break;
		case 13: // Grey50
			opacity = 50;
			break;
		case 14: // Grey92
			opacity = 92;
			break;
		case 15: // Grey04
			opacity = 4;
			break;
		}

		if (opacity == -1)
		{
			fprintf(KiCadFile, "\t\t\t(type none)\n");
			myPrint("\tFill: None\n");
		}
		else
		{
			fprintf(KiCadFile, "\t\t\t(type color)\n");
			if (ColorExt[index].Key == 0xff)
			{ // Default Color
				myPrint("\tFill: %d%c\n", opacity, 0x25);
				myPrint("\tColor: Default\n");
				fprintf(KiCadFile, "\t\t\t(color 0 0 0 %.2f)\n", ((float)opacity / 100.0));
			}
			else
			{ // Custom Color
				fprintf(KiCadFile, "\t\t\t(color %d %d %d %.2f)\n", ColorExt[index].Red, ColorExt[index].Green, ColorExt[index].Blue, ((float)opacity / 100.0));
				myPrint("\tFill: %d%c\n", opacity, 0x25);
				myPrint("\tColor: R:%d G:%d B:%d\n", ColorExt[index].Red, ColorExt[index].Green, ColorExt[index].Blue);
			}
		}
		fprintf(KiCadFile, "\t\t)\n");
#endif
	}
	fprintf(KiCadFile, "\t)\n");
}

/*
******************************************************************
* - function name:	KiCadProcessText()
*
* - description: 	Stores texts in KiCad
*
* - parameter: 		Pointer to KiCad file; 1 for texts, 0 for labels
*
* - return value: 	-
******************************************************************
*/
void KiCadProcessText(FILE* KiCadFile, uint8_t type)
{
	text_struct* InputString;
	uint32_t NumData;
	uint32_t index;
	uint8_t Overbar;

	if (type == 0) // Text
	{
		InputString = TextString;
		NumData = NumTextData;
	}
	else // Label
	{
		InputString = NetLabel;
		NumData = NumNetLabel;
	}

	if (InputString != 0 &&
		TextPos != 0 &&
		TextOrigin != 0 &&
		TextSize != 0 &&
		TextOrient != 0 &&
		TextFont != 0 &&
		TextColor != 0 &&
		NumData > 0 &&
		((Label2TextData != 0 && type != 0) || (Text2TextData != 0 && type == 0)))
	{
		if (type == 0) // Text
		{
			myPrint("\nText:\n");
		}
		else // Label
		{
			myPrint("\nLabel:\n");
		}
		for (uint32_t i = 0; i < NumText; i++)
		{
			if (type == 0)
			{
				index = Text2TextData[i] - 1;
			}
			else
			{
				index = Label2TextData[i].ID - 1;
			}
			if (TextSize[index] != 0 ||
				TextPos[index].X != 0 ||
				TextPos[index].Y != 0 ||
				TextOrigin[index] != 0 ||
				TextOrient[index] != 0 ||
				TextColor[index].Red != 0 ||
				TextColor[index].Green != 0 ||
				TextColor[index].Blue != 0 ||
				TextColor[index].Key != 0)
			{
				if (type == 0) // Text
				{
					fprintf(KiCadFile, "\t(text %c", 0x22);
				}
				else // Label
				{
					fprintf(KiCadFile, "\t(label %c", 0x22);
				}
				// Text String
				Overbar = 0;
				for (uint32_t j = 0; j < InputString[i].Lenth; j++)
				{
					if (InputString[i].Text[j] == '\n') // Check for line feed
					{
						fprintf(KiCadFile, "%cn", 0x5C); // Newline

					}
					else if (InputString[i].Text[j] == '~') // Check for overbar
					{
						if (Overbar)
						{
							Overbar = 0;
							fprintf(KiCadFile, "}"); // Close Overbar
						}
						else
						{
							Overbar = 1;
							fprintf(KiCadFile, "~{"); // Open Overbar
						}
					}
					else
					{
						fprintf(KiCadFile, "%c", InputString[i].Text[j]);
					}
				}
				if (Overbar)
				{
					fprintf(KiCadFile, "}"); // Close Overbar
				}
				if (type == 0) // Text
				{
					myPrint("Text %d\n", i + 1);
				}
				else // Label
				{
					myPrint("Label %d\n", i + 1);
				}
				myPrint("\t[%s]\n", InputString[i].Text);

				fprintf(KiCadFile, "%c\n", 0x22);

				// Position
				num_struct X = numProcess(TextPos[index].X, CoordinateScaleX, CoordinateOffsetX);
				num_struct Y = numProcess(TextPos[index].Y, CoordinateScaleY, CoordinateOffsetY);
				myPrint("\tX: %d.%05d, Y: %d.%05d\n", X.Integ, X.Frac, Y.Integ, Y.Frac);
				fprintf(KiCadFile, "\t\t(at %d.%05d %d.%05d %d)\n", X.Integ, X.Frac, Y.Integ, Y.Frac, TextOrient[index] * 90);

				fprintf(KiCadFile, "\t\t(effects\n");
				fprintf(KiCadFile, "\t\t\t(font \n");

				// Font
				if (TextFont[index].Lenth > 2) // custom font
				{
					uint32_t index2 = (uint32_t)strchr(TextFont[index].Text, '|') - (uint32_t)TextFont[index].Text + 1; // get index of Font name
					uint32_t FontNameLen = (uint32_t)strchr(TextFont[index].Text + index2, '|') - (uint32_t)TextFont[index].Text - index2;
					char* FontString = calloc(FontNameLen + 1, sizeof(char));
					if (FontString != 0) // Custom Font
					{
						memcpy(FontString, TextFont[index].Text + index2, FontNameLen);
						FontString[FontNameLen] = '\0'; // Zero terminate

						fprintf(KiCadFile, "\t\t\t\t(face %c%s%c)\n", 0x22, &FontString[0], 0x22);
						myPrint("\tFont: %s\n", &FontString[0]);

						if (TextFont[index].Text[FontNameLen + 7] == '1') // Strikeout
						{

						}
						if (TextFont[index].Text[FontNameLen + 9] == '1') // Underlined
						{

						}
						if (TextFont[index].Text[FontNameLen + 11] == '1') // Bold
						{
							fprintf(KiCadFile, "\t\t\t\t(bold yes)\n");
							myPrint("\tBold\n");
						}
						free(FontString);
					}
				}
				else if (strcmp(TextFont[index].Text, "2") == 0) // Roman Italic
				{
					myPrint("\tFont: Default\n");
					fprintf(KiCadFile, "\t\t\t\t(italic yes)\n");
					myPrint("\tItalic\n");
				}
				else if (strcmp(TextFont[index].Text, "3") == 0) // Roman Bold
				{
					myPrint("\tFont: Default\n");
					fprintf(KiCadFile, "\t\t\t\t(bold yes)\n");
					myPrint("\tBold\n");
				}
				else if (strcmp(TextFont[index].Text, "4") == 0) // Roman Bold Italic
				{
					myPrint("\tFont: Default\n");
					fprintf(KiCadFile, "\t\t\t\t(bold yes)\n");
					fprintf(KiCadFile, "\t\t\t\t(italic yes)\n");
					myPrint("\tBold & Italic\n");
				}
				else if (strcmp(TextFont[index].Text, "5") == 0) // SansSerif
				{
					fprintf(KiCadFile, "\t\t\t\t(face %c%s%c)\n", 0x22, "SansSerif", 0x22);
					myPrint("\tFont: SansSerif\n");
				}
				else if (strcmp(TextFont[index].Text, "6") == 0) // Script
				{
					fprintf(KiCadFile, "\t\t\t\t(face %c%s%c)\n", 0x22, "ScriptS", 0x22);
					myPrint("\tFont: ScriptS\n");
				}
				else if (strcmp(TextFont[index].Text, "7") == 0) // SansSerif Bold
				{
					fprintf(KiCadFile, "\t\t\t\t(face %c%s%c)\n", 0x22, "SansSerif", 0x22);
					myPrint("\tFont: SansSerif\n");
					fprintf(KiCadFile, "\t\t\t\t(bold yes)\n");
					myPrint("\tBold\n");
				}
				else if (strcmp(TextFont[index].Text, "8") == 0) // Script Bold
				{
					fprintf(KiCadFile, "\t\t\t\t(face %c%s%c)\n", 0x22, "ScriptS", 0x22);
					myPrint("\tFont: ScriptS\n");
					fprintf(KiCadFile, "\t\t\t\t(bold yes)\n");
					myPrint("\tBold\n");
				}
				else if (strcmp(TextFont[index].Text, "9") == 0) // Gothic
				{
					fprintf(KiCadFile, "\t\t\t\t(face %c%s%c)\n", 0x22, "GothicE", 0x22);
					myPrint("\tFont: GothicE\n");
				}
				else if (strcmp(TextFont[index].Text, "10") == 0) // Old English
				{
					fprintf(KiCadFile, "\t\t\t\t(face %c%s%c)\n", 0x22, "Old English Text MT", 0x22);
					myPrint("\tFont: Old English Text MT\n");
				}
				else if (strcmp(TextFont[index].Text, "11") == 0) // Kanji
				{
					myPrint("\tFont: Kanji => Not supported in KiCad!\n");
				}
				else if (strcmp(TextFont[index].Text, "12") == 0) // Plot
				{
					myPrint("\tFont: Plot => Not supported in KiCad!\n");
				}
				else
				{
					myPrint("\tFont: Default\n");
				}

				// Text Size
				num_struct Size = numProcess(TextSize[index] * FontScale, 1, 0);

				fprintf(KiCadFile, "\t\t\t\t(size %d.%05d %d.%05d)\n", Size.Integ, Size.Frac, Size.Integ, Size.Frac);

				// Text Color
				if (TextColor[index].Key != 0xff)
				{
					//(color 21 255 12 1)
					fprintf(KiCadFile, "\t\t\t\t(color %d %d %d 1)\n", TextColor[index].Red, TextColor[index].Green, TextColor[index].Blue);
					myPrint("\tColor: R:%d G:%d B:%d\n", TextColor[index].Red, TextColor[index].Green, TextColor[index].Blue);
				}
				else
				{
					myPrint("\tColor: Default");
					fprintf(KiCadFile, "\t\t\t\t(color 0 0 0 0)\n");
				}

				fprintf(KiCadFile, "\t\t\t)\n");

				myPrint("\tSize: %d.%05d\n", Size.Integ, Size.Frac);

				// Text Orientation
				fprintf(KiCadFile, "\t\t\t(justify");

				/*
				Orientation codes:
				None = 0 (Lower left)
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
				uint32_t tempOrigin = TextOrigin[index];
				if (tempOrigin == 0)
				{
					tempOrigin = DefaultTextOrigin;
				}
				else if (tempOrigin > 9)
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

				fprintf(KiCadFile, "\t)\n");

				// Rotation
				myPrint("\tRotation: %d Degree\n", TextOrient[index] * 90);

			}
		}
	}
}

/*
******************************************************************
* - function name:	Processing()
*
* - description: 	rearange nets into a more userfriendly form
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void Processing(void)
{
	myPrint("\n------------------- Processing -------------------\n");
	myPrint("\nAssigning Segments:\n");
	NumNetsBus = NumSegments;
	myPrint("Number of Nets/Busse: %d\n", NumNetsBus);
	NetBus = calloc(NumNetsBus, sizeof(net_bus_struct)); // Reseve memory
	uint32_t* PointOccurrence = calloc(NumJoint, sizeof(uint32_t)); // Temporary joint counter

	// Assign Coordinates
	if (Segments != 0 && NumNetsBus > 0 && NetBus != 0)
	{
		for (uint32_t i = 0; i < NumNetsBus; i++)
		{
			uint32_t address = 0;
			NetBus[i].NumSegments = Segments[i].Len;
			NetBus[i].Segments = calloc(NetBus[i].NumSegments, sizeof(net_bus_segment_struct));
			if (Segments[i].Data != 0 && Segments[i].Len > 0 && NetBus[i].Segments != 0)
			{
				NetBus[i].NumSegments = Segments[i].Len;
				for (uint32_t j = 0; j < Segments[i].Len; j++)
				{
					if (Segments[i].Data[j] != 0)
					{
						address += Segments[i].Data[j] + 1;
					}
					else
					{
						address++;
					}
					NetBus[i].Segments[j].ID = address - 1;
					NetBus[i].Segments[j].StartID = Segment2Joints[address - 2].StartJoint;
					NetBus[i].Segments[j].EndID = Segment2Joints[address - 2].EndJoint;
					NetBus[i].Segments[j].StartCoord = JointPoss[NetBus[i].Segments[j].StartID - 1];
					NetBus[i].Segments[j].EndCoord = JointPoss[NetBus[i].Segments[j].EndID - 1];
					NetBus[i].Style = Net2NetDxD[i].ID - 1; // ToDo: Needs rework by using Net2GrpStyles or Bus2GrpStyles and putting Net2NetDxD at NetBUS address

					// Count usages of each point
					PointOccurrence[NetBus[i].Segments[j].StartID - 1]++;
					PointOccurrence[NetBus[i].Segments[j].EndID - 1]++;
					if (PointOccurrence[NetBus[i].Segments[j].StartID - 1] == 3) // Count up joints counter when 3 nets meet
					{
						NetBus[i].NumJoints++;
					}
					if (PointOccurrence[NetBus[i].Segments[j].EndID - 1] == 3) // Count up joints counter when 3 nets meet
					{
						NetBus[i].NumJoints++;
					}
				}
				NetBus[i].Joints = calloc(NetBus[i].NumJoints, sizeof(coordinate_struct)); // Allocate memmory for joints
				if (NetBus[i].Joints != 0 && PointOccurrence != 0)
				{
					uint32_t k = 0;
					// Add Joints where 3 nets meet
					for (uint32_t j = 0; j < Segments[i].Len; j++)
					{
						if (PointOccurrence[NetBus[i].Segments[j].StartID - 1] > 2)
						{
							NetBus[i].Joints[k] = NetBus[i].Segments[j].StartCoord;
							PointOccurrence[NetBus[i].Segments[j].StartID - 1] = 0; // Reset counter
							k++;
						}
						if (PointOccurrence[NetBus[i].Segments[j].EndID - 1] > 2)
						{
							NetBus[i].Joints[k] = NetBus[i].Segments[j].EndCoord;
							PointOccurrence[NetBus[i].Segments[j].EndID - 1] = 0; // Reset counter
							k++;
						}
					}
				}
			}
		}
		free(PointOccurrence);
	}

	// Add Net/Bus info

	//if (Bus2GrpSegments != 0 && Net2GrpSegments != 0 && NumSegments > 0 && NetBus != NULL)
	//{
	//	for (uint32_t i = 0; i < NumNet; i++)
	//	{
	//		NetBus[Net2GrpSegments[i].ID - 1].IsBus = 0; // this causes Memory leak!
	//	}
	//	for (uint32_t i = 0; i < NumBus; i++)
	//	{
	//		NetBus[Bus2GrpSegments[i].ID - 1].IsBus = 1;
	//	}
	//}


	// Diag output
	if (NumNetsBus > 0 && NetBus != 0)
	{
		for (uint32_t i = 0; i < NumNetsBus; i++)
		{
			myPrint("\tNet/Bus %d: \n\t\tType: ", i + 1);
			if (NetBus[i].IsBus)
			{
				myPrint("Bus\n");
			}
			else
			{
				myPrint("Net\n");
			}
			myPrint("\t\tStyle: %d\n", NetBus[i].Style + 1);
			myPrint("\t\tSegments: %d\n", NetBus[i].NumSegments);
			for (uint32_t j = 0; j < NetBus[i].NumSegments; j++)
			{
				if (NetBus[i].Segments != 0)
				{
					myPrint("\t\t\t ID: %d, Start X: %d, Start Y: %d, End X: %d, End Y: %d\n", NetBus[i].Segments[j].ID, NetBus[i].Segments[j].StartCoord.X, NetBus[i].Segments[j].StartCoord.Y, NetBus[i].Segments[j].EndCoord.X, NetBus[i].Segments[j].EndCoord.Y);
				}
			}
			myPrint("\t\tJoints: %d\n", NetBus[i].NumJoints);
			for (uint32_t j = 0; j < NetBus[i].NumJoints; j++)
			{
				if (NetBus[i].Joints != 0)
				{
					myPrint("\t\t\tX: %d, Y: %d\n", NetBus[i].Joints[j].X, NetBus[i].Joints[j].Y);
				}
			}
		}
	}
}