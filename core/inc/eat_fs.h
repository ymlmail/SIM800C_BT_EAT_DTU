#if !defined(__EAT_FS_H__)
#define __EAT_FS_H__

#include "eat_type.h"
#include "eat_fs_errcode.h"
#include "eat_fs_type.h"
/*****************************************************************************
 * Function :eat_fs_Open
 * Description:Opens and possibly creates a file for subsequent read and/or write access.
 * Parameters :
 *  	FileName  : [IN] Must point to the name of the file to open/create. 
 *                    File names are not case sensitive; they will be converted to 
 *                    upper case when file system searches this file, and they should 
 *                    be two-byte aligned and UCS2 encoded. The file name can have one 
 *                    of the following formats:
 *                    [Drive:][\][Path\]Name[.Ext]  A data file or directory file name with optional drive and path information.
 *                    [Drive:]\                     A root directory.
 *  	Flag      : [IN] Can be a combination of the following flags: 
 *                      FS_READ_WRITE       FS_READ_ONLY          FS_CREATE       FS_CREATE_ALWAYS        FS_OPEN_DIR                                                                
 * Returns:
 *  	Please refer to eat_fs_error_enum.
 * NOTE 
 *  	If the function succeeds, the return value is a file handle for the 
 *  	opened file and the file pointer of the file is set to 0. 
 *  	If the return value is less than 0, the function has failed and the 
 *  	return value is the error code. 
 *  	For data and directory files, the same rules for file name syntax apply 
 *  	as under MS-DOS. 
 *  	For logical drive and physical disk file names, the same rules as under 
 *  	Windows NT apply.
 * 	    The function can continuous create most 24 files if all file handles have closed.
 *  	Internal FS drive name is "C:". 
 *  	SD card drive name is "D:".
 *****************************************************************************/
extern int (* const eat_fs_Open)(const unsigned short * FileName, UINT Flag);
    
/*****************************************************************************
 * Function :eat_fs_Close
 * Description:Closes an open file and return a file handle back.
 * Parameters :
 *  	FileHandle  : [IN] the handle. Must have been assigned in a previous 
 *                      successful call to eat_fs_Open. eat_fs_Close will write 
 *                      any un-flushed file data to the disk and release all 
 *                      resources associated with the file handle.
 * Returns:
 *  	Successful: EAT_FS_NO_ERROR
 *  	Failed:     Please refer to eat_fs_error_enum.
* NOTE
*     Nothing
 *****************************************************************************/
extern int (* const eat_fs_Close)(FS_HANDLE FileHandle);

/*****************************************************************************
 * Function :eat_fs_Read
 * Description:Reads data from an opened file.
 * Parameters :
 *  	FileHandle : [IN]  References the open file from which to read.
 *  	DataPtr    : [OUT] Specifies the address for the data to be read.
 *  	Length     : [IN]  Specifies the number of bytes to read
 *  	Read       : [OUT] Pointer to an unsigned integer to receive the number of 
 *                      bytes actually read. Usually, *Read will contain Length 
 *                      after the call. However, in case of an error or if the 
 *                      end of file is encountered during the read, the value may 
 *                      be less. Read may be set to NULL if this information is 
 *                      not required by an application.
 * Returns:
 *  	Successful: EAT_FS_NO_ERROR
 *  	Failed:     Please refer to eat_fs_error_enum.
 * NOTE
 *     Reading past the end of file is not regarded as an error. 
 *     If eat_fs_Read returns EAT_FS_NO_ERROR, but *Read is less than 
 *     Length, the end of file has been encountered.  This function 
 *     advances the file's file pointer by the amount given in 
 *     *Read. In case of an error and if parameter Read is NULL, 
 *     the new file pointer is undefined.
 *****************************************************************************/
extern int (* const eat_fs_Read)(FS_HANDLE FileHandle, void * DataPtr, UINT Length, UINT * Read);
                          
/*****************************************************************************
 * Function :eat_fs_Write
 * Description:Writes data to a file.
 * Parameters :
 *  	FileHandle : [IN]  References the open file to write to.
 *  	DataPtr    : [IN]  Specifies the address of the data to be written.
 *  	Length     : [IN]  Specifies the number of bytes to write.
 *  	Written    : [OUT] Pointer to an unsigned integer to receive the number of 
 *                      bytes actually been written. Usually, *Written will contain 
 *                      Length after the call. However, in case of an error, the 
 *                      returned value may be less. Written may be set to NULL if 
 *                      this information is not required by an application.
 * Returns:
 *  	Successful: EAT_FS_NO_ERROR
 *  	Failed:     Please refer to eat_fs_error_enum.
 * NOTE
 *     Writing past the current end of file will automatically 
 *     extend the file. When the file size exceeds the current 
 *     allocated file size, new clusters are allocated for the 
 *     file. FS will allocate new clusters immediately following 
 *     the current last cluster, if possible. This function 
 *     advances the file's file pointer by the amount given in 
 *     *Written. In case of an error and if parameter Written 
 *     is NULL, the new file pointer is undefined.
 *****************************************************************************/
extern int (* const eat_fs_Write)(FS_HANDLE FileHandle, void * DataPtr, UINT Length, UINT * Written);

/*****************************************************************************
 * Function :eat_fs_Seek
 * Description:Repositions a file pointer and possibly extends a file.
 * Parameters :
 *  	FileHandle  : [IN]  References the open file for which to reposition the file pointer.
 *  	Offset      : [IN]  Specifies how far the file pointer should be moved.
 *  	Whence      : [IN]  Specifies Offset's meaning. Please refer to EAT_FS_SEEK_POS_ENUM.
 * Returns:
 *  	If the function succeeds, the return value is the new file pointer value, or, 
 *  	if the file pointer is larger than (2^31)-1, EAT_FS_LONG_FILE_POS is returned. 
 *  	If the function fails, the return value is some other negative error code.
 *  	Moving the file pointer before the beginning of the file is an error. 
 *  	However, moving it beyond the current file size is supported. 
 *  	In this case, the file is extended. The data between the previous file size 
 *  	and the new file size is undefined. This method to extend a file is much faster 
 *  	than actually writing data to it.
 * NOTE
 *     Nothing 
 *****************************************************************************/
extern int (* const eat_fs_Seek)(FS_HANDLE FileHandle, int Offset, int Whence);

/*****************************************************************************
 * Function :eat_fs_Commit
 * Description:Immediately flushes all buffers associated with a file to disk.
 * Parameters :
 *  	FileHandle  : [IN] References the open file to be committed.
 * Returns:
 *  	Successful: EAT_FS_NO_ERROR
 *  	Failed:     Please refer to eat_fs_error_enum.
 * NOTE
 *  	FS_Commit guarantees that all of the file's data is flushed.
 *	The file's data buffer, directory entry, and the 
 *  	complete FAT are flushed.
 *****************************************************************************/
extern int (* const eat_fs_Commit)(FS_HANDLE FileHandle);

/*****************************************************************************
 * Function :eat_fs_GetFileSize
 * Description:Gets the current size of an open file.
 * Parameters :
 *  	FileHandle : [IN]    References the open file for which to retrieve the file size.
 *  	Size       : [OUT]   Must point to the int to receive the file's size.
 * Returns:
 *  	Successful  : EAT_FS_NO_ERROR
 *  	Failed      : Please refer to eat_fs_error_enum.
 * NOTE
 *     Nothing 
*****************************************************************************/
extern int (* const eat_fs_GetFileSize)(FS_HANDLE FileHandle, UINT * Size);

/*****************************************************************************
 * Function :eat_fs_GetFilePosition
 * Description:Gets the current file pointer position of an open file.
 * Parameters :
 * 	FileHandle : [IN]  File handle to reference the open file for which to retrieve the file pointer.
 *  	Position   : [OUT] The current file pointer of the opened file
 * Returns:
 *  	This function always returns EAT_FS_NO_ERROR. 
 * NOTE 
 *     Nothing 
 *****************************************************************************/
extern int (* const eat_fs_GetFilePosition)(FS_HANDLE FileHandle, UINT * Position);

/*****************************************************************************
 * Function :eat_fs_GetAttributes
 * Description:Returns the attributes set for a specific file.
 * Parameters :
 * 	FileName : [IN] It's the file path for which to retrieve the attributes. 
 * Returns:
 * 	The return value(s) below
 *  		FS_ATTR_READ_ONLY
 *  		FS_ATTR_HIDDEN
 *  		FS_ATTR_SYSTEM
 *  		FS_ATTR_VOLUME
 *  		FS_ATTR_DIR
 *  		FS_ATTR_ARCHIVE
 *  	Failed      : Please refer to eat_fs_error_enum.
 * NOTE
 *     Nothing 
 *****************************************************************************/
extern int (* const eat_fs_GetAttributes)(const WCHAR * FileName);

/*****************************************************************************
 * Function :eat_fs_SetAttributes
 * Description:Assigns a new set of attributes to a given file.
 * Parameters :
 *  	FileName    : [IN]  It's the name of the file for which to set the new attributes. 
 *  	Attributes  : [IN]  
 *  					FS_ATTR_READ_ONLY
 *  					FS_ATTR_HIDDEN
 *  					FS_ATTR_SYSTEM	
 *  					FS_ATTR_ARCHIVE
 * Returns:
 *  	Successful  : EAT_FS_NO_ERROR
 *  	Failed      : Please refer to eat_fs_error_enum.
 * NOTE
 *     Nothing 
 *****************************************************************************/
extern int (* const eat_fs_SetAttributes)(const WCHAR * FileName, BYTE Attributes);

/*****************************************************************************
 * Function :eat_fs_Delete
 * Description:Deletes a file.
 * Parameters :
 * 	FileName    : [IN]  Must point to the name of the file to be deleted and may not contain wildcards. 
 *					 This function cannot delete directories.
 * Returns:
 *  	Successful  : EAT_FS_NO_ERROR
 *  	Failed      : Please refer to eat_fs_error_enum.
 * NOTE
 *     Nothing 
 *****************************************************************************/
extern int (* const eat_fs_Delete)(const WCHAR * FileName);

/*****************************************************************************
 * Function :eat_fs_CreateDir
 * Description:Creates a new directory.
 * Parameters :
 * 	FileName: [IN]  It must point to the name of the directory to be created. The 
 *                  directory can have any legal file name syntax.
 * Returns:
 *  	Successful  : EAT_FS_NO_ERROR
 *  	Failed      : Please refer to eat_fs_error_enum.
 * NOTE
 *     Nothing 
 *****************************************************************************/
extern int (* const eat_fs_CreateDir)(const WCHAR * FileName);

/*****************************************************************************
 * Function :eat_fs_Truncate
 * Description:Sets the current file size to the current file pointer position.
 * Parameters :
 * 	FileHandle : [IN]  References the open file to be truncated.
 * Returns:
 *  	Successful  : EAT_FS_NO_ERROR
 *  	Failed      : Please refer to eat_fs_error_enum.
 * NOTE
 *     Nothing 
 *****************************************************************************/
extern int (* const eat_fs_Truncate)(FS_HANDLE FileHandle);

/*****************************************************************************
 * Function :eat_fs_RemoveDir
 * Description:Removes a directory.
 * Parameters :
 * 	DirName : [IN]  It must point to the name of the directory to be removed.
 * Returns:
 *  	Successful  : EAT_FS_NO_ERROR
 *  	Failed      : Please refer to eat_fs_error_enum.
 * NOTE
 *     Nothing 
 *****************************************************************************/
extern int (* const eat_fs_RemoveDir)(const WCHAR * DirName);


/*****************************************************************************
 * Function :eat_fs_GetFolderSize
 * Description:Gets the folder  size.
 * Parameters :
 * 	FullPath : [IN]  Full source path to be counted.
 * Returns:
 *  	Successful  : number of bytes.
 *  	Failed      : Please refer to eat_fs_error_enum.
 * NOTE
 *     Nothing 
 *****************************************************************************/
extern int (* const eat_fs_GetFolderSize)(const WCHAR * FullPath);

/*****************************************************************************
 * Function :eat_fs_GetDiskFreeSize
 * Description:To get the disk free size.
 * Parameters :
 * 	Disk : [IN]  EAT_FS is internal file systerm.
 * 	             EAT_EXTERN is SD card.
 *  Size : [OUT] Must point to the signed int64 to receive the size.
 * Returns:
 *  	Successful : EAT_FS_NO_ERROR.\n
 *  	Failed	   : Please refer to eat_fs_error_enum.
 * NOTE
 *     Nothing 
 *****************************************************************************/
extern int (* const eat_fs_GetDiskFreeSize)(EatDisk_enum Disk, SINT64 *Size);

/*****************************************************************************
 * Function :eat_fs_GetDiskSize
 * Description:To get the disk size.
 * Parameters :
 * 	Disk : [IN]  EAT_FS is internal file systerm.
 * 	             EAT_EXTERN is SD card.
 *  Size : [OUT] Must point to the signed int64 to receive the size.
 * Returns:
 *  	Successful : EAT_FS_NO_ERROR.\n
 *  	Failed	   : Please refer to eat_fs_error_enum.
 * NOTE
 *     Nothing 
 *****************************************************************************/
extern int (* const  eat_fs_GetDiskSize)(EatDisk_enum Disk, SINT64 *Size);

/*****************************************************************************
 * Function :eat_fs_Rename
 * Description:To rename a file.
 * Parameters :
 * 	FileName : [IN]  the name of the file to be renamed.
 * 	NewName : [IN]  the new name of the file.
 * Returns:
 *  	Successful  : EAT_FS_NO_ERROR
 *  	Failed      : Please refer to eat_fs_error_enum.
 * NOTE
 *     Nothing 
 *****************************************************************************/
extern int (* const  eat_fs_Rename)(const WCHAR * FileName, const WCHAR * NewName);

/*****************************************************************************
 * Function :eat_fs_FindFirst
 * Description:To search a directory for a file satisfying certain criteria.
 * Parameters :
 * 	NamePattern : [IN]  Pointer to a file name which may contain wildcard characters '*' 
 *                                (match zero or more characters) and/or '?' (match exactly one 
 *                                character), and can optionally be preceded by a path.
 * 	Attr : IN] Specifies a set of all file attributes a file must have to match the request.
 * 	AttrMask : [IN] Specified the set of attributes, which are matched against Attr. 
 * 	FileInfo : [IN/OUT] It is a pointer to FS_DOSDirEntry structure. If the function succeeds, 
 *                              this structure will be filled with the directory entry of the file found. .
 * 	FileName : [IN]  Pointer to a string buffer to receive the file name without path if a file is found.
 * 	MaxLength : [IN]  Size in bytes of the buffer pointed to by FileName.
 * Returns:
 *  	If the function succeeds, at least one file satisfies the search criteria. *FileInfo contains the  
 *    directory entry of the first file and the function return value is greater than or equal to 0. The  
 *    return value is a file handle, which may be passed to subsequent calls to FS_FindNext. If the function  
 *    fails, the return value is a negative error code. In this case, no handle is allocated and FS_FindClose  
 *    need not be called. 
 *    It is important to close the handle using FS_FindClose when no longer needed. Failing to do so will  
 *    quickly exhaust the available file handles
 * NOTE
 *     Nothing 
 *****************************************************************************/
extern int (* const  eat_fs_FindFirst)(const WCHAR * NamePattern, BYTE Attr, BYTE AttrMask, EAT_FS_DOSDirEntry * FileInfo, WCHAR * FileName, UINT MaxLength);

/*****************************************************************************
 * Function :eat_fs_FindNext
 * Description:To find more files with the same search criteria as a preceding call to FS_FindFirst.
 * Parameters :
 * 	FileHandle : [IN]  Handle returned by a previous successful call to FS_FindFirst.
 * 	FileInfo :  [IN/OUT] It is a pointer to FS_DOSDirEntry structure (may be NULL).
 * 	FileName : [IN]  It is a pointer to a string buffer to receive a file name without path
 * 	MaxLength : [IN]  Size in bytes of the buffer pointed to by FileName.
 * Returns:
 *  	If the function succeeds, the return value is FS_NO_ERROR. If the function fails or no more files  
 *   are found, the return value is a negative error code.If the function succeeds, *FileInfo and  
 *   *FileName will be filled with the directory entry and name of the file found.
 * NOTE
 *     Nothing 
 *****************************************************************************/
extern int (* const  eat_fs_FindNext)(FS_HANDLE FileHandle, EAT_FS_DOSDirEntry * FileInfo, WCHAR * FileName, UINT MaxLength);

/*****************************************************************************
 * Function :eat_fs_FindClose
 * Description:To close the handle of FS_FindFirst.
 * Parameters :
 * 	FileHandle : [IN]  the handle of FS_FindFirst.
 * Returns:
 *  	Successful  : EAT_FS_NO_ERROR
 *  	Failed      : Please refer to eat_fs_error_enum.
 * NOTE
 *     Nothing 
 *****************************************************************************/
extern int (* const  eat_fs_FindClose)(FS_HANDLE FileHandle);

/*****************************************************************************
 * Function :eat_fs_XDelete
 * Description: The function provides a facility to delete a file or folder recursively
 * Parameters :
 * 	FullPath : [IN]  Source path to be deleted
 * 	Flag : [IN] FS_FILE_TYPE, FS_DIR_TYPE, FS_RECURSIVE_TYPE
 * Returns:
 *  	Successful  : EAT_FS_NO_ERROR
 *  	Failed      : Please refer to eat_fs_error_enum.
 * NOTE
 *     Nothing 
 *****************************************************************************/
extern int (* const  eat_fs_XDelete)(const WCHAR * FullPath, UINT Flag);

#endif

