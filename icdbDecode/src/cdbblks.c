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
#include "cdbblks.h"
#include <stdint.h>		// Required for int32_t, uint32_t, ...
#include <stdlib.h>		// Required for calloc to work properly
#include "common.h"		// Required for parseFile

/*
******************************************************************
* Defines
******************************************************************
*/
#define PATH_DXDATL "dxdatl.v"
#define PATH_BLKATL "blkatl.v"

/*
******************************************************************
* Global Variables
******************************************************************
*/
int32_t NumJoint = -1;
coordinate_struct* JointPoss = NULL;
uint32_t* JointIDs = NULL;

int32_t NumSegment2Joints = -1;
segment2joint_struct* Segment2Joints = NULL;
uint32_t* SegmentNets = 0;

int32_t NumStyles = -1;
color_struct* Color = NULL;
color_struct* ColorExt = NULL;
uint32_t* Style = NULL;
uint32_t* Thickness = NULL;
uint32_t* Fill = NULL;

int32_t NumSegments = -1;
segment_struct* Segments = NULL;

int32_t NumLines = -1;
linepoint_struct* LinePointX = NULL;
linepoint_struct* LinePointY = NULL;
style_struct* Line2Style = NULL;

int32_t NumLineEnds = -1;
uint32_t* LineEndingCap = NULL;
uint32_t* LineStartingCap = NULL;
uint32_t* LineArrowSize = NULL;

int32_t NumRectangle = -1;
rectangle_struct* RectCoord = NULL;
style_struct* Rect2Style = NULL;

int32_t NumCircle = - 1;
coordinate_struct* CirclesPos = NULL;
uint32_t* CircleRadius = NULL;
style_struct* Circle2Style = NULL;

int32_t NumArc = -1;
coordinate_struct* ArcEnd = NULL;
coordinate_struct* ArcStart = NULL;
coordinate_struct* ArcMid = NULL;
style_struct* Arc2Style = NULL;

int32_t NumTextData = -1;
text_struct* TextFont = NULL;
coordinate_struct* TextPos = NULL;
uint32_t* TextOrigin = NULL;
uint32_t* TextSize = NULL;
uint32_t* TextOrient = NULL;
color_struct* TextColor = NULL;
uint32_t* TextIDDxD = NULL;

int32_t NumText = -1;
text_struct* TextString = NULL;
uint32_t* Text2TextData = NULL;

int32_t NumNetLabel = -1;
text_struct* NetLabel = NULL;
group_struct* Label2TextData = NULL;

int32_t NumBus = -1;
bus_id_struct* BusID = NULL;
group_struct* Bus2GrpSegments = NULL;
group_struct* Bus2GrpStyles = NULL;

int32_t NumNet = -1;
net_id_struct* NetID = NULL;
group_struct* Net2GrpSegments = NULL;
group_struct* Net2GrpStyles = NULL;
group_struct* Net2NetDxD = NULL;


/*
******************************************************************
* Local Function Protptypes
******************************************************************
*/
void ProcessKeyDxdatl(FILE*, char*);
void ProcessKeyBlkatl(FILE*, char*);
void InitDxdatl(void);
void InitBlkatl(void);
segment_struct* ParseSegment(FILE*, int32_t*, char*);
void InitSegment(int32_t, segment_struct*);
void WriteOutput(void);

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
int parseCdbblks(char* path, uint32_t pathlenth)
{
	InitDxdatl();
	InitBlkatl();
	int errorcode = 0;
	errorcode |= parseFile(path, pathlenth, PATH_DXDATL, sizeof(PATH_DXDATL), ProcessKeyDxdatl);
	errorcode |= parseFile(path, pathlenth, PATH_BLKATL, sizeof(PATH_BLKATL), ProcessKeyBlkatl);


	WriteOutput();
	return errorcode;
}

/*
******************************************************************
* - function name:	ProcessKeyDxdatl()
*
* - description: 	Check for dxdatl keys
*
* - parameter: 		file pointer; Key to check
*
* - return value: 	-
******************************************************************
*/
void ProcessKeyDxdatl(FILE* sourceFile, char* Name)
{
	if (strcmp(Name, "Arc2Style") == 0)
	{
		Arc2Style = Parse(sourceFile, &NumArc, sizeof(style_struct), sizeof(style_struct), Name);
	}
	else if (strcmp(Name, "ArcEnd") == 0)
	{
		ArcEnd = Parse(sourceFile, &NumArc, sizeof(coordinate_struct), sizeof(uint32_t), Name);
	}
	else if (strcmp(Name, "ArcGUID") == 0)
	{
	}
	else if (strcmp(Name, "ArcIDDxD") == 0)
	{
	}
	else if (strcmp(Name, "ArcMid") == 0)
	{
		ArcMid = Parse(sourceFile, &NumArc, sizeof(coordinate_struct), sizeof(uint32_t), Name);	
	}
	else if (strcmp(Name, "ArcStart") == 0)
	{
		ArcStart = Parse(sourceFile, &NumArc, sizeof(coordinate_struct), sizeof(uint32_t), Name);
	}
	else if (strcmp(Name, "ArcUID") == 0)
	{
	}
	else if (strcmp(Name, "Attr2TextData") == 0)
	{
	}
	else if (strcmp(Name, "AttrID") == 0)
	{
	}
	else if (strcmp(Name, "AttrIDDxD") == 0)
	{
	}
	else if (strcmp(Name, "AttrIndexDxD") == 0)
	{
	}
	else if (strcmp(Name, "AttrIndexDxDNet") == 0)
	{
	}
	else if (strcmp(Name, "AttrIndexNdx") == 0)
	{
	}
	else if (strcmp(Name, "AttrSegmentNum") == 0)
	{
	}
	else if (strcmp(Name, "AttrVisibility") == 0)
	{
	}
	else if (strcmp(Name, "Block2Grp") == 0)
	{
	}
	else if (strcmp(Name, "BlockBBox") == 0)
	{
	}
	else if (strcmp(Name, "BlockGUID") == 0)
	{
	}
	else if (strcmp(Name, "BlockSize") == 0)
	{
	}
	else if (strcmp(Name, "Bus2GUID") == 0)
	{
	}
	else if (strcmp(Name, "Bus2GrpLabels") == 0)
	{
	}
	else if (strcmp(Name, "Bus2GrpSegments") == 0)
	{
		Bus2GrpSegments = Parse(sourceFile, &NumBus, sizeof(group_struct), sizeof(group_struct), Name);
	}
	else if (strcmp(Name, "Bus2GrpStyles") == 0)
	{
		Bus2GrpStyles = Parse(sourceFile, &NumBus, sizeof(group_struct), sizeof(group_struct), Name);

	}
	else if (strcmp(Name, "Bus2Grps") == 0)
	{
	}
	else if (strcmp(Name, "BusGUID") == 0)
	{
	}
	else if (strcmp(Name, "BusID") == 0)
	{
		BusID = Parse(sourceFile, &NumBus, sizeof(bus_id_struct), sizeof(bus_id_struct), Name);
	}
	else if (strcmp(Name, "Circle2Style") == 0)
	{
		Circle2Style = Parse(sourceFile, &NumCircle, sizeof(style_struct), sizeof(style_struct), Name);
	}
	else if (strcmp(Name, "CircleGUID") == 0)
	{
	}
	else if (strcmp(Name, "CircleIDDxD") == 0)
	{
	}
	else if (strcmp(Name, "CirclePos") == 0)
	{
		CirclesPos = Parse(sourceFile, &NumCircle, sizeof(coordinate_struct), sizeof(uint32_t), Name);
	}
	else if (strcmp(Name, "CircleRadius") == 0)
	{
		CircleRadius = Parse(sourceFile, &NumCircle, sizeof(uint32_t), sizeof(uint32_t), Name);
	}
	else if (strcmp(Name, "CircleUID") == 0)
	{
	}
	else if (strcmp(Name, "Cmp2GUID") == 0)
	{
	}
	else if (strcmp(Name, "Cmp2Grps") == 0)
	{
	}
	else if (strcmp(Name, "Cmp2Label") == 0)
	{
	}
	else if (strcmp(Name, "CmpDblScale") == 0)
	{
	}
	else if (strcmp(Name, "CmpGUID") == 0)
	{
	}
	else if (strcmp(Name, "CmpID") == 0)
	{
	}
	else if (strcmp(Name, "CmpOrient") == 0)
	{
	}
	else if (strcmp(Name, "CmpPos") == 0)
	{
	}
	else if (strcmp(Name, "CmpScale") == 0)
	{
	}
	else if (strcmp(Name, "Color") == 0)
	{
		Color = Parse(sourceFile, &NumStyles, sizeof(color_struct), sizeof(color_struct), Name);
	}
	else if (strcmp(Name, "ColorExt") == 0)
	{
		ColorExt = Parse(sourceFile, &NumStyles, sizeof(color_struct), sizeof(color_struct), Name);
	}
	else if (strcmp(Name, "ComplexShape2Style") == 0)
	{
	}
	else if (strcmp(Name, "ComplexShapeEdges") == 0)
	{
	}
	else if (strcmp(Name, "ComplexShapeGUID") == 0)
	{
	}
	else if (strcmp(Name, "ComplexShapeIDDxD") == 0)
	{
	}
	else if (strcmp(Name, "ComplexShapePoints") == 0)
	{
	}
	else if (strcmp(Name, "ComplexShapeUID") == 0)
	{
	}
	else if (strcmp(Name, "CompoundOrient") == 0)
	{
	}
	else if (strcmp(Name, "CompoundOrigin") == 0)
	{
	}
	else if (strcmp(Name, "CompoundPos") == 0)
	{
	}
	else if (strcmp(Name, "DrawingOrder") == 0)
	{
	}
	else if (strcmp(Name, "DrawingOrder2Grps") == 0)
	{
	}
	else if (strcmp(Name, "DxDNet2DxDNets") == 0)
	{
	}
	else if (strcmp(Name, "DxDNet2Signals") == 0)
	{
	}
	else if (strcmp(Name, "DxDNetRefID") == 0)
	{
	}
	else if (strcmp(Name, "Fill") == 0)
	{
		Fill = Parse(sourceFile, &NumStyles, sizeof(uint32_t), sizeof(uint32_t), Name);
	}
	else if (strcmp(Name, "Fixes") == 0)
	{
	}
	else if (strcmp(Name, "Grp2GUID") == 0)
	{
	}
	else if (strcmp(Name, "Grp2Grps") == 0)
	{
	}
	else if (strcmp(Name, "Grp2Label") == 0)
	{
	}
	else if (strcmp(Name, "Grp2Styles") == 0)
	{
	}
	else if (strcmp(Name, "GrpBox") == 0)
	{
	}
	else if (strcmp(Name, "GrpGUID") == 0)
	{
	}
	else if (strcmp(Name, "GrpID") == 0)
	{
	}
	else if (strcmp(Name, "GrpLocked") == 0)
	{
	}
	else if (strcmp(Name, "Joint2UID") == 0)
	{
	}
	else if (strcmp(Name, "JointGUID") == 0)
	{
	}
	else if (strcmp(Name, "JointID") == 0)
	{
		JointIDs = Parse(sourceFile, &NumJoint, sizeof(uint32_t), sizeof(uint32_t), Name);
	}
	else if (strcmp(Name, "JointPos") == 0)
	{
		JointPoss = Parse(sourceFile, &NumJoint, sizeof(coordinate_struct), sizeof(uint32_t), Name);
	}
	else if (strcmp(Name, "Label2TextData") == 0)
	{
		Label2TextData = Parse(sourceFile, &NumNetLabel, sizeof(group_struct), sizeof(group_struct), Name);
	}
	else if (strcmp(Name, "LabelIDDxD") == 0)
	{
	}
	else if (strcmp(Name, "LabelIndexDxDNet") == 0)
	{
	}
	else if (strcmp(Name, "LabelInverted") == 0)
	{
	}
	else if (strcmp(Name, "LabelScope") == 0)
	{
	}
	else if (strcmp(Name, "LabelSegmentNum") == 0)
	{
	}
	else if (strcmp(Name, "LabelVisibility") == 0)
	{
	}
	else if (strcmp(Name, "Line2Style") == 0)
	{
		Line2Style = Parse(sourceFile, &NumLines, sizeof(style_struct), sizeof(style_struct), Name);
	}
	else if (strcmp(Name, "LineArrowSize") == 0)
	{
		LineArrowSize = Parse(sourceFile, &NumLineEnds, sizeof(uint32_t), sizeof(uint32_t), Name);
	}
	else if (strcmp(Name, "LineEndingCap") == 0)
	{
		LineEndingCap = Parse(sourceFile, &NumLineEnds, sizeof(uint32_t), sizeof(uint32_t), Name);
	}
	else if (strcmp(Name, "LineGUID") == 0)
	{
	}
	else if (strcmp(Name, "LineIDDxD") == 0)
	{
	}
	else if (strcmp(Name, "LinePointX") == 0)
	{
		LinePointX = Parse(sourceFile, &NumLines, sizeof(linepoint_struct), sizeof(linepoint_struct), Name);
	}
	else if (strcmp(Name, "LinePointY") == 0)
	{
		LinePointY = Parse(sourceFile, &NumLines, sizeof(linepoint_struct), sizeof(linepoint_struct), Name);
	}
	else if (strcmp(Name, "LineStartingCap") == 0)
	{
		LineStartingCap = Parse(sourceFile, &NumLineEnds, sizeof(uint32_t), sizeof(uint32_t), Name);
	}
	else if (strcmp(Name, "LineUID") == 0)
	{
	}
	else if (strcmp(Name, "Net2GrpLabels") == 0)
	{
	}
	else if (strcmp(Name, "Net2GrpSegments") == 0)
	{
		Net2GrpSegments = Parse(sourceFile, &NumNet, sizeof(group_struct), sizeof(group_struct), Name);
	}
	else if (strcmp(Name, "Net2GrpStyles") == 0)
	{
		Net2GrpStyles = Parse(sourceFile, &NumNet, sizeof(group_struct), sizeof(group_struct), Name);
	}
	else if (strcmp(Name, "Net2Grps") == 0)
	{
	}
	else if (strcmp(Name, "Net2NetDxD") == 0)
	{
		Net2NetDxD = Parse(sourceFile, &NumNet, sizeof(group_struct), sizeof(group_struct), Name);
	}
	else if (strcmp(Name, "NetDxD2Net") == 0)
	{
	}
	else if (strcmp(Name, "NetDxDUID") == 0)
	{
	}
	else if (strcmp(Name, "NetGUID") == 0)
	{
	}
	else if (strcmp(Name, "NetID") == 0)
	{
		NetID = Parse(sourceFile, &NumNet, sizeof(net_id_struct), sizeof(net_id_struct), Name);
	}
	else if (strcmp(Name, "NetLabel") == 0)
	{
		NetLabel = ParseString(sourceFile, &NumNetLabel, Name);
	}
	else if (strcmp(Name, "NetsDxD") == 0)
	{
	}
	else if (strcmp(Name, "Pin2Grps") == 0)
	{
	}
	else if (strcmp(Name, "Rect2Style") == 0)
	{
		Rect2Style = Parse(sourceFile, &NumRectangle, sizeof(style_struct), sizeof(style_struct), Name);
	}
	else if (strcmp(Name, "RectCoord") == 0)
	{
		RectCoord = Parse(sourceFile, &NumRectangle, sizeof(rectangle_struct), sizeof(uint32_t), Name);
	}
	else if (strcmp(Name, "RectGUID") == 0)
	{
	}
	else if (strcmp(Name, "RectIDDxD") == 0)
	{
	}
	else if (strcmp(Name, "RectUID") == 0)
	{
	}
	else if (strcmp(Name, "RipperDxDUID") == 0)
	{
	}
	else if (strcmp(Name, "Segment2Joints") == 0)
	{
		Segment2Joints = Parse(sourceFile, &NumSegment2Joints, sizeof(segment2joint_struct), sizeof(uint32_t), Name);
	}
	else if (strcmp(Name, "SegmentGUID") == 0)
	{
	}
	else if (strcmp(Name, "SegmentID") == 0)
	{
	}
	else if (strcmp(Name, "Segments") == 0)
	{
		Segments = ParseSegment(sourceFile, &NumSegments, Name);
	}
	else if (strcmp(Name, "SignalName") == 0)
	{
	}
	else if (strcmp(Name, "Style") == 0)
	{
		Style = Parse(sourceFile, &NumStyles, sizeof(uint32_t), sizeof(uint32_t), Name);
	}
	else if (strcmp(Name, "Text2TextData") == 0)
	{
		Text2TextData = Parse(sourceFile, &NumText, sizeof(uint32_t), sizeof(uint32_t), Name);
	}
	else if (strcmp(Name, "TextColor") == 0)
	{
		TextColor = Parse(sourceFile, &NumTextData, sizeof(color_struct), sizeof(color_struct), Name);
	}
	else if (strcmp(Name, "TextColorExt") == 0)
	{
	}
	else if (strcmp(Name, "TextCompounds") == 0)
	{
	}
	else if (strcmp(Name, "TextFont") == 0)
	{
		TextFont = ParseString(sourceFile, &NumTextData, Name);
	}
	else if (strcmp(Name, "TextGUID") == 0)
	{
	}
	else if (strcmp(Name, "TextIDDxD") == 0)
	{
		TextIDDxD = Parse(sourceFile, &NumTextData, sizeof(uint32_t), sizeof(uint32_t), Name);
	}
	else if (strcmp(Name, "TextOrient") == 0)
	{
		TextOrient = Parse(sourceFile, &NumTextData, sizeof(uint32_t), sizeof(uint32_t), Name);
	}
	else if (strcmp(Name, "TextOrigin") == 0)
	{
		TextOrigin = Parse(sourceFile, &NumTextData, sizeof(uint32_t), sizeof(uint32_t), Name);
	}
	else if (strcmp(Name, "TextPos") == 0)
	{
		TextPos = Parse(sourceFile, &NumTextData, sizeof(coordinate_struct), sizeof(uint32_t), Name);
	}
	else if (strcmp(Name, "TextSize") == 0)
	{
		TextSize = Parse(sourceFile, &NumTextData, sizeof(uint32_t), sizeof(uint32_t), Name);
	}
	else if (strcmp(Name, "TextString") == 0)
	{
		TextString = ParseString(sourceFile, &NumText, Name);
	}
	else if (strcmp(Name, "TextUID") == 0)
	{
	}
	else if (strcmp(Name, "Thickness") == 0)
	{
		Thickness = Parse(sourceFile, &NumStyles, sizeof(uint32_t), sizeof(uint32_t), Name);
	}
	else if (strcmp(Name, "TxtGUID") == 0)
	{
	}
	else if (strcmp(Name, "TxtIDDxD") == 0)
	{
	}
	else if (strcmp(Name, "Version") == 0)
	{
	}
	else
	{
		myPrint("Unknown Key [%s]\n", Name);
	}
}

/*
******************************************************************
* - function name:	ProcessKeyBlkatl()
*
* - description: 	Check for blkatl keys
*
* - parameter: 		file pointer; Key to check
*
* - return value: 	-
******************************************************************
*/
void ProcessKeyBlkatl(FILE* sourceFile, char* Name)
{
	if (strcmp(Name, "BNetFlg") == 0)
		{

		}
	else if (strcmp(Name, "BPin2Nets") == 0)
		{

		}
	else if (strcmp(Name, "BPin2Prps") == 0)
		{

		}
	else if (strcmp(Name, "BPinTyp") == 0)
		{

		}
	else if (strcmp(Name, "BPinUID") == 0)
		{

		}
	else if (strcmp(Name, "BPins2Pins") == 0)
		{

		}
	else if (strcmp(Name, "BSym2BPins") == 0)
		{

		}
	else if (strcmp(Name, "BSym2CMatrix") == 0)
		{

		}
	else if (strcmp(Name, "BSym2Prps") == 0)
		{

		}
	else if (strcmp(Name, "BSym2Sub") == 0)
		{

		}
	else if (strcmp(Name, "BSym2Xtrs") == 0)
		{

		}
	else if (strcmp(Name, "BSymArrayed") == 0)
		{

		}
	else if (strcmp(Name, "BSymForwardPCB") == 0)
		{

		}
	else if (strcmp(Name, "BSymRef") == 0)
		{

		}
	else if (strcmp(Name, "BSymUID") == 0)
		{

		}
	else if (strcmp(Name, "BlkPrps") == 0)
		{

		}
	else if (strcmp(Name, "Bus2Nets") == 0)
		{

		}
	else if (strcmp(Name, "Bus2Prps") == 0)
		{

		}
	else if (strcmp(Name, "BusNam") == 0)
		{

		}
	else if (strcmp(Name, "BusParent") == 0)
		{

		}
	else if (strcmp(Name, "BusType") == 0)
		{

		}
	else if (strcmp(Name, "BusUID") == 0)
		{

		}
	else if (strcmp(Name, "CMatrixName") == 0)
		{

		}
	else if (strcmp(Name, "CMatrixPairs") == 0)
		{

		}
	else if (strcmp(Name, "CMatrixUID") == 0)
		{

		}
	else if (strcmp(Name, "Connection2CMatrix") == 0)
		{

		}
	else if (strcmp(Name, "ConnectionBit1") == 0)
		{

		}
	else if (strcmp(Name, "ConnectionBit2") == 0)
		{

		}
	else if (strcmp(Name, "ConnectionPin1") == 0)
		{

		}
	else if (strcmp(Name, "ConnectionPin2") == 0)
		{

		}
	else if (strcmp(Name, "DiffPrs") == 0)
		{

		}
	else if (strcmp(Name, "Fixes") == 0)
		{

		}
	else if (strcmp(Name, "GrpPrp2Prp") == 0)
		{

		}
	else if (strcmp(Name, "Net2BNet") == 0)
		{

		}
	else if (strcmp(Name, "Net2Prps") == 0)
		{

		}
	else if (strcmp(Name, "NetNam") == 0)
		{

		}
	else if (strcmp(Name, "PNetUID") == 0)
		{

		}
	else if (strcmp(Name, "Prp2AssocObj") == 0)
		{

		}
	else if (strcmp(Name, "PrpAttr") == 0)
		{

		}
	else if (strcmp(Name, "PrpBit") == 0)
		{

		}
	else if (strcmp(Name, "PrpId") == 0)
		{

		}
	else if (strcmp(Name, "PrpNam") == 0)
		{

		}
	else if (strcmp(Name, "PrpOrder") == 0)
		{

		}
	else if (strcmp(Name, "PrpStr") == 0)
		{

		}
	else if (strcmp(Name, "PrpUID") == 0)
		{

		}
	else if (strcmp(Name, "Sym2Cmp") == 0)
		{

		}
	else if (strcmp(Name, "Version") == 0)
		{

		}
	else if (strcmp(Name, "Xtr2BPin") == 0)
		{

		}
	else if (strcmp(Name, "Xtr2Prps") == 0)
		{

		}
	else if (strcmp(Name, "XtrBPin") == 0)
		{

		}
	else if (strcmp(Name, "XtrNam") == 0)
		{

		}
	else if (strcmp(Name, "XtrTyp") == 0)
		{

		}
	else if (strcmp(Name, "XtrUID") == 0)
	{

	}
	else
	{
		myPrint("Unknown Key [%s]\n", Name);
	}
}

/*
******************************************************************
* - function name:	InitDxdatl()
*
* - description: 	Resets all dxdatl data
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void InitDxdatl(void)
{
	InitRegular(NumJoint, &JointPoss);
	InitRegular(NumJoint, &JointIDs);
	NumJoint = -1;

	InitRegular(NumSegment2Joints, &Segment2Joints);
	NumSegment2Joints = -1;

	InitRegular(NumStyles, &Color);
	InitRegular(NumStyles, &ColorExt);
	InitRegular(NumStyles, &Style);
	InitRegular(NumStyles, &Thickness);
	InitRegular(NumStyles, &Fill);
	NumStyles = -1;

	InitSegment(NumSegments, Segments);
	NumSegments = -1;

	InitRegular(NumLines, &LinePointX);
	InitRegular(NumLines, &LinePointY);
	InitRegular(NumLines, &Line2Style);
	NumLines = -1;

	InitRegular(NumLineEnds, &LineEndingCap);
	InitRegular(NumLineEnds, &LineStartingCap);
	InitRegular(NumLineEnds, &LineArrowSize);
	NumLineEnds = -1;

	InitRegular(NumRectangle, &RectCoord);
	InitRegular(NumRectangle, &Rect2Style);
	NumRectangle = -1;

	InitRegular(NumCircle, &CirclesPos);
	InitRegular(NumCircle, &CircleRadius);
	InitRegular(NumCircle, &Circle2Style);
	NumCircle = -1;

	InitRegular(NumArc, &ArcEnd);
	InitRegular(NumArc, &ArcStart);
	InitRegular(NumArc, &ArcMid);
	InitRegular(NumArc, &Arc2Style);
	NumArc = -1;

	InitString(NumTextData, &TextFont);
	InitRegular(NumTextData, &TextPos);
	InitRegular(NumTextData, &TextOrigin);
	InitRegular(NumTextData, &TextSize);
	InitRegular(NumTextData, &TextOrient);
	InitRegular(NumTextData, &TextColor);
	InitRegular(NumTextData, &TextIDDxD);
	NumTextData = -1;

	InitString(NumText, &TextString);
	InitRegular(NumText, &Text2TextData);
	NumText = -1;

	InitString(NumNetLabel, &NetLabel);
	InitRegular(NumNetLabel, &Label2TextData);
	NumNetLabel = -1;

	InitRegular(NumBus, &BusID);
	InitRegular(NumBus, &Bus2GrpSegments);
	InitRegular(NumBus, &Bus2GrpStyles);
	NumBus = -1;

	InitRegular(NumNet, &NetID);
	InitRegular(NumNet, &Net2GrpSegments);
	InitRegular(NumNet, &Net2GrpStyles);
	InitRegular(NumNet, &Net2NetDxD);
	NumNet = -1;
}

/*
******************************************************************
* - function name:	InitBlkatl()
*
* - description: 	Resets all blkatl data
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void InitBlkatl(void)
{
	//InitRegular(int32_t, uint32_t*);
	//InitString(int32_t, text_struct*);
}

/*
******************************************************************
* - function name:	ParseSegment()
*
* - description: 	Parsing code for Segments
*
* - parameter: 		pointer to source file; pointer to number of elements to read; name of parameter (for error messages)
*
* - return value: 	pointer to parsed data
******************************************************************
*/
segment_struct* ParseSegment(FILE* sourceFile, int32_t* NumElements, char* Name)
{
	uint32_t PayloadLen = 0;
	uint32_t PayloadLenRaw;
	uint32_t BlockStartAddress;
	uint32_t EntryLen = 0;
	segment_struct* Struct = NULL;
	int32_t Repetitions;
	uint32_t Type = 0;
	uint32_t SizeAccumulator = 0;

	fseek(sourceFile, sizeof(uint32_t) * -1, SEEK_CUR);
	fread(&Type, sizeof(uint32_t), 1, sourceFile);
	if (Type != 2)
	{
		myPrint("Error! Wrong Type %d code in %s\n", Type, Name);
		return 0;
	}
	fread(&PayloadLenRaw, sizeof(uint32_t), 1, sourceFile);
	fseek(sourceFile, 16 + 4, SEEK_CUR);
	// Store initial address
	BlockStartAddress = ftell(sourceFile);

	// Count Entry (I haven't found a way to derive the number of entrys from PayloadLenRaw)
	while (PayloadLenRaw > SizeAccumulator && IsInsideFile(sourceFile))
	{
		fseek(sourceFile, EntryLen * sizeof(uint32_t), SEEK_CUR);
		fread(&EntryLen, sizeof(uint32_t), 1, sourceFile);

		if (EntryLen == 0x4FFFFFFF) // No more entrys
		{
			break;
		}
		else if (EntryLen == 0x4FFFFFFE) // Padding
		{
			fread(&Repetitions, sizeof(uint32_t), 1, sourceFile);
			EntryLen = 0;
		}
		else
		{
			PayloadLen++;
			SizeAccumulator += 8 + (EntryLen * sizeof(uint32_t));
		}
	}

	fseek(sourceFile, BlockStartAddress, SEEK_SET);

	if (*NumElements < 0)
	{
		*NumElements = PayloadLen;
	}
	if (*NumElements != PayloadLen)
	{
		printf("%s Missmatch! Expect %d, Got %d!\n", Name, *NumElements, PayloadLen);
		return 0;
	}
	else
	{
		Struct = calloc(*NumElements, sizeof(segment_struct));
		if (Struct != 0)
		{
			for (uint32_t i = 0; i < *NumElements;)
			{
				fread(&EntryLen, sizeof(uint32_t), 1, sourceFile);

				if (EntryLen == 0x4FFFFFFF) // No more entrys
				{
					// Seek back to not skip encode
					fseek(sourceFile, sizeof(uint32_t) * -1, SEEK_CUR);
					break;
				}
				else if (EntryLen == 0x4FFFFFFE) // Padding
				{
					fread(&Repetitions, sizeof(uint32_t), 1, sourceFile);
				}
				else
				{
					Struct[i].Len = EntryLen;
					Struct[i].Data = (uint32_t*)calloc(Struct[i].Len, sizeof(uint32_t));
					if (Struct[i].Data != 0)
					{
						fread(Struct[i].Data, sizeof(uint32_t), Struct[i].Len, sourceFile);
					}
					i++;
				}
			}
		}
	}
	return Struct;
}

/*
******************************************************************
* - function name:	InitSegment()
*
* - description: 	Resets all segments
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void InitSegment(int32_t len, segment_struct* structure)
{
	if (len > 0 && structure != 0)
	{
		for (uint32_t i = 0; i < len; i++)
		{
			if (structure[i].Len > 0 && structure[i].Data != 0)
			{
				free(structure[i].Data); // Free data
			}
		}
		free(structure); // Free Array of segmens
	}
}

/*
******************************************************************
* - function name:	WriteOutput()
*
* - description: 	Prints out parsed data
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void WriteOutput(void)
{
	myPrint("\n------------------ Debug Output ------------------\n");

	myPrint("\nJoints:\n");
	if (JointPoss != 0 && JointIDs != 0 && NumJoint > 0)
	{
		for (uint32_t i = 0; i < NumJoint; i++)
		{
			myPrint("\tIndex: %d ID: %d, X: %d, Y: %d\n", i + 1, JointIDs[i], JointPoss[i].X, JointPoss[i].Y);
		}
	}

	myPrint("\nSegments2Joints:\n");
	if (Segment2Joints != 0 && NumSegment2Joints > 0)
	{
		for (uint32_t i = 0; i < NumSegment2Joints; i++)
		{
			myPrint("\tSegment: %d Start Joint: %d End Joint: %d\n", i + 1, Segment2Joints[i].StartJoint, Segment2Joints[i].EndJoint);
		}
	}

	myPrint("\nSegments:\n");
	if (Segments != 0 && NumSegments > 0)
	{
		for (uint32_t i = 0; i < NumSegments; i++)
		{
			myPrint("\tSegments: %d Len: %d\n", i + 1, Segments[i].Len);
			if (Segments[i].Data != 0 && Segments[i].Len > 0)
			{
				for (uint32_t j = 0; j < Segments[i].Len; j++)
				{
					myPrint("\t\tEntry: %d Data: %d\n", j + 1, Segments[i].Data[j]);
				}
			}
		}
	}

	myPrint("\nLines:\n");
	if (LinePointX != 0 && LinePointY != 0 && Line2Style != 0 && NumLines > 0)
	{
		for (uint32_t i = 0; i < NumLines; i++)
		{
			myPrint("\tLine: %d X1: %d X2: %d Y1: %d Y2: %d Style: %d\n", i + 1, LinePointX[i].StartPoint, LinePointX[i].Offset, LinePointY[i].StartPoint, LinePointY[i].Offset, Line2Style[i].index);
		}
	}

	myPrint("\nLineEndings:\n");
	if (LineEndingCap != 0 && LineStartingCap != 0 && LineArrowSize != 0 && NumLineEnds > 0)
	{
		for (uint32_t i = 0; i < NumLineEnds; i++)
		{
			myPrint("\tLineEnding: %d StartArrow: %d EndArrow: %d ArrowSize: %d\n", i + 1, LineStartingCap[i], LineEndingCap[i], LineArrowSize[i]);
		}
	}

	myPrint("\nRectangles:\n");
	if (RectCoord != 0 && Line2Style != 0 && NumRectangle > 0)
	{
		for (uint32_t i = 0; i < NumRectangle; i++)
		{
			myPrint("\tRectangle: %d X1: %d X2: %d Y1: %d Y2: %d Style: %d\n", i + 1, RectCoord[i].Start.X, RectCoord[i].End.X, RectCoord[i].Start.Y, RectCoord[i].End.Y, Rect2Style[i].index);
		}
	}

	myPrint("\nCircles:\n");
	if (CircleRadius != 0 && CirclesPos != 0 && Circle2Style != 0 && NumCircle > 0)
	{
		for (uint32_t i = 0; i < NumCircle; i++)
		{
			myPrint("\tCircle: %d X: %d Y: %d R: %d Style: %d\n", i + 1, CirclesPos[i].X, CirclesPos[i].Y, CircleRadius[i], Circle2Style[i].index);
		}
	}

	myPrint("\nArcs:\n");
	if (ArcEnd != 0 && ArcStart != 0 && ArcMid != 0 && Arc2Style != 0 && NumArc > 0)
	{
		for (uint32_t i = 0; i < NumArc; i++)
		{
			myPrint("\tArcs: %d Start X: %d Y: %d Mid X: %d Y: %d End X: %d Y: %d Style: %d\n", i + 1, ArcStart[i].X, ArcStart[i].Y, ArcMid[i].X, ArcMid[i].Y, ArcEnd[i].X, ArcEnd[i].Y, Arc2Style[i].index);
		}
	}

	myPrint("\nTexts:\n");
	if (TextString != 0 && TextPos != 0 && TextOrigin != 0 && TextSize != 0 && TextOrient != 0 && TextFont != 0 && TextColor != 0 && NumTextData > 0 && Text2TextData != 0)
	{
		for (uint32_t i = 0; i < NumText; i++)
		{
			uint32_t index = Text2TextData[i] - 1;
			myPrint("\tText %d:\n", i + 1);
			myPrint("\t\t[%s]\n", TextString[i].Text);
			myPrint("\t\tX: %d\n", TextPos[index].X);
			myPrint("\t\tY: %d\n", TextPos[index].Y);
			myPrint("\t\tOrigin: %d\n", TextOrigin[index]);
			myPrint("\t\tSize: %d\n", TextSize[index]);
			myPrint("\t\tOrient: %d\n", TextOrient[index]);
			myPrint("\t\tFont: [%s]\n", TextFont[index].Text);
			myPrint("\t\tColor: R:%d G:%d B:%d Key: %d\n", TextColor[index].Red, TextColor[index].Green, TextColor[index].Blue, TextColor[index].Key);
			myPrint("\t\tTextID: %d\n", TextIDDxD[index]);

			myPrint("\n");
		}
		myPrint("\n");
	}

	myPrint("\nColor:\n");
	if (Color != 0 && NumStyles > 0)
	{
		for (uint32_t i = 0; i < NumStyles; i++)
		{
			myPrint("\tColor: %d Red: %d Green: %d Blue: %d Key: %d\n", i + 1, Color[i].Red, Color[i].Green, Color[i].Blue, Color[i].Key);
		}
	}

	myPrint("\nColorExt:\n");
	if (Color != 0 && NumStyles > 0)
	{
		for (uint32_t i = 0; i < NumStyles; i++)
		{
			myPrint("\tColorExt: %d Red: %d Green: %d Blue: %d Key: %d\n", i + 1, ColorExt[i].Red, ColorExt[i].Green, ColorExt[i].Blue, ColorExt[i].Key);
		}
	}

	myPrint("\nStyles:\n");
	if (Style != 0 && NumStyles > 0)
	{
		for (uint32_t i = 0; i < NumStyles; i++)
		{
			myPrint("\tIndex: %d Value: %d [ ", i + 1, Style[i]);
			switch (Style[i])
			{
			case -1:
				myPrint("Solid (Automatic)");
				break;
			case 0:
				myPrint("Solid");
				break;
			case 1:
				myPrint("Dash");
				break;
			case 2:
				myPrint("Center");
				break;
			case 3:
				myPrint("Phantom");
				break;
			case 4:
				myPrint("Big dash");
				break;
			case 5:
				myPrint("Dot");
				break;
			case 6:
				myPrint("Dash-Dot");
				break;
			case 7:
				myPrint("Medium dash");
				break;
			default:
				myPrint("Unknown Type!");
				break;
			}
			myPrint(" ]\n");

		}
	}

	myPrint("\nThickness:\n");
	if (Thickness != 0 && NumStyles > 0)
	{
		for (uint32_t i = 0; i < NumStyles; i++)
		{
			myPrint("\tIndex: %d Value: %d [ ", i + 1, Thickness[i]);
			if (Thickness[i] == -1)
			{
				myPrint("1 (Automatic)");
			}
			else if (Thickness[i] >= 1 && Thickness[i] <= 10)
			{
				myPrint("%d", Thickness[i]);
			}
			else
			{
				myPrint("Unknown Type!");
			}
			myPrint(" ]\n");
		}
	}

	myPrint("\nFill:\n");
	if (Fill != 0 && NumStyles > 0)
	{
		for (uint32_t i = 0; i < NumStyles; i++)
		{
			myPrint("\tIndex: %d Value: %d [ ", i + 1, Fill[i]);
			switch (Fill[i])
			{
			case -1:
				myPrint("Hollow (Automatic)");
				break;
			case 0:
				myPrint("Hollow");
				break;
			case 1:
				myPrint("Solid");
				break;
			case 2:
				myPrint("Diagdn1");
				break;
			case 3:
				myPrint("Diagup2");
				break;
			case 4:
				myPrint("Grey08");
				break;
			case 5:
				myPrint("Diagdn2");
				break;
			case 6:
				myPrint("Diagup1");
				break;
			case 7:
				myPrint("Horiz");
				break;
			case 8:
				myPrint("Vert");
				break;
			case 9:
				myPrint("Grid2");
				break;
			case 10:
				myPrint("Grid1");
				break;
			case 11:
				myPrint("X2");
				break;
			case 12:
				myPrint("X1");
				break;
			case 13:
				myPrint("Grey50");
				break;
			case 14:
				myPrint("Grey92");
				break;
			case 15:
				myPrint("Grey04");
				break;
			default:
				myPrint("Unknown Type!");
				break;
			}
			myPrint(" ]\n");
		}
	}

	myPrint("\nBusID:\n");
	if (BusID != 0 && NumBus > 0)
	{
		for (uint32_t i = 0; i < NumBus; i++)
		{
			myPrint("\tIndex: %d Idx: %d ID: %d\n", i + 1, BusID[i].index, BusID[i].ID);
		}
	}

	myPrint("\nNetID:\n");
	if (NetID != 0 && NumNet > 0)
	{
		for (uint32_t i = 0; i < NumNet; i++)
		{
			myPrint("\tIndex: %d ID: %d\n", i + 1, NetID[i].ID);
		}
	}

	myPrint("\nBus2GrpSegments:\n");
	if (Bus2GrpSegments != 0 && NumBus > 0)
	{
		for (uint32_t i = 0; i < NumBus; i++)
		{
			myPrint("\tIndex: %d Idx: %d ID: %d\n", i + 1, Bus2GrpSegments[i].index, Bus2GrpSegments[i].ID);
		}
	}

	myPrint("\nBus2GrpStyles:\n");
	if (Bus2GrpStyles != 0 && NumBus > 0)
	{
		for (uint32_t i = 0; i < NumBus; i++)
		{
			myPrint("\tIndex: %d Idx: %d ID: %d\n", i + 1, Bus2GrpStyles[i].index, Bus2GrpStyles[i].ID);
		}
	}

	myPrint("\nNet2GrpSegments:\n");
	if (Net2GrpSegments != 0 && NumNet > 0)
	{
		for (uint32_t i = 0; i < NumNet; i++)
		{
			myPrint("\tIndex: %d Idx: %d ID: %d\n", i + 1, Net2GrpSegments[i].index, Net2GrpSegments[i].ID);
		}
	}

	myPrint("\nNet2GrpStyles:\n");
	if (Net2GrpStyles != 0 && NumNet > 0)
	{
		for (uint32_t i = 0; i < NumNet; i++)
		{
			myPrint("\tIndex: %d Idx: %d ID: %d\n", i + 1, Net2GrpStyles[i].index, Net2GrpStyles[i].ID);
		}
	}

	myPrint("\nNet2NetDxD:\n");
	if (Net2NetDxD != 0 && NumNet > 0)
	{
		for (uint32_t i = 0; i < NumNet; i++)
		{
			myPrint("\tIndex: %d Idx: %d ID: %d\n", i + 1, Net2NetDxD[i].index, Net2NetDxD[i].ID);
		}
	}

	myPrint("\nNetLabel:\n");
	if (NetLabel != 0 && TextPos != 0 && TextOrigin != 0 && TextSize != 0 && TextOrient != 0 && TextFont != 0 && TextColor != 0 && NumTextData > 0 && Label2TextData != 0)
	{
		for (uint32_t i = 0; i < NumNetLabel; i++)
		{
			uint32_t index = Label2TextData[i].ID - 1;
			myPrint("\tLabel %d:\n", i + 1);
			myPrint("\t\t[%s]\n", NetLabel[i].Text);
			myPrint("\t\tX: %d\n", TextPos[index].X);
			myPrint("\t\tY: %d\n", TextPos[index].Y);
			myPrint("\t\tOrigin: %d\n", TextOrigin[index]);
			myPrint("\t\tSize: %d\n", TextSize[index]);
			myPrint("\t\tOrient: %d\n", TextOrient[index]);
			myPrint("\t\tFont: [%s]\n", TextFont[index].Text);
			myPrint("\t\tColor: R:%d G:%d B:%d Key: %d\n", TextColor[index].Red, TextColor[index].Green, TextColor[index].Blue, TextColor[index].Key);
			myPrint("\t\tTextID: %d\n", TextIDDxD[index]);

			myPrint("\n");
		}
	}
}