/*
******************************************************************
* Info
******************************************************************
* iCDBdecode
*

*/
#ifndef _UNPACK_H
#define _UNPACK_H

/*
******************************************************************
* Global Variables
******************************************************************
*/
extern int nontDecompress;
extern int linkLongLongFiles;
extern int noLongLongFiles;

/*
******************************************************************
* Global Functions
******************************************************************
*/
extern int UnpackIcdb(char*, int, char*, int);

#endif //_UNPACK_H