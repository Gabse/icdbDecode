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
#include "dxdatl.h"
#include <stdint.h>					// Required for int32_t, uint32_t, ...
#include <string.h>					// Required for strcmp
#include "../common.h"				// Required for parseFile

/*
******************************************************************
* Global Variables
******************************************************************
*/
key_struct* Dxdatl_Arc2Style = NULL;
key_struct* Dxdatl_ArcEnd = NULL;
key_struct* Dxdatl_ArcGUID = NULL;
key_struct* Dxdatl_ArcIDDxD = NULL;
key_struct* Dxdatl_ArcMid = NULL;
key_struct* Dxdatl_ArcStart = NULL;
key_struct* Dxdatl_ArcUID = NULL;
key_struct* Dxdatl_Attr2TextData = NULL;
key_struct* Dxdatl_AttrID = NULL;
key_struct* Dxdatl_AttrIDDxD = NULL;
key_struct* Dxdatl_AttrIndexDxD = NULL;
key_struct* Dxdatl_AttrIndexDxDNet = NULL;
key_struct* Dxdatl_AttrIndexNdx = NULL;
key_struct* Dxdatl_AttrSegmentNum = NULL;
key_struct* Dxdatl_AttrVisibility = NULL;
key_struct* Dxdatl_Block2Grp = NULL;
key_struct* Dxdatl_BlockBBox = NULL;
key_struct* Dxdatl_BlockGUID = NULL;
key_struct* Dxdatl_BlockSize = NULL;
key_struct* Dxdatl_Bus2GUID = NULL;
key_struct* Dxdatl_Bus2GrpLabels = NULL;
key_struct* Dxdatl_Bus2GrpSegments = NULL;
key_struct* Dxdatl_Bus2GrpStyles = NULL;
key_struct* Dxdatl_Bus2Grps = NULL;
key_struct* Dxdatl_BusExcluded = NULL;
key_struct* Dxdatl_BusGUID = NULL;
key_struct* Dxdatl_BusID = NULL;
key_struct* Dxdatl_BusLabels = NULL;
key_struct* Dxdatl_Circle2Style = NULL;
key_struct* Dxdatl_CircleGUID = NULL;
key_struct* Dxdatl_CircleIDDxD = NULL;
key_struct* Dxdatl_CirclePos = NULL;
key_struct* Dxdatl_CircleRadius = NULL;
key_struct* Dxdatl_CircleUID = NULL;
key_struct* Dxdatl_Cmp2GUID = NULL;
key_struct* Dxdatl_Cmp2Grps = NULL;
key_struct* Dxdatl_Cmp2Label = NULL;
key_struct* Dxdatl_CmpArcs = NULL;
key_struct* Dxdatl_CmpCircles = NULL;
key_struct* Dxdatl_CmpComplexShapes = NULL;
key_struct* Dxdatl_CmpDblScale = NULL;
key_struct* Dxdatl_CmpGUID = NULL;
key_struct* Dxdatl_CmpID = NULL;
key_struct* Dxdatl_CmpLines = NULL;
key_struct* Dxdatl_CmpOrient = NULL;
key_struct* Dxdatl_CmpPos = NULL;
key_struct* Dxdatl_CmpRects = NULL;
key_struct* Dxdatl_CmpScale = NULL;
key_struct* Dxdatl_CmpTexts = NULL;
key_struct* Dxdatl_Color = NULL;
key_struct* Dxdatl_ColorExt = NULL;
key_struct* Dxdatl_CompBBox = NULL;
key_struct* Dxdatl_ComplexShape2Style = NULL;
key_struct* Dxdatl_ComplexShapeEdges = NULL;
key_struct* Dxdatl_ComplexShapeGUID = NULL;
key_struct* Dxdatl_ComplexShapeIDDxD = NULL;
key_struct* Dxdatl_ComplexShapePoints = NULL;
key_struct* Dxdatl_ComplexShapeUID = NULL;
key_struct* Dxdatl_CompoundOrient = NULL;
key_struct* Dxdatl_CompoundOrigin = NULL;
key_struct* Dxdatl_CompoundPos = NULL;
key_struct* Dxdatl_DrawingOrder = NULL;
key_struct* Dxdatl_DrawingOrder2Grps = NULL;
key_struct* Dxdatl_DxDNet2DxDNets = NULL;
key_struct* Dxdatl_DxDNet2Signals = NULL;
key_struct* Dxdatl_DxDNetRefID = NULL;
key_struct* Dxdatl_Fill = NULL;
key_struct* Dxdatl_Fixes = NULL;
key_struct* Dxdatl_Grp2GUID = NULL;
key_struct* Dxdatl_Grp2Grps = NULL;
key_struct* Dxdatl_Grp2Label = NULL;
key_struct* Dxdatl_Grp2Styles = NULL;
key_struct* Dxdatl_GrpBox = NULL;
key_struct* Dxdatl_GrpGUID = NULL;
key_struct* Dxdatl_GrpID = NULL;
key_struct* Dxdatl_GrpLocked = NULL;
key_struct* Dxdatl_Joint2UID = NULL;
key_struct* Dxdatl_JointGUID = NULL;
key_struct* Dxdatl_JointID = NULL;
key_struct* Dxdatl_JointPos = NULL;
key_struct* Dxdatl_Label2TextData = NULL;
key_struct* Dxdatl_LabelIDDxD = NULL;
key_struct* Dxdatl_LabelIndexDxDNet = NULL;
key_struct* Dxdatl_LabelInverted = NULL;
key_struct* Dxdatl_LabelScope = NULL;
key_struct* Dxdatl_LabelSegmentNum = NULL;
key_struct* Dxdatl_LabelVisibility = NULL;
key_struct* Dxdatl_Line2Style = NULL;
key_struct* Dxdatl_LineArrowSize = NULL;
key_struct* Dxdatl_LineEndingCap = NULL;
key_struct* Dxdatl_LineGUID = NULL;
key_struct* Dxdatl_LineIDDxD = NULL;
key_struct* Dxdatl_LinePointX = NULL;
key_struct* Dxdatl_LinePointY = NULL;
key_struct* Dxdatl_LineStartingCap = NULL;
key_struct* Dxdatl_LineUID = NULL;
key_struct* Dxdatl_Net2GrpLabels = NULL;
key_struct* Dxdatl_Net2GrpSegments = NULL;
key_struct* Dxdatl_Net2GrpStyles = NULL;
key_struct* Dxdatl_Net2Grps = NULL;
key_struct* Dxdatl_Net2NetDxD = NULL;
key_struct* Dxdatl_NetDxD2Net = NULL;
key_struct* Dxdatl_NetDxDUID = NULL;
key_struct* Dxdatl_NetExcluded = NULL;
key_struct* Dxdatl_NetGUID = NULL;
key_struct* Dxdatl_NetID = NULL;
key_struct* Dxdatl_NetLabel = NULL;
key_struct* Dxdatl_NetsDxD = NULL;
key_struct* Dxdatl_Pin2Grps = NULL;
key_struct* Dxdatl_Pin2Label = NULL;
key_struct* Dxdatl_Pin2Style = NULL;
key_struct* Dxdatl_PinEnd = NULL;
key_struct* Dxdatl_PinGUID = NULL;
key_struct* Dxdatl_PinID = NULL;
key_struct* Dxdatl_PinInverted = NULL;
key_struct* Dxdatl_PinSide = NULL;
key_struct* Dxdatl_PinStart = NULL;
key_struct* Dxdatl_Rect2Style = NULL;
key_struct* Dxdatl_RectCoord = NULL;
key_struct* Dxdatl_RectGUID = NULL;
key_struct* Dxdatl_RectIDDxD = NULL;
key_struct* Dxdatl_RectUID = NULL;
key_struct* Dxdatl_RipperDxDUID = NULL;
key_struct* Dxdatl_Segment2Joints = NULL;
key_struct* Dxdatl_SegmentGUID = NULL;
key_struct* Dxdatl_SegmentID = NULL;
key_struct* Dxdatl_Segments = NULL;
key_struct* Dxdatl_SignalName = NULL;
key_struct* Dxdatl_Style = NULL;
key_struct* Dxdatl_Text2TextData = NULL;
key_struct* Dxdatl_TextColor = NULL;
key_struct* Dxdatl_TextColorExt = NULL;
key_struct* Dxdatl_TextCompounds = NULL;
key_struct* Dxdatl_TextFont = NULL;
key_struct* Dxdatl_TextGUID = NULL;
key_struct* Dxdatl_TextIDDxD = NULL;
key_struct* Dxdatl_TextOrient = NULL;
key_struct* Dxdatl_TextOrigin = NULL;
key_struct* Dxdatl_TextPos = NULL;
key_struct* Dxdatl_TextSize = NULL;
key_struct* Dxdatl_TextString = NULL;
key_struct* Dxdatl_TextUID = NULL;
key_struct* Dxdatl_Thickness = NULL;
key_struct* Dxdatl_TxtGUID = NULL;
key_struct* Dxdatl_TxtIDDxD = NULL;
key_struct* Dxdatl_Version = NULL;

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	ProcessKeyDxdatl()
*
* - description: 	Check for dxdatl keys
*
* - parameter: 		file pointer; Key to check; length of key
*
* - return value: 	-
******************************************************************
*/
void ProcessKeyDxdatl(FILE * sourceFile, char* Key, unsigned int KeyLen)
{
	if (strcmp(Key, "Arc2Style") == 0)
	{
		Dxdatl_Arc2Style = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "ArcEnd") == 0)
	{
		Dxdatl_ArcEnd = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "ArcGUID") == 0)
	{
		Dxdatl_ArcGUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "ArcIDDxD") == 0)
	{
		Dxdatl_ArcIDDxD = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "ArcMid") == 0)
	{
		Dxdatl_ArcMid = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "ArcStart") == 0)
	{
		Dxdatl_ArcStart = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "ArcUID") == 0)
	{
		Dxdatl_ArcUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Attr2TextData") == 0)
	{
		Dxdatl_Attr2TextData = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "AttrID") == 0)
	{
		Dxdatl_AttrID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "AttrIDDxD") == 0)
	{
		Dxdatl_AttrIDDxD = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "AttrIndexDxD") == 0)
	{
		Dxdatl_AttrIndexDxD = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "AttrIndexDxDNet") == 0)
	{
		Dxdatl_AttrIndexDxDNet = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "AttrIndexNdx") == 0)
	{
		Dxdatl_AttrIndexNdx = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "AttrSegmentNum") == 0)
	{
		Dxdatl_AttrSegmentNum = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "AttrVisibility") == 0)
	{
		Dxdatl_AttrVisibility = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Block2Grp") == 0)
	{
		Dxdatl_Block2Grp = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "BlockBBox") == 0)
	{
		Dxdatl_BlockBBox = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "BlockGUID") == 0)
	{
		Dxdatl_BlockGUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "BlockSize") == 0)
	{
		Dxdatl_BlockSize = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Bus2GUID") == 0)
	{
		Dxdatl_Bus2GUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Bus2GrpLabels") == 0)
	{
		Dxdatl_Bus2GrpLabels = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Bus2GrpSegments") == 0)
	{
		Dxdatl_Bus2GrpSegments = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Bus2GrpStyles") == 0)
	{
		Dxdatl_Bus2GrpStyles = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Bus2Grps") == 0)
	{
		Dxdatl_Bus2Grps = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "BusExcluded") == 0)
	{
		Dxdatl_BusExcluded = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "BusGUID") == 0)
	{
		Dxdatl_BusGUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "BusID") == 0)
	{
		Dxdatl_BusID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "BusLabels") == 0)
	{
		Dxdatl_BusLabels = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Circle2Style") == 0)
	{
		Dxdatl_Circle2Style = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CircleGUID") == 0)
	{
		Dxdatl_CircleGUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CircleIDDxD") == 0)
	{
		Dxdatl_CircleIDDxD = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CirclePos") == 0)
	{
		Dxdatl_CirclePos = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CircleRadius") == 0)
	{
		Dxdatl_CircleRadius = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CircleUID") == 0)
	{
		Dxdatl_CircleUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Cmp2GUID") == 0)
	{
		Dxdatl_Cmp2GUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Cmp2Grps") == 0)
	{
		Dxdatl_Cmp2Grps = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Cmp2Label") == 0)
	{
		Dxdatl_Cmp2Label = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CmpArcs") == 0)
	{
		Dxdatl_CmpArcs = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CmpCircles") == 0)
	{
		Dxdatl_CmpCircles = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CmpComplexShapes") == 0)
	{
		Dxdatl_CmpComplexShapes = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CmpDblScale") == 0)
	{
		Dxdatl_CmpDblScale = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CmpGUID") == 0)
	{
		Dxdatl_CmpGUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CmpID") == 0)
	{
		Dxdatl_CmpID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CmpLines") == 0)
	{
		Dxdatl_CmpLines = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CmpOrient") == 0)
	{
		Dxdatl_CmpOrient = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CmpPos") == 0)
	{
		Dxdatl_CmpPos = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CmpRects") == 0)
	{
		Dxdatl_CmpRects = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CmpScale") == 0)
	{
		Dxdatl_CmpScale = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CmpTexts") == 0)
	{
		Dxdatl_CmpTexts = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Color") == 0)
	{
		Dxdatl_Color = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "ColorExt") == 0)
	{
		Dxdatl_ColorExt = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CompBBox") == 0)
	{
		Dxdatl_CompBBox = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "ComplexShape2Style") == 0)
	{
		Dxdatl_ComplexShape2Style = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "ComplexShapeEdges") == 0)
	{
		Dxdatl_ComplexShapeEdges = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "ComplexShapeGUID") == 0)
	{
		Dxdatl_ComplexShapeGUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "ComplexShapeIDDxD") == 0)
	{
		Dxdatl_ComplexShapeIDDxD = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "ComplexShapePoints") == 0)
	{
		Dxdatl_ComplexShapePoints = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "ComplexShapeUID") == 0)
	{
		Dxdatl_ComplexShapeUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CompoundOrient") == 0)
	{
		Dxdatl_CompoundOrient = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CompoundOrigin") == 0)
	{
		Dxdatl_CompoundOrigin = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CompoundPos") == 0)
	{
		Dxdatl_CompoundPos = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "DrawingOrder") == 0)
	{
		Dxdatl_DrawingOrder = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "DrawingOrder2Grps") == 0)
	{
		Dxdatl_DrawingOrder2Grps = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "DxDNet2DxDNets") == 0)
	{
		Dxdatl_DxDNet2DxDNets = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "DxDNet2Signals") == 0)
	{
		Dxdatl_DxDNet2Signals = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "DxDNetRefID") == 0)
	{
		Dxdatl_DxDNetRefID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Fill") == 0)
	{
		Dxdatl_Fill = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Fixes") == 0)
	{
		Dxdatl_Fixes = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Grp2GUID") == 0)
	{
		Dxdatl_Grp2GUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Grp2Grps") == 0)
	{
		Dxdatl_Grp2Grps = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Grp2Label") == 0)
	{
		Dxdatl_Grp2Label = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Grp2Styles") == 0)
	{
		Dxdatl_Grp2Styles = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "GrpBox") == 0)
	{
		Dxdatl_GrpBox = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "GrpGUID") == 0)
	{
		Dxdatl_GrpGUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "GrpID") == 0)
	{
		Dxdatl_GrpID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "GrpLocked") == 0)
	{
		Dxdatl_GrpLocked = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Joint2UID") == 0)
	{
		Dxdatl_Joint2UID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "JointGUID") == 0)
	{
		Dxdatl_JointGUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "JointID") == 0)
	{
		Dxdatl_JointID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "JointPos") == 0)
	{
		Dxdatl_JointPos = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Label2TextData") == 0)
	{
		Dxdatl_Label2TextData = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "LabelIDDxD") == 0)
	{
		Dxdatl_LabelIDDxD = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "LabelIndexDxDNet") == 0)
	{
		Dxdatl_LabelIndexDxDNet = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "LabelInverted") == 0)
	{
		Dxdatl_LabelInverted = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "LabelScope") == 0)
	{
		Dxdatl_LabelScope = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "LabelSegmentNum") == 0)
	{
		Dxdatl_LabelSegmentNum = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "LabelVisibility") == 0)
	{
		Dxdatl_LabelVisibility = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Line2Style") == 0)
	{
		Dxdatl_Line2Style = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "LineArrowSize") == 0)
	{
		Dxdatl_LineArrowSize = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "LineEndingCap") == 0)
	{
		Dxdatl_LineEndingCap = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "LineGUID") == 0)
	{
		Dxdatl_LineGUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "LineIDDxD") == 0)
	{
		Dxdatl_LineIDDxD = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "LinePointX") == 0)
	{
		Dxdatl_LinePointX = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "LinePointY") == 0)
	{
		Dxdatl_LinePointY = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "LineStartingCap") == 0)
	{
		Dxdatl_LineStartingCap = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "LineUID") == 0)
	{
		Dxdatl_LineUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Net2GrpLabels") == 0)
	{
		Dxdatl_Net2GrpLabels = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Net2GrpSegments") == 0)
	{
		Dxdatl_Net2GrpSegments = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Net2GrpStyles") == 0)
	{
		Dxdatl_Net2GrpStyles = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Net2Grps") == 0)
	{
		Dxdatl_Net2Grps = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Net2NetDxD") == 0)
	{
		Dxdatl_Net2NetDxD = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "NetDxD2Net") == 0)
	{
		Dxdatl_NetDxD2Net = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "NetDxDUID") == 0)
	{
		Dxdatl_NetDxDUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "NetExcluded") == 0)
	{
		Dxdatl_NetExcluded = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "NetGUID") == 0)
	{
		Dxdatl_NetGUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "NetID") == 0)
	{
		Dxdatl_NetID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "NetLabel") == 0)
	{
		Dxdatl_NetLabel = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "NetsDxD") == 0)
	{
		Dxdatl_NetsDxD = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Pin2Grps") == 0)
	{
		Dxdatl_Pin2Grps = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Pin2Label") == 0)
	{
		Dxdatl_Pin2Label = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Pin2Style") == 0)
	{
		Dxdatl_Pin2Style = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PinEnd") == 0)
	{
		Dxdatl_PinEnd = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PinGUID") == 0)
	{
		Dxdatl_PinGUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PinID") == 0)
	{
		Dxdatl_PinID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PinInverted") == 0)
	{
		Dxdatl_PinInverted = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PinSide") == 0)
	{
		Dxdatl_PinSide = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PinStart") == 0)
	{
		Dxdatl_PinStart = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Rect2Style") == 0)
	{
		Dxdatl_Rect2Style = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "RectCoord") == 0)
	{
		Dxdatl_RectCoord = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "RectGUID") == 0)
	{
		Dxdatl_RectGUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "RectIDDxD") == 0)
	{
		Dxdatl_RectIDDxD = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "RectUID") == 0)
	{
		Dxdatl_RectUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "RipperDxDUID") == 0)
	{
		Dxdatl_RipperDxDUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Segment2Joints") == 0)
	{
		Dxdatl_Segment2Joints = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SegmentGUID") == 0)
	{
		Dxdatl_SegmentGUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SegmentID") == 0)
	{
		Dxdatl_SegmentID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Segments") == 0)
	{
		Dxdatl_Segments = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SignalName") == 0)
	{
		Dxdatl_SignalName = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Style") == 0)
	{
		Dxdatl_Style = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Text2TextData") == 0)
	{
		Dxdatl_Text2TextData = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "TextColor") == 0)
	{
		Dxdatl_TextColor = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "TextColorExt") == 0)
	{
		Dxdatl_TextColorExt = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "TextCompounds") == 0)
	{
		Dxdatl_TextCompounds = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "TextFont") == 0)
	{
		Dxdatl_TextFont = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "TextGUID") == 0)
	{
		Dxdatl_TextGUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "TextIDDxD") == 0)
	{
		Dxdatl_TextIDDxD = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "TextOrient") == 0)
	{
		Dxdatl_TextOrient = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "TextOrigin") == 0)
	{
		Dxdatl_TextOrigin = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "TextPos") == 0)
	{
		Dxdatl_TextPos = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "TextSize") == 0)
	{
		Dxdatl_TextSize = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "TextString") == 0)
	{
		Dxdatl_TextString = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "TextUID") == 0)
	{
		Dxdatl_TextUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Thickness") == 0)
	{
		Dxdatl_Thickness = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "TxtGUID") == 0)
	{
		Dxdatl_TxtGUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "TxtIDDxD") == 0)
	{
		Dxdatl_TxtIDDxD = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Version") == 0)
	{
		Dxdatl_Version = ParseKey(sourceFile);
	}
	else 
	{
		myPrint("Unknown Key in dxdatl [%s]\n", Key);
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
	InitKey(&Dxdatl_Arc2Style);
	InitKey(&Dxdatl_ArcEnd);
	InitKey(&Dxdatl_ArcGUID);
	InitKey(&Dxdatl_ArcIDDxD);
	InitKey(&Dxdatl_ArcMid);
	InitKey(&Dxdatl_ArcStart);
	InitKey(&Dxdatl_ArcUID);
	InitKey(&Dxdatl_Attr2TextData);
	InitKey(&Dxdatl_AttrID);
	InitKey(&Dxdatl_AttrIDDxD);
	InitKey(&Dxdatl_AttrIndexDxD);
	InitKey(&Dxdatl_AttrIndexDxDNet);
	InitKey(&Dxdatl_AttrIndexNdx);
	InitKey(&Dxdatl_AttrSegmentNum);
	InitKey(&Dxdatl_AttrVisibility);
	InitKey(&Dxdatl_Block2Grp);
	InitKey(&Dxdatl_BlockBBox);
	InitKey(&Dxdatl_BlockGUID);
	InitKey(&Dxdatl_BlockSize);
	InitKey(&Dxdatl_Bus2GUID);
	InitKey(&Dxdatl_Bus2GrpLabels);
	InitKey(&Dxdatl_Bus2GrpSegments);
	InitKey(&Dxdatl_Bus2GrpStyles);
	InitKey(&Dxdatl_Bus2Grps);
	InitKey(&Dxdatl_BusExcluded);
	InitKey(&Dxdatl_BusGUID);
	InitKey(&Dxdatl_BusID);
	InitKey(&Dxdatl_BusLabels);
	InitKey(&Dxdatl_Circle2Style);
	InitKey(&Dxdatl_CircleGUID);
	InitKey(&Dxdatl_CircleIDDxD);
	InitKey(&Dxdatl_CirclePos);
	InitKey(&Dxdatl_CircleRadius);
	InitKey(&Dxdatl_CircleUID);
	InitKey(&Dxdatl_Cmp2GUID);
	InitKey(&Dxdatl_Cmp2Grps);
	InitKey(&Dxdatl_Cmp2Label);
	InitKey(&Dxdatl_CmpArcs);
	InitKey(&Dxdatl_CmpCircles);
	InitKey(&Dxdatl_CmpComplexShapes);
	InitKey(&Dxdatl_CmpDblScale);
	InitKey(&Dxdatl_CmpGUID);
	InitKey(&Dxdatl_CmpID);
	InitKey(&Dxdatl_CmpLines);
	InitKey(&Dxdatl_CmpOrient);
	InitKey(&Dxdatl_CmpPos);
	InitKey(&Dxdatl_CmpRects);
	InitKey(&Dxdatl_CmpScale);
	InitKey(&Dxdatl_CmpTexts);
	InitKey(&Dxdatl_Color);
	InitKey(&Dxdatl_ColorExt);
	InitKey(&Dxdatl_CompBBox);
	InitKey(&Dxdatl_ComplexShape2Style);
	InitKey(&Dxdatl_ComplexShapeEdges);
	InitKey(&Dxdatl_ComplexShapeGUID);
	InitKey(&Dxdatl_ComplexShapeIDDxD);
	InitKey(&Dxdatl_ComplexShapePoints);
	InitKey(&Dxdatl_ComplexShapeUID);
	InitKey(&Dxdatl_CompoundOrient);
	InitKey(&Dxdatl_CompoundOrigin);
	InitKey(&Dxdatl_CompoundPos);
	InitKey(&Dxdatl_DrawingOrder);
	InitKey(&Dxdatl_DrawingOrder2Grps);
	InitKey(&Dxdatl_DxDNet2DxDNets);
	InitKey(&Dxdatl_DxDNet2Signals);
	InitKey(&Dxdatl_DxDNetRefID);
	InitKey(&Dxdatl_Fill);
	InitKey(&Dxdatl_Fixes);
	InitKey(&Dxdatl_Grp2GUID);
	InitKey(&Dxdatl_Grp2Grps);
	InitKey(&Dxdatl_Grp2Label);
	InitKey(&Dxdatl_Grp2Styles);
	InitKey(&Dxdatl_GrpBox);
	InitKey(&Dxdatl_GrpGUID);
	InitKey(&Dxdatl_GrpID);
	InitKey(&Dxdatl_GrpLocked);
	InitKey(&Dxdatl_Joint2UID);
	InitKey(&Dxdatl_JointGUID);
	InitKey(&Dxdatl_JointID);
	InitKey(&Dxdatl_JointPos);
	InitKey(&Dxdatl_Label2TextData);
	InitKey(&Dxdatl_LabelIDDxD);
	InitKey(&Dxdatl_LabelIndexDxDNet);
	InitKey(&Dxdatl_LabelInverted);
	InitKey(&Dxdatl_LabelScope);
	InitKey(&Dxdatl_LabelSegmentNum);
	InitKey(&Dxdatl_LabelVisibility);
	InitKey(&Dxdatl_Line2Style);
	InitKey(&Dxdatl_LineArrowSize);
	InitKey(&Dxdatl_LineEndingCap);
	InitKey(&Dxdatl_LineGUID);
	InitKey(&Dxdatl_LineIDDxD);
	InitKey(&Dxdatl_LinePointX);
	InitKey(&Dxdatl_LinePointY);
	InitKey(&Dxdatl_LineStartingCap);
	InitKey(&Dxdatl_LineUID);
	InitKey(&Dxdatl_Net2GrpLabels);
	InitKey(&Dxdatl_Net2GrpSegments);
	InitKey(&Dxdatl_Net2GrpStyles);
	InitKey(&Dxdatl_Net2Grps);
	InitKey(&Dxdatl_Net2NetDxD);
	InitKey(&Dxdatl_NetDxD2Net);
	InitKey(&Dxdatl_NetDxDUID);
	InitKey(&Dxdatl_NetExcluded);
	InitKey(&Dxdatl_NetGUID);
	InitKey(&Dxdatl_NetID);
	InitKey(&Dxdatl_NetLabel);
	InitKey(&Dxdatl_NetsDxD);
	InitKey(&Dxdatl_Pin2Grps);
	InitKey(&Dxdatl_Pin2Label);
	InitKey(&Dxdatl_Pin2Style);
	InitKey(&Dxdatl_PinEnd);
	InitKey(&Dxdatl_PinGUID);
	InitKey(&Dxdatl_PinID);
	InitKey(&Dxdatl_PinInverted);
	InitKey(&Dxdatl_PinSide);
	InitKey(&Dxdatl_PinStart);
	InitKey(&Dxdatl_Rect2Style);
	InitKey(&Dxdatl_RectCoord);
	InitKey(&Dxdatl_RectGUID);
	InitKey(&Dxdatl_RectIDDxD);
	InitKey(&Dxdatl_RectUID);
	InitKey(&Dxdatl_RipperDxDUID);
	InitKey(&Dxdatl_Segment2Joints);
	InitKey(&Dxdatl_SegmentGUID);
	InitKey(&Dxdatl_SegmentID);
	InitKey(&Dxdatl_Segments);
	InitKey(&Dxdatl_SignalName);
	InitKey(&Dxdatl_Style);
	InitKey(&Dxdatl_Text2TextData);
	InitKey(&Dxdatl_TextColor);
	InitKey(&Dxdatl_TextColorExt);
	InitKey(&Dxdatl_TextCompounds);
	InitKey(&Dxdatl_TextFont);
	InitKey(&Dxdatl_TextGUID);
	InitKey(&Dxdatl_TextIDDxD);
	InitKey(&Dxdatl_TextOrient);
	InitKey(&Dxdatl_TextOrigin);
	InitKey(&Dxdatl_TextPos);
	InitKey(&Dxdatl_TextSize);
	InitKey(&Dxdatl_TextString);
	InitKey(&Dxdatl_TextUID);
	InitKey(&Dxdatl_Thickness);
	InitKey(&Dxdatl_TxtGUID);
	InitKey(&Dxdatl_TxtIDDxD);
	InitKey(&Dxdatl_Version);
}