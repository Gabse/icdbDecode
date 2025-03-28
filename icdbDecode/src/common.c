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
TypeCodes:
1 = Char
2 = TBD
3 = TBD
4 = TBD
5 = TBD
6 = Two uint32_t
7 = TBD
8 = TBD
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
#include "stringutil.h"	// Required for assemblePath

/*
******************************************************************
* Defines
******************************************************************
*/
#define debug flase		// 1 for keys as list, 2 for keys as C code

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
* - parameter: 		fopen parameter; source path string; lenth of sourcepath
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
* - function name:	parseFile()
*
* - description: 	File Parser code
*
* - parameter: 		source path string; string length; file name string; file name length; pointer to CheckKey function
*
* - return value: 	-
******************************************************************
*/
int parseFile(char* path, uint32_t pathlenth, char* file, uint32_t filelenth, void(*CheckKey)(FILE*, uint32_t, char*))
{
	FILE* sourceFile;
	uint32_t filesize = 0;
	uint32_t KeyLenth = 0;
	char* Key = NULL;
	uint32_t type = 0;
	
	// Open file
	sourceFile = myfopen("rb", path, pathlenth, file, filelenth, DIR_SEPERATOR);
	if (sourceFile != 0)
	{
		// Determine total file size
		fseek(sourceFile, 0, SEEK_END);
		filesize = ftell(sourceFile);
		rewind(sourceFile);

		// Itterate over whole file
		while (ftell(sourceFile) < (filesize - sizeof(uint32_t)))
		{
			fread(&KeyLenth, sizeof(uint32_t), 1, sourceFile); // Get key lenght
			if (KeyLenth > 0 && KeyLenth < 0xFFFFFFFF)
			{
				Key = (char*)calloc(KeyLenth + 1, sizeof(char)); // Reseve memory for key + zero termination
				if (Key != 0)
				{
					fread(Key, sizeof(char), KeyLenth, sourceFile); // Read key
					Key[KeyLenth] = '\0'; // Zero terminate key
					fread(&type, sizeof(uint32_t), 1, sourceFile);	// Read data type

					#if debug
						PrintKey(sourceFile, filesize, Key); // Debugging code
					#endif

					CheckKey(sourceFile, filesize, Key); // Process Key
					SkipBlock(sourceFile, filesize, type); // Skip untill next block
					free(Key);
				}
			}
		}
		return 0;
	}
	else
	{
		myPrint("Failed to open [%s%c%s]!\n", path, DIR_SEPERATOR, file);
		return 1;
	}
}

/*
******************************************************************
* - function name:	SkipBlock()
*
* - description: 	Skips data until the beging of the next block
*
* - parameter: 		pointer to source file; total filesize; 1 for strings (and other 8 bit values) all other vaues for 32bit exit code
*
* - return value: 	-
******************************************************************
*/
void SkipBlock(FILE* sourceFile, uint32_t filesize, uint32_t type)
{
	// Skip untill Magic
	if (type == 1) // 8 exit code
	{
		uint8_t Data = 0;
		while (Data != 0xFF && ftell(sourceFile) < filesize)
		{
			fread(&Data, sizeof(uint8_t), 1, sourceFile);
		}
	}
	else // 32 bit exit code
	{
		uint32_t Data = 0;
		uint8_t Temp = 0;
		while (Data != 0x4FFFFFFF && ftell(sourceFile) < filesize)
		{
			fread(&Temp, sizeof(uint8_t), 1, sourceFile); // Read byte from file
			Data = (Data >> 8) | (Temp << 24); // Make 32bit number from last 8bit numbers
		}
	}
}

/*
******************************************************************
* - function name:	Parse()
*
* - description: 	Default parsing code
*
* - parameter: 		pointer to source file; pointer to number of elements to read; size of structure;
*					size of struct on most cases but sizeof(uint32_t) on coordinate structs (DXD seams to store coordinates seperatly, but storing them as struct makes more sense IMHO);
*					name of parameter (for error messages)
*
* - return value: 	pointer to parsed data
******************************************************************
*/
void* Parse(FILE* sourceFile, int32_t* NumElements, uint32_t strutSize, uint32_t strutSizeNorm, char* Name)
{
	uint32_t PayloadLen;
	uint32_t PayloadLenRaw;
	uint32_t Magic;
	int32_t Repetitions;
	uint32_t* Struct = NULL;
	uint32_t Type = 0;
	uint32_t structSize32 = (strutSize / sizeof(uint32_t));
	fseek(sourceFile, sizeof(uint32_t) * -1, SEEK_CUR);
	fread(&Type, sizeof(uint32_t), 1, sourceFile);
	if (Type == 1)
	{
		myPrint("Error! Wrong Type %d code in %s\n", Type, Name);
		return 0;
	}

	// Bytes to first entry
	fread(&PayloadLenRaw, sizeof(uint32_t), 1, sourceFile);
	fseek(sourceFile, 20, SEEK_CUR);
	PayloadLen = PayloadLenRaw / (strutSizeNorm + 4);
	
	if (*NumElements < 0)
	{
		*NumElements = PayloadLen;
	}
	if (*NumElements != PayloadLen)
	{
		myPrint("%s Missmatch! Expect %d, Got %d!\n", Name, *NumElements, PayloadLen);
		return 0;
	}
	else
	{
		Struct = calloc(*NumElements, strutSize);
		if (Struct != 0)
		{
			for (uint32_t i = 0; i < *NumElements;)
			{
				// Check entry for magic values
				fread(&Magic, sizeof(uint32_t), 1, sourceFile);
				if (Magic == 0x4FFFFFFC) // Increasing value
				{
					fread(&Repetitions, sizeof(uint32_t), 1, sourceFile);
					while (Repetitions != 0 && i < *NumElements)
					{
						for (uint32_t j = 0; j < structSize32; j++) // Copy whole struct
						{
							if (j == (structSize32 - 1)) // Increment only last entry of struct
							{
								Struct[i] = Struct[i - 1] + 1;
							}
							else
							{
								Struct[i] = Struct[i - 1];
							}
						}
						Repetitions++;
						i++;
					}
				}
				else if (Magic == 0x4FFFFFFD) // Repeated value
				{
					fread(&Repetitions, sizeof(uint32_t), 1, sourceFile);
					while (Repetitions != 0 && i < *NumElements)
					{
						for (uint32_t j = 0; j < structSize32; j++) // Copy whole struct
						{
							Struct[i * structSize32 + j] = Struct[(i - 1) * structSize32 + j];
						}
						Repetitions++;
						i++;
					}
				}
				else if (Magic == 0x4FFFFFFE) // Padding block. Skip next block
				{
					fread(&Repetitions, sizeof(uint32_t), 1, sourceFile);
					//fseek(sourceFile, sizeof(uint32_t), SEEK_CUR);
				}
				else if (Magic == 0x4FFFFFFF) // No more entrys
				{
					// Seek back for Skip function to work
					fseek(sourceFile, (int32_t)sizeof(uint32_t) * -1, SEEK_CUR);
					break;
				}
				else // Regular entry
				{
					// No magic, seek back & read
					fseek(sourceFile, (int32_t)sizeof(uint32_t) * -1, SEEK_CUR);
					fread(Struct + (i * structSize32), strutSize, 1, sourceFile);
					i++;
				}
			}
		}
	}
	return (void*)Struct;
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
text_struct* ParseString(FILE* sourceFile, int32_t* NumElements, char* Name)
{
	uint32_t PayloadLen = 0;
	uint32_t PayloadLenRaw;
	uint8_t EntryLen8 = 0;
	uint32_t EntryLen32 = 0;
	uint32_t BlockStartAddress;
	text_struct* Struct = NULL;
	uint32_t Type = 0;
	fseek(sourceFile, sizeof(uint32_t) * -1, SEEK_CUR);
	fread(&Type, sizeof(uint32_t), 1, sourceFile);
	if (Type != 1)
	{
		myPrint("Error! Wrong Type %d code in %s\n", Type, Name);
		return 0;
	}

	fread(&PayloadLenRaw, sizeof(uint32_t), 1, sourceFile);
	fseek(sourceFile, 16, SEEK_CUR);
	// Store initial address
	BlockStartAddress = ftell(sourceFile);

	// Count Entry (I haven't found a way to derive the number of entrys from PayloadLenRaw)
	do
	{
		fread(&EntryLen8, sizeof(uint8_t), 1, sourceFile);
		// Get file entry
		if (EntryLen8 == 0xfd) // More than 255 char in this string
		{
			fread(&EntryLen32, sizeof(uint32_t), 1, sourceFile);
			fseek(sourceFile, EntryLen32, SEEK_CUR);
			PayloadLen++;
		}
		else if (EntryLen8 == 0xfe) // Padding block. Skip next block
		{
			//fseek(sourceFile, sizeof(uint32_t), SEEK_CUR);
			fread(&EntryLen32, sizeof(uint32_t), 1, sourceFile);
		}
		else if (EntryLen8 == 0xff) // No more entrys
		{
			break;
		}
		else // Regular entry
		{
			fseek(sourceFile, EntryLen8, SEEK_CUR);
			PayloadLen++;
		}
	} while(1); // ToDO accumulate and compare PayloadLenRaw
	//} while (ftell(sourceFile) < filesize);

	fseek(sourceFile, BlockStartAddress, SEEK_SET);

	if (*NumElements < 0)
	{
		*NumElements = PayloadLen;
	}
	if (*NumElements != PayloadLen)
	{
		myPrint("%s Missmatch! Expect %d, Got %d!\n", Name, *NumElements, PayloadLen);
		return 0;
	}
	else
	{
		// Seek back to begining of block
		Struct = calloc(*NumElements, sizeof(text_struct));
		if (Struct != 0)
		{
			for (uint32_t i = 0; i < *NumElements;)
			{
				// Get file entry
				fread(&EntryLen8, sizeof(uint8_t), 1, sourceFile);
				if (EntryLen8 == 0xfd) // More than 255 char in this string
				{
					fread(&Struct[i].Lenth, sizeof(uint32_t), 1, sourceFile);
					Struct[i].Text = (char*)calloc(Struct[i].Lenth + 1, sizeof(char));
					if (Struct[i].Text != 0)
					{
						fread(Struct[i].Text, sizeof(char), Struct[i].Lenth, sourceFile);
						Struct[i].Text[Struct[i].Lenth] = '\0'; // Zero terminate string
					}
					i++;
				}
				else if (EntryLen8 == 0xfe) // Padding block. Skip next block
				{
					fread(&EntryLen32, sizeof(uint32_t), 1, sourceFile);
				}
				else if (EntryLen8 == 0xff) // No more entrys
				{
					break;
				}
				else // Regular entry
				{
					Struct[i].Lenth = EntryLen8;
					Struct[i].Text = (char*)calloc(Struct[i].Lenth + 1, sizeof(char));
					if (Struct[i].Text != 0)
					{
						fread(Struct[i].Text, sizeof(char), Struct[i].Lenth, sourceFile);
						Struct[i].Text[Struct[i].Lenth] = '\0'; // Zero terminate Name
					}
					i++;
				}
			}
			// Seek back to not skip encode
			fseek(sourceFile, sizeof(uint8_t) * -1, SEEK_CUR);
		}
	}
	return Struct;
}

/*
******************************************************************
* - function: numProcess()
*
* - desc: Splits fixed-point number into integer and fractional part
*
* - par: Value to convert, scaling, offset
*
* - ret: number as struct containing integer and fractional content
******************************************************************
*/
num_struct numProcess(int32_t input, int32_t Ratio, int32_t Offset)
{
	num_struct Output;
	int32_t Temp = input * Ratio + Offset;
	Output.Integ = Temp / 100e3;
	Output.Frac = abs(Temp - (Output.Integ * 100e3));
	return Output;
}

/*
******************************************************************
* - function name:	PrintKey()
*
* - description: 	Prints out all the Keys whitin a file (usefull for debugging)
*
* - parameter: 		pointer to source file; total filesize; Key to print
*
* - return value: 	-
******************************************************************
*/
void PrintKey(FILE* sourceFile, int32_t filesize, char* Key)
{
	// Print Key
#if debug == 2
	myPrint("\telse if (strcmp(Name, %c%s%c) == 0)\n\t{\n\n\t}\n",'"' , Key ,'"');
#else
	myPrint("%s\n", Key);
#endif

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
void InitString(int32_t len, text_struct** structure)
{
	if (len > 0 && *structure != NULL)
	{
		for (uint32_t i = 0; i < len; i++)
		{
			if ((*structure)[i].Lenth > 0 && (*structure)[i].Text != 0)
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