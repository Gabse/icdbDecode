/*
******************************************************************
* Info
******************************************************************
* iCDBanalyzer
* 
* This tool can be used to extract and format keys and typecodes from .v files, found in SiemensEDA (former Mentor Graphics) icdb.dat databses.
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
#include <string.h>		// Required for memcpy
#include "../../../icdbDecode/src/common.h"		// Required for parseFile
#include "../../../icdbDecode/src/stringutil.h"	// Required for string manipulation
#include "../../../icdbDecode/src/list.h"		// Required for list

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
FILE* Cfile = NULL;
FILE* Hfile = NULL;
FILE* Txtfile = NULL;
void* list = NULL;
char* name = NULL;
char* nameSmall = NULL;
char* nameBig = NULL;
char* nameAllBig = NULL;
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
	nameLen = strlen(argv[1]) + 1;
	
	// Check parameter
	if(argc < 2)
	{
		printf("No source file specified!\n");
		return -1;
	}
	printf("Using source file %s\n", argv[1]);

	nameTemp = malloc(nameLen);
	if(nameTemp == NULL)
	{
		return -1;
	}
	memcpy(nameTemp, argv[1], nameLen);

	// Get filename without path
	removeFilenameExtension(nameTemp, &nameLen);
	nameLen = removeFilePath(nameTemp, nameLen, &name);
	nameSmall = stringAllSmall(name, nameLen);
	nameBig = stringBig(nameSmall, nameLen);
	nameAllBig = stringAllBig(nameSmall, nameLen);
	PathLenTemp = addStrings(&PathTemp, name, nameLen, "data", sizeof("data"), '\\');

	if(AnalyzerOpen())
	{
		return -1;
	}
	if (parseFile(NULL, 0, argv[1], strlen(argv[1]), AnalyzerWrite))
	{
		return -1;
	}
	
	AnalyzerClose();
	free(nameTemp); // also clears name
	free(nameSmall);
	free(nameBig);
	free(nameAllBig);
	free(PathTemp);
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

	// C file
	NameLenTemp = addStrings(&NameTemp, nameSmall, nameLen, ".c", sizeof(".c"), '\0');
	createPath(&FullnameTemp, name, nameLen, NameTemp, NameLenTemp, '\\');
	Cfile = fopen(FullnameTemp, "w");
	free(FullnameTemp);
	free(NameTemp);

	// H file
	NameLenTemp = addStrings(&NameTemp, nameSmall, nameLen, ".h", sizeof(".h"), '\0');
	createPath(&FullnameTemp, name, nameLen, NameTemp, NameLenTemp, '\\');
	Hfile = fopen(FullnameTemp, "w");
	free(FullnameTemp);
	free(NameTemp);

	// Data file
	NameLenTemp = addStrings(&NameTemp, nameSmall, nameLen, ".txt", sizeof(".txt"), '\0');
	createPath(&FullnameTemp, name, nameLen, NameTemp, NameLenTemp, '\\');
	Txtfile = fopen(FullnameTemp, "w");
	free(FullnameTemp);
	free(NameTemp);

	if (KeyCsvFile == NULL || Cfile == NULL || Hfile == NULL || Txtfile == NULL)
	{
		return -1;
	}

	// Csv file
	fprintf(KeyCsvFile, "Key,TypeCode,TypeCodeText,RawSize,CalcSize\n");

	// C file
	fprintf(Cfile, "/*\n");
	fprintf(Cfile, "******************************************************************\n");
	fprintf(Cfile, "* Info\n");
	fprintf(Cfile, "******************************************************************\n");
	fprintf(Cfile, "* iCDBdecode\n");
	fprintf(Cfile, "*\n");
	fprintf(Cfile, "* This tool can be used to analyze and decompress Siemens EDA (former Mentor Graphics) icdb.dat files.\n");
	fprintf(Cfile, "* It's intend is to gain understanding of the file format, in order to allow interoperability with other EDA packages.\n");
	fprintf(Cfile, "*\n");
	fprintf(Cfile, "* The tool is based on initial research done by Patrick Yeon (https://github.com/patrickyeon/icdb2fs) in 2011.\n");
	fprintf(Cfile, "* The research was performed by analyzing various icdb.dat files (basically staring at the hex editor for hours),\n");
	fprintf(Cfile, "* No static or dynamic code analysis of any proprietary executable files was used to gain information’s about the file format.\n");
	fprintf(Cfile, "*\n");
	fprintf(Cfile, "* This project uses the Zlib library (https://www.zlib.net/) for decompression.\n");
	fprintf(Cfile, "*/\n");
	fprintf(Cfile, "\n");
	fprintf(Cfile, "/*\n");
	fprintf(Cfile, "******************************************************************\n");
	fprintf(Cfile, "* Includes\n");
	fprintf(Cfile, "******************************************************************\n");
	fprintf(Cfile, "*/\n");
	fprintf(Cfile, "#include \"%s.h\"\n", nameSmall);
	fprintf(Cfile, "#include <stdint.h>					// Required for int32_t, uint32_t, ...\n");
	fprintf(Cfile, "#include <string.h>					// Required for strcmp\n");
	fprintf(Cfile, "#include \"../common.h\"				// Required for parseFile\n");
	fprintf(Cfile, "\n");
	fprintf(Cfile, "/*\n");
	fprintf(Cfile, "******************************************************************\n");
	fprintf(Cfile, "* Global Variables\n");
	fprintf(Cfile, "******************************************************************\n");
	fprintf(Cfile, "*/\n");

	// H file
	fprintf(Hfile, "/*\n");
	fprintf(Hfile, "******************************************************************\n");
	fprintf(Hfile, "* Info\n");
	fprintf(Hfile, "******************************************************************\n");
	fprintf(Hfile, "* iCDBdecode\n");
	fprintf(Hfile, "*\n");
	fprintf(Hfile, "* This tool can be used to analyze and decompress Siemens EDA (former Mentor Graphics) icdb.dat files.\n");
	fprintf(Hfile, "* It's intend is to gain understanding of the file format, in order to allow interoperability with other EDA packages.\n");
	fprintf(Hfile, "*\n");
	fprintf(Hfile, "* The tool is based on initial research done by Patrick Yeon (https://github.com/patrickyeon/icdb2fs) in 2011.\n");
	fprintf(Hfile, "* The research was performed by analyzing various icdb.dat files (basically staring at the hex editor for hours),\n");
	fprintf(Hfile, "* No static or dynamic code analysis of any proprietary executable files was used to gain information’s about the file format.\n");
	fprintf(Hfile, "*\n");
	fprintf(Hfile, "* This project uses the Zlib library (https://www.zlib.net/) for decompression.\n");
	fprintf(Hfile, "*/\n");
	fprintf(Hfile, "#ifndef _%s_H\n", nameAllBig);
	fprintf(Hfile, "#define _%s_H\n", nameAllBig);
	fprintf(Hfile, "\n");
	fprintf(Hfile, "/*\n");
	fprintf(Hfile, "******************************************************************\n");
	fprintf(Hfile, "* Global Includes\n");
	fprintf(Hfile, "******************************************************************\n");
	fprintf(Hfile, "*/\n");
	fprintf(Hfile, "#include <stdio.h>		// Required for FILE\n");
	fprintf(Hfile, "#include \"../common.h\"	// Required for key_struct\n");
	fprintf(Hfile, "\n");
	fprintf(Hfile, "/*\n");
	fprintf(Hfile, "******************************************************************\n");
	fprintf(Hfile, "* Global Defines\n");
	fprintf(Hfile, "******************************************************************\n");
	fprintf(Hfile, "*/\n");
	fprintf(Hfile, "#define PATH_%s \"%s.v\"\n", nameAllBig, nameSmall);
	fprintf(Hfile, "\n");
	fprintf(Hfile, "/*\n");
	fprintf(Hfile, "******************************************************************\n");
	fprintf(Hfile, "* Global Variables\n");
	fprintf(Hfile, "******************************************************************\n");
	fprintf(Hfile, "*/\n");
	fprintf(Hfile, "// %s\n", nameBig);

	list = list_init();
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
	fprintf(Cfile, "key_struct* %s_%s = NULL;\n", nameBig, Key);
	fprintf(Hfile, "extern key_struct* %s_%s;\n", nameBig, Key);
	fprintf(Txtfile, "\n\n");

	unsigned int temp = 0;

	// Append Names to list
	list_append(list, Key, KeyLen + 1);

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
	unsigned int numNames = list_elements(list) - 1;
	char* nameTemp = NULL;

	// Csv file
	fclose(KeyCsvFile);

	// C file
	fprintf(Cfile, "\n");
	fprintf(Cfile, "/*\n");
	fprintf(Cfile, "******************************************************************\n");
	fprintf(Cfile, "* Global Functions\n");
	fprintf(Cfile, "******************************************************************\n");
	fprintf(Cfile, "*/\n");
	fprintf(Cfile, "/*\n");
	fprintf(Cfile, "******************************************************************\n");
	fprintf(Cfile, "* - function name:	ProcessKey%s()\n", nameBig);
	fprintf(Cfile, "*\n");
	fprintf(Cfile, "* - description: 	Check for %s keys\n", nameSmall);
	fprintf(Cfile, "*\n");
	fprintf(Cfile, "* - parameter: 		file pointer; Key to check; length of key\n");
	fprintf(Cfile, "*\n");
	fprintf(Cfile, "* - return value: 	-\n");
	fprintf(Cfile, "******************************************************************\n");
	fprintf(Cfile, "*/\n");
	fprintf(Cfile, "void ProcessKey%s(FILE * sourceFile, char* Key, unsigned int KeyLen)\n", nameBig);
	fprintf(Cfile, "{\n");
	fprintf(Cfile, "\t");
	for (unsigned int i = 0; i < numNames; i++)
	{
		list_get(list, i, (void**)&nameTemp);
		fprintf(Cfile, "if (strcmp(Key, \"%s\") == 0)\n\t{\n\t\t%s_%s = ParseKey(sourceFile);\n\t}\n\telse ", nameTemp, nameBig, nameTemp);
	}
	fprintf(Cfile, "\n\t{\n");
	fprintf(Cfile, "\t\tmyPrint(%cUnknown Key in %s [%cs]\\n%c, Key);\n", '"', name, '%', '"');
	fprintf(Cfile, "\t}\n");
	fprintf(Cfile, "}\n");
	fprintf(Cfile, "\n");
	fprintf(Cfile, "/*\n");
	fprintf(Cfile, "******************************************************************\n");
	fprintf(Cfile, "* - function name:	Init%s()\n", nameBig);
	fprintf(Cfile, "*\n");
	fprintf(Cfile, "* - description: 	Resets all %s data\n", nameSmall);
	fprintf(Cfile, "*\n");
	fprintf(Cfile, "* - parameter: 		-\n");
	fprintf(Cfile, "*\n");
	fprintf(Cfile, "* - return value: 	-\n");
	fprintf(Cfile, "******************************************************************\n");
	fprintf(Cfile, "*/\n");
	fprintf(Cfile, "void Init%s(void)\n", nameBig);
	fprintf(Cfile, "{\n");
	for (unsigned int i = 0; i < numNames; i++)
	{
		list_get(list, i, (void**)&nameTemp);
		fprintf(Cfile, "\tInitKey(&%s_%s);\n", nameBig, nameTemp);
	}

	fprintf(Cfile, "}");
	fclose(Cfile);

	// H file
	fprintf(Hfile, "\n");
	fprintf(Hfile, "/*\n");
	fprintf(Hfile, "******************************************************************\n");
	fprintf(Hfile, "* Global Functions\n");
	fprintf(Hfile, "******************************************************************\n");
	fprintf(Hfile, "*/\n");
	fprintf(Hfile, "extern void ProcessKey%s(FILE*, char*, unsigned int);\n", nameBig);
	fprintf(Hfile, "extern void Init%s(void);\n", nameBig);
	fprintf(Hfile, "\n");
	fprintf(Hfile, "#endif //_%s_H", nameAllBig);
	fclose(Hfile);

	// Txt file
	fclose(Txtfile);
	
	list_cleanup(&list);
}