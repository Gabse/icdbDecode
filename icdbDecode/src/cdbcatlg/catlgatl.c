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
#include "catlgatl.h"
#include <stdint.h>					// Required for int32_t, uint32_t, ...
#include <string.h>					// Required for strcmp
#include "../common.h"				// Required for parseFile

/*
******************************************************************
* Global Variables
******************************************************************
*/
key_struct* Catlgatl_BlkTime = NULL;
key_struct* Catlgatl_BlkUID = NULL;
key_struct* Catlgatl_CESConfigUID = NULL;
key_struct* Catlgatl_Ces2DefConfig = NULL;
key_struct* Catlgatl_CesTime = NULL;
key_struct* Catlgatl_Cmp2DestBlk = NULL;
key_struct* Catlgatl_Cmp2Mdl = NULL;
key_struct* Catlgatl_Cmp2Mvu = NULL;
key_struct* Catlgatl_Cmp2Sub = NULL;
key_struct* Catlgatl_CmpCached = NULL;
key_struct* Catlgatl_CmpDxDView = NULL;
key_struct* Catlgatl_CmpDxDViewTime = NULL;
key_struct* Catlgatl_CmpFlg = NULL;
key_struct* Catlgatl_CmpNam = NULL;
key_struct* Catlgatl_CmpPart = NULL;
key_struct* Catlgatl_CmpTime = NULL;
key_struct* Catlgatl_CmpUID = NULL;
key_struct* Catlgatl_CnfgNam = NULL;
key_struct* Catlgatl_CnfgTime = NULL;
key_struct* Catlgatl_CnfgUID = NULL;
key_struct* Catlgatl_CvuNam = NULL;
key_struct* Catlgatl_CvuSrcTm = NULL;
key_struct* Catlgatl_CvuVer = NULL;
key_struct* Catlgatl_CvuVutp = NULL;
key_struct* Catlgatl_Dsgn2Prps = NULL;
key_struct* Catlgatl_DsgnTime = NULL;
key_struct* Catlgatl_Fixes = NULL;
key_struct* Catlgatl_GlobalNetsAliases = NULL;
key_struct* Catlgatl_Group2CesConfig = NULL;
key_struct* Catlgatl_GroupDescription = NULL;
key_struct* Catlgatl_GroupDxDView = NULL;
key_struct* Catlgatl_GroupDxDViewTime = NULL;
key_struct* Catlgatl_GroupName = NULL;
key_struct* Catlgatl_GroupOwnerUID = NULL;
key_struct* Catlgatl_GroupTime = NULL;
key_struct* Catlgatl_GroupType = NULL;
key_struct* Catlgatl_GroupUID = NULL;
key_struct* Catlgatl_ManufacturerName = NULL;
key_struct* Catlgatl_ManufacturerPart2Manufacturer = NULL;
key_struct* Catlgatl_ManufacturerPartDatasheetURL = NULL;
key_struct* Catlgatl_ManufacturerPartDatasheetURL_valueNdx = NULL;
key_struct* Catlgatl_ManufacturerPartNumber = NULL;
key_struct* Catlgatl_ManufacturerPartSupplierParts = NULL;
key_struct* Catlgatl_ManufacturerParts = NULL;
key_struct* Catlgatl_Mdl2CesConfig = NULL;
key_struct* Catlgatl_Mdl2DefConfig = NULL;
key_struct* Catlgatl_Mdl2DefGroup = NULL;
key_struct* Catlgatl_Mdl2Mvus = NULL;
key_struct* Catlgatl_MdlFlg = NULL;
key_struct* Catlgatl_MdlNam = NULL;
key_struct* Catlgatl_Mvu2Subs = NULL;
key_struct* Catlgatl_MvuNam = NULL;
key_struct* Catlgatl_MvuSrcTm = NULL;
key_struct* Catlgatl_MvuVutp = NULL;
key_struct* Catlgatl_PriceBracketMinValue = NULL;
key_struct* Catlgatl_PriceBracketPrice = NULL;
key_struct* Catlgatl_PriceBracketPriceCurrency = NULL;
key_struct* Catlgatl_PriceCurrency = NULL;
key_struct* Catlgatl_PriceCurrencyCounter = NULL;
key_struct* Catlgatl_PrpAttr_S = NULL;
key_struct* Catlgatl_PrpBit_S = NULL;
key_struct* Catlgatl_PrpId_S = NULL;
key_struct* Catlgatl_PrpNam_S = NULL;
key_struct* Catlgatl_PrpOrder_S = NULL;
key_struct* Catlgatl_PrpStr_S = NULL;
key_struct* Catlgatl_Sub2Mdl = NULL;
key_struct* Catlgatl_SubRefUID = NULL;
key_struct* Catlgatl_SubVutp = NULL;
key_struct* Catlgatl_SupplierName = NULL;
key_struct* Catlgatl_SupplierName_valueNdx = NULL;
key_struct* Catlgatl_SupplierPart2ManufacturerPart = NULL;
key_struct* Catlgatl_SupplierPart2ManufacturerParts = NULL;
key_struct* Catlgatl_SupplierPart2Supplier = NULL;
key_struct* Catlgatl_SupplierPartActualManufacturerName = NULL;
key_struct* Catlgatl_SupplierPartActualManufacturerName_valueNdx = NULL;
key_struct* Catlgatl_SupplierPartActualManufacturerPartNumber = NULL;
key_struct* Catlgatl_SupplierPartActualManufacturerPartNumber_valueNdx = NULL;
key_struct* Catlgatl_SupplierPartCaseSize = NULL;
key_struct* Catlgatl_SupplierPartCaseSize_valueNdx = NULL;
key_struct* Catlgatl_SupplierPartClickToBuyURL = NULL;
key_struct* Catlgatl_SupplierPartClickToBuyURL_valueNdx = NULL;
key_struct* Catlgatl_SupplierPartDatasheetURL = NULL;
key_struct* Catlgatl_SupplierPartDatasheetURL_valueNdx = NULL;
key_struct* Catlgatl_SupplierPartDescription = NULL;
key_struct* Catlgatl_SupplierPartDescription_valueNdx = NULL;
key_struct* Catlgatl_SupplierPartLifecycle = NULL;
key_struct* Catlgatl_SupplierPartLifecycle_valueNdx = NULL;
key_struct* Catlgatl_SupplierPartMOQ = NULL;
key_struct* Catlgatl_SupplierPartMOQ_valueNdx = NULL;
key_struct* Catlgatl_SupplierPartMarketCost = NULL;
key_struct* Catlgatl_SupplierPartMarketCostCurrency = NULL;
key_struct* Catlgatl_SupplierPartMarketCost_valueNdx = NULL;
key_struct* Catlgatl_SupplierPartNumber = NULL;
key_struct* Catlgatl_SupplierPartNumber_valueNdx = NULL;
key_struct* Catlgatl_SupplierPartPackagingType = NULL;
key_struct* Catlgatl_SupplierPartPackagingType_valueNdx = NULL;
key_struct* Catlgatl_SupplierPartPrice = NULL;
key_struct* Catlgatl_SupplierPartPriceBreakdown = NULL;
key_struct* Catlgatl_SupplierPartPriceBreakdown_valueNdx = NULL;
key_struct* Catlgatl_SupplierPartPriceCurrency = NULL;
key_struct* Catlgatl_SupplierPartPrice_valueNdx = NULL;
key_struct* Catlgatl_SupplierPartROHS = NULL;
key_struct* Catlgatl_SupplierPartROHS_valueNdx = NULL;
key_struct* Catlgatl_SupplierPartStockCount = NULL;
key_struct* Catlgatl_SupplierPartStockCount_valueNdx = NULL;
key_struct* Catlgatl_SupplierPartUpdateTime = NULL;
key_struct* Catlgatl_SupplierPartUpdateTime_valueNdx = NULL;
key_struct* Catlgatl_SupplierParts = NULL;
key_struct* Catlgatl_TopMdls = NULL;
key_struct* Catlgatl_UpgradeHistory = NULL;
key_struct* Catlgatl_VMTime = NULL;
key_struct* Catlgatl_ValueChanged = NULL;
key_struct* Catlgatl_ValueErrorCode = NULL;
key_struct* Catlgatl_ValueErrorText = NULL;
key_struct* Catlgatl_ValueErrorTextCounter = NULL;
key_struct* Catlgatl_ValueErrorTextNdx = NULL;
key_struct* Catlgatl_Version = NULL;

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	ProcessKeyCatlgatl()
*
* - description: 	Check for catlgatl keys
*
* - parameter: 		file pointer; Key to check; length of key
*
* - return value: 	-
******************************************************************
*/
void ProcessKeyCatlgatl(FILE * sourceFile, char* Key, unsigned int KeyLen)
{
	if (strcmp(Key, "BlkTime") == 0)
	{
		Catlgatl_BlkTime = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "BlkUID") == 0)
	{
		Catlgatl_BlkUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CESConfigUID") == 0)
	{
		Catlgatl_CESConfigUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Ces2DefConfig") == 0)
	{
		Catlgatl_Ces2DefConfig = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CesTime") == 0)
	{
		Catlgatl_CesTime = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Cmp2DestBlk") == 0)
	{
		Catlgatl_Cmp2DestBlk = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Cmp2Mdl") == 0)
	{
		Catlgatl_Cmp2Mdl = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Cmp2Mvu") == 0)
	{
		Catlgatl_Cmp2Mvu = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Cmp2Sub") == 0)
	{
		Catlgatl_Cmp2Sub = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CmpCached") == 0)
	{
		Catlgatl_CmpCached = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CmpDxDView") == 0)
	{
		Catlgatl_CmpDxDView = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CmpDxDViewTime") == 0)
	{
		Catlgatl_CmpDxDViewTime = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CmpFlg") == 0)
	{
		Catlgatl_CmpFlg = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CmpNam") == 0)
	{
		Catlgatl_CmpNam = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CmpPart") == 0)
	{
		Catlgatl_CmpPart = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CmpTime") == 0)
	{
		Catlgatl_CmpTime = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CmpUID") == 0)
	{
		Catlgatl_CmpUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CnfgNam") == 0)
	{
		Catlgatl_CnfgNam = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CnfgTime") == 0)
	{
		Catlgatl_CnfgTime = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CnfgUID") == 0)
	{
		Catlgatl_CnfgUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CvuNam") == 0)
	{
		Catlgatl_CvuNam = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CvuSrcTm") == 0)
	{
		Catlgatl_CvuSrcTm = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CvuVer") == 0)
	{
		Catlgatl_CvuVer = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CvuVutp") == 0)
	{
		Catlgatl_CvuVutp = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Dsgn2Prps") == 0)
	{
		Catlgatl_Dsgn2Prps = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "DsgnTime") == 0)
	{
		Catlgatl_DsgnTime = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Fixes") == 0)
	{
		Catlgatl_Fixes = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "GlobalNetsAliases") == 0)
	{
		Catlgatl_GlobalNetsAliases = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Group2CesConfig") == 0)
	{
		Catlgatl_Group2CesConfig = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "GroupDescription") == 0)
	{
		Catlgatl_GroupDescription = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "GroupDxDView") == 0)
	{
		Catlgatl_GroupDxDView = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "GroupDxDViewTime") == 0)
	{
		Catlgatl_GroupDxDViewTime = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "GroupName") == 0)
	{
		Catlgatl_GroupName = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "GroupOwnerUID") == 0)
	{
		Catlgatl_GroupOwnerUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "GroupTime") == 0)
	{
		Catlgatl_GroupTime = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "GroupType") == 0)
	{
		Catlgatl_GroupType = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "GroupUID") == 0)
	{
		Catlgatl_GroupUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "ManufacturerName") == 0)
	{
		Catlgatl_ManufacturerName = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "ManufacturerPart2Manufacturer") == 0)
	{
		Catlgatl_ManufacturerPart2Manufacturer = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "ManufacturerPartDatasheetURL") == 0)
	{
		Catlgatl_ManufacturerPartDatasheetURL = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "ManufacturerPartDatasheetURL_valueNdx") == 0)
	{
		Catlgatl_ManufacturerPartDatasheetURL_valueNdx = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "ManufacturerPartNumber") == 0)
	{
		Catlgatl_ManufacturerPartNumber = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "ManufacturerPartSupplierParts") == 0)
	{
		Catlgatl_ManufacturerPartSupplierParts = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "ManufacturerParts") == 0)
	{
		Catlgatl_ManufacturerParts = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Mdl2CesConfig") == 0)
	{
		Catlgatl_Mdl2CesConfig = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Mdl2DefConfig") == 0)
	{
		Catlgatl_Mdl2DefConfig = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Mdl2DefGroup") == 0)
	{
		Catlgatl_Mdl2DefGroup = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Mdl2Mvus") == 0)
	{
		Catlgatl_Mdl2Mvus = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "MdlFlg") == 0)
	{
		Catlgatl_MdlFlg = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "MdlNam") == 0)
	{
		Catlgatl_MdlNam = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Mvu2Subs") == 0)
	{
		Catlgatl_Mvu2Subs = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "MvuNam") == 0)
	{
		Catlgatl_MvuNam = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "MvuSrcTm") == 0)
	{
		Catlgatl_MvuSrcTm = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "MvuVutp") == 0)
	{
		Catlgatl_MvuVutp = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PriceBracketMinValue") == 0)
	{
		Catlgatl_PriceBracketMinValue = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PriceBracketPrice") == 0)
	{
		Catlgatl_PriceBracketPrice = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PriceBracketPriceCurrency") == 0)
	{
		Catlgatl_PriceBracketPriceCurrency = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PriceCurrency") == 0)
	{
		Catlgatl_PriceCurrency = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PriceCurrencyCounter") == 0)
	{
		Catlgatl_PriceCurrencyCounter = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PrpAttr_S") == 0)
	{
		Catlgatl_PrpAttr_S = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PrpBit_S") == 0)
	{
		Catlgatl_PrpBit_S = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PrpId_S") == 0)
	{
		Catlgatl_PrpId_S = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PrpNam_S") == 0)
	{
		Catlgatl_PrpNam_S = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PrpOrder_S") == 0)
	{
		Catlgatl_PrpOrder_S = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PrpStr_S") == 0)
	{
		Catlgatl_PrpStr_S = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Sub2Mdl") == 0)
	{
		Catlgatl_Sub2Mdl = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SubRefUID") == 0)
	{
		Catlgatl_SubRefUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SubVutp") == 0)
	{
		Catlgatl_SubVutp = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierName") == 0)
	{
		Catlgatl_SupplierName = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierName_valueNdx") == 0)
	{
		Catlgatl_SupplierName_valueNdx = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPart2ManufacturerPart") == 0)
	{
		Catlgatl_SupplierPart2ManufacturerPart = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPart2ManufacturerParts") == 0)
	{
		Catlgatl_SupplierPart2ManufacturerParts = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPart2Supplier") == 0)
	{
		Catlgatl_SupplierPart2Supplier = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPartActualManufacturerName") == 0)
	{
		Catlgatl_SupplierPartActualManufacturerName = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPartActualManufacturerName_valueNdx") == 0)
	{
		Catlgatl_SupplierPartActualManufacturerName_valueNdx = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPartActualManufacturerPartNumber") == 0)
	{
		Catlgatl_SupplierPartActualManufacturerPartNumber = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPartActualManufacturerPartNumber_valueNdx") == 0)
	{
		Catlgatl_SupplierPartActualManufacturerPartNumber_valueNdx = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPartCaseSize") == 0)
	{
		Catlgatl_SupplierPartCaseSize = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPartCaseSize_valueNdx") == 0)
	{
		Catlgatl_SupplierPartCaseSize_valueNdx = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPartClickToBuyURL") == 0)
	{
		Catlgatl_SupplierPartClickToBuyURL = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPartClickToBuyURL_valueNdx") == 0)
	{
		Catlgatl_SupplierPartClickToBuyURL_valueNdx = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPartDatasheetURL") == 0)
	{
		Catlgatl_SupplierPartDatasheetURL = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPartDatasheetURL_valueNdx") == 0)
	{
		Catlgatl_SupplierPartDatasheetURL_valueNdx = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPartDescription") == 0)
	{
		Catlgatl_SupplierPartDescription = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPartDescription_valueNdx") == 0)
	{
		Catlgatl_SupplierPartDescription_valueNdx = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPartLifecycle") == 0)
	{
		Catlgatl_SupplierPartLifecycle = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPartLifecycle_valueNdx") == 0)
	{
		Catlgatl_SupplierPartLifecycle_valueNdx = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPartMOQ") == 0)
	{
		Catlgatl_SupplierPartMOQ = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPartMOQ_valueNdx") == 0)
	{
		Catlgatl_SupplierPartMOQ_valueNdx = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPartMarketCost") == 0)
	{
		Catlgatl_SupplierPartMarketCost = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPartMarketCostCurrency") == 0)
	{
		Catlgatl_SupplierPartMarketCostCurrency = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPartMarketCost_valueNdx") == 0)
	{
		Catlgatl_SupplierPartMarketCost_valueNdx = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPartNumber") == 0)
	{
		Catlgatl_SupplierPartNumber = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPartNumber_valueNdx") == 0)
	{
		Catlgatl_SupplierPartNumber_valueNdx = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPartPackagingType") == 0)
	{
		Catlgatl_SupplierPartPackagingType = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPartPackagingType_valueNdx") == 0)
	{
		Catlgatl_SupplierPartPackagingType_valueNdx = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPartPrice") == 0)
	{
		Catlgatl_SupplierPartPrice = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPartPriceBreakdown") == 0)
	{
		Catlgatl_SupplierPartPriceBreakdown = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPartPriceBreakdown_valueNdx") == 0)
	{
		Catlgatl_SupplierPartPriceBreakdown_valueNdx = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPartPriceCurrency") == 0)
	{
		Catlgatl_SupplierPartPriceCurrency = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPartPrice_valueNdx") == 0)
	{
		Catlgatl_SupplierPartPrice_valueNdx = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPartROHS") == 0)
	{
		Catlgatl_SupplierPartROHS = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPartROHS_valueNdx") == 0)
	{
		Catlgatl_SupplierPartROHS_valueNdx = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPartStockCount") == 0)
	{
		Catlgatl_SupplierPartStockCount = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPartStockCount_valueNdx") == 0)
	{
		Catlgatl_SupplierPartStockCount_valueNdx = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPartUpdateTime") == 0)
	{
		Catlgatl_SupplierPartUpdateTime = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierPartUpdateTime_valueNdx") == 0)
	{
		Catlgatl_SupplierPartUpdateTime_valueNdx = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "SupplierParts") == 0)
	{
		Catlgatl_SupplierParts = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "TopMdls") == 0)
	{
		Catlgatl_TopMdls = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "UpgradeHistory") == 0)
	{
		Catlgatl_UpgradeHistory = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "VMTime") == 0)
	{
		Catlgatl_VMTime = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "ValueChanged") == 0)
	{
		Catlgatl_ValueChanged = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "ValueErrorCode") == 0)
	{
		Catlgatl_ValueErrorCode = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "ValueErrorText") == 0)
	{
		Catlgatl_ValueErrorText = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "ValueErrorTextCounter") == 0)
	{
		Catlgatl_ValueErrorTextCounter = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "ValueErrorTextNdx") == 0)
	{
		Catlgatl_ValueErrorTextNdx = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Version") == 0)
	{
		Catlgatl_Version = ParseKey(sourceFile);
	}
	else
	{
		myPrint("Unknown Key in catlgatl [%s]\n", Key);
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
	InitKey(&Catlgatl_BlkTime);
	InitKey(&Catlgatl_BlkUID);
	InitKey(&Catlgatl_CESConfigUID);
	InitKey(&Catlgatl_Ces2DefConfig);
	InitKey(&Catlgatl_CesTime);
	InitKey(&Catlgatl_Cmp2DestBlk);
	InitKey(&Catlgatl_Cmp2Mdl);
	InitKey(&Catlgatl_Cmp2Mvu);
	InitKey(&Catlgatl_Cmp2Sub);
	InitKey(&Catlgatl_CmpCached);
	InitKey(&Catlgatl_CmpDxDView);
	InitKey(&Catlgatl_CmpDxDViewTime);
	InitKey(&Catlgatl_CmpFlg);
	InitKey(&Catlgatl_CmpNam);
	InitKey(&Catlgatl_CmpPart);
	InitKey(&Catlgatl_CmpTime);
	InitKey(&Catlgatl_CmpUID);
	InitKey(&Catlgatl_CnfgNam);
	InitKey(&Catlgatl_CnfgTime);
	InitKey(&Catlgatl_CnfgUID);
	InitKey(&Catlgatl_CvuNam);
	InitKey(&Catlgatl_CvuSrcTm);
	InitKey(&Catlgatl_CvuVer);
	InitKey(&Catlgatl_CvuVutp);
	InitKey(&Catlgatl_Dsgn2Prps);
	InitKey(&Catlgatl_DsgnTime);
	InitKey(&Catlgatl_Fixes);
	InitKey(&Catlgatl_GlobalNetsAliases);
	InitKey(&Catlgatl_Group2CesConfig);
	InitKey(&Catlgatl_GroupDescription);
	InitKey(&Catlgatl_GroupDxDView);
	InitKey(&Catlgatl_GroupDxDViewTime);
	InitKey(&Catlgatl_GroupName);
	InitKey(&Catlgatl_GroupOwnerUID);
	InitKey(&Catlgatl_GroupTime);
	InitKey(&Catlgatl_GroupType);
	InitKey(&Catlgatl_GroupUID);
	InitKey(&Catlgatl_ManufacturerName);
	InitKey(&Catlgatl_ManufacturerPart2Manufacturer);
	InitKey(&Catlgatl_ManufacturerPartDatasheetURL);
	InitKey(&Catlgatl_ManufacturerPartDatasheetURL_valueNdx);
	InitKey(&Catlgatl_ManufacturerPartNumber);
	InitKey(&Catlgatl_ManufacturerPartSupplierParts);
	InitKey(&Catlgatl_ManufacturerParts);
	InitKey(&Catlgatl_Mdl2CesConfig);
	InitKey(&Catlgatl_Mdl2DefConfig);
	InitKey(&Catlgatl_Mdl2DefGroup);
	InitKey(&Catlgatl_Mdl2Mvus);
	InitKey(&Catlgatl_MdlFlg);
	InitKey(&Catlgatl_MdlNam);
	InitKey(&Catlgatl_Mvu2Subs);
	InitKey(&Catlgatl_MvuNam);
	InitKey(&Catlgatl_MvuSrcTm);
	InitKey(&Catlgatl_MvuVutp);
	InitKey(&Catlgatl_PriceBracketMinValue);
	InitKey(&Catlgatl_PriceBracketPrice);
	InitKey(&Catlgatl_PriceBracketPriceCurrency);
	InitKey(&Catlgatl_PriceCurrency);
	InitKey(&Catlgatl_PriceCurrencyCounter);
	InitKey(&Catlgatl_PrpAttr_S);
	InitKey(&Catlgatl_PrpBit_S);
	InitKey(&Catlgatl_PrpId_S);
	InitKey(&Catlgatl_PrpNam_S);
	InitKey(&Catlgatl_PrpOrder_S);
	InitKey(&Catlgatl_PrpStr_S);
	InitKey(&Catlgatl_Sub2Mdl);
	InitKey(&Catlgatl_SubRefUID);
	InitKey(&Catlgatl_SubVutp);
	InitKey(&Catlgatl_SupplierName);
	InitKey(&Catlgatl_SupplierName_valueNdx);
	InitKey(&Catlgatl_SupplierPart2ManufacturerPart);
	InitKey(&Catlgatl_SupplierPart2ManufacturerParts);
	InitKey(&Catlgatl_SupplierPart2Supplier);
	InitKey(&Catlgatl_SupplierPartActualManufacturerName);
	InitKey(&Catlgatl_SupplierPartActualManufacturerName_valueNdx);
	InitKey(&Catlgatl_SupplierPartActualManufacturerPartNumber);
	InitKey(&Catlgatl_SupplierPartActualManufacturerPartNumber_valueNdx);
	InitKey(&Catlgatl_SupplierPartCaseSize);
	InitKey(&Catlgatl_SupplierPartCaseSize_valueNdx);
	InitKey(&Catlgatl_SupplierPartClickToBuyURL);
	InitKey(&Catlgatl_SupplierPartClickToBuyURL_valueNdx);
	InitKey(&Catlgatl_SupplierPartDatasheetURL);
	InitKey(&Catlgatl_SupplierPartDatasheetURL_valueNdx);
	InitKey(&Catlgatl_SupplierPartDescription);
	InitKey(&Catlgatl_SupplierPartDescription_valueNdx);
	InitKey(&Catlgatl_SupplierPartLifecycle);
	InitKey(&Catlgatl_SupplierPartLifecycle_valueNdx);
	InitKey(&Catlgatl_SupplierPartMOQ);
	InitKey(&Catlgatl_SupplierPartMOQ_valueNdx);
	InitKey(&Catlgatl_SupplierPartMarketCost);
	InitKey(&Catlgatl_SupplierPartMarketCostCurrency);
	InitKey(&Catlgatl_SupplierPartMarketCost_valueNdx);
	InitKey(&Catlgatl_SupplierPartNumber);
	InitKey(&Catlgatl_SupplierPartNumber_valueNdx);
	InitKey(&Catlgatl_SupplierPartPackagingType);
	InitKey(&Catlgatl_SupplierPartPackagingType_valueNdx);
	InitKey(&Catlgatl_SupplierPartPrice);
	InitKey(&Catlgatl_SupplierPartPriceBreakdown);
	InitKey(&Catlgatl_SupplierPartPriceBreakdown_valueNdx);
	InitKey(&Catlgatl_SupplierPartPriceCurrency);
	InitKey(&Catlgatl_SupplierPartPrice_valueNdx);
	InitKey(&Catlgatl_SupplierPartROHS);
	InitKey(&Catlgatl_SupplierPartROHS_valueNdx);
	InitKey(&Catlgatl_SupplierPartStockCount);
	InitKey(&Catlgatl_SupplierPartStockCount_valueNdx);
	InitKey(&Catlgatl_SupplierPartUpdateTime);
	InitKey(&Catlgatl_SupplierPartUpdateTime_valueNdx);
	InitKey(&Catlgatl_SupplierParts);
	InitKey(&Catlgatl_TopMdls);
	InitKey(&Catlgatl_UpgradeHistory);
	InitKey(&Catlgatl_VMTime);
	InitKey(&Catlgatl_ValueChanged);
	InitKey(&Catlgatl_ValueErrorCode);
	InitKey(&Catlgatl_ValueErrorText);
	InitKey(&Catlgatl_ValueErrorTextCounter);
	InitKey(&Catlgatl_ValueErrorTextNdx);
	InitKey(&Catlgatl_Version);
}