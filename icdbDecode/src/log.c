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
#include "log.h"
#include <stdio.h>		// Required for fprint, fopen, ...
#include <stdarg.h>		// Required for va_list
#include "common.h"		// Required for myfopen



/*
******************************************************************
* Global Defines
******************************************************************
*/
#define LOGFILE_NAME "\\icdbDecode.log"
#define _CRT_SECURE_NO_DEPRECATE			// Disable insecure function warning in VisualStudio


/*
******************************************************************
* Global Variables
******************************************************************
*/
FILE* logFile = NULL;
int quietMode = 0;

/*
******************************************************************
* - function name:	myPrint()
*
* - description: 	prints to logfile & to terminal, if quiet mode is deactivated
*
* - parameter: 		text to print
*
* - return value: 	-
******************************************************************
*/
void myPrint(const char* text, ...)
{
	// Print to terminal
	va_list args;
	va_start(args, text);
	if (quietMode == 0)
	{
		vfprintf(stdout, text, args);
		fflush(stdout);
	}
	va_end(args);

	// Print to log file
	va_start(args, text);
	if (logFile != 0)
	{
		vfprintf(logFile, text, args);
	}
	va_end(args);
}

/*
******************************************************************
* - function name:	CreateLogfile()
*
* - description: 	creates a log file
*
* - parameter: 		text to print
*
* - return value: 	-
******************************************************************
*/
void CreateLogfile(char* storepath, int storepathLength)
{
	//Create log file path
	logFile = myfopen("w", storepath, storepathLength, LOGFILE_NAME, sizeof(LOGFILE_NAME), '\0');
	if (logFile == 0)
	{
		printf("Logfile not written! \n");
	}
	else
	{
		printf("Writing Logfile! \n");
	}
}
/*
******************************************************************
* - function name:	CloseLogfile()
*
* - description: 	closes the log file
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void CloseLogfile(void)
{
	if (logFile != 0)
	{
		quietMode = 0;
		printf("\n%s Written\n", LOGFILE_NAME);
		fclose(logFile);
	}
}