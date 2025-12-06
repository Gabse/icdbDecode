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
#include "common.h"
#include <stdio.h>		// Required for fprint, fopen, ...
#include <stdlib.h>		// Required for calloc to work properly
#include <stdint.h>		// Required for int32_t, uint32_t, ...
#include <string.h>		// Required for strcmp
#include "stringutil.h"	// Required for assemblePath
#include <math.h>		// Required for fabs

/*
******************************************************************
* Defines
******************************************************************
*/
#ifdef B64Bit
	#pragma message ("building 64bit application")
#else
	#pragma message ("building 32bit application")
#endif

/*
******************************************************************
* Function Prototypes 
******************************************************************
*/
string_struct* ParseString(FILE*, int32_t, uint32_t*);
int_array_struct* ParseIntArray(FILE*, int32_t, uint32_t*);
void* ParseInt(FILE*, uint32_t, uint32_t);

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	myfopen()
*
* - description: 	Create source path & open/create file
*
* - parameter: 		fopen parameter; source path string; length of sourcepath
*
* - return value: 	filepointer
******************************************************************
*/
FILE* myfopen(char* option, char* leftstring, uint32_t leftstringlength, char* rightstring, uint32_t rightstringlength, char seperator)
{
	FILE* returnpath = NULL;
	char* Path = NULL;

	if(strcmp(option, "r") == 0 || strcmp(option, "rb") == 0) //Read operation
	{
		assemblePath(&Path, leftstring, leftstringlength, rightstring, rightstringlength, seperator);
	}
	else // Write operatione
	{	
		createPath(&Path, leftstring, leftstringlength, rightstring, rightstringlength, seperator);
	}
	if (Path != NULL)
	{
		returnpath = fopen(Path, option);
		free(Path);
	}
	return returnpath;
}

/*
******************************************************************
* - function name:	swpnib()
*
* - description: 	Swaps the nibbles of a given char
*
* - parameter: 		char to swap
*
* - return value: 	swapped nibbles
******************************************************************
*/
char swpnib(char input)
{
	return ((input << 4) & 0xF0) | ((input >> 4) & 0x0F);
}

/*
******************************************************************
* - function name:	parseFile()
*
* - description: 	File Parser code
*
* - parameter: 		source path string; string length; file name string; file name length; pointer to CheckKey function
*
* - return value: 	error code
******************************************************************
*/
int parseFile(char* path, uint32_t pathlength, char* file, uint32_t filelength, void(*CheckKey)(FILE*, char*, unsigned int))
{
	FILE* sourceFile;
	uint32_t KeyLength = 0;
	char* Key = NULL;
	uint32_t type = 0;
	
	// Open file
	sourceFile = myfopen("rb", path, pathlength, file, filelength, DIR_SEPARATOR);
	if (sourceFile != 0)
	{
		uint32_t FileStart = ftell(sourceFile);
		fseek(sourceFile, 0, SEEK_END);
		uint32_t FileEnd = ftell(sourceFile);
		fseek(sourceFile, FileStart, SEEK_SET);

		// Iterate over whole file
		while(ftell(sourceFile) < FileEnd)
		{
			(void)!fread(&KeyLength, sizeof(uint32_t), 1, sourceFile); // Get key lenght
			if (KeyLength > 0 && KeyLength < 0xFFFFFFFF)
			{
				Key = (char*)calloc(KeyLength + 1, sizeof(char)); // Reserve memory for key + zero termination
				if (Key != 0)
				{
					(void)!fread(Key, sizeof(char), KeyLength, sourceFile); // Read key
					Key[KeyLength] = '\0'; // Zero terminate key

					// Better to be redone
					(void)!fread(&type, sizeof(uint32_t), 1, sourceFile);
					fseek(sourceFile, (int)sizeof(uint32_t) * -1, SEEK_CUR);

					CheckKey(sourceFile, Key, KeyLength); // Process Key
					SkipBlock(sourceFile, type); // Skip until next block
					free(Key);
				}
			}
			else  // End of file
			{
				break;
			}
		}
		return 0;
	}
	else
	{
		myPrint("Failed to open [%s%c%s]!\n", path, DIR_SEPARATOR, file);
		return -1;
	}
}

/*
******************************************************************
* - function name:	SkipBlock()
*
* - description: 	Skips data until the beging of the next block
*
* - parameter: 		pointer to source file; 1 for strings (and other 8 bit values) all other vaues for 32bit exit code
*
* - return value: 	-
******************************************************************
*/
void SkipBlock(FILE* sourceFile, uint32_t type)
{
	uint32_t FileStart = ftell(sourceFile);
	fseek(sourceFile, 0, SEEK_END);
	uint32_t FileEnd = ftell(sourceFile);
	fseek(sourceFile, FileStart, SEEK_SET);

	// Skip until Magic
	if (type == 1) // 8 exit code
	{
		uint8_t Data = 0;
		while (Data != 0xFF && ftell(sourceFile) < FileEnd)
		{
			(void)!fread(&Data, sizeof(uint8_t), 1, sourceFile);
		}
	}
	else // 32 bit exit code
	{
		uint32_t Data = 0;
		while (Data != 0x4FFFFFFF && ftell(sourceFile) < FileEnd)
		{
			(void)!fread(&Data, sizeof(uint32_t), 1, sourceFile); // Read byte from file
		}
	}
}
/*
******************************************************************
* - function name:	numProcess()
*
* - description: 	Splits fixed-point number into integer and fractional part
*
* - parameter: 		Value to convert, scaling, offset
*
* - return value: 	number as struct containing integer and fractional content
******************************************************************
*/
num_struct numProcess(int32_t input, int32_t Ratio, int32_t Offset)
{
	num_struct Output;
	int32_t Temp = input * Ratio + Offset;
	Output.Integ = Temp / 100e3;
	Output.Frac = fabs(Temp - (Output.Integ * 100e3));
	return Output;
}

/*
******************************************************************
* - function name:	InitString()
*
* - description: 	Resets all strings
*
* - parameter: 		number of elements; pointer to struct pointer
*
* - return value: 	-
******************************************************************
*/
void InitString(int32_t len, string_struct** structure)
{
	if (len > 0 && *structure != NULL)
	{
		for (uint32_t i = 0; i < len; i++)
		{
			if ((*structure)[i].Length > 0 && (*structure)[i].Text != 0)
			{
				free((*structure)[i].Text); // Free string
			}
		}
		free(*structure); // Free Array of strings
	}
	*structure = NULL;
}

/*
******************************************************************
* - function name:	InitRegular()
*
* - description: 	Resets all regular data
*
* - parameter: 		number of elements; pointer to struct pointer
*
* - return value: 	-
******************************************************************
*/
void InitRegular(int32_t len, void** structure)
{
	if (len > 0 && *structure != 0)
	{
		free(*structure); // Free Array of data
	}
	*structure = NULL;
}

/*
******************************************************************
* - function name:	ParseKey()
*
* - description: 	Read data from *.v file into key_struct
*
* - parameter: 		-
*
* - return value: 	pointer to key struct
******************************************************************
*/
key_struct* ParseKey(FILE* sourceFile)
{
	key_struct* key = malloc(sizeof(key_struct));
	if (key != NULL)
	{
		(void)!fread(&(*key).Typecode, sizeof(uint32_t), 1, sourceFile);
		(void)!fread(&(*key).Length, sizeof(uint32_t), 1, sourceFile);
		fseek(sourceFile, 16, SEEK_CUR);

		if ((*key).Length < 0) // ToDo: Look into negative size
		{
			(*key).Length = 0;
		}

		switch ((*key).Typecode)
		{
		case typecode_String:
			(*key).Data = ParseString(sourceFile, (*key).Length, (uint32_t*) &(*key).LengthCalc);
			break;
		case typecode_IntArray:
			(*key).Data = ParseIntArray(sourceFile, (*key).Length, (uint32_t*) &(*key).LengthCalc);
			break;
		case typecode_UID:
		case typecode_Long:
			(*key).LengthCalc = (*key).Length / 8;
			(*key).Data = ParseInt(sourceFile, (*key).LengthCalc, sizeof(LongData));
			break;
		case typecode_SUID:
		case typecode_LongLong:
			(*key).LengthCalc = (*key).Length / 8;
			(*key).Data = ParseInt(sourceFile, (*key).LengthCalc, sizeof(LongLongData));
			break;
		case typecode_Time:
			(*key).LengthCalc = (*key).Length / 8;
			(*key).Data = ParseInt(sourceFile, (*key).LengthCalc, sizeof(TimeData));
			break;
		case typecode_Int:
			(*key).LengthCalc = (*key).Length / 8;
			(*key).Data = ParseInt(sourceFile, (*key).LengthCalc, sizeof(IntData));
			break;
		case typecode_GUID:
			if ((*key).Length != 0)
			{
				myPrint("Typecode GUID not zero but [%d]!\n", (*key).Length);
			}
			(*key).Data = NULL;
			(*key).LengthCalc = 0;
			break;

		default:
			myPrint("Unknown typecode [%d]!\n", (*key).Typecode);
			break;
		}
	}
	return key;
}

/*
******************************************************************
* - function name:	InitKey()
*
* - description: 	Initializes data in a key struct
*
* - parameter: 		pointer to key structure to initialize
*
* - return value: 	-
******************************************************************
*/
void InitKey(key_struct** key)
{
	if (*key != NULL)
	{
		switch ((*key)->Typecode)
		{
		case typecode_String:
		case typecode_IntArray:
			if ((*key)->Data != NULL)
			{
				for (unsigned int i = 0; i < (*key)->LengthCalc; i++)
				{
					if ((((int_array_struct*)(*key)->Data)[i]).Data != NULL)
					{
						free((((int_array_struct*)(**key).Data)[i]).Data);
					}
				}
				free((*key)->Data);
			}
			break;
		case typecode_UID:
		case typecode_Long:
		case typecode_SUID:
		case typecode_LongLong:
		case typecode_Time:
		case typecode_Int:
		case typecode_GUID:
			if ((*key)->Data != NULL )
			{
				free((*key)->Data);
			}
			break;
		default:
			break;
		}

		free(*key);
		*key = NULL;
	}
}

/*
******************************************************************
* - function name:	ParseString()
*
* - description: 	Parsing code for strings
*
* - parameter: 		pointer to source file; pointer to number of elements to read; name of parameter (for error messages)
*
* - return value: 	pointer to parsed data
******************************************************************
*/
string_struct* ParseString(FILE* sourceFile, int32_t PayloadLenRaw, uint32_t* NumElements)
{
	*NumElements = 0;
	uint8_t EntryLen8 = 0;
	uint32_t EntryLen32 = 0;
	string_struct* Struct = NULL;
	uint32_t SizeAccumulator = 0;

	uint32_t FileStart = ftell(sourceFile);
	fseek(sourceFile, 0, SEEK_END);
	uint32_t FileEnd = ftell(sourceFile);
	fseek(sourceFile, FileStart, SEEK_SET);

	// Count Entry (I haven't found a way to derive the number)
	while (PayloadLenRaw > SizeAccumulator && ftell(sourceFile) < FileEnd)
	{
		(void)!fread(&EntryLen8, sizeof(uint8_t), 1, sourceFile);
		// Get file entry
		if (EntryLen8 == 0xfd) // More than 255 char in this string
		{
			(void)!fread(&EntryLen32, sizeof(uint32_t), 1, sourceFile);
			fseek(sourceFile, EntryLen32, SEEK_CUR);
			(*NumElements)++;
			SizeAccumulator += 12 + 4 * (EntryLen32 / 4); // Always round to 4 character
		}
		else if (EntryLen8 == 0xfe) // Padding block. Skip next block
		{
			//fseek(sourceFile, sizeof(uint32_t), SEEK_CUR);
			(void)!fread(&EntryLen32, sizeof(uint32_t), 1, sourceFile);
		}
		else if (EntryLen8 == 0xff) // No more entry�s
		{
			break;
		}
		else // Regular entry
		{
			fseek(sourceFile, EntryLen8, SEEK_CUR);
			(*NumElements)++;
			SizeAccumulator += 12 + 4 * (EntryLen8 / 4); // Always round to 4 character
		}
	}
	// Seek back to begining of block
	fseek(sourceFile, FileStart, SEEK_SET);

	Struct = calloc(*NumElements, sizeof(string_struct));
	if (Struct != NULL)
	{
		for (uint32_t i = 0; i < *NumElements;)
		{
			// Get file entry
			(void)!fread(&EntryLen8, sizeof(uint8_t), 1, sourceFile);
			if (EntryLen8 == 0xfd) // More than 255 char in this string
			{
				(void)!fread(&Struct[i].Length, sizeof(uint32_t), 1, sourceFile);
				Struct[i].Text = (char*)calloc(Struct[i].Length + 1, sizeof(char));
				if (Struct[i].Text != NULL)
				{
					(void)!fread(Struct[i].Text, sizeof(char), Struct[i].Length, sourceFile);
					Struct[i].Text[Struct[i].Length] = '\0'; // Zero terminate string
				}
				i++;
			}
			else if (EntryLen8 == 0xfe) // Unknown. Skip next block
			{
				(void)!fread(&EntryLen32, sizeof(uint32_t), 1, sourceFile);
			}
			else if (EntryLen8 == 0xff) // No more entry�s
			{
				break;
			}
			//else if (EntryLen8 == 0) // length 0
			//{
			//	break;
			//}
			else // Regular entry
			{
				Struct[i].Length = EntryLen8;
				Struct[i].Text = (char*)calloc(Struct[i].Length + 1, sizeof(char));
				if (Struct[i].Text != NULL)
				{
					(void)!fread(Struct[i].Text, sizeof(char), Struct[i].Length, sourceFile);
					Struct[i].Text[Struct[i].Length] = '\0'; // Zero terminate String
				}
				i++;
			}
		}
		// Seek back to not skip encode
		fseek(sourceFile, (int)sizeof(uint8_t) * -1, SEEK_CUR);
	}
	return Struct;
}

/*
******************************************************************
* - function name:	ParseIntArray()
*
* - description: 	Parsing code for strings
*
* - parameter: 		pointer to source file; pointer to number of elements to read; name of parameter (for error messages)
*
* - return value: 	pointer to parsed data
******************************************************************
*/
int_array_struct* ParseIntArray(FILE* sourceFile, int32_t PayloadLenRaw, uint32_t* NumElements)
{
	*NumElements = 0;
	uint32_t EntryLen = 0;
	int_array_struct* Struct = NULL;
	uint32_t SizeAccumulator = 0;

	uint32_t blockaddress;	// Just a guess
	(void)!fread(&blockaddress, sizeof(uint32_t), 1, sourceFile);

	uint32_t FileStart = ftell(sourceFile);
	fseek(sourceFile, 0, SEEK_END);
	uint32_t FileEnd = ftell(sourceFile);
	fseek(sourceFile, FileStart, SEEK_SET);

	// Count Entry (I haven't found a way to derive the number)
	while (PayloadLenRaw > SizeAccumulator && ftell(sourceFile) < FileEnd)
	{
		fseek(sourceFile, EntryLen * sizeof(uint32_t), SEEK_CUR);
		(void)!fread(&EntryLen, sizeof(uint32_t), 1, sourceFile);
	
		if (EntryLen == 0x4FFFFFFF) // No more entry�s
		{
			break;
		}
		else if (EntryLen == 0x4FFFFFFE) // Unknown. Skip next block
		{
			(void)!fread(&blockaddress, sizeof(uint32_t), 1, sourceFile);
			EntryLen = 0;
		}
		else
		{
			(*NumElements)++;
			SizeAccumulator += 8 + (EntryLen * sizeof(uint32_t));
		}
	}
	// Seek back to begining of block
	fseek(sourceFile, FileStart, SEEK_SET);

	Struct = calloc(*NumElements, sizeof(int_array_struct));
	if (Struct != NULL)
	{
		for (uint32_t i = 0; i < *NumElements;)
		{
			(void)!fread(&EntryLen, sizeof(uint32_t), 1, sourceFile);
			if (EntryLen == 0x4FFFFFFF) // No more entry�s
			{
				break;
			}
			else if (EntryLen == 0x4FFFFFFE) // Padding
			{
				(void)!fread(&blockaddress, sizeof(uint32_t), 1, sourceFile);
			}
			else
			{
				Struct[i].Length = EntryLen;
				Struct[i].Data = (IntData*)calloc(Struct[i].Length, sizeof(uint32_t));
				if (Struct[i].Data != NULL)
				{
					(void)!fread(Struct[i].Data, sizeof(uint32_t), Struct[i].Length, sourceFile);
				}
				i++;
			}
		}
		// Seek back to not skip encode
		fseek(sourceFile, (int)sizeof(uint32_t) * -1, SEEK_CUR);
	}
	return Struct;
}


/*
******************************************************************
* - function name:	Parse()
*
* - description: 	Default parsing code
*
* - parameter: 		pointer to source file; pointer to number of elements to read; size of structure;
*					size of struct on most cases but sizeof(uint32_t) on coordinate structs (DXD seams to store coordinates separately, but storing them as struct makes more sense IMHO);
*					name of parameter (for error messages)
*
* - return value: 	pointer to parsed data
******************************************************************
*/
void* ParseInt(FILE* sourceFile, uint32_t NumElements, uint32_t strutSize)
{
	uint32_t Magic;
	int32_t Repetitions;
	uint32_t* Struct = NULL;
	uint32_t structSize32 = (strutSize / sizeof(uint32_t));

	uint32_t blockaddress;	// Just a guess
	(void)!fread(&blockaddress, sizeof(uint32_t), 1, sourceFile);

	uint32_t FileStart = ftell(sourceFile);
	fseek(sourceFile, 0, SEEK_END);
	uint32_t FileEnd = ftell(sourceFile);
	fseek(sourceFile, FileStart, SEEK_SET);

	Struct = calloc(NumElements, strutSize);
	if (Struct != NULL)
	{
		for (uint32_t i = 0; i < NumElements;)
		{
			// Check entry for magic values
			(void)!fread(&Magic, sizeof(uint32_t), 1, sourceFile);
			if (Magic == 0x4FFFFFFC) // Increasing value
			{
				(void)!fread(&Repetitions, sizeof(uint32_t), 1, sourceFile);
				while (Repetitions != 0 && i < NumElements && ftell(sourceFile) < FileEnd)
				{
					for (uint32_t j = 0; j < structSize32; j++) // Copy whole struct
					{
						if (j == (structSize32 - 1)) // Increment only last entry of struct
						{
							Struct[i * structSize32 + j] = Struct[(i - 1) * structSize32 + j] + 1;
						}
						else
						{
							Struct[i * structSize32 + j] = Struct[(i - 1) * structSize32 + j];
						}
					}
					Repetitions++;
					i++;
				}
			}
			else if (Magic == 0x4FFFFFFD) // Repeated value
			{
				(void)!fread(&Repetitions, sizeof(uint32_t), 1, sourceFile);
				while (Repetitions != 0 && i < NumElements && ftell(sourceFile) < FileEnd)
				{
					for (uint32_t j = 0; j < structSize32; j++) // Copy whole struct
					{
						Struct[i * structSize32 + j] = Struct[(i - 1) * structSize32 + j];
					}
					Repetitions++;
					i++;
				}
			}
			else if (Magic == 0x4FFFFFFE) // Unknown. Skip next block
			{
				(void)!fread(&blockaddress, sizeof(uint32_t), 1, sourceFile);
			}
			else if (Magic == 0x4FFFFFFF) // No more entry�s
			{
				break;
			}
			else // Regular entry
			{
				// No magic, seek back & read
				fseek(sourceFile, (int)sizeof(uint32_t) * -1, SEEK_CUR);
				(void)!fread(Struct + (i * structSize32), strutSize, 1, sourceFile);
				i++;
			}
		}
	}
	// Seek back for Skip function to work
	fseek(sourceFile, (int)sizeof(uint32_t) * -1, SEEK_CUR);
	return (void*)Struct;
}


/*
******************************************************************
* - function name:	CopyString()
*
* - description: 	Copy a string struct and add linefeed
*
* - parameter: 		Structure to copy
*
* - return value: 	copy from structure with zero termination
******************************************************************
*/
string_struct CopyString(string_struct Input)
{
	string_struct Output;
	Output.Length = Input.Length;
	Output.Text = calloc(Output.Length + 1, sizeof(char));
	if (Output.Text == NULL)
	{
		Output.Length = 0;
		return Output;
	}
	memcpy(Output.Text, Input.Text, Output.Length + 1);
	Output.Text[Output.Length] = '\0'; // Zero terminate
	return Output;
}