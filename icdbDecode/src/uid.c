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
#include "uid.h"
#include <stdio.h>		// Required for fprint, fopen, ...
#include <stdlib.h>		// Required for calloc to work properly
#include <stdint.h>		// Required for int32_t, uint32_t, ...
#include <string.h>		// Required for strcmp
#include "stringutil.h"	// Required for assemblePath

/*
******************************************************************
* Defines
******************************************************************
*/

/*
******************************************************************
* Function Prototypes 
******************************************************************
*/

/*
******************************************************************
* Global Variables
******************************************************************
*/
const char text_uid_type_Cnfg[] = "CnfgUID";
const char text_uid_type_Path[] = "PathUID";
const char text_uid_type_Blk[] = "BlkUID";
const char text_uid_type_Cmp[] = "CmpUID";
const char text_uid_type_BSym[] = "BSymUID";
const char text_uid_type_ISym[] = "ISymUID";
const char text_uid_type_Pin[] = "PinUID";
const char text_uid_type_BPin[] = "BPinUID";
const char text_uid_type_IPin[] = "IPinUID";
const char text_uid_type_INet[] = "INetUID";
const char text_uid_type_FNet[] = "FNetUID";
const char text_uid_type_PNet[] = "PNetUID";
const char text_uid_type_Xtr[] = "XtrUID";
const char text_uid_type_Bus[] = "BusUID";
const char text_uid_type_Design[] = "DesignUID";
const char text_uid_type_Const[] = "ConstUID";
const char text_uid_type_Layer[] = "LayerUID";
const char text_uid_type_NetClass[] = "NetClassUID";
const char text_uid_type_ClearRule[] = "ClearRuleUID";
const char text_uid_type_ConstraintClass[] = "ConstraintClassUID";
const char text_uid_type_ViaSpan[] = "ViaSpanUID";
const char text_uid_type_GenClearClass[] = "GenClearClassUID";
const char text_uid_type_PartTech[] = "PartTechUID";
const char text_uid_type_C2CClear[] = "C2CClearUID";
const char text_uid_type_Scheme[] = "SchemeUID";
const char text_uid_type_SchemeNetClassRule[] = "SchemeNetClassRuleUID";
const char text_uid_type_ViaAssign[] = "ViaAssignUID";
const char text_uid_type_LayerRuleUID[] = "LayerRuleUIDUID";
const char text_uid_type_SchemaClearRule[] = "SchemaClearRuleUID";
const char text_uid_type_LayerClearRule[] = "LayerClearRuleUID";
const char text_uid_type_GenObjType[] = "GenObjTypeUID";
const char text_uid_type_GenCompObj[] = "GenCompObjUID";
const char text_uid_type_DiffPair[] = "DiffPairUID";
const char text_uid_type_ElectricalNet[] = "ElectricalNetUID";
const char text_uid_type_PinPair[] = "PinPairUID";
const char text_uid_type_PhysicalNet[] = "PhysicalNetUID";
const char text_uid_type_FromTo[] = "FromToUID";
const char text_uid_type_Component[] = "ComponentUID";
const char text_uid_type_CesPin[] = "CesPinUID";
const char text_uid_type_Part[] = "PartUID";
const char text_uid_type_PartPin[] = "PartPinUID";
const char text_uid_type_NetDxd[] = "NetDxdUID";
const char text_uid_type_Line[] = "LineUID";
const char text_uid_type_Circle[] = "CircleUID";
const char text_uid_type_Arc[] = "ArcUID";
const char text_uid_type_Rectangle[] = "RectangleUID";
const char text_uid_type_Text[] = "TextUID";
const char text_uid_type_Via[] = "ViaUID";
const char text_uid_type_CESconfig[] = "CESconfigUID";
const char text_uid_type_Group[] = "GroupUID";
const char text_uid_type_Unknown[] = "unknown";

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	printUidType()
*
* - description: 	returns the uid type as string
*
* - parameter: 		uid type
*
* - return value: 	string as char pointer
******************************************************************
*/
const char* printUidType(enum uid_type_enum type)
{
    switch(type)
	{
		case uid_type_Cnfg:
		{
			return &text_uid_type_Cnfg[0];
		}
		case uid_type_Path:
		{
			return &text_uid_type_Path[0];
		}
		case uid_type_Blk:
		{
			return &text_uid_type_Blk[0];
		}
		case uid_type_Cmp:
		{
			return &text_uid_type_Cmp[0];
		}
		case uid_type_BSym:
		{
			return &text_uid_type_BSym[0];
		}
		case uid_type_ISym:
		{
			return &text_uid_type_ISym[0];
		}
	    case uid_type_Pin:
	    {
	    	return &text_uid_type_Pin[0];
		}
		case uid_type_BPin:
		{
			return &text_uid_type_BPin[0];
		}
		case uid_type_IPin:
		{
			return &text_uid_type_IPin[0];
		}
		case uid_type_INet:
		{
			return &text_uid_type_INet[0];
		}
		case uid_type_FNet:
		{
			return &text_uid_type_FNet[0];
		}
		case uid_type_PNet:
		{
			return &text_uid_type_PNet[0];
		}
		case uid_type_Xtr:
		{
			return &text_uid_type_Xtr[0];
		}
		case uid_type_Bus:
		{
			return &text_uid_type_Bus[0];
		}
		case uid_type_Design:
		{
			return &text_uid_type_Design[0];
		}
		case uid_type_Const:
		{
			return &text_uid_type_Const[0];
		}
		case uid_type_Layer:
		{
			return &text_uid_type_Layer[0];
		}
		case uid_type_NetClass:
		{
			return &text_uid_type_NetClass[0];
		}
		case uid_type_ClearRule:
		{
			return &text_uid_type_ClearRule[0];
		}
		case uid_type_ConstraintClass:
		{
			return &text_uid_type_ConstraintClass[0];
		}
		case uid_type_ViaSpan:
		{
			return &text_uid_type_ViaSpan[0];
		}
		case uid_type_GenClearClass:
		{
			return &text_uid_type_GenClearClass[0];
		}
		case uid_type_PartTech:
		{
			return &text_uid_type_PartTech[0];
		}
		case uid_type_C2CClear:
		{
			return &text_uid_type_C2CClear[0];
		}
		case uid_type_Scheme:
		{
			return &text_uid_type_Scheme[0];
		}
		case uid_type_SchemeNetClassRule:
		{
			return &text_uid_type_SchemeNetClassRule[0];
		}
		case uid_type_ViaAssign:
		{
			return &text_uid_type_ViaAssign[0];
		}
		case uid_type_LayerRuleUID:
		{
			return &text_uid_type_LayerRuleUID[0];
		}
		case uid_type_SchemaClearRule:
		{
			return &text_uid_type_SchemaClearRule[0];
		}
		case uid_type_LayerClearRule:
		{
			return &text_uid_type_LayerClearRule[0];
		}
		case uid_type_GenObjType:
		{
			return &text_uid_type_GenObjType[0];
		}
		case uid_type_GenCompObj:
		{
			return &text_uid_type_GenCompObj[0];
		}
		case uid_type_DiffPair:
		{
			return &text_uid_type_DiffPair[0];
		}
		case uid_type_ElectricalNet:
		{
			return &text_uid_type_ElectricalNet[0];
		}
		case uid_type_PinPair:
		{
			return &text_uid_type_PinPair[0];
		}
		case uid_type_PhysicalNet:
		{
			return &text_uid_type_PhysicalNet[0];
		}
		case uid_type_FromTo:
		{
			return &text_uid_type_FromTo[0];
		}
		case uid_type_Component:
		{
			return &text_uid_type_Component[0];
		}
		case uid_type_CesPin:
		{
			return &text_uid_type_CesPin[0];
		}
		case uid_type_Part:
		{
			return &text_uid_type_Part[0];
		}
		case uid_type_PartPin:
		{
			return &text_uid_type_PartPin[0];
		}
		case uid_type_NetDxd:
		{
			return &text_uid_type_NetDxd[0];
		}
		case uid_type_Line:
		{
			return &text_uid_type_Line[0];
		}
		case uid_type_Circle:
		{
			return &text_uid_type_Circle[0];
		}
		case uid_type_Arc:
		{
			return &text_uid_type_Arc[0];
		}
		case uid_type_Rectangle:
		{
			return &text_uid_type_Rectangle[0];
		}
		case uid_type_Text:
		{
			return &text_uid_type_Text[0];
		}
		case uid_type_Via:
		{
			return &text_uid_type_Via[0];
		}
		case uid_type_CESconfig:
		{
			return &text_uid_type_CESconfig[0];
		}
		case uid_type_Group:
		{
			return &text_uid_type_Group[0];
		}
		default:
		{
			return &text_uid_type_Unknown[0];
		}
	}
}