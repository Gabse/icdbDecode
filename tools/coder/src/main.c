/*
******************************************************************
* Info
******************************************************************
* iCDBanalyzer
* 
* This tool can be used to generate code to parse keys from .v files, found in SiemensEDA (former Mentor Graphics) icdb.dat databses.
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
FILE* Cfile = NULL;
FILE* Hfile = NULL;
void* list = NULL;
char* name = NULL;
char* nameSmall = NULL;
char* nameBig = NULL;
char* nameAllBig = NULL;
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
	string_struct* names = NULL;
	unsigned int numNames = 0;

	// Check parameter
	if (argc < 2)
	{
		printf("No source file specified!\n");
		return -1;
	}
	else if (argc == 2) // One file
	{
		printf("Using source file %s\n", argv[1]);
	}
	else // Multiple files
	{
		printf("Using source files: %s", argv[1]);
		for (unsigned int i = 2; i < argc; i++)
		{
			printf(", %s", argv[i]);
		}
		printf("\n");
	}
	numNames = argc - 1;

	names = calloc(numNames, sizeof(string_struct));
	if (names == NULL)
	{
		return -1;
	}
	for (unsigned int i = 0; i < numNames; i++)
	{
		char* nameTemp = NULL;
		char* nameTemp2 = NULL;

		nameLen = strlen(argv[i + 1]) + 1;
		nameTemp = malloc(nameLen);
		if (nameTemp == NULL)
		{
			return -1;
		}
		memcpy(nameTemp, argv[i + 1], nameLen);

		// Get filename without path
		removeFilenameExtension(nameTemp, &nameLen);
		(names[i]).Length = removeFilePath(nameTemp, nameLen, &nameTemp2);
		(names[i]).Text = malloc((names[i]).Length);
		if ((names[i]).Text == NULL)
		{
			return -1;
		}
		memcpy((names[i]).Text, nameTemp2, (names[i]).Length);
		free(nameTemp); // also clears name
	}

	for (unsigned int i = 0; i < numNames; i++)
	{
		if ((names[i]).Length >= 1) // Check if file is already parsed
		{
			name = (names[i]).Text;
			nameLen = (names[i]).Length;
			nameSmall = stringAllSmall(name, nameLen);
			nameBig = stringBig(nameSmall, nameLen);
			nameAllBig = stringAllBig(nameSmall, nameLen);

			if (AnalyzerOpen())
			{
				printf("Error opening destination file!");
				return -1;
			}

			// Check for identical file names 
			for (unsigned int j = i; j < ((argc - 1)); j++)
			{
				// Check if file is already parsed and name match
				if ((names[j]).Length != 0 && (strcmp((names[i]).Text, (names[j]).Text) == 0))
				{
					if (parseFile(NULL, 0, argv[j + 1], strlen(argv[j + 1]), AnalyzerWrite))
					{
						printf("Error opening source file!");
						return -1;
					}
					(names[j]).Length = 0;
				}
			}
			free((names[i]).Text);
			(names[i]).Text = NULL;
			AnalyzerClose();
			free(nameSmall);
			free(nameBig);
			free(nameAllBig);
		}
	}
	free(names);
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
	
	if (Cfile == NULL || Hfile == NULL)
	{
		return -1;
	}

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
	fprintf(Cfile, "* No static or dynamic code analysis of any proprietary executable files was used to gain information about the file format.\n");
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
	fprintf(Hfile, "* No static or dynamic code analysis of any proprietary executable files was used to gain information about the file format.\n");
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
	unsigned int numNames = list_elements(list) - 1;
	char* nameTemp = NULL;

	for (unsigned int i = 0; i < numNames; i++)
	{
		// Check for duplicates
		nameLen = list_get(list, i, (void**)&nameTemp);
		int comp = strcmp(nameTemp, Key);
		if (comp == 0) // Key already in list
		{
			return;
		}
		else if (comp > 0) // Append keys to list in alphabetic order
		{
			list_add(list, i, Key, KeyLen + 1);
			return;
		}
	}
	// Append Names to list
	list_append(list, Key, KeyLen + 1);
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
	
	for (unsigned int i = 0; i < numNames; i++)
	{
		list_get(list, i, (void**)&nameTemp);
		fprintf(Cfile, "key_struct* %s_%s = NULL;\n", nameBig, nameTemp);
		fprintf(Hfile, "extern key_struct* %s_%s;\n", nameBig, nameTemp);
	}

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
	
	list_cleanup(&list);
}