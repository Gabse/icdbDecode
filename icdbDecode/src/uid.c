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
const char* text_uid_type[] = {
	[0x00] = "unknown",
	[0x01] = "unknown",
	[0x02] = "unknown",
	[0x03] = "CnfgUID",
	[0x04] = "PathUID",
	[0x05] = "BlkUID",
	[0x06] = "CmpUID",
	[0x07] = "BSymUID",
	[0x08] = "ISymUID",
	[0x09] = "PinUID",
	[0x0A] = "BPinUID",
	[0x0B] = "IPinUID",
	[0x0C] = "INetUID",
	[0x0D] = "FNetUID",
	[0x0E] = "PNetUID",
	[0x0F] = "XtrUID",
	[0x10] = "BusUID",
	[0x11] = "unknown",
	[0x12] = "unknown",
	[0x13] = "unknown",
	[0x14] = "unknown",
	[0x15] = "DesignUID",
	[0x16] = "ConstUID",
	[0x17] = "LayerUID",
	[0x18] = "NetClassUID",
	[0x19] = "ClearRuleUID",
	[0x1A] = "ConstraintClassUID",
	[0x1B] = "ViaSpanUID",
	[0x1C] = "GenClearClassUID",
	[0x1D] = "PartTechUID",
	[0x1E] = "unknown",
	[0x1F] = "C2CClearUID",
	[0x20] = "unknown",
	[0x21] = "unknown",
	[0x22] = "unknown",
	[0x23] = "unknown",
	[0x24] = "unknown",
	[0x25] = "SchemeUID",
	[0x26] = "SchemeNetClassRuleUID",
	[0x27] = "ViaAssignUID",
	[0x28] = "LayerRuleUIDUID",
	[0x29] = "SchemaClearRuleUID",
	[0x2A] = "LayerClearRuleUID",
	[0x2B] = "GenObjTypeUID",
	[0x2C] = "GenCompObjUID",
	[0x2D] = "unknown",
	[0x2E] = "DiffPairUID",
	[0x2F] = "ElectricalNetUID",
	[0x30] = "PinPairUID",
	[0x31] = "PhysicalNetUID",
	[0x32] = "FromToUID",
	[0x33] = "ComponentUID",
	[0x34] = "CesPinUID",
	[0x35] = "PartUID",
	[0x36] = "PartPinUID",
	[0x37] = "NetDxdUID",
	[0x38] = "LineUID",
	[0x39] = "CircleUID",
	[0x3A] = "ArcUID",
	[0x3B] = "RectangleUID",
	[0x3C] = "TextUID",
	[0x3D] = "unknown",
	[0x3E] = "ViaUID",
	[0x3F] = "CESconfigUID",
	[0x40] = "unknown",
	[0x41] = "unknown",
	[0x42] = "unknown",
	[0x43] = "unknown",
	[0x44] = "unknown",
	[0x45] = "GroupUID",
	[0x46] = "unknown",
	[0x47] = "unknown",
	[0x48] = "unknown",
	[0x49] = "unknown",
	
	
	"unknown", // Last element
};

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
	if(type >= UIDSize)
	{
		type = UIDSize;
	}
	return text_uid_type[type];
}