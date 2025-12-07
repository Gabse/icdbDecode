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
#ifndef _UID_H
#define _UID_H

/*
******************************************************************
* Global Includes
******************************************************************
*/
#include <stdint.h>		// Required for int32_t, uint32_t, ...
#include <stdio.h>		// Required for file type
#include <stdlib.h>		// Required for min/max
#include "common.h"

/*
******************************************************************
* Global Defines
******************************************************************
*/

/*
******************************************************************
* Enums
******************************************************************
*/
typedef enum uid_type_enum : uint8_t
{
//	uid_type_					= 0x00,
//	uid_type_					= 0x01,
//	uid_type_					= 0x02,
	uid_type_Cnfg	 			= 0x03,
	uid_type_Path	 			= 0x04,
	uid_type_Blk	 			= 0x05,
	uid_type_Cmp	 			= 0x06,
	uid_type_BSym		 		= 0x07,
	uid_type_ISym		 		= 0x08,
	uid_type_Pin	 			= 0x09,
	uid_type_BPin	 			= 0x0A,
	uid_type_IPin	 			= 0x0B,
	uid_type_INet	 			= 0x0C,
	uid_type_FNet	 			= 0x0D,
	uid_type_PNet	 			= 0x0E,
	uid_type_Xtr	 			= 0x0F,
	uid_type_Bus	 			= 0x10,
//	uid_type_					= 0x11,
//	uid_type_					= 0x12,
//	uid_type_					= 0x13,
//	uid_type_					= 0x14,
	uid_type_Design				= 0x15,
	uid_type_Const				= 0x16,
	uid_type_Layer				= 0x17,
	uid_type_NetClass			= 0x18,
	uid_type_ClearRule 			= 0x19,
	uid_type_ConstraintClass	= 0x1A,
	uid_type_ViaSpan			= 0x1B,
	uid_type_GenClearClass		= 0x1C,
	uid_type_PartTech			= 0x1D,
//	uid_type_					= 0x1E,
	uid_type_C2CClear			= 0x1F,
//	uid_type_					= 0x20,
//	uid_type_					= 0x21,
//	uid_type_					= 0x22,
//	uid_type_					= 0x23,
//	uid_type_					= 0x24,
	uid_type_Scheme				= 0x25,
	uid_type_SchemeNetClassRule	= 0x26,
	uid_type_ViaAssign			= 0x27,
	uid_type_LayerRuleUID		= 0x28,
	uid_type_SchemaClearRule	= 0x29,
	uid_type_LayerClearRule		= 0x2A,
	uid_type_GenObjType			= 0x2B,
	uid_type_GenCompObj			= 0x2C,
//	uid_type_					= 0x2D,
	uid_type_DiffPair			= 0x2E,
	uid_type_ElectricalNet		= 0x2F,
	uid_type_PinPair			= 0x30,
	uid_type_PhysicalNet		= 0x31,
	uid_type_FromTo 			= 0x32,
	uid_type_Component 			= 0x33,
	uid_type_CesPin	 			= 0x34,
	uid_type_Part	 			= 0x35,
	uid_type_PartPin 			= 0x36,
	uid_type_NetDxd	 			= 0x37,
	uid_type_Line	 			= 0x38,
	uid_type_Circle	 			= 0x39,
	uid_type_Arc	 			= 0x3A,
	uid_type_Rectangle 			= 0x3B,
	uid_type_Text	 			= 0x3C,
//	uid_type_					= 0x3D,
	uid_type_Via				= 0x3E,
	uid_type_CESconfig			= 0x3F,
//	uid_type_					= 0x40,
//	uid_type_					= 0x41,
//	uid_type_					= 0x42,
//	uid_type_					= 0x43,
//	uid_type_					= 0x44,
	uid_type_Group				= 0x45,
} uid_type_enum;

/*
******************************************************************
* Structures
******************************************************************
*/
typedef struct uid_split_struct
{
	uint32_t UID_Owner;
	uint16_t UID_ID;
	uint8_t UID_Padding;
	uid_type_enum UID_Type;
} uid_split_struct;

/*
******************************************************************
* Unions
******************************************************************
*/
typedef union uid_union
{
	uint8_t UID8[8];
	uid_split_struct UID_Splitt;
#ifdef B64Bit
	uint64_t UID64;
#else
	uint32_t UID32[2];
#endif
} uid_union;

/*
******************************************************************
* Global Functions
******************************************************************
*/

#endif //_UID_H