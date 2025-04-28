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
#include "catlgatl.h"
#include <stdint.h>		// Required for int32_t, uint32_t, ...
#include "common.h"		// Required for parseFile

/*
******************************************************************
* Defines
******************************************************************
*/
#define PATH_CATLGATL "catlgatl.v"

/*
******************************************************************
* Global Variables
******************************************************************
*/
int32_t NumBlkUID = -1;
uid_struct* BlkUID = NULL;

int32_t NumGroupOwnerUID = -1;
uid_struct* GroupOwnerUID = NULL;

int32_t NumMdlNam = -1;
text_struct* MdlNam = NULL;

/*
******************************************************************
* Local Functions
******************************************************************
*/
void ProcessKeyCatlgatl(FILE*, char*);
void InitCatlgatl(void);


/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	parseCatlgatl()
*
* - description: 	Catlgatl Parser code
*
* - parameter: 		source path string, string length
*
* - return value: 	error code
******************************************************************
*/
int parseCatlgatl(char* path, uint32_t pathlenth)
{
	InitCatlgatl();
	return parseFile(path, pathlenth, PATH_CATLGATL, sizeof(PATH_CATLGATL), ProcessKeyCatlgatl);
}

/*
******************************************************************
* - function name:	ProcessKeyCatlgatl()
*
* - description: 	Check for catlgatl keys
*
* - parameter: 		file pointer; Key to check
*
* - return value: 	-
******************************************************************
*/
void ProcessKeyCatlgatl(FILE* sourceFile, char* Name)
{
	if(strcmp(Name, "BlkTime") == 0)
    {

    }
	else if (strcmp(Name, "BlkUID") == 0)
    {
        BlkUID = Parse(sourceFile, &NumBlkUID, sizeof(uid_struct), sizeof(uint32_t), Name);
    }
	else if (strcmp(Name, "CESConfigUID") == 0)
    {

    }
	else if (strcmp(Name, "Ces2DefConfig") == 0)
	{
	
	}
	else if (strcmp(Name, "CesTime") == 0)
	{
	
	}
	else if (strcmp(Name, "Cmp2DestBlk") == 0)
	{
	
	}
	else if (strcmp(Name, "Cmp2Mdl") == 0)
	{
	
	}
	else if (strcmp(Name, "Cmp2Mvu") == 0)
	{
	
	}
	else if (strcmp(Name, "Cmp2Sub") == 0)
	{
	
	}
	else if (strcmp(Name, "CmpCached") == 0)
	{
	
	}
	else if (strcmp(Name, "CmpDxDView") == 0)
	{
	
	}
	else if (strcmp(Name, "CmpDxDViewTime") == 0)
	{
	
	}
	else if (strcmp(Name, "CmpFlg") == 0)
	{
	
	}
	else if (strcmp(Name, "CmpNam") == 0)
	{
	
	}
	else if (strcmp(Name, "CmpPart") == 0)
	{
	
	}
	else if (strcmp(Name, "CmpTime") == 0)
	{
	
	}
	else if (strcmp(Name, "CmpUID") == 0)
	{
	
	}
	else if (strcmp(Name, "CnfgNam") == 0)
	{
	
	}
	else if (strcmp(Name, "CnfgTime") == 0)
	{
	
	}
	else if (strcmp(Name, "CnfgUID") == 0)
	{
	
	}
	else if (strcmp(Name, "CvuNam") == 0)
	{
	
	}
	else if (strcmp(Name, "CvuSrcTm") == 0)
	{
	
	}
	else if (strcmp(Name, "CvuVer") == 0)
	{
	
	}
	else if (strcmp(Name, "CvuVutp") == 0)
	{
	
	}
	else if (strcmp(Name, "Dsgn2Prps") == 0)
	{
	
	}
	else if (strcmp(Name, "DsgnTime") == 0)
	{
	
	}
	else if (strcmp(Name, "Fixes") == 0)
	{
	
	}
	else if (strcmp(Name, "GlobalNetsAliases") == 0)
	{
	
	}
	else if (strcmp(Name, "Group2CesConfig") == 0)
	{
	
	}
	else if (strcmp(Name, "GroupDescription") == 0)
	{
	
	}
	else if (strcmp(Name, "GroupDxDView") == 0)
	{
	
	}
	else if (strcmp(Name, "GroupDxDViewTime") == 0)
	{
	
	}
	else if (strcmp(Name, "GroupName") == 0)
	{
	
	}
	else if (strcmp(Name, "GroupOwnerUID") == 0)
	{
	    GroupOwnerUID = Parse(sourceFile, &NumGroupOwnerUID, sizeof(uid_struct), sizeof(uint32_t), Name);
	}
	else if (strcmp(Name, "GroupTime") == 0)
	{
	
	}
	else if (strcmp(Name, "GroupType") == 0)
	{
	
	}
	else if (strcmp(Name, "GroupUID") == 0)
	{
	
	}
	else if (strcmp(Name, "ManufacturerName") == 0)
	{
	
	}
	else if (strcmp(Name, "ManufacturerPart2Manufacturer") == 0)
	{
	
	}
	else if (strcmp(Name, "ManufacturerPartDatasheetURL") == 0)
	{
	
	}
	else if (strcmp(Name, "ManufacturerPartDatasheetURL_valueNdx") == 0)
	{
	
	}
	else if (strcmp(Name, "ManufacturerPartNumber") == 0)
	{
	
	}
	else if (strcmp(Name, "ManufacturerPartSupplierParts") == 0)
	{
	
	}
	else if (strcmp(Name, "ManufacturerParts") == 0)
	{
	
	}
	else if (strcmp(Name, "Mdl2CesConfig") == 0)
	{
	
	}
	else if (strcmp(Name, "Mdl2DefConfig") == 0)
	{
	
	}
	else if (strcmp(Name, "Mdl2DefGroup") == 0)
	{
	
	}
	else if (strcmp(Name, "Mdl2Mvus") == 0)
	{
	
	}
	else if (strcmp(Name, "MdlFlg") == 0)
	{
	
	}
	else if (strcmp(Name, "MdlNam") == 0)
	{
	    MdlNam = ParseString(sourceFile, &NumMdlNam, Name);
	}
	else if (strcmp(Name, "Mvu2Subs") == 0)
	{
	
	}
	else if (strcmp(Name, "MvuNam") == 0)
	{
	
	}
	else if (strcmp(Name, "MvuSrcTm") == 0)
	{
	
	}
	else if (strcmp(Name, "MvuVutp") == 0)
	{
	
	}
	else if (strcmp(Name, "PriceBracketMinValue") == 0)
	{
	
	}
	else if (strcmp(Name, "PriceBracketPrice") == 0)
	{
	
	}
	else if (strcmp(Name, "PriceBracketPriceCurrency") == 0)
	{
	
	}
	else if (strcmp(Name, "PriceCurrency") == 0)
	{
	
	}
	else if (strcmp(Name, "PriceCurrencyCounter") == 0)
	{
	
	}
	else if (strcmp(Name, "PrpAttr_S") == 0)
	{
	
	}
	else if (strcmp(Name, "PrpBit_S") == 0)
	{
	
	}
	else if (strcmp(Name, "PrpId_S") == 0)
	{
	
	}
	else if (strcmp(Name, "PrpNam_S") == 0)
	{
	
	}
	else if (strcmp(Name, "PrpOrder_S") == 0)
	{
	
	}
	else if (strcmp(Name, "PrpStr_S") == 0)
	{
	
	}
	else if (strcmp(Name, "Sub2Mdl") == 0)
	{
	
	}
	else if (strcmp(Name, "SubRefUID") == 0)
	{
	
	}
	else if (strcmp(Name, "SubVutp") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierName") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierName_valueNdx") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPart2ManufacturerPart") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPart2ManufacturerParts") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPart2Supplier") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPartActualManufacturerName") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPartActualManufacturerName_valueNdx") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPartActualManufacturerPartNumber") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPartActualManufacturerPartNumber_valueNdx") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPartCaseSize") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPartCaseSize_valueNdx") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPartClickToBuyURL") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPartClickToBuyURL_valueNdx") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPartDatasheetURL") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPartDatasheetURL_valueNdx") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPartDescription") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPartDescription_valueNdx") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPartLifecycle") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPartLifecycle_valueNdx") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPartMOQ") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPartMOQ_valueNdx") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPartMarketCost") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPartMarketCostCurrency") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPartMarketCost_valueNdx") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPartNumber") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPartNumber_valueNdx") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPartPackagingType") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPartPackagingType_valueNdx") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPartPrice") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPartPriceBreakdown") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPartPriceBreakdown_valueNdx") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPartPriceCurrency") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPartPrice_valueNdx") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPartROHS") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPartROHS_valueNdx") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPartStockCount") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPartStockCount_valueNdx") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPartUpdateTime") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierPartUpdateTime_valueNdx") == 0)
	{
	
	}
	else if (strcmp(Name, "SupplierParts") == 0)
	{
	
	}
	else if (strcmp(Name, "TopMdls") == 0)
	{
	
	}
	else if (strcmp(Name, "UpgradeHistory") == 0)
	{
	
	}
	else if (strcmp(Name, "VMTime") == 0)
	{
	
	}
	else if (strcmp(Name, "ValueChanged") == 0)
	{
	
	}
	else if (strcmp(Name, "ValueErrorCode") == 0)
	{
	
	}
	else if (strcmp(Name, "ValueErrorText") == 0)
	{
	
	}
	else if (strcmp(Name, "ValueErrorTextCounter") == 0)
	{
	
	}
	else if (strcmp(Name, "ValueErrorTextNdx") == 0)
	{
	
	}
	else if (strcmp(Name, "Version") == 0)
	{
	
	}
	else
	{
		myPrint("Unknown Key in Catlgatl [%s]\n", Name);
	}
}

/*
******************************************************************
* - function name:	InitCatlgatl()
*
* - description: 	Resets all catlgatl data
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void InitCatlgatl(void)
{
	InitRegular(NumBlkUID, &BlkUID);
	NumBlkUID = -1;

	InitRegular(NumGroupOwnerUID, &GroupOwnerUID);
	NumGroupOwnerUID = -1;

	InitString(NumMdlNam, &MdlNam);
	NumMdlNam = -1;
}