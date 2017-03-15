#if !defined(__EAT_FS_TYPE__)
#define __EAT_FS_TYPE__


#define FS_MAX_PATH              64

/* Attributes of files or folders */
/* The entry is read only. Any attempt to open it with read/write access will return error "access denied". */
#define FS_ATTR_READ_ONLY        0x01 

/* The entry is marked as hidden. This attribute has no effect on any FS function. */
#define FS_ATTR_HIDDEN           0x02 

/* The entry is marked as being a system file. If a file in system drive brings this flag, file system will fix it if its size is abnormal instead of deleting it (default behavior). */
#define FS_ATTR_SYSTEM           0x04 

/* The entry is a volume label. Only a directory entry on a logical drive in the root directory can have this attribute set. */
#define FS_ATTR_VOLUME           0x08 

/* The entry is a directory. */
#define FS_ATTR_DIR              0x10 

/* The entry is marked as being backed up. This attribute has no effect on any FS function, but it is set on every write operation. */
#define FS_ATTR_ARCHIVE          0x20 

/* The entry is a long file name entry */
#define FS_LONGNAME_ATTR         0x0F 




/* FS_open flags */
/* The file is opened for read and write access. */
#define FS_READ_WRITE           0x00000000L 

/* The file is opened for read only access. */
#define FS_READ_ONLY            0x00000100L 

/* 
 * Overrides flag FS_OPEN_NO_DIR and forces support for opening a directory. 
 * <color red>Directories can only be opened with read only access.</color> 
 */
#define FS_OPEN_DIR             0x00000800L 
#define FS_CREATE               0x00010000L 
#define FS_CREATE_ALWAYS        0x00020000L 
#define FS_OPEN_NO_DIR          0x00000400L 

#define FS_FILE_TYPE             0x00000004     
#define FS_DIR_TYPE              0x00000008     
#define FS_RECURSIVE_TYPE        0x00000010     

/*-------------------- Type Define --------------------*/

// [NOTE] DWORD type definition is removed since W10.49 to avoid conflcting type with Windows's DWORD

typedef unsigned short WORD;
typedef unsigned short WCHAR;
typedef unsigned int   UINT;

typedef unsigned char  BYTE;
typedef void * HANDLE;
typedef int FS_HANDLE;
typedef signed long long     SINT64;


/* FS_Seek Parameter */
typedef enum
{
   EAT_FS_FILE_BEGIN,
   EAT_FS_FILE_CURRENT,
   EAT_FS_FILE_END
}EAT_FS_SEEK_POS_ENUM;

typedef enum {
    EAT_FS, /*internal file system*/
    EAT_EXTERN, /*extern file system,fo rexample SD card*/
    EAT_NONE_DRIVE /*none*/
} EatDisk_enum;

/* Date/time that the dir entry is created or modified */
typedef __packed struct
{
   unsigned int Second2:5; /* holds the seconds part of the desired time divided by 2. */
   unsigned int Minute:6;
   unsigned int Hour:5;
   unsigned int Day:5;
   unsigned int Month:4;
   unsigned int Year1980:7; /* holds the year part of the desired date minus 1980. */
} EAT_FS_DOSDateTime;


typedef struct
{
   char                    FileName[8];
   __packed char Extension[3];
   __packed BYTE Attributes;
   __packed BYTE NTReserved;
   __packed BYTE CreateTimeTenthSecond;
   EAT_FS_DOSDateTime          CreateDateTime;
   __packed WORD LastAccessDate;
   __packed WORD FirstClusterHi;
   EAT_FS_DOSDateTime          DateTime;
   __packed WORD FirstCluster;
   __packed UINT FileSize;
   // FS_FileOpenHint members (!Note that RTFDOSDirEntry structure is not changed!)
   UINT                    Cluster;
   UINT                    Index;
   UINT                    Stamp;
   UINT                    Drive;
   UINT                    SerialNumber;
} EAT_FS_DOSDirEntry;

#endif


