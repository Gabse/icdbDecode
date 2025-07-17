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
#ifndef _DXDATL_H
#define _DXDATL_H

/*
******************************************************************
* Global Includes
******************************************************************
*/
#include <stdio.h>		// Required for FILE
#include "../common.h"	// Required for key_struct

/*
******************************************************************
* Global Defines
******************************************************************
*/
#define PATH_DXDATL "dxdatl.v"

/*
******************************************************************
* Global Variables
******************************************************************
*/
// Dxdatl
extern key_struct* Dxdatl_Arc2Style;
extern key_struct* Dxdatl_ArcEnd;
extern key_struct* Dxdatl_ArcGUID;
extern key_struct* Dxdatl_ArcIDDxD;
extern key_struct* Dxdatl_ArcMid;
extern key_struct* Dxdatl_ArcStart;
extern key_struct* Dxdatl_ArcUID;
extern key_struct* Dxdatl_Attr2TextData;
extern key_struct* Dxdatl_AttrID;
extern key_struct* Dxdatl_AttrIDDxD;
extern key_struct* Dxdatl_AttrIndexDxD;
extern key_struct* Dxdatl_AttrIndexDxDNet;
extern key_struct* Dxdatl_AttrIndexNdx;
extern key_struct* Dxdatl_AttrSegmentNum;
extern key_struct* Dxdatl_AttrVisibility;
extern key_struct* Dxdatl_Block2Grp;
extern key_struct* Dxdatl_BlockBBox;
extern key_struct* Dxdatl_BlockGUID;
extern key_struct* Dxdatl_BlockSize;
extern key_struct* Dxdatl_Bus2GUID;
extern key_struct* Dxdatl_Bus2GrpLabels;
extern key_struct* Dxdatl_Bus2GrpSegments;
extern key_struct* Dxdatl_Bus2GrpStyles;
extern key_struct* Dxdatl_Bus2Grps;
extern key_struct* Dxdatl_BusExcluded;
extern key_struct* Dxdatl_BusGUID;
extern key_struct* Dxdatl_BusID;
extern key_struct* Dxdatl_BusLabels;
extern key_struct* Dxdatl_Circle2Style;
extern key_struct* Dxdatl_CircleGUID;
extern key_struct* Dxdatl_CircleIDDxD;
extern key_struct* Dxdatl_CirclePos;
extern key_struct* Dxdatl_CircleRadius;
extern key_struct* Dxdatl_CircleUID;
extern key_struct* Dxdatl_Cmp2GUID;
extern key_struct* Dxdatl_Cmp2Grps;
extern key_struct* Dxdatl_Cmp2Label;
extern key_struct* Dxdatl_CmpDblScale;
extern key_struct* Dxdatl_CmpGUID;
extern key_struct* Dxdatl_CmpID;
extern key_struct* Dxdatl_CmpOrient;
extern key_struct* Dxdatl_CmpPos;
extern key_struct* Dxdatl_CmpScale;
extern key_struct* Dxdatl_Color;
extern key_struct* Dxdatl_ColorExt;
extern key_struct* Dxdatl_ComplexShape2Style;
extern key_struct* Dxdatl_ComplexShapeEdges;
extern key_struct* Dxdatl_ComplexShapeGUID;
extern key_struct* Dxdatl_ComplexShapeIDDxD;
extern key_struct* Dxdatl_ComplexShapePoints;
extern key_struct* Dxdatl_ComplexShapeUID;
extern key_struct* Dxdatl_CompoundOrient;
extern key_struct* Dxdatl_CompoundOrigin;
extern key_struct* Dxdatl_CompoundPos;
extern key_struct* Dxdatl_DrawingOrder;
extern key_struct* Dxdatl_DrawingOrder2Grps;
extern key_struct* Dxdatl_DxDNet2DxDNets;
extern key_struct* Dxdatl_DxDNet2Signals;
extern key_struct* Dxdatl_DxDNetRefID;
extern key_struct* Dxdatl_Fill;
extern key_struct* Dxdatl_Fixes;
extern key_struct* Dxdatl_Grp2GUID;
extern key_struct* Dxdatl_Grp2Grps;
extern key_struct* Dxdatl_Grp2Label;
extern key_struct* Dxdatl_Grp2Styles;
extern key_struct* Dxdatl_GrpBox;
extern key_struct* Dxdatl_GrpGUID;
extern key_struct* Dxdatl_GrpID;
extern key_struct* Dxdatl_GrpLocked;
extern key_struct* Dxdatl_Joint2UID;
extern key_struct* Dxdatl_JointGUID;
extern key_struct* Dxdatl_JointID;
extern key_struct* Dxdatl_JointPos;
extern key_struct* Dxdatl_Label2TextData;
extern key_struct* Dxdatl_LabelIDDxD;
extern key_struct* Dxdatl_LabelIndexDxDNet;
extern key_struct* Dxdatl_LabelInverted;
extern key_struct* Dxdatl_LabelScope;
extern key_struct* Dxdatl_LabelSegmentNum;
extern key_struct* Dxdatl_LabelVisibility;
extern key_struct* Dxdatl_Line2Style;
extern key_struct* Dxdatl_LineArrowSize;
extern key_struct* Dxdatl_LineEndingCap;
extern key_struct* Dxdatl_LineGUID;
extern key_struct* Dxdatl_LineIDDxD;
extern key_struct* Dxdatl_LinePointX;
extern key_struct* Dxdatl_LinePointY;
extern key_struct* Dxdatl_LineStartingCap;
extern key_struct* Dxdatl_LineUID;
extern key_struct* Dxdatl_Net2GrpLabels;
extern key_struct* Dxdatl_Net2GrpSegments;
extern key_struct* Dxdatl_Net2GrpStyles;
extern key_struct* Dxdatl_Net2Grps;
extern key_struct* Dxdatl_Net2NetDxD;
extern key_struct* Dxdatl_NetDxD2Net;
extern key_struct* Dxdatl_NetDxDUID;
extern key_struct* Dxdatl_NetExcluded;
extern key_struct* Dxdatl_NetGUID;
extern key_struct* Dxdatl_NetID;
extern key_struct* Dxdatl_NetLabel;
extern key_struct* Dxdatl_NetsDxD;
extern key_struct* Dxdatl_Pin2Grps;
extern key_struct* Dxdatl_Rect2Style;
extern key_struct* Dxdatl_RectCoord;
extern key_struct* Dxdatl_RectGUID;
extern key_struct* Dxdatl_RectIDDxD;
extern key_struct* Dxdatl_RectUID;
extern key_struct* Dxdatl_RipperDxDUID;
extern key_struct* Dxdatl_Segment2Joints;
extern key_struct* Dxdatl_SegmentGUID;
extern key_struct* Dxdatl_SegmentID;
extern key_struct* Dxdatl_Segments;
extern key_struct* Dxdatl_SignalName;
extern key_struct* Dxdatl_Style;
extern key_struct* Dxdatl_Text2TextData;
extern key_struct* Dxdatl_TextColor;
extern key_struct* Dxdatl_TextColorExt;
extern key_struct* Dxdatl_TextCompounds;
extern key_struct* Dxdatl_TextFont;
extern key_struct* Dxdatl_TextGUID;
extern key_struct* Dxdatl_TextIDDxD;
extern key_struct* Dxdatl_TextOrient;
extern key_struct* Dxdatl_TextOrigin;
extern key_struct* Dxdatl_TextPos;
extern key_struct* Dxdatl_TextSize;
extern key_struct* Dxdatl_TextString;
extern key_struct* Dxdatl_TextUID;
extern key_struct* Dxdatl_Thickness;
extern key_struct* Dxdatl_TxtGUID;
extern key_struct* Dxdatl_TxtIDDxD;
extern key_struct* Dxdatl_Version;

/*
******************************************************************
* Global Functions
******************************************************************
*/
extern void ProcessKeyDxdatl(FILE*, char*, unsigned int);
extern void InitDxdatl(void);

#endif //_DXDATL_H