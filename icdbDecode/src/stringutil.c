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
#include "stringutil.h"
#include <stdlib.h>		// Required for calloc to work properly
#include <string.h>		// Required for memcpy
#include <stdint.h>		// Required for int32_t, uint32_t, ...
#include <sys/stat.h>	// Required for mkdir
#include "common.h"		// Required for DIR_SEPARATOR

/*
******************************************************************
* - function name: assemblePath()
*
* - description:  Assemble file path
*
* - parameter:  pointer to string pointer of both texts united, string pointer to first part; length of first part; string pointer to second part; length of second part; separator character
*
* - return value: complete path length
******************************************************************
*/
uint32_t assemblePath(char** destination, char* leftstring, uint32_t leftstringlength, char* rightstring, uint32_t rightstringlength, char seperator)
{
	uint32_t completePathLength = addStrings(destination, leftstring, leftstringlength, rightstring, rightstringlength, seperator);
	// replace slash/backslash
	for (uint32_t i = 0; i < completePathLength; i++)
	{
		if ((*destination)[i] == DIR_SEPARATOR_WINDOWS || (*destination)[i] == DIR_SEPARATOR_UNIX)
		{
			(*destination)[i] = DIR_SEPARATOR; // Replace Slash / Backslash
		}
	}
	return completePathLength;
}

/*
******************************************************************
* - function name: createPath()
*
* - description:  Assemble file path & create folder structure
*
* - parameter:  pointer to string pointer of both texts united, string pointer to first part; length of first part; string pointer to second part; length of second part; separator character
*
* - return value: complete path length
******************************************************************
*/
uint32_t createPath(char** destination, char* leftstring, uint32_t leftstringlength, char* rightstring, uint32_t rightstringlength, char seperator)
{
	uint32_t completePathLength = addStrings(destination, leftstring, leftstringlength, rightstring, rightstringlength, seperator);
	// replace slash/backslash
	for (uint32_t i = 0; i < completePathLength; i++)
	{
		if ((*destination)[i] == DIR_SEPARATOR_WINDOWS || (*destination)[i] == DIR_SEPARATOR_UNIX)
		{
			(*destination)[i] = '\0'; // Zero termintate string for mkdir command
			#ifdef WIN32 // Building for Windows
				mkdir(*destination);
			#else // Building for Unix
				mkdir(*destination, 0777);
			#endif
			(*destination)[i] = DIR_SEPARATOR; // Replace Slash / Backslash
		}
	}	
	return completePathLength;
}

/*
******************************************************************
* - function name: removeFilenameExtension()
*
* - description:  removes the file ending if present
*
* - parameter:  pointer to string, pointer to length of string
*
* - return value: -
******************************************************************
*/
void removeFilenameExtension(char* string, uint32_t* length)
{
	if (string != 0)
	{
		// Remove ending from database path if present
		for (uint32_t i = 0; i < *length; i++)
		{
			if (string[*length-i] == '.')
			{
				string[*length - i] = '\0'; // Zero terminate string
				*length += 1 - i; // String length
				break;
			}
			else if (string[*length - i] == DIR_SEPARATOR_WINDOWS || string[*length - i] == DIR_SEPARATOR_UNIX) // No ending
			{
				break;
			}
		}
	}
}

/*
******************************************************************
* - function name: removeFilePath()
*
* - description:  separates the filename from the filepath
*
* - parameter:  pointer to string, pointer to length of string, pointer to destination
*
* - return value: length of filename
******************************************************************
*/
unsigned int removeFilePath(char* input, unsigned int length, char** output)
{
	unsigned int nameLen = length;
	*output = input;
	for (unsigned int i = 0; i < length; i++)
	{
		if (input[length - i] == '\\' || input[length - i] == '/')
		{
			nameLen = i - 1;
			*output = &(input[length - nameLen]);
			break;
		}
	}
	return nameLen;
}

/*
******************************************************************
* - function name: addStrings()
*
* - description:  adds two strings together
*
* - parameter:  pointer to string pointer of both texts united, string pointer of first part; length of first part; string pointer of second part; length of second part; separator character
*
* - return value: complete path length
******************************************************************
*/
uint32_t addStrings(char** destination, char* leftstring, uint32_t leftstringlength, char* rightstring, uint32_t rightstringlength, char seperator)
{
	// Handle if one of the parameter is zero
	uint32_t completePathLength;
	if ((leftstring == NULL || leftstringlength) <= 0 && (rightstring == NULL || rightstringlength <= 0))
	{
		*destination = NULL;
		return 0;
	}
	else if (leftstring == NULL || leftstringlength <= 0)
	{
		completePathLength = rightstringlength;
		*destination = (char*)calloc(completePathLength + 1, sizeof(char));
		if (*destination == 0)
		{
			return 0;
		}
		memcpy((char*)*destination, rightstring, completePathLength);
		(*destination)[completePathLength] = '\0'; // Zero terminate string
	}
	else if (rightstring == NULL || rightstringlength <= 0)
	{
		completePathLength = leftstringlength;
		*destination = (char*)calloc(completePathLength + 1, sizeof(char));
		if (*destination == 0)
		{
			return 0;
		}
		memcpy((char*)*destination, leftstring, completePathLength);
		(*destination)[completePathLength] = '\0'; // Zero terminate string
	}
	else
	{
		if (leftstring[leftstringlength - 1] == '\0')
		{
			leftstringlength -= 1; // Remove zero termination	
		}
		if (rightstring[rightstringlength - 1] != '\0')
		{
			rightstringlength++; // Add space for zero termination if not already present
		}
		// Buffer for complete path
		completePathLength = rightstringlength + leftstringlength;
		if (seperator != '\0')
		{
			completePathLength++; // add space for seperator
		}
		*destination = (char*)calloc(completePathLength, sizeof(char));
		if (*destination != 0)
		{
			// Unite the path
			memcpy((char*)*destination, leftstring, leftstringlength);
			if (seperator != '\0')
			{
				leftstringlength++; // add seperator
				(*destination)[leftstringlength - 1] = seperator;
			}
			memcpy((char*)*destination + leftstringlength, rightstring, rightstringlength);
			completePathLength--; // Remove zero termination
			(*destination)[completePathLength] = '\0'; // Zero terminate string
		}
	}
	return completePathLength;
}

/*
******************************************************************
* - function name: stringSmall()
*
* - description:  make first character lower case
*
* - parameter:  string pointer text; length of text
*
* - return value: char pointer
******************************************************************
*/
char* stringSmall(char* name, unsigned int nameLen)
{
	// make first character lower case
	char* output = malloc(nameLen);
	if (output == NULL)
	{
		return NULL;
	}
	memcpy(output, name, nameLen);
	if (output[0] >= 'A' && output[0] <= 'Z')
	{
		output[0] = output[0] + 'a' - 'A';
	}
	return output;
}

/*
******************************************************************
* - function name: stringBig()
*
* - description:  make first character upper case
*
* - parameter:  string pointer text; length of text
*
* - return value: char pointer
******************************************************************
*/
char* stringBig(char* name, unsigned int nameLen)
{
	char* output = malloc(nameLen);
	if (output == NULL)
	{
		return NULL;
	}
	memcpy(output, name, nameLen);
	if (output[0] >= 'a' && output[0] <= 'z')
	{
		output[0] = output[0] + 'A' - 'a';
	}
	return output;
}

/*
******************************************************************
* - function name: stringAllBig()
*
* - description:  make all character upper case
*
* - parameter:  string pointer text; length of text
*
* - return value: char pointer
******************************************************************
*/
char* stringAllBig(char* name, unsigned int nameLen)
{
	char* output = malloc(nameLen);
	if (output == NULL)
	{
		return NULL;
	}
	for (unsigned int i = 0; i < nameLen; i++)
	{
		if (name[i] >= 'a' && name[i] <= 'z')
		{
			output[i] = name[i] + 'A' - 'a';
		}
		else
		{
			output[i] = name[i];
		}
	}
	return output;
}

/*
******************************************************************
* - function name: stringAllSmall()
*
* - description:  make all character upper case
*
* - parameter:  string pointer text; length of text
*
* - return value: char pointer
******************************************************************
*/
char* stringAllSmall(char* name, unsigned int nameLen)
{
	char* output = malloc(nameLen);
	if (output == NULL)
	{
		return NULL;
	}
	for (unsigned int i = 0; i < nameLen; i++)
	{
		if (name[i] >= 'A' && name[i] <= 'Z')
		{
			output[i] = name[i] + 'a' - 'A';
		}
		else
		{
			output[i] = name[i];
		}
	}
	return output;
}

/*
******************************************************************
* - function name: stringLen()
*
* - description:  counts the length of a string until the null character
*
* - parameter:  string pointer text; length of text
*
* - return value: counted length
******************************************************************
*/
unsigned int stringLen(char* name, unsigned int nameLen)
{
	for (unsigned int i = 0; i < nameLen; i++)
	{
		if (name[i] == '\0')
		{
			return i;
		}
	}
	return nameLen;
}