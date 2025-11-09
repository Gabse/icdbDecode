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
#ifndef _CATLGATL_H
#define _CATLGATL_H

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
#define PATH_CATLGATL "catlgatl.v"

/*
******************************************************************
* Global Variables
******************************************************************
*/
// Catlgatl
extern key_struct* Catlgatl_BlkTime;
extern key_struct* Catlgatl_BlkUID;
extern key_struct* Catlgatl_CESConfigUID;
extern key_struct* Catlgatl_Ces2DefConfig;
extern key_struct* Catlgatl_CesTime;
extern key_struct* Catlgatl_Cmp2DestBlk;
extern key_struct* Catlgatl_Cmp2Mdl;
extern key_struct* Catlgatl_Cmp2Mvu;
extern key_struct* Catlgatl_Cmp2Sub;
extern key_struct* Catlgatl_CmpCached;
extern key_struct* Catlgatl_CmpDxDView;
extern key_struct* Catlgatl_CmpDxDViewTime;
extern key_struct* Catlgatl_CmpFlg;
extern key_struct* Catlgatl_CmpNam;
extern key_struct* Catlgatl_CmpPart;
extern key_struct* Catlgatl_CmpTime;
extern key_struct* Catlgatl_CmpUID;
extern key_struct* Catlgatl_CnfgNam;
extern key_struct* Catlgatl_CnfgTime;
extern key_struct* Catlgatl_CnfgUID;
extern key_struct* Catlgatl_CvuNam;
extern key_struct* Catlgatl_CvuSrcTm;
extern key_struct* Catlgatl_CvuVer;
extern key_struct* Catlgatl_CvuVutp;
extern key_struct* Catlgatl_Dsgn2Prps;
extern key_struct* Catlgatl_DsgnTime;
extern key_struct* Catlgatl_Fixes;
extern key_struct* Catlgatl_GlobalNetsAliases;
extern key_struct* Catlgatl_Group2CesConfig;
extern key_struct* Catlgatl_GroupDescription;
extern key_struct* Catlgatl_GroupDxDView;
extern key_struct* Catlgatl_GroupDxDViewTime;
extern key_struct* Catlgatl_GroupName;
extern key_struct* Catlgatl_GroupOwnerUID;
extern key_struct* Catlgatl_GroupTime;
extern key_struct* Catlgatl_GroupType;
extern key_struct* Catlgatl_GroupUID;
extern key_struct* Catlgatl_ManufacturerName;
extern key_struct* Catlgatl_ManufacturerPart2Manufacturer;
extern key_struct* Catlgatl_ManufacturerPartDatasheetURL;
extern key_struct* Catlgatl_ManufacturerPartDatasheetURL_valueNdx;
extern key_struct* Catlgatl_ManufacturerPartNumber;
extern key_struct* Catlgatl_ManufacturerPartSupplierParts;
extern key_struct* Catlgatl_ManufacturerParts;
extern key_struct* Catlgatl_Mdl2CesConfig;
extern key_struct* Catlgatl_Mdl2DefConfig;
extern key_struct* Catlgatl_Mdl2DefGroup;
extern key_struct* Catlgatl_Mdl2Mvus;
extern key_struct* Catlgatl_MdlFlg;
extern key_struct* Catlgatl_MdlNam;
extern key_struct* Catlgatl_Mvu2Subs;
extern key_struct* Catlgatl_MvuNam;
extern key_struct* Catlgatl_MvuSrcTm;
extern key_struct* Catlgatl_MvuVutp;
extern key_struct* Catlgatl_PriceBracketMinValue;
extern key_struct* Catlgatl_PriceBracketPrice;
extern key_struct* Catlgatl_PriceBracketPriceCurrency;
extern key_struct* Catlgatl_PriceCurrency;
extern key_struct* Catlgatl_PriceCurrencyCounter;
extern key_struct* Catlgatl_PrpAttr_S;
extern key_struct* Catlgatl_PrpBit_S;
extern key_struct* Catlgatl_PrpId_S;
extern key_struct* Catlgatl_PrpNam_S;
extern key_struct* Catlgatl_PrpOrder_S;
extern key_struct* Catlgatl_PrpStr_S;
extern key_struct* Catlgatl_Sub2Mdl;
extern key_struct* Catlgatl_SubRefUID;
extern key_struct* Catlgatl_SubVutp;
extern key_struct* Catlgatl_SupplierName;
extern key_struct* Catlgatl_SupplierName_valueNdx;
extern key_struct* Catlgatl_SupplierPart2ManufacturerPart;
extern key_struct* Catlgatl_SupplierPart2ManufacturerParts;
extern key_struct* Catlgatl_SupplierPart2Supplier;
extern key_struct* Catlgatl_SupplierPartActualManufacturerName;
extern key_struct* Catlgatl_SupplierPartActualManufacturerName_valueNdx;
extern key_struct* Catlgatl_SupplierPartActualManufacturerPartNumber;
extern key_struct* Catlgatl_SupplierPartActualManufacturerPartNumber_valueNdx;
extern key_struct* Catlgatl_SupplierPartCaseSize;
extern key_struct* Catlgatl_SupplierPartCaseSize_valueNdx;
extern key_struct* Catlgatl_SupplierPartClickToBuyURL;
extern key_struct* Catlgatl_SupplierPartClickToBuyURL_valueNdx;
extern key_struct* Catlgatl_SupplierPartDatasheetURL;
extern key_struct* Catlgatl_SupplierPartDatasheetURL_valueNdx;
extern key_struct* Catlgatl_SupplierPartDescription;
extern key_struct* Catlgatl_SupplierPartDescription_valueNdx;
extern key_struct* Catlgatl_SupplierPartLifecycle;
extern key_struct* Catlgatl_SupplierPartLifecycle_valueNdx;
extern key_struct* Catlgatl_SupplierPartMOQ;
extern key_struct* Catlgatl_SupplierPartMOQ_valueNdx;
extern key_struct* Catlgatl_SupplierPartMarketCost;
extern key_struct* Catlgatl_SupplierPartMarketCostCurrency;
extern key_struct* Catlgatl_SupplierPartMarketCost_valueNdx;
extern key_struct* Catlgatl_SupplierPartNumber;
extern key_struct* Catlgatl_SupplierPartNumber_valueNdx;
extern key_struct* Catlgatl_SupplierPartPackagingType;
extern key_struct* Catlgatl_SupplierPartPackagingType_valueNdx;
extern key_struct* Catlgatl_SupplierPartPrice;
extern key_struct* Catlgatl_SupplierPartPriceBreakdown;
extern key_struct* Catlgatl_SupplierPartPriceBreakdown_valueNdx;
extern key_struct* Catlgatl_SupplierPartPriceCurrency;
extern key_struct* Catlgatl_SupplierPartPrice_valueNdx;
extern key_struct* Catlgatl_SupplierPartROHS;
extern key_struct* Catlgatl_SupplierPartROHS_valueNdx;
extern key_struct* Catlgatl_SupplierPartStockCount;
extern key_struct* Catlgatl_SupplierPartStockCount_valueNdx;
extern key_struct* Catlgatl_SupplierPartUpdateTime;
extern key_struct* Catlgatl_SupplierPartUpdateTime_valueNdx;
extern key_struct* Catlgatl_SupplierParts;
extern key_struct* Catlgatl_TopMdls;
extern key_struct* Catlgatl_UpgradeHistory;
extern key_struct* Catlgatl_VMTime;
extern key_struct* Catlgatl_ValueChanged;
extern key_struct* Catlgatl_ValueErrorCode;
extern key_struct* Catlgatl_ValueErrorText;
extern key_struct* Catlgatl_ValueErrorTextCounter;
extern key_struct* Catlgatl_ValueErrorTextNdx;
extern key_struct* Catlgatl_Version;

/*
******************************************************************
* Global Functions
******************************************************************
*/
extern void ProcessKeyCatlgatl(FILE*, char*, unsigned int);
extern void InitCatlgatl(void);

#endif //_CATLGATL_H