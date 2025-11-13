/*
******************************************************************
* Info
******************************************************************
* iCDBanalyzer
* 
* This tool can be used to extract and format keys, typecodes and data payload from .v files, found in SiemensEDA (former Mentor Graphics) icdb.dat databses.
* It is part of icdbDecode.
*/

/*
******************************************************************
* Includes
******************************************************************
*/
#include <stdio.h>		// Required for fprint, fopen, ...
#include <stdlib.h>		// Required for calloc to work properly
#include <stdint.h>		// Required for int32_t, uint32_t, ...
#include <string.h>		// Required for strlen
#include <time.h>		// Required for clock_t
#include "../../../icdbDecode/src/common.h"		// Required for parseFile
#include "../../../icdbDecode/src/stringutil.h"	// Required for string manipulation

/*
******************************************************************
* Global Defines
******************************************************************
*/
#define _CRT_SECURE_NO_DEPRECATE			// Disable insecure function warning in VisualStudio

/*
******************************************************************
* Function Prototypes
******************************************************************
*/
int AnalyzerOpen();
void AnalyzerWrite(FILE*, char*, unsigned int);
void AnalyzerClose();

/*
******************************************************************
* Global Variables
******************************************************************
*/
FILE* KeyCsvFile = NULL;
FILE* Txtfile = NULL;
char* name = NULL;
char* nameSmall = NULL;
char* PathTemp = NULL;
unsigned int nameLen = 0;
unsigned int PathLenTemp = 0;

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	main()
*
* - description: 	Application entry point
*
* - parameter: 		arguments
*
* - return value: 	exit code
******************************************************************
*/
int main(int argc, char** argv)
{
	clock_t starttime = clock();
	char* nameTemp = NULL;
	
	// Check parameter
	if(argc < 2)
	{
		printf("No source file specified!\n");
		return -1;
	}
	else if(argc == 2) // One file
	{
		printf("Using source file %s\n", argv[1]);
	}
	else // Multiple files
	{
		printf("Using source files: %s", argv[1]);
		for( uint32_t i = 2; i < argc; i++)
		{
			printf(", %s", argv[i]);
		}
		printf("\n");
	}

	for (uint32_t i = 1; i < argc; i++)
	{
		nameLen = strlen(argv[i]) + 1;
		
		nameTemp = malloc(nameLen);
		if(nameTemp == NULL)
		{
			return -1;
		}
		memcpy(nameTemp, argv[i], nameLen);
	
		// Get filename without path
		removeFilenameExtension(nameTemp, &nameLen);
		nameLen = removeFilePath(nameTemp, nameLen, &name);
		nameSmall = stringAllSmall(name, nameLen);
		PathLenTemp = addStrings(&PathTemp, name, nameLen, "data", sizeof("data"), '\\');
	
		if (AnalyzerOpen())
		{
			printf("Error opening file!");
			return -1;
		}
		if (parseFile(NULL, 0, argv[i], strlen(argv[i]), AnalyzerWrite))
		{
			printf("Error parsing file!");
			return -1;
		}

		AnalyzerClose();
		free(nameTemp); // also clears name
		free(nameSmall);
		free(PathTemp);
	}
	printf("Finnish after %fs\n", (float)(clock() - starttime)/(float) CLOCKS_PER_SEC);
	return 0;
}

/*
******************************************************************
* Local Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	AnalyzerOpen()
*
* - description: 	Opens the destination files
*
* - parameter: 		Key of file, length of name
*
* - return value: 	0 if file opened sucessfully
******************************************************************
*/
int AnalyzerOpen()
{
	// Open files
	char* FullnameTemp = NULL;
	char* NameTemp = NULL;
	unsigned int NameLenTemp = 0;

	// Keys File
	createPath(&FullnameTemp, name, nameLen, "Keys.csv", sizeof("Keys.csv"), '\\');
	KeyCsvFile = fopen(FullnameTemp, "w");
	free(FullnameTemp);

	// Data file
	NameLenTemp = addStrings(&NameTemp, nameSmall, nameLen, ".txt", sizeof(".txt"), '\0');
	createPath(&FullnameTemp, name, nameLen, NameTemp, NameLenTemp, '\\');
	Txtfile = fopen(FullnameTemp, "w");
	free(FullnameTemp);
	free(NameTemp);

	if (KeyCsvFile == NULL || Txtfile == NULL)
	{
		return -1;
	}

	// Csv file
	fprintf(KeyCsvFile, "Key,TypeCode,TypeCodeText,RawSize,CalcSize\n");

	return 0;
}

/*
******************************************************************
* - function name:	AnalyzerWrite()
*
* - description: 	Stores keys to file
*
* - parameter: 		file pointer; Key; length of key
*
* - return value: 	-
******************************************************************
*/
void AnalyzerWrite(FILE* sourceFile, char* Key, unsigned int KeyLen)
{
	FILE* Datafile = NULL;
	char* NameTemp = NULL;
	key_struct* Data = NULL;

	char* FullnameTemp = NULL;
	unsigned int NameLenTemp = 0;

	// Get Data
	Data = ParseKey(sourceFile);

	// Data file
	NameLenTemp = addStrings(&NameTemp, Key, KeyLen, ".txt", sizeof(".txt"), '\0');
	createPath(&FullnameTemp, PathTemp, PathLenTemp, NameTemp, NameLenTemp, '\\');
	Datafile = fopen(FullnameTemp, "w");
	free(FullnameTemp);
	free(NameTemp);

	// Write data
	fprintf(Txtfile, "%s\n", Key);
	fprintf(Datafile, "Key:\t\t[%s]\n", Key);
	fprintf(Txtfile, "\tKey:\t\t[%s]\n", Key);
	fprintf(Datafile, "RawSize:\t[%d]\n", Data->Length);
	fprintf(Txtfile, "\tRawSize:\t[%d]\n", Data->Length);
	fprintf(Datafile, "CalcSize:\t[%d]\n", Data->LengthCalc);
	fprintf(Txtfile, "\tCalcSize:\t[%d]\n", Data->LengthCalc);
	fprintf(Datafile, "Typecode:\t[%d]\n", Data->Typecode);
	fprintf(Txtfile, "\tTypecode:\t[%d]\n", Data->Typecode);
	switch (Data->Typecode)
	{
	case typecode_String:
		fprintf(KeyCsvFile, "%s,%d,String,%d,%d\n", Key, Data->Typecode, Data->Length, Data->LengthCalc);
		fprintf(Datafile, "Type:\t\t[String]\n");
		fprintf(Txtfile, "\tType:\t\t[String]\n");
		fprintf(Datafile, "Payload:\n");
		fprintf(Txtfile, "\tPayload:\n");
		for (unsigned int i = 0; i < Data->LengthCalc; i++)
		{
			fprintf(Datafile, "\t%d:\t[%s]\n", i + 1, ((string_struct*)(Data->Data))[i].Text);
			fprintf(Txtfile, "\t\t%d:\t[%s]\n", i + 1, ((string_struct*)(Data->Data))[i].Text);
		}
		break;
	case typecode_IntArray:
		fprintf(KeyCsvFile, "%s,%d,IntArray,%d,%d\n", Key, Data->Typecode, Data->Length, Data->LengthCalc);
		fprintf(Datafile, "Type:\t\t[IntArray]\n");
		fprintf(Txtfile, "\tType:\t\t[IntArray]\n");
		for (unsigned int i = 0; i < Data->LengthCalc; i++)
		{
			fprintf(Datafile, "\t%d:\n", i + 1);
			fprintf(Txtfile, "\t\t%d:\n", i + 1);
			for (unsigned int j = 0; j < ((int_array_struct*)(Data->Data))[i].Length; j++)
			{
				fprintf(Datafile, "\t\t%d:\t[%08X : %d]\n", j + 1, ((int_array_struct*)(Data->Data))[i].Data[j].u32[0], ((int_array_struct*)(Data->Data))[i].Data[j].u32[0]);
				fprintf(Txtfile, "\t\t\t%d:\t[%08X : %d]\n", j + 1, ((int_array_struct*)(Data->Data))[i].Data[j].u32[0], ((int_array_struct*)(Data->Data))[i].Data[j].u32[0]);
			}
		}
		break;
	case typecode_UID:
		fprintf(KeyCsvFile, "%s,%d,UID,%d,%d\n", Key, Data->Typecode, Data->Length, Data->LengthCalc);
		fprintf(Datafile, "Type:\t\t[UID]\n");
		fprintf(Txtfile, "\tType:\t\t[UID]\n");
		fprintf(Datafile, "Payload:\n");
		fprintf(Txtfile, "\tPayload:\n");
		for (unsigned int i = 0; i < Data->LengthCalc; i++)
		{
			fprintf(Datafile, "\t%d:\t[0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X]\n", i + 1, ((uid_struct*)(Data->Data))[i].UID[0], ((uid_struct*)(Data->Data))[i].UID[1], ((uid_struct*)(Data->Data))[i].UID[2], ((uid_struct*)(Data->Data))[i].UID[3], ((uid_struct*)(Data->Data))[i].UID[4], ((uid_struct*)(Data->Data))[i].UID[5], ((uid_struct*)(Data->Data))[i].UID[6], ((uid_struct*)(Data->Data))[i].UID[7]);
			fprintf(Txtfile, "\t\t%d:\t[0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X]\n", i + 1, ((uid_struct*)(Data->Data))[i].UID[0], ((uid_struct*)(Data->Data))[i].UID[1], ((uid_struct*)(Data->Data))[i].UID[2], ((uid_struct*)(Data->Data))[i].UID[3], ((uid_struct*)(Data->Data))[i].UID[4], ((uid_struct*)(Data->Data))[i].UID[5], ((uid_struct*)(Data->Data))[i].UID[6], ((uid_struct*)(Data->Data))[i].UID[7]);
		}
		break;
	case typecode_SUID:
		fprintf(KeyCsvFile, "%s,%d,SUID,%d,%d\n", Key, Data->Typecode, Data->Length, Data->LengthCalc);
		fprintf(Datafile, "Type:\t\t[SUID]\n");
		fprintf(Txtfile, "\tType:\t\t[SUID]\n");
		for (unsigned int i = 0; i < Data->LengthCalc; i++)
		{
			fprintf(Datafile, "\t%d:\t[%08X %08X %08X %08X : %d %d %d %d]\n", i + 1, ((LongLongData*)(Data->Data))[i].u32[0], ((LongLongData*)(Data->Data))[i].u32[1], ((LongLongData*)(Data->Data))[i].u32[2], ((LongLongData*)(Data->Data))[i].u32[3], ((LongLongData*)(Data->Data))[i].u32[0], ((LongLongData*)(Data->Data))[i].u32[1], ((LongLongData*)(Data->Data))[i].u32[2], ((LongLongData*)(Data->Data))[i].u32[3]);
			fprintf(Txtfile, "\t\t%d:\t[%08X %08X %08X %08X : %d %d %d %d]\n", i + 1, ((LongLongData*)(Data->Data))[i].u32[0], ((LongLongData*)(Data->Data))[i].u32[1], ((LongLongData*)(Data->Data))[i].u32[2], ((LongLongData*)(Data->Data))[i].u32[3], ((LongLongData*)(Data->Data))[i].u32[0], ((LongLongData*)(Data->Data))[i].u32[1], ((LongLongData*)(Data->Data))[i].u32[2], ((LongLongData*)(Data->Data))[i].u32[3]);
		}
		break;
	case typecode_Int:
		fprintf(KeyCsvFile, "%s,%d,Int,%d,%d\n", Key, Data->Typecode, Data->Length, Data->LengthCalc);
		fprintf(Datafile, "Type:\t\t[Int]\n");
		fprintf(Txtfile, "\tType:\t\t[Int]\n");
		fprintf(Datafile, "Payload:\n");
		fprintf(Txtfile, "\tPayload:\n");
		for (unsigned int i = 0; i < Data->LengthCalc; i++)
		{
			fprintf(Datafile, "\t%d:\t[%08X : %d]\n", i + 1, ((IntData*)(Data->Data))[i].u32[0], ((IntData*)(Data->Data))[i].u32[0]);
			fprintf(Txtfile, "\t\t%d:\t[%08X : %d]\n", i + 1, ((IntData*)(Data->Data))[i].u32[0], ((IntData*)(Data->Data))[i].u32[0]);
		}
		break;
	case typecode_Long:
		fprintf(KeyCsvFile, "%s,%d,Long,%d,%d\n", Key, Data->Typecode, Data->Length, Data->LengthCalc);
		fprintf(Datafile, "Type:\t\t[Long]\n");
		fprintf(Txtfile, "\tType:\t\t[Long]\n");
		for (unsigned int i = 0; i < Data->LengthCalc; i++)
		{
			fprintf(Datafile, "\t%d:\t[%08X %08X : %d %d]\n", i + 1, ((LongData*)(Data->Data))[i].u32[0], ((LongData*)(Data->Data))[i].u32[1], ((LongData*)(Data->Data))[i].u32[0], ((LongData*)(Data->Data))[i].u32[1]);
			fprintf(Txtfile, "\t\t%d:\t[%08X %08X : %d %d]\n", i + 1, ((LongData*)(Data->Data))[i].u32[0], ((LongData*)(Data->Data))[i].u32[1], ((LongData*)(Data->Data))[i].u32[0], ((LongData*)(Data->Data))[i].u32[1]);
		}
		break;
	case typecode_LongLong:
		fprintf(KeyCsvFile, "%s,%d,LongLong,%d,%d\n", Key, Data->Typecode, Data->Length, Data->LengthCalc);
		fprintf(Datafile, "Type:\t\t[LongLong]\n");
		fprintf(Txtfile, "\tType:\t\t[LongLong]\n");
		for (unsigned int i = 0; i < Data->LengthCalc; i++)
		{
			char temp1[20];
			char temp2[20];
			char temp3[20];
			time_t time = ((TimeData*)(Data->Data))[i].u32[0];
			strftime(temp1, 20, "%Y-%m-%d %H:%M:%S", localtime(&time));
			time = ((TimeData*)(Data->Data))[i].u32[1];
			strftime(temp2, 20, "%Y-%m-%d %H:%M:%S", localtime(&time));
			time = ((TimeData*)(Data->Data))[i].u32[2];
			strftime(temp3, 20, "%Y-%m-%d %H:%M:%S", localtime(&time));

			fprintf(Datafile, "\t%d:\t[%08X %08X %08X %08X : %d %d %d %d :  %s   %s   %s ]\n", i + 1, ((LongLongData*)(Data->Data))[i].u32[0], ((LongLongData*)(Data->Data))[i].u32[1], ((LongLongData*)(Data->Data))[i].u32[2], ((LongLongData*)(Data->Data))[i].u32[3], ((LongLongData*)(Data->Data))[i].u32[0], ((LongLongData*)(Data->Data))[i].u32[1], ((LongLongData*)(Data->Data))[i].u32[2], ((LongLongData*)(Data->Data))[i].u32[3], temp1, temp2, temp3);
			fprintf(Txtfile, "\t\t%d:\t[%08X %08X %08X %08X : %d %d %d %d :  %s   %s   %s ]\n", i + 1, ((LongLongData*)(Data->Data))[i].u32[0], ((LongLongData*)(Data->Data))[i].u32[1], ((LongLongData*)(Data->Data))[i].u32[2], ((LongLongData*)(Data->Data))[i].u32[3], ((LongLongData*)(Data->Data))[i].u32[0], ((LongLongData*)(Data->Data))[i].u32[1], ((LongLongData*)(Data->Data))[i].u32[2], ((LongLongData*)(Data->Data))[i].u32[3], temp1, temp2, temp3);
		}
		break;
	case typecode_Time:
		fprintf(KeyCsvFile, "%s,%d,Time,%d,%d\n", Key, Data->Typecode, Data->Length, Data->LengthCalc);
		fprintf(Datafile, "Type:\t\t[Time]\n");
		fprintf(Txtfile, "\tType:\t\t[Time]\n");
		for (unsigned int i = 0; i < Data->LengthCalc; i++)
		{
			char temp1[20];
			char temp2[20];
			char temp3[20];
			time_t time = ((TimeData*)(Data->Data))[i].u32[0];
			strftime(temp1, 20, "%Y-%m-%d %H:%M:%S", localtime(&time));
			time = ((TimeData*)(Data->Data))[i].u32[1];
			strftime(temp2, 20, "%Y-%m-%d %H:%M:%S", localtime(&time));
			time = ((TimeData*)(Data->Data))[i].u32[2];
			strftime(temp3, 20, "%Y-%m-%d %H:%M:%S", localtime(&time));

			fprintf(Datafile, "\t%d:\t[%08X %08X %08X : %d %d %d :  %s   %s   %s ]\n", i + 1, ((TimeData*)(Data->Data))[i].u32[0], ((TimeData*)(Data->Data))[i].u32[1], ((TimeData*)(Data->Data))[i].u32[2], ((TimeData*)(Data->Data))[i].u32[0], ((TimeData*)(Data->Data))[i].u32[1], ((TimeData*)(Data->Data))[i].u32[2], temp1, temp2, temp3);
			fprintf(Txtfile, "\t\t%d:\t[%08X %08X %08X : %d %d %d :  %s   %s   %s ]\n", i + 1, ((TimeData*)(Data->Data))[i].u32[0], ((TimeData*)(Data->Data))[i].u32[1], ((TimeData*)(Data->Data))[i].u32[2], ((TimeData*)(Data->Data))[i].u32[0], ((TimeData*)(Data->Data))[i].u32[1], ((TimeData*)(Data->Data))[i].u32[2], temp1, temp2, temp3);
		}
		break;
	case typecode_GUID:
		fprintf(KeyCsvFile, "%s,%d,GUID,%d,%d\n", Key, Data->Typecode, Data->Length, Data->LengthCalc);
		fprintf(Datafile, "Type:\t\t[GUID]\n");
		fprintf(Txtfile, "\tType:\t\t[GUID]\n");
		break;
	default:
		fprintf(KeyCsvFile, "%s,%d,Unknown,%d,%d\n", Key, Data->Typecode, Data->Length, Data->LengthCalc);
		fprintf(Datafile, "Type:\t\t[Unknown]\n");
		fprintf(Txtfile, "\tType:\t\t[Unknown]\n");
		break;
	}
	fprintf(Txtfile, "\n\n");

	// Close data file
	fclose(Datafile);
	InitKey(&Data);
}

/*
******************************************************************
* - function name:	AnalyzerClose()
*
* - description: 	Close the destination files
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void AnalyzerClose()
{
	// Csv file
	fclose(KeyCsvFile);
	
	// Txt file
	fclose(Txtfile);
}