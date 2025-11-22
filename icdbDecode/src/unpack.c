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
#include "unpack.h"
#include <stdio.h>		// Required for fprint, fopen, ...
#include <stdlib.h>		// Required for calloc to work properly
#include <stdint.h>		// Required for int32_t, uint32_t, ...
#include <time.h>		// Required for time_t
#include <zlib.h>		// Required for decompression
#include "common.h"		// Required for myfopen
#include "stringutil.h" // Required for createPath
#include "list.h"		// Required for list

#ifdef WIN32 // Building for Windows
	#include <windows.h> // Required for Linking of files
#endif
/*
******************************************************************
* Global Defines
******************************************************************
*/
#define DECOMPRESS_CHUNK_SIZE 1024
#define _CRT_SECURE_NO_DEPRECATE			// Disable insecure function warning in VisualStudio

/*
******************************************************************
* Global Structures
******************************************************************
*/
typedef struct databaseHeader_Struct
{
	uint32_t file_version; // Version of iCDB file. Current version = 1009, others might not decode correctly
	uint32_t iCDB_version; // Version of iCDB Server used to create the file. 2.10 = 4000, 2.11 = 4100...
	uint8_t project_GUID[24]; // Project GUID
	uint8_t server_GUID[24]; // Server GUID
	uint32_t opening_counter; // Opening Counter?
	uint32_t always_zero; // Zero?
	uint32_t unknown1; // Unknown, seems like a part of a GUID
	uint32_t unknown3[2]; // Unknown. Both seems to always hold the same data
	uint32_t always_zero2; // Zero?
	uint32_t num_files; // Total number of files in this db
	uint32_t first_list; // Address of the first file list
	uint32_t num_lists;	 // Total number of file lists
	uint32_t unknown2;	// Zero most of the time, sometimes different value
	uint32_t filesize;	 // Total file size

	// This block starts at address 0x085C
	uint32_t iCDBdiagnostic_length; // Length of iCDB Server version info
	char* iCDBdiagnostic; // iCDB Server diagnostic info
	uint32_t pc_name_length; // Length of pc name
	char* pc_name; // Name of the last pc it was last edited with
	uint32_t user_name_length; // length of username name
	char* user_name; // Windows username of the last designer
	time_t edittime; // Last edit time
	uint32_t pid; // User PID of the last designer
	uint32_t os_version_length; // Length of os version info
	char* os_version; // Windows os info
	uint32_t iCDB_string_length; // Length of iCDB string
	char* iCDB_string; // iCDB string. always "iCDB Server"?
	uint32_t filepath_length; // Length of filepath string
	char* filepath; // Filepath of this file on the last edit
	uint32_t settingspath_length; // Length of settings path string
	char* settingspath; // Settings path string
} databaseHeader_Struct;

typedef struct file_list_struct
{
	uint8_t file_cnt; // Number of files in this file list, Max 100 files per list
	uint8_t always_zero[4];	// Always zero?
	uint8_t file_cnt2; // Again?
	uint16_t always_zero2;	// Always zero?
	uint32_t magic;	// Always 0x6410?
	uint32_t next_file_list; // Start address of the next list, 0 if there is no next list
} file_list_struct;

typedef struct file_struct
{
	uint32_t file_address;	// Start address of this file (address of this byte)
	uint32_t always_zero;	// Maybe always zero?

	uint32_t filename_length; // Length of filename string
	char filename[160]; // Filename

	uint8_t data[0x10]; // Sometimes data in human readable format
	uint32_t otherData[2]; // Unknown data. Both entries seam to always hold the same data
	uint32_t zero_padding; // Zeros
	uint32_t unknown;
	uint8_t fileGUID[24]; // File GUID
	uint32_t data_size; // Size of file payload
	uint32_t data_address; // Starting address of payload
	uint32_t one[2]; // Unknown. 1 most of the time?
	uint32_t unknown2[2]; // Unknown
	uint32_t padded_file_length; // Each file entry is always a multiple of 64 bytes in size, unused space is padded with 0
}file_struct;

typedef struct fragment_struct
{
	int payload_length; // Length of payload in this fragment
	int fragment_length; // Total length of this fragment (Header + Payload + Padding)
	int duplicates; // Number of files containing the same fragment of data
	int next_fragment; // Start address of the next fragment, 0 if there is no next fragment
}fragment_struct;

/*
******************************************************************
* Global Variables
******************************************************************
*/
int nontDecompress = 0;
int linkLongLongFiles = 0;
int noLongLongFiles = 0;

int storepathLength = 0;
char* storepath = NULL;

int databasepathLength = 0;
char* databasepath = NULL;


/*
******************************************************************
* Function Prototypes
******************************************************************
*/
int decodeDatabaseHeader(FILE*, databaseHeader_Struct*);
int decodefileList(FILE*, databaseHeader_Struct*, file_list_struct*);
int decodefile(FILE*, file_struct*);
int exportFiles(FILE*, databaseHeader_Struct*, file_struct**);
char* makePath(char*, int, char*, int);
int mylink(char*, int, char*, int, char*, int);
void printGUID(uint8_t Guid[24]);
int decompress(char*, int, char**);

/*
******************************************************************
* Global Functions
******************************************************************
*/

/*
******************************************************************
* - function name:	UnpackIcdb()
*
* - description: 	Unpack filestructure from database file
*
* - parameter: 		filepath string
*
* - return value: 	error code
******************************************************************
*/
int UnpackIcdb(char* sourcepath, int sourcepathLength, char* destinationpath, int destinationpathLength)
{
	int error = 0;
	FILE* sourceFile = NULL;
	databaseHeader_Struct* databaseHeader = NULL;
	file_list_struct** fileList = NULL;
	file_struct** file = NULL;
	unsigned int fileCNT = 0;
	
	storepathLength = destinationpathLength;
	storepath = destinationpath;

	databasepathLength = sourcepathLength;
	databasepath = sourcepath;
	
	// Open source file
	sourceFile = fopen(sourcepath, "rb");
	if (sourceFile != 0)
	{
		// Read database header
		databaseHeader = (databaseHeader_Struct*)malloc(sizeof(databaseHeader_Struct));
		error |= decodeDatabaseHeader(sourceFile, databaseHeader);

		// Allocate memory for file array
		fileList = (file_list_struct**)malloc(sizeof(file_list_struct) * databaseHeader->num_lists);
		file = (file_struct**)malloc(sizeof(file_struct) * databaseHeader->num_files);
		
		// Go to first file list
		fseek(sourceFile, databaseHeader->first_list, SEEK_SET);

		// Read in all files
		for (unsigned int i = 0; i < databaseHeader->num_lists; i++)
		{
			file_list_struct* temp_fileList = (file_list_struct*)malloc(sizeof(file_list_struct));

			error |= decodefileList(sourceFile, (databaseHeader_Struct*)&databaseHeader, (file_list_struct*) temp_fileList);

			*(fileList + i) = temp_fileList;
			
			// Read in all files from this list
			for (int j = 0; j < temp_fileList->file_cnt; j++)
			{
				file_struct* temp_file = (file_struct*)malloc(sizeof(file_struct));

				error |= decodefile(sourceFile, temp_file);

				*(file + fileCNT) = temp_file;

				fileCNT++;

				if (fileCNT > databaseHeader->num_files)
				{
					myPrint("file entry count mismatch! %d and %d\n", fileCNT, databaseHeader->num_files);
					error |= 1;
					break;
				}
			}

			if (temp_fileList->next_file_list != 0)
			{
				// Go to next file list
				fseek(sourceFile, temp_fileList->next_file_list, SEEK_SET);
			}
			else
			{
				if (i + 1 != databaseHeader->num_lists)
				{
					myPrint("file size mismatch! %d and %d\n", i + 1, databaseHeader->num_lists);
					error |= 1;
				}
				break;
			}
		}
		if (fileCNT != databaseHeader->num_files)
		{
			myPrint("file count mismatch! %d and %d\n", fileCNT, databaseHeader->num_files);
			error |= 1;
		}

		myPrint("%d block(s) with %d total entries loaded.\n\n", databaseHeader->num_lists, databaseHeader->num_files);

		if (exportFiles(sourceFile, databaseHeader, file) == 0 && error == 0)
		{
			myPrint("Successfully written %d files!\n", databaseHeader->num_files);
		}
		else
		{
			myPrint("Write operation was not successful! \n");
		}

		fclose(sourceFile);

		free(databaseHeader);
		free(fileList);
		free(file);

		return error;
	}
	else
	{
		myPrint("Failed to open [%s]!\n", sourcepath);
		return -1;
	}
}


/*
******************************************************************
* Local Functions
******************************************************************
*/

/*
******************************************************************
* - function name:	decodeDatabaseHeader()
*
* - description: 	Read in, and print database header
*
* - parameter: 		file pointer.
*
* - return value: 	error code
******************************************************************
*/
int decodeDatabaseHeader(FILE* sourceFile, databaseHeader_Struct* databaseHeader)
{
	// Determine total file size
	fseek(sourceFile, 0, SEEK_END);
	int filesize = ftell(sourceFile);
	rewind(sourceFile);

	// Read beginning of file header
	fread(databaseHeader, 0x68, 1, sourceFile);

	if (databaseHeader != 0)
	{
		// Check file size
		if (filesize != databaseHeader->filesize && databaseHeader->file_version == 1009) // Pre 1009 doesn't contain filesize in the header
		{
			myPrint("Filesize mismatch! %d and %d\n", filesize, databaseHeader->filesize);
			return -1;
		}

		// Go to second block of data in Header
		fseek(sourceFile, 0x85C, SEEK_SET);

		// Editor version
		fread(&databaseHeader->iCDBdiagnostic_length, sizeof(uint32_t), 1, sourceFile);
		if (databaseHeader->iCDBdiagnostic_length > 0)
		{
			databaseHeader->iCDBdiagnostic = malloc(databaseHeader->iCDBdiagnostic_length + 1);
			if (databaseHeader->iCDBdiagnostic != 0)
			{
				fread(databaseHeader->iCDBdiagnostic, sizeof(char), databaseHeader->iCDBdiagnostic_length, sourceFile);
				*((databaseHeader->iCDBdiagnostic) + databaseHeader->iCDBdiagnostic_length) = '\0';
			}
		}

		// PC Name
		fread(&databaseHeader->pc_name_length, sizeof(uint32_t), 1, sourceFile);
		if (databaseHeader->pc_name_length > 0)
		{
			databaseHeader->pc_name = malloc(databaseHeader->pc_name_length + 1);
			if (databaseHeader->pc_name != 0)
			{
				fread(databaseHeader->pc_name, sizeof(char), databaseHeader->pc_name_length, sourceFile);
				*((databaseHeader->pc_name) + databaseHeader->pc_name_length) = '\0';
			}
		}

		// Username
		fread(&databaseHeader->user_name_length, sizeof(uint32_t), 1, sourceFile);
		if (databaseHeader->user_name_length > 0)
		{
			databaseHeader->user_name = malloc(databaseHeader->user_name_length + 1);
			if (databaseHeader->user_name != 0)
			{
				fread(databaseHeader->user_name, sizeof(char), databaseHeader->user_name_length, sourceFile);
				*((databaseHeader->user_name) + databaseHeader->user_name_length) = '\0';
			}
		}

		// Opening time
		uint32_t tempTime;
		fread(&tempTime, sizeof(uint32_t), 1, sourceFile);
		databaseHeader->edittime = tempTime;

		// User PID
		fread(&databaseHeader->pid, sizeof(uint32_t), 1, sourceFile);

		// Windows version info
		fread(&databaseHeader->os_version_length, sizeof(uint32_t), 1, sourceFile);
		if (databaseHeader->os_version_length > 0)
		{
			databaseHeader->os_version = malloc(databaseHeader->os_version_length + 1);
			if (databaseHeader->os_version != 0)
			{
				fread(databaseHeader->os_version, sizeof(char), databaseHeader->os_version_length, sourceFile);
				*((databaseHeader->os_version) + databaseHeader->os_version_length) = '\0';
			}
		}

		// iCDBstring
		fread(&databaseHeader->iCDB_string_length, sizeof(uint32_t), 1, sourceFile);
		if (databaseHeader->iCDB_string_length > 0)
		{
			databaseHeader->iCDB_string = malloc(databaseHeader->iCDB_string_length + 1);
			if (databaseHeader->iCDB_string != 0)
			{
				fread(databaseHeader->iCDB_string, sizeof(char), databaseHeader->iCDB_string_length, sourceFile);
				*((databaseHeader->iCDB_string) + databaseHeader->iCDB_string_length) = '\0';
			}
		}

		// Filepath
		fread(&databaseHeader->filepath_length, sizeof(uint32_t), 1, sourceFile);
		if (databaseHeader->filepath_length > 0)
		{
			databaseHeader->filepath = malloc(databaseHeader->filepath_length + 1);
			if (databaseHeader->filepath != 0)
			{
				fread(databaseHeader->filepath, sizeof(char), databaseHeader->filepath_length, sourceFile);
				*((databaseHeader->filepath) + databaseHeader->filepath_length) = '\0';
			}
		}

		// Settings path
		fread(&databaseHeader->settingspath_length, sizeof(uint32_t), 1, sourceFile);
		if (databaseHeader->settingspath_length > 0)
		{
			databaseHeader->settingspath = malloc(databaseHeader->settingspath_length + 1);
			if (databaseHeader->settingspath != 0)
			{
				fread(databaseHeader->settingspath, sizeof(char), databaseHeader->settingspath_length, sourceFile);
				*((databaseHeader->settingspath) + databaseHeader->settingspath_length) = '\0';
			}
		}

		// Print version info
		myPrint("********* Database Info *********\n");
		myPrint("Database: \t\t[%s]\n", databasepath);
		myPrint("File Format Version: \t[%d]", databaseHeader->file_version);
		if (databaseHeader->file_version != 1009)
		{
			myPrint(" -> Unsupported Version. Results might be invalid! \n");
		}
		else
		{
			myPrint("\n");
		}
		myPrint("iCDB Server Version: \t[%d]\n", databaseHeader->iCDB_version);
		myPrint("iCDB Database: \t\t[%s]\n", databaseHeader->iCDBdiagnostic);

		char temp[20];
		strftime(temp, 20, "%Y-%m-%d %H:%M:%S", localtime(&databaseHeader->edittime));
		myPrint("Time: \t\t\t[%s]\n", temp);
		myPrint("Location: \t\t[%s]\n", databaseHeader->filepath);
		myPrint("Mashine: \t\t[%s]\n", databaseHeader->pc_name);
		myPrint("User: \t\t\t[%s]\n", databaseHeader->user_name);
		myPrint("PID: \t\t\t[%d]\n", databaseHeader->pid);
		myPrint("Application: \t\t[%s]\n", databaseHeader->iCDB_string);
		myPrint("Operating System: \t[%s]\n", databaseHeader->os_version);
		myPrint("WDIR: \t\t\t[%s]\n", databaseHeader->settingspath);

		myPrint("Database GUID: \t\t");
		printGUID(databaseHeader->project_GUID);
		myPrint("\n");

		myPrint("Server GUID: \t\t");
		printGUID(databaseHeader->server_GUID);
		myPrint("\n");

		myPrint("Total Filesize: \t[%d]\n", filesize);
		myPrint("Unknown Value1: \t[%d]\n", databaseHeader->unknown1);
		myPrint("Unknown Value2: \t[%d]\t\t\n", databaseHeader->unknown2);
		myPrint("Unknown Value3: \t[%d] and [%d]\n", databaseHeader->unknown3[0], databaseHeader->unknown3[1]);
		
		
		if (databaseHeader->always_zero != 0)
		{
			myPrint("always_zero in header is not zero, but %d!\n", databaseHeader->always_zero);
		}
		if (databaseHeader->always_zero2 != 0)
		{
			myPrint("always_zero2 in header is not zero, but %d!\n", databaseHeader->always_zero2);
		}
		myPrint("The file was saved %d times.\n", databaseHeader->opening_counter);
		
		myPrint("******* Database Info End *******\n");
		myPrint("\n");
		return 0;
	}
	else
	{
		return -1;
	}
}

/*
******************************************************************
* - function name:	decodefileList()
*
* - description: 	Read in, and print data file list
*
* - parameter: 		file pointer; pointer to databaseHeader, pointer to fileList
*
* - return value: 	error code
******************************************************************
*/
int decodefileList(FILE* sourceFile, databaseHeader_Struct* databaseHeader, file_list_struct* fileList)
{
	// Read struct
	fread(fileList, sizeof(file_list_struct), 1, sourceFile);

	// Check if counter match
	if (fileList->file_cnt != fileList->file_cnt2)
	{
		myPrint("File counter mismatch! %d and %d \n", fileList->file_cnt, fileList->file_cnt2);
		return 1;
	}
	// Check number of entries
	if (fileList->file_cnt > 100)
	{
		myPrint("File counter to large! %d\n", fileList->file_cnt);
		return 1;
	}
	// Check start code
	if (fileList->magic != 0x6410)
	{
		myPrint("Startcode mismatch! %d\n", fileList->magic);
		return 1;
	}

	// Check "always zeros"
	for (int i = 0; i < 4; i++)
	{
		if (fileList->always_zero[i] != 0)
		{
			myPrint("always_zero[%d] in file entry header is not zero, but %d!\n", i, fileList->always_zero);
		}
	}
	if (fileList->always_zero2 != 0)
	{
		myPrint("always_zero2 in file entry header is not zero, but %d!\n", fileList->always_zero2);
	}
	return 0;
}

/*
******************************************************************
* - function name:	decodefile()
*
* - description: 	Read in file
*
* - parameter: 		file pointer; pointer to databaseHeader, pointer to file
*
* - return value: 	error code
******************************************************************
*/
int decodefile(FILE* sourceFile, file_struct* file)
{
	int Address = ftell(sourceFile);
	// Read file entry
	fread(file, sizeof(file_struct), 1, sourceFile);

	// Check address
	if (Address != file->file_address)
	{
		myPrint("Address mismatch! %d and %d\n", Address, file->file_address);
		return 1;
	}

	// Check filename length
	if ( file->filename_length < 160 && file->filename_length > 0)
	{
		file->filename[file->filename_length] = '\0'; // Terminate filename. just in case
	}
	else
	{
		myPrint("Wrong filename length: [%d]\n", file->filename_length);
		return 1;
	}

	return 0;
}

/*
******************************************************************
* - function name:	exportFiles()
*
* - description: 	Read in file payload, unite fragmented files, export files
*
* - parameter: 		file pointer; pointer to databaseHeader, pointer to file pointer
*
* - return value: 	error code
******************************************************************
*/
int exportFiles(FILE* sourceFile, databaseHeader_Struct* databaseHeader, file_struct** file)
{
	char* Payload = 0;
	fragment_struct fragment;
	unsigned int payload_lengthAcc = 0; // Accumulate length of multiple fragments
	uint32_t DuplicateTo = 0; // Duplicate files
	unsigned int DuplicateCnt = 0; // Count duplicate files
	unsigned int FragmentCnt = 0; // Count data fragments
	for (unsigned int i = 0; i < databaseHeader->num_files; i++)
	{
		myPrint("%d: reading % .*s \n", i + 1, file[i]->filename_length, file[i]->filename);
		// Check if payload is allocated to multiple files
		
		for (unsigned int j = 0; j < i; j++)
		{
			if(file[i]->data_address == file[j]->data_address)
			{
				DuplicateTo = j;
				DuplicateCnt++;
				myPrint("    File is identical to: [%s]!\n", file[DuplicateTo]->filename);
				break;
			}
		}
		
		myPrint("    GUID:\t\t");
		printGUID(file[i]->fileGUID);
		myPrint("\n");
		
		myPrint("    Readable data:\t[");
		for(int j = 0; j < 0x10; j++)
		{
			if(file[i]->data[j] >= 32) // Printable characters
			{
				myPrint("%c", file[i]->data[j]);
			}
			else // Control characters
			{
				myPrint(" 0x%02x ", file[i]->data[j]);
			}
		}
		myPrint("]\n");
		
		myPrint("    Total file size:\t[%d]\n", file[i]->data_size);
		if(DuplicateTo == 0 || (linkLongLongFiles == 0 && noLongLongFiles == 0))
		{
			// Create and open destination file
			FILE* destFile = myfopen("wb", storepath, storepathLength, file[i]->filename, file[i]->filename_length, 0);
			if (destFile != 0)
			{
				// Allocate memory
				Payload = (char*)malloc(file[i]->data_size);
				if (Payload != 0)
				{
					payload_lengthAcc = 0;
					fragment.next_fragment = file[i]->data_address;
					FragmentCnt = 0;
	
					// Load file fragments
					do {
						fseek(sourceFile, fragment.next_fragment, SEEK_SET);
						fread(&fragment, sizeof(fragment_struct), 1, sourceFile);

						// Read data if in range
						if ((fragment.payload_length != 0) && ((fragment.payload_length + payload_lengthAcc) <= file[i]->data_size))
						{
							fread(Payload + payload_lengthAcc, sizeof(char), fragment.payload_length, sourceFile);
						}
	
						payload_lengthAcc += fragment.payload_length;
						
						FragmentCnt++;
						// More than one data fragment
						if (FragmentCnt > 1)
						{
							myPrint("    Data fragment %d loaded!\n", FragmentCnt);
						}
						
						myPrint("    Fragment size:\t[%d]\n", fragment.payload_length);
						
						if (DuplicateTo != 0 && fragment.duplicates == 0 && FragmentCnt == 1)
							// Duplicates are only checked on the first data fragment, to allow files with a mix of unique and shared fragments
						{
							myPrint("    Duplicate error!\n");
							return 1;
						}
						
						
						// Repeat for each fragment
						} while (fragment.next_fragment != 0 && payload_lengthAcc < file[i]->data_size);
						
					// Compression check
					if (((uint8_t) * (Payload + 1) == 0xfd) &&
						((uint8_t) * (Payload + 2) == 0xff) &&
						((uint8_t) * (Payload + 3) == 0xff) &&
						((uint8_t) * (Payload + 4) == 0x01) &&
						nontDecompress == 0)
					{
						// Decompress file and write data
						char* decompressedData;
						int decompressedSize;
						decompressedSize = decompress((Payload + 5), file[i]->data_size - 5, &decompressedData);
						if (decompressedSize >= 1)
						{
							fwrite(decompressedData, sizeof(char), decompressedSize, destFile);
							free(decompressedData);
						}
					}
					else
					{
						// Just write data
						fwrite(Payload, sizeof(char), file[i]->data_size, destFile);
					}
					if (payload_lengthAcc != file[i]->data_size)
					{
						myPrint("    Wrong filesize %d and %d!\n", payload_lengthAcc, file[i]->data_size);
						return 1;
					}
					if(fragment.duplicates != 0)
					{
						myPrint("    Copies of data:\t[%d]\n", fragment.duplicates);
					}
					free(Payload);
				}
				fclose(destFile);
				myPrint("\n");
			}
			else
			{
				myPrint("writing % .*s failed!\n", file[i]->filename_length, file[i]->filename);
				return 1;
			}
		}
		else if (linkLongLongFiles == 1 && noLongLongFiles == 0)
		{ // Link
			myPrint("    Linking file [%s] to file [%s]\n", file[i]->filename, file[DuplicateTo]->filename);
			if(mylink(storepath, storepathLength, file[i]->filename, file[i]->filename_length, file[DuplicateTo]->filename, file[DuplicateTo]->filename_length) != 0)
			{
				return 1;
			}
			myPrint("\n");
		}
		else // (linkLongLongFiles == 0 && noLongLongFiles == 1)
		{
			myPrint("    Skipping file!\n\n");
		}
		DuplicateTo = 0;
	}
	if(DuplicateCnt != 0)
	{
		myPrint("[%d] total duplicate file(s) found!\n", DuplicateCnt);
	}

	return 0;
}

/*
******************************************************************
* - function name:	makePath()
*
* - description: 	Remove file ending from Database, Assemble & create filepath
*
* - parameter: 		pointer to first part; length of first part; pointer to second part; length of second part
*
* - return value: 	pointer to assembled path
******************************************************************
*/
char* makePath(char* absolutepath, int absolutepathlength, char* filepath, int filepathlength)
{
	char* returnpath = NULL;
	absolutepathlength -= 1; // Remove zero termination
	// Remove .dat ending from database path if present
	for (int i = 0; i < absolutepathlength - 3; i++)
	{
		if	(absolutepath[i] == '.' &&
			(absolutepath[i + 1] == 'd' || absolutepath[i + 1] == 'D') &&
			(absolutepath[i + 2] == 'a' || absolutepath[i + 2] == 'A') &&
			(absolutepath[i + 3] == 't' || absolutepath[i + 3] == 'T'))
		{
			absolutepathlength = i;
			break;
		}
	}
	createPath(&returnpath, absolutepath, absolutepathlength, filepath, filepathlength, '\0');
	return returnpath;
}

/*
******************************************************************
* - function name:	mylink()
*
* - description: 	Assemble & create filepath, create link to existing file
*
* - parameter: 		pointer to first part; length of first part; pointer to source; length of source; pointer to destination; length of destination
*
* - return value: 	error code
******************************************************************
*/
int mylink(char* absolutepath, int absolutepathlength, char* sourcepath, int sourcepathlength, char* destpath, int destpathlength)
{
	char* absolutesourcepath = makePath(absolutepath, absolutepathlength, sourcepath, sourcepathlength);
	char* absolutedestpath = makePath(absolutepath, absolutepathlength, destpath, destpathlength);
	int returnvalue = 1;
	if (absolutesourcepath != 0 && absolutedestpath != 0)
	{
		#ifdef WIN32 // Building for Windows
			remove(absolutesourcepath);
			returnvalue = CreateSymbolicLinkA(absolutesourcepath, absolutedestpath, 0);
			if(returnvalue == 0)
			{
				returnvalue = GetLastError();
				myPrint("    Error [%d]!", returnvalue);
				if(returnvalue == 1314) // Admin privileges required to create link
				{
					myPrint("Try to run the program as Administrator!");
				}
				myPrint("\n");
			}
			else
			{
				returnvalue = 0;
			}
		#else // Building for Unix
			returnvalue = symlink(absolutesourcepath, absolutedestpath);
		#endif
		free(absolutesourcepath);
		free(absolutedestpath);
	}
	return returnvalue;
}

/*
******************************************************************
* - function name:	printGUID()
*
* - description: 	Decode and print GUID to match format in DXD logs
*
* - parameter: 		guid to decode
*
* - return value: 	-
******************************************************************
*/
void printGUID(uint8_t Guid[24])
{
	// Format: Byte 0-7 = First Part, Byte 8-15 = Third Part, Byte 16-23 Second Part. Value in hex with the nibbles of each byte swapped.
	//						   (              First Part             ) [              Third Part             ] {              Second Part            }
	// Consider the hex value: 0xDE 0xAD 0xC0 0xFE 0x01 0x23 0x45 0x67 0x89 0xAB 0xCD 0xEF 0xDE 0xAD 0xC0 0xFE 0x01 0x23 0x45 0x67 0x89 0xAB 0xCD 0xEF
	// is decoded as: EDDA-0CEF-1032-5476-1032-5476-98BA-DCEF-98BA-DCEF-EDDA-0CEF
	//				  (   First Part    ) {   Second Part   } [   Third Part    ]
	unsigned char temp1 = 0;
	unsigned char temp2 = 0;
	myPrint("[");
	for (size_t i = 0; i < 4; i++)
	{
		// First 4 Byte
		temp1 = swpnib(Guid[i << 1]);
		temp2 = swpnib(Guid[(i << 1) + 1]);
		myPrint("%02x%02x-", temp1, temp2);
	}
	for (size_t i = 8; i < 12; i++)
	{
		// Next 4 Byte
		temp1 = swpnib(Guid[i << 1]);
		temp2 = swpnib(Guid[(i << 1) + 1]);
		myPrint("%02x%02x-", temp1, temp2);
	}
	for (size_t i = 4; i < 8; i++)
	{
		// Last 4 Byte
		temp1 = swpnib(Guid[i << 1]);
		temp2 = swpnib(Guid[(i << 1) + 1]);
		myPrint("%02x%02x", temp1, temp2);
		if (i < 7)
		{
			myPrint("-");
		}
	}
	myPrint("]");
}

/*
******************************************************************
* - function name:	decompress()
*
* - description: 	Decompress and write data to file. Uses Zlib
*
* - parameter: 		pointer to data to decompress; pointer to data to decompress; filepointer to destination
*
* - return value: 	error code
******************************************************************
*/
int decompress(char* input, int InputSize, char** output)
{
	char Output[DECOMPRESS_CHUNK_SIZE];
	z_stream ZStream;
	ZStream.zalloc = Z_NULL;
	ZStream.zfree = Z_NULL;
	ZStream.opaque = Z_NULL;
	ZStream.avail_in = InputSize;
	ZStream.next_in = (unsigned char*)input;
	int DecompressedSize = 0;

	void* list = list_init();
	myPrint("    Compressed file. decompressing...\n");

	// Init Decompression
	inflateInit(&ZStream);

	// Decompress data in chunks
	do
	{
		ZStream.avail_out = DECOMPRESS_CHUNK_SIZE;
		ZStream.next_out = (unsigned char*)&Output[0];
		int Returnvalue = inflate(&ZStream, Z_NO_FLUSH);

		//Error check
		if (Returnvalue > 1)
		{
			myPrint("    Decompression Error:\t[%d]\n", Returnvalue);
			return -1;
		}

		// Append data to list
		list_append(list, &Output[0], (DECOMPRESS_CHUNK_SIZE - ZStream.avail_out));

		// Reperat until end of file
	} while (ZStream.avail_out == 0);

	DecompressedSize = list_to_memblk(list, (void**)output);
	list_cleanup(&list);
	myPrint("    Decompressed size:\t[%d]\n", DecompressedSize);
	inflateEnd(&ZStream); // Just in case

	return DecompressedSize;
}