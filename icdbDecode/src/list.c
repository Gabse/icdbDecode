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
#include "list.h"
#include <stdlib.h>		// Required for calloc to work properly

/*
******************************************************************
* Global Defines
******************************************************************
*/
typedef struct list_element_struct
{
	void* previous_element;
	void* next_element;
	void* data;
	unsigned int lenth;
}list_element_struct;

/*
******************************************************************
* Global Functions
******************************************************************
*/

/*
******************************************************************
* - function name:	list_init()
*
* - description: 	initailizes a new list
*
* - parameter: 		-
*
* - return value: 	pointer to first list element;
******************************************************************
*/
void* list_init()
{
	list_element_struct* list = NULL;
	list = calloc(1, sizeof(list_element_struct)); // Reseve memory
	if (list != NULL)
	{
		list->previous_element = NULL;			// Link next element back
		list->next_element = NULL;				// Terminate list
		list->data = NULL;						// Zero data of next element
		list->lenth = 0;						// Zero lenth of next element
	}
	return list;
}

/*
******************************************************************
* - function name:	list_address()
*
* - description: 	returns the memory address of a specific list element
*
* - parameter: 		pointer to head element; index to element
*
* - return value: 	memory address of specified or last element
******************************************************************
*/
void* list_address(void* head, unsigned int address)
{
	list_element_struct* list = head;
	unsigned int element = 0;
	while (list->next_element != NULL && element < address) // Itterate to element or end of list
	{
		list = list->next_element;							// Go to next entry
		element++;											// Count up list elements
	}
	return list;
}

/*
******************************************************************
* - function name:	list_size()
*
* - description: 	count the total payload size of a list
*
* - parameter: 		pointer to head element
*
* - return value: 	size
******************************************************************
*/
unsigned int list_size(void* head)
{
	list_element_struct* list = head;
	unsigned int totalsize = 0;
	char* dest = NULL;
	while (list != NULL)					// Itterate over whole list
	{
		totalsize += list->lenth;			// Calculate total entry size
		list = list->next_element;			// Jump to next entry
	}
	return totalsize;
}

/*
******************************************************************
* - function name:	list_elements()
*
* - description: 	count the number of entrys in a list
*
* - parameter: 		pointer to head element
*
* - return value: 	number of elements in this list
******************************************************************
*/
unsigned int list_elements(void* head)
{
	list_element_struct* list = head;
	unsigned int returnvalue = 0;
	while (list != NULL)					// Itterate to end of list
	{
		list = list->next_element;			// Go to next entry
		returnvalue++;						// Count list elements
	}
	return returnvalue;
}

/*
******************************************************************
* - function name:	list_append()
*
* - description: 	appends data to the end of a list
*
* - parameter: 		pointer to head element; pointer to data; size of data
*
* - return value: 	number of elements in this list
******************************************************************
*/
unsigned int list_append(void* head, void* element, unsigned int size)
{
	list_add(head, 0xFFFFFFFF, element, size);	// Append to list
	return list_elements(head);					// Return size of list
}

/*
******************************************************************
* - function name:	list_add()
*
* - description: 	adds an element to an arbitrary position of a list
*
* - parameter: 		pointer to head element; index into list; pointer to data; size of data
*
* - return value: 	-
******************************************************************
*/
void list_add(void* head, unsigned int address, void* element, unsigned int size)
{
	list_element_struct* newelement = calloc(1, sizeof(list_element_struct));	// Reseve memory for new element
	if (newelement != NULL)
	{
		// Remap Data
		list_element_struct* currentelement = list_address(head, address);	// Insert new element before this element
		newelement->data = currentelement->data;
		newelement->lenth = currentelement->lenth;

		// New data
		currentelement->data = calloc(size, sizeof(char));			// Reseve memory for data
		memcpy(currentelement->data, element, size);				// Copy over data
		currentelement->lenth = size;								// Store size of data to new list entry

		// Insert newelement after currentelement in the list
		newelement->next_element = currentelement->next_element;
		currentelement->next_element = newelement;
		if (((list_element_struct*)(currentelement->previous_element)) != NULL) // Head element has no previous element
		{
			((list_element_struct*)(currentelement->previous_element))->next_element = newelement;
		}
		newelement->previous_element = currentelement->previous_element;
	}
}

/*
******************************************************************
* - function name:	list_remove()
*
* - description: 	removes an element from an arbitrary position of a list
*
* - parameter: 		pointer to head element; index into list
*
* - return value: 	-
******************************************************************
*/
void list_remove(void* head, unsigned int address)
{
	list_element_struct* currentelement = list_address(head, address);	// Remove this element
	free(currentelement->data);
	if (currentelement == head) // Head element can not be removed. copy data form next, and remove next
	{
		if (currentelement->next_element != NULL) // Next element existing
		{
			currentelement = currentelement->next_element;
			((list_element_struct*)head)->data = currentelement->data;
			((list_element_struct*)head)->lenth = currentelement->lenth;
			((list_element_struct*)head)->next_element = currentelement->next_element;
		}
		else
		{
			return;
		}
	}
	else
	{
		((list_element_struct*)(currentelement->previous_element))->next_element = currentelement->next_element;
	}
	if (((list_element_struct*)(currentelement->next_element)) != NULL) // last element has no next element either
	{
		((list_element_struct*)(currentelement->next_element))->previous_element = currentelement->previous_element;
	}
	free(currentelement);
}

/*
******************************************************************
* - function name:	list_get()
*
* - description: 	returns the data and lenth of a list element
*
* - parameter: 		pointer to head element; index into list; pointer to where the data is
*
* - return value: 	-
******************************************************************
*/
unsigned int list_get(void* head, unsigned int address, void** destination)
{
	list_element_struct* currentelement = list_address(head, address);	// Get element
	*destination = currentelement->data;
	return currentelement->lenth;
}

/*
******************************************************************
* - function name:	list_cleanup()
*
* - description: 	recursively removes list
*
* - parameter: 		pointer to head element
*
* - return value: 	-
******************************************************************
*/
void list_cleanup(void** head)
{
	list_element_struct* list = *head;
	list_element_struct* nextlist = NULL;
	while (list != NULL)			// Itterate over whole list
	{
		nextlist = list->next_element;	// Backup next entry address
		if (list->data != NULL)
		{
			free(list->data);		// Free data
		}
		free(list);					// Free list element
		list = nextlist;			// Jump to next list entry
	}
	head = NULL;					// Clear head element
}

/*
******************************************************************
* - function name:	list_to_memblk()
*
* - description: 	transfrorms a list into a block of memory
*
* - parameter: 		pointer to head element; pointer pointer to where to put the data
*
* - return value: 	size of block
******************************************************************
*/
unsigned int list_to_memblk(void* head, void** destination)
{
	list_element_struct* list = head;
	unsigned int totalsize = 0;
	char* dest = NULL;
	dest = calloc(list_size(head), sizeof(char));	// Reserve memory for entire data
	list = head;									// Return to first list element
	while (list->next_element != NULL)				// Itterate over whole list
	{
		memcpy(dest + totalsize, list->data, list->lenth);	// Append data from list to complete data block
		totalsize += list->lenth;					// Inc size
		list = list->next_element;					// Jump to next entry
	}
	*destination = dest;
	return totalsize;
}