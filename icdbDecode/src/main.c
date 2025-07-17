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
#include <stdlib.h>			// Required for malloc
#include <string.h>			// Required for memcpy
#include <stdio.h>			// Required for sprintf
#include <time.h>			// Required for clock_t
#include "log.h"			// Required for logging functions
#include "unpack.h"			// Required for UnpackIcdb
#include "stringutil.h"		// Required for removeFilenameExtension
#include "parser.h"			// Required for ParseIcdb

/*
******************************************************************
* Global Defines
******************************************************************
*/
#define DEFAULT_SOURCE "icdb.dat"
#define _CRT_SECURE_NO_DEPRECATE			// Disable insecure function warning in VisualStudio

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
	int error = 0;
	int filepathLength = 0;
	char* filepath = NULL;
	int storepathLength = 0;
	char* storepath = NULL;

	// Check parameter
	for (int i = 0; i < argc; ++i)
	{
		if ((strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "-S") == 0) && filepathLength == 0 && argc >= i + 1)
		{	// Source
			filepathLength = strlen(argv[i + 1]) + 1;
			filepath = calloc(filepathLength, sizeof(char));
			memcpy(filepath, argv[i + 1], filepathLength);
			printf("Using source path: \t[%s]\n", filepath);
		}
		else if ((strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "-D") == 0) && storepathLength == 0 && argc >= i + 1)
		{	// Database
			storepathLength = strlen(argv[i + 1]) + 1;
			storepath = calloc(storepathLength, sizeof(char));
			memcpy(storepath, argv[i + 1], storepathLength);
			printf("Using storing path: \t[%s]\n", storepath);
		}
		else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "-C") == 0)
		{
			nontDecompress = 1;
		}
		else if (strcmp(argv[i], "-q") == 0 || strcmp(argv[i], "-Q") == 0)
		{
			quietMode = 1;
		}
		else if ((strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "-L") == 0) && noLongLongFiles == 0)
		{
			linkLongLongFiles = 1;
		}
		else if ((strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "-N") == 0) && linkLongLongFiles == 0)
		{	
			noLongLongFiles = 1;
		}
		else if ((strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "-H") == 0))
		{
			// Print Help
			printf("********** iCDBdecode **********\n\n");
			printf("Tool to decode Siemens EDA (former Mentor Graphics) icdb.dat files\n");
			printf("Usage:\n");
			printf("Use parameter -s to specify the source file\n");
			printf("Use parameter -d to specify the destination path\n");
			printf("Use parameter -c to not decompress files\n");
			printf("Use parameter -l to link double files\n");
			printf("Use parameter -n to skip double files\n");
			printf("Use parameter -q for quiet mode (faster)\n");
			printf("Use parameter -h for this help\n\n");
			printf("This project uses the Zlib library (https://www.zlib.net/) for decompression.\n\n");
			printf("********************************\n\n");
		}
	}
	if (filepathLength == 0) // No argument specified
	{
		printf("No source specified. Using default path.\n");
		filepathLength = sizeof(DEFAULT_SOURCE);
		filepath = calloc(filepathLength, sizeof(char));
		if (filepath != 0)
		{
			memcpy(filepath, DEFAULT_SOURCE, filepathLength);
		}
	}
	if (storepathLength == 0) // No argument specified
	{
		printf("No destination specified. Using source path.\n");
		storepathLength = filepathLength;
		storepath = calloc(storepathLength, sizeof(char));
		if (filepath != 0 && storepath != 0)
		{
			memcpy(storepath, filepath, storepathLength);
		}
	}
	
	// Remove .dat ending from database
	removeFilenameExtension(storepath, &storepathLength);

	// Create Log file
	CreateLogfile(storepath, storepathLength);
	
	int quietModeTemp = quietMode;
	quietMode = 0;
	if (nontDecompress)
	{
		myPrint("Decompression disabled\n");
	}
	if (quietModeTemp)
	{
		myPrint("Quiet mode enabled\n");
	}
	if (linkLongLongFiles)
	{
		myPrint("Linking double files\n");
	}
	if (noLongLongFiles)
	{	
		myPrint("Skipping double files\n");
	}
	myPrint("\n");
	quietMode = quietModeTemp;

	if(quietMode == 1)
	{
		printf("Working, please wait...\n\n");
	}

	error = UnpackIcdb(filepath, filepathLength, storepath, storepathLength);
	if (error == 0)
	{
		myPrint("\n\n************************ Parsing Database ************************\n\n");
		if (nontDecompress == 0 && noLongLongFiles == 0)
		{
			error = ParseIcdb(storepath, storepathLength);
		}
		else
		{
			myPrint("Parsing disabled!\n\n");
		}
	}
	quietMode = 0;
	myPrint("Finnish after %fs\n", (float)(clock() - starttime)/(float) CLOCKS_PER_SEC);
	void CloseLogfile(void);
	return error;
}
