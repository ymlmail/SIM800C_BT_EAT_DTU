#ifndef __EAT_FS_ERRCODE_H__
#define __EAT_FS_ERRCODE_H__

/* File System error codes */
typedef enum
{
    EAT_FS_NO_ERROR                    =    0,    /* Success */
    EAT_FS_ERROR_RESERVED              =   -1,    /* Reserved error code. Could be the initial value of result */
    EAT_FS_PARAM_ERROR                 =   -2,    /* Given parameters for current called FS function is wrong */
    EAT_FS_INVALID_FILENAME            =   -3,    /* Given path for current called FS function is wrong */
    EAT_FS_DRIVE_NOT_FOUND             =   -4,    /* Can't find specified drive */
    EAT_FS_TOO_MANY_FILES              =   -5,    /* Too many files are opened */
    EAT_FS_NO_MORE_FILES               =   -6,    /* There is no more files could be found. */
    EAT_FS_WRONG_MEDIA                 =   -7,    /* Fatal: FS_WRONG_MEDIA */
    EAT_FS_INVALID_FILE_SYSTEM         =   -8,    /* Fatal: FS_INVALID_FILE_SYSTEM */
    EAT_FS_FILE_NOT_FOUND              =   -9,    /* Can't find specified file */
    EAT_FS_INVALID_FILE_HANDLE         =  -10,    /* Given handle for current called FS function is wrong */
    EAT_FS_UNSUPPORTED_DEVICE          =  -11,    /* Specified device is not supported. */
    EAT_FS_UNSUPPORTED_DRIVER_FUNCTION =  -12,    /* Specified device function is not supported. */
    EAT_FS_CORRUPTED_PARTITION_TABLE   =  -13,    /* fatal: FS_CORRUPTED_PARTITION_TABLE */
    EAT_FS_TOO_MANY_DRIVES             =  -14,    /* Obsolete: FS_TOO_MANY_DRIVES */
    EAT_FS_INVALID_FILE_POS            =  -15,    /* Given file position to seek is wrong */
    EAT_FS_ACCESS_DENIED               =  -16,    /* Access denied. */
    EAT_FS_STRING_BUFFER_TOO_SMALL     =  -17,    /* Given buffer is too small */
    EAT_FS_GENERAL_FAILURE             =  -18,    /* Normal: FS_GENERAL_FAILURE */
    EAT_FS_PATH_NOT_FOUND              =  -19,    /* Can't find specified path(folder) */
    EAT_FS_FAT_ALLOC_ERROR             =  -20,    /* Fatal: disk crash */
    EAT_FS_ROOT_DIR_FULL               =  -21,    /* Root directory is full. */
    EAT_FS_DISK_FULL                   =  -22,    /* Disk is full. */
    EAT_FS_TIMEOUT                     =  -23,    /* Normal: FS_CloseMSDC with nonblock */
    EAT_FS_BAD_SECTOR                  =  -24,    /* Normal: NAND flash bad block(sector) */
    EAT_FS_DATA_ERROR                  =  -25,    /* Normal: NAND flash bad block(data) */
    EAT_FS_MEDIA_CHANGED               =  -26,    /* Hot-plug storge is removed. */
    EAT_FS_SECTOR_NOT_FOUND            =  -27,    /* Specified location is not found. */
    EAT_FS_ADDRESS_MARK_NOT_FOUND      =  -28,    /* Obsolete: not use so far */
    EAT_FS_DRIVE_NOT_READY             =  -29,    /* The specified device is not ready yet. */
    EAT_FS_WRITE_PROTECTION            =  -30,    /* Target media is protected from writing. Only for MSDC */
    EAT_FS_DMA_OVERRUN                 =  -31,    /* Obsolete: not use so far */
    EAT_FS_CRC_ERROR                   =  -32,    /* Obsolete: not use so far */
    EAT_FS_DEVICE_RESOURCE_ERROR       =  -33,    /* Fatal: Device crash */
    EAT_FS_INVALID_SECTOR_SIZE         =  -34,    /* Invalid sector size of the target media. It will cause mount failure. */
    EAT_FS_OUT_OF_BUFFERS              =  -35,    /* FS internal buffer is run out. */
    EAT_FS_FILE_EXISTS                 =  -36,    /* Specified file already exists. */
    EAT_FS_LONG_FILE_POS               =  -37,    /* User : FS_Seek new pos over sizeof int */
    EAT_FS_FILE_TOO_LARGE              =  -38,    /* User: filesize + pos over sizeof int */
    EAT_FS_BAD_DIR_ENTRY               =  -39,    /* The specified directory entry, file or folder, is invalid. */
    EAT_FS_ATTR_CONFLICT               =  -40,    /* User: Can't specify FS_PROTECTION_MODE and FS_NONBLOCK_MOD */
    EAT_FS_CHECKDISK_RETRY             =  -41,    /* System: don't care */
    EAT_FS_LACK_OF_PROTECTION_SPACE    =  -42,    /* Fatal: Device crash */
    EAT_FS_SYSTEM_CRASH                =  -43,    /* Normal: FS_SYSTEM_CRASH */
    EAT_FS_FAIL_GET_MEM                =  -44,    /* FS fails to get memory for the operation. */
    EAT_FS_READ_ONLY_ERROR             =  -45,    /* Caller is trying to write a read-only file. */
    EAT_FS_DEVICE_BUSY                 =  -46,    /* The target media is busy. */
    EAT_FS_ABORTED_ERROR               =  -47,    /* The operation was aborted. */
    EAT_FS_QUOTA_OVER_DISK_SPACE       =  -48,    /* Quota configuration mistake */
    EAT_FS_PATH_OVER_LEN_ERROR         =  -49,    /* The length of given path is over than spec. */
    EAT_FS_APP_QUOTA_FULL              =  -50,    /* App's folder is full. */
    EAT_FS_VF_MAP_ERROR                =  -51,    /* Virtual file is invalid. */
    EAT_FS_DEVICE_EXPORTED_ERROR       =  -52,    /* The target media is exported to PC. */
    EAT_FS_DISK_FRAGMENT               =  -53,    /* There are fragmentations in the target media. */
    EAT_FS_DIRCACHE_EXPIRED            =  -54,    /* The target file/folder is changed by some other background event. */
    EAT_FS_QUOTA_USAGE_WARNING         =  -55,    /* System Drive Free Space Not Enought */
    EAT_FS_ERR_DIRDATA_LOCKED          =  -56,    /* Normal */
    EAT_FS_INVALID_OPERATION           =  -57,    /* Normal */

    EAT_FS_MSDC_MOUNT_ERROR            = -100,    /* Fail to mount memory card. */
    EAT_FS_MSDC_READ_SECTOR_ERROR      = -101,    /* Fail to read from memory card. */
    EAT_FS_MSDC_WRITE_SECTOR_ERROR     = -102,    /* Fail to write to memory card. */
    EAT_FS_MSDC_DISCARD_SECTOR_ERROR   = -103,    /* Fail to discard data from memory card. */
    EAT_FS_MSDC_PRESNET_NOT_READY      = -104,    /* The memory card is inserted but not mouned yet. */
    EAT_FS_MSDC_NOT_PRESENT            = -105,    /* The memory card isn't there. */

    EAT_FS_EXTERNAL_DEVICE_NOT_PRESENT = -106,    /* The card reader isn't there. */
    EAT_FS_HIGH_LEVEL_FORMAT_ERROR     = -107,    /* Fail to format */

    EAT_FS_CARD_BATCHCOUNT_NOT_PRESENT = -110,    /* Obsolete: free cluster number is invalid. */

    EAT_FS_FLASH_MOUNT_ERROR           = -120,    /* Fail to mount NOR/NAND flash. */
    EAT_FS_FLASH_ERASE_BUSY            = -121,    /* Fail to erase from flash. Only for nonblock mode */
    EAT_FS_NAND_DEVICE_NOT_SUPPORTED   = -122,    /* Configuration Mistake: NAND type is not supported. */
    EAT_FS_FLASH_OTP_UNKNOWERR         = -123,    /* OTP unknown error. */
    EAT_FS_FLASH_OTP_OVERSCOPE         = -124,    /* The write address or length to OTP is not correct. */
    EAT_FS_FLASH_OTP_WRITEFAIL         = -125,    /* Fail to write to OTP. Might be written before */
    EAT_FS_FDM_VERSION_MISMATCH        = -126,    /* FDM has been upgraded. Need to format. */
    EAT_FS_FLASH_OTP_LOCK_ALREADY      = -127,    /* Fail to write to OTP because it's locked already */
    EAT_FS_FDM_FORMAT_ERROR            = -128,    /* The format of the disk content is not correct */

    EAT_FS_FDM_USER_DRIVE_BROKEN       = -129,    /* User drive unrecoverable errors are dectected */
    EAT_FS_FDM_SYS_DRIVE_BROKEN        = -130,    /* System drive unrecoverable errors are dectected */
    EAT_FS_FDM_MULTIPLE_BROKEN         = -131,    /* multiple unrecoverable errors are dectected on the flash device. */

    EAT_FS_LOCK_MUTEX_FAIL             = -141,    /* FS fails to lock specific resource. */
    EAT_FS_NO_NONBLOCKMODE             = -142,    /* User: try to call nonblock mode other than NOR flash */
    EAT_FS_NO_PROTECTIONMODE           = -143,    /* User: try to call protection mode other than NOR flash */

    /*
     * If disk size exceeds FS_MAX_DISK_SIZE (unit is MB, defined in custom_fs.h),
     * FS_TestMSDC(), FS_GetDevStatus(FS_MOUNT_STATE_ENUM) and all access behaviors will
     * get this error code.
     */
    EAT_FS_DISK_SIZE_TOO_LARGE         = (EAT_FS_MSDC_MOUNT_ERROR),

    EAT_FS_MINIMUM_ERROR_CODE          = -65536 /* 0xFFFF0000 */
} eat_fs_error_enum;
#endif


