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
#include <time.h>		// Required for clock_t
#include "../../../icdbDecode/src/common.h"		// Required for parseFile
#include "../../../icdbDecode/src/stringutil.h"	// Required for string manipualtion

/*
******************************************************************
* Global Defines
******************************************************************
*/
#define _CRT_SECURE_NO_DEPRECATE			// Dissable unsecure function warning in VisualStudio

/*
******************************************************************
* Function Prototypes
******************************************************************
*/
int AnalyzerOpen(char*, char*);
void AnalyzerWrite(FILE*, char*);
void AnalyzerClose(char*, char*);

/*
******************************************************************
* Global Variables
******************************************************************
*/
FILE* KeyCsvFile = NULL;
FILE* ProcessKeyCFile = NULL;
FILE* InitKeyCFile = NULL;
FILE* KeyCFile = NULL;

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
	char* name = NULL;
	unsigned int nameLen = strlen(argv[1]) + 1;
	
	// Check parameter
	if(argc < 2)
	{
		printf("No source file specified!\n");
		return -1;
	}
	printf("Using source file %s\n", argv[1]);

	name = malloc(nameLen);
	if(name == NULL)
	{
		return -1;
	}
	memcpy(name, argv[1], nameLen);

	// Get filename without path
	char* nameTemp;
	removeFilenameExtension(name, &nameLen);
	nameLen = removeFilePath(name, nameLen, &nameTemp);
	char* nameSmall = stringSmall(nameTemp, nameLen);
	char* nameBig = stringBig(nameTemp, nameLen);

	if(AnalyzerOpen(nameBig, nameSmall))
	{
		return -1;
	}
	if (parseFile(NULL, 0, argv[1], strlen(argv[1]), AnalyzerWrite))
	{
		return -1;
	}
	
	AnalyzerClose(nameBig, nameSmall);
	free(name);
	free(nameBig);
	free(nameSmall);
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
* - parameter: 		Name of file, lenth of name
*
* - return value: 	0 if file opened sucessfully
******************************************************************
*/
int AnalyzerOpen(char* nameBig, char* nameSmall)
{
	// Open files
	KeyCsvFile = fopen("Keys.csv", "w");
	ProcessKeyCFile = fopen("ProcessKey.c", "w");
	InitKeyCFile = fopen("InitKey.c", "w");
	KeyCFile = fopen("Key.c", "w");
	if (KeyCsvFile == NULL || ProcessKeyCFile == NULL || InitKeyCFile == NULL || KeyCFile == NULL)
	{
		return -1;
	}

	// Write file headers
	fprintf(KeyCsvFile, "Name,TypeCode\n");

	fprintf(ProcessKeyCFile, "/*\n");
	fprintf(ProcessKeyCFile, "******************************************************************\n");
	fprintf(ProcessKeyCFile, "* - function name:	ProcessKey%s()\n", nameBig);
	fprintf(ProcessKeyCFile, "*\n");
	fprintf(ProcessKeyCFile, "* - description: 	Check for %s keys\n", nameSmall);
	fprintf(ProcessKeyCFile, "*\n");
	fprintf(ProcessKeyCFile, "* - parameter: 		file pointer; Key to check\n");
	fprintf(ProcessKeyCFile, "*\n");
	fprintf(ProcessKeyCFile, "* - return value: 	-\n");
	fprintf(ProcessKeyCFile, "******************************************************************\n");
	fprintf(ProcessKeyCFile, "*/\n");
	fprintf(ProcessKeyCFile, "void ProcessKey%s(FILE * sourceFile, char* Name)\n", nameBig);
	fprintf(ProcessKeyCFile, "{\n");
	fprintf(ProcessKeyCFile, "\t");

	fprintf(InitKeyCFile, "/*\n");
	fprintf(InitKeyCFile, "******************************************************************\n");
	fprintf(InitKeyCFile, "* - function name:	Init%s()\n", nameBig);
	fprintf(InitKeyCFile, "*\n");
	fprintf(InitKeyCFile, "* - description: 	Resets all %s data\n", nameSmall);
	fprintf(InitKeyCFile, "*\n");
	fprintf(InitKeyCFile, "* - parameter: 		-\n");
	fprintf(InitKeyCFile, "*\n");
	fprintf(InitKeyCFile, "* - return value: 	-\n");
	fprintf(InitKeyCFile, "******************************************************************\n");
	fprintf(InitKeyCFile, "*/\n");
	fprintf(InitKeyCFile, "void Init%s(void)\n", nameBig);
	fprintf(InitKeyCFile, "{\n");

	fprintf(KeyCFile, "// %s\n", nameBig);
	return 0;
}

/*
******************************************************************
* - function name:	AnalyzerWrite()
*
* - description: 	Stores keys to file
*
* - parameter: 		file pointer; Key
*
* - return value: 	-
******************************************************************
*/
void AnalyzerWrite(FILE* sourceFile, char* Name)
{
	uint32_t Type = 0;
	fseek(sourceFile, (int)sizeof(uint32_t) * -1, SEEK_CUR);
	fread(&Type, sizeof(uint32_t), 1, sourceFile);

	// Write data
	fprintf(KeyCsvFile, "%s,%d\n", Name, Type);
	fprintf(ProcessKeyCFile, "if (strcmp(Name, %c%s%c) == 0)\n\t{\n\t\t%s = ParseKey(sourceFile);\n\t}\n\telse ", '"', Name, '"', Name);
	fprintf(InitKeyCFile, "\tInitKey(&%s);\n", Name);
	fprintf(KeyCFile, "key_struct* %s = NULL;\n", Name);
}

/*
******************************************************************
* - function name:	AnalyzerClose()
*
* - description: 	Close the destination files
*
* - parameter: 		Name of file
*
* - return value: 	-
******************************************************************
*/
void AnalyzerClose(char* nameBig, char* nameSmall)
{
	// Write file leader
	fprintf(ProcessKeyCFile, "\n\t{\n");
	fprintf(ProcessKeyCFile, "\t\tmyPrint(%cUnknown Key in %s [%cs]\\n%c, Name);\n", '"', nameSmall, '%', '"');
	fprintf(ProcessKeyCFile, "\t}\n");
	fprintf(ProcessKeyCFile, "}\n");

	fprintf(InitKeyCFile, "}\n");
	
	// Close files
	fclose(KeyCsvFile);
	fclose(ProcessKeyCFile);
	fclose(InitKeyCFile);
	fclose(KeyCFile);
}


