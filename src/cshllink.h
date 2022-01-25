/*
    This library provides an interface for the MS Shell Link Binary File Format in C [MS-SHLLINK](https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-shllink/16cb4ca1-9339-4d0c-a68d-bf1d6cc0f943)

    
    This library is completely based on [MS-SHLLINK](https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-shllink/16cb4ca1-9339-4d0c-a68d-bf1d6cc0f943)
    The descriptions of the variables are taken directly from the official documentation.

    Author: Felix Kröhnert
    License: GNU GPL V3 (refer to LICENSE for more information)
*/

#ifndef _CSHLLINK_H_
#define _CSHLLINK_H_

    #include <inttypes.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    typedef uint_least16_t char16_t;

    /*
        error handling

        0x00            NO ERROR
        0x01            FILE CLOSED
        0x02            FILE INPUT/OUTPUT ERROR
        0x03            NULL pointer argument
        0x04            WRONG HEADER SIZE
        0x05            WRONG HEADER CLSID
        0x06            NULL pointer IDList
        0x07            NULL pointer IDList Member
        0x08            Invalid IDList
        0x09            Invalid LinkInfoHeaderSize
        0x0A            VIDSIZE too low
        0x0B            NULL pointer VolumeID Data
        0x0C            NULL pointer LocalBasePath
        0x0D            NULL pointer CommonPathSuffix
        0x0E            NULL pointer LocalBasePathUnicode
        0x0F            NULL pointer CommonPathSuffixUnicode
        0x10            NULL pointer NetName
        0x11            NULL pointer DeviceName
        0x12            NULL pointer NetNameUnicode
        0x13            NULL pointer DeviceNameUnicode
        0x14            NULL pointer NameString
        0x15            NULL pointer RelativePath
        0x16            NULL pointer WorkingDir
        0x17            NULL pointer CommandLineArguments
        0x18            NULL pointer IconLocation
        0x19            Unknown ExtraDataBlock Signature
        0x1A            ExtraDataBlock ConsoleDataBlock duplicate
        0x1B            ExtraDataBlock ConsoleFEDataBlock duplicate
        0x1C            ExtraDataBlock DarwinDataBlock duplicate
        0x1D            ExtraDataBlock EnvironmentVariableDataBlock duplicate
        0x1E            ExtraDataBlock IconEnvironmentDataBlock duplicate
        0x1F            ExtraDataBlock KnownFolderDataBlock duplicate
        0x20            ExtraDataBlock PropertyStoreDataBlock duplicate
        0x21            ExtraDataBlock ShimDataBlock duplicate
        0x22            ExtraDataBlock SpecialFolderDataBlock duplicate
        0x23            ExtraDataBlock TrackerDataBlock duplicate
        0x24            ExtraDataBlock VistaAndAboveIDListDataBlock duplicate
        0x25            ExtraDataBlock Incorrect Block Size
        0x26            ExtraDataBlock Wrong Version
        0x27            NULL pointer ExtraDataBlock
        0x28            NULL pointer ExtraDataBlock FaceName
        0x29            NULL pointer ExtraDataBlock DarwinDataAnsi
        0x2A            NULL pointer ExtraDataBlock DarwinDataUnicode
        0x2B            NULL pointer ExtraDataBlock EnvironmentVariableDataAnsi
        0x2C            NULL pointer ExtraDataBlock EnvironmentVariableDataUnicode
        0x2D            NULL pointer ExtraDataBlock IconEnvironmentDataAnsi
        0x2E            NULL pointer ExtraDataBlock IconEnvironmentDataUnicode
    */
    extern uint8_t cshllink_error;
    #define _CSHLLINK_ERR_FCL 0x01
    #define _CSHLLINK_ERR_FIO 0x02
    #define _CSHLLINK_ERR_NULLPA 0x03
    #define _CSHLLINK_ERR_WHEADS 0x04
    #define _CSHLLINK_ERR_WCLSIDS 0x05
    #define _CSHLLINK_ERR_NULLPIDL 0x06
    #define _CSHLLINK_ERR_NULLPIDLM 0x07
    #define _CSHLLINK_ERR_INVIDL 0x08
    #define _CSHLLINK_ERR_INVLIHS 0x09
    #define _CSHLLINK_ERR_VIDSLOW 0x0A
    #define _CSHLLINK_ERR_NULLPVIDD 0x0B
    #define _CSHLLINK_ERR_NULLPLBP 0x0C
    #define _CSHLLINK_ERR_NULLPCPS 0x0D
    #define _CSHLLINK_ERR_NULLPLBPU 0x0E
    #define _CSHLLINK_ERR_NULLPCPSU 0x0F
    #define _CSHLLINK_ERR_NULLPNETN 0x10
    #define _CSHLLINK_ERR_NULLPDEVN 0x11
    #define _CSHLLINK_ERR_NULLPNNU 0x12
    #define _CSHLLINK_ERR_NULLPDNU 0x13
    #define _CSHLLINK_ERR_NULLPSTRDNAME 0x14
    #define _CSHLLINK_ERR_NULLPSTRDRPATH 0x15
    #define _CSHLLINK_ERR_NULLPSTRDWDIR 0x16
    #define _CSHLLINK_ERR_NULLPSTRDARG 0x17
    #define _CSHLLINK_ERR_NULLPSTRDICO 0x18
    #define _CSHLLINK_ERR_UNKEDBSIG 0x19
    #define _CSHLLINK_DUPEEX_ConsoleDataBlock 0x1A
    #define _CSHLLINK_DUPEEX_ConsoleFEDataBlock 0x1B
    #define _CSHLLINK_DUPEEX_DarwinDataBlock 0x1C
    #define _CSHLLINK_DUPEEX_EnvironmentVariableDataBlock 0x1D
    #define _CSHLLINK_DUPEEX_IconEnvironmentDataBlock 0x1E
    #define _CSHLLINK_DUPEEX_KnownFolderDataBlock 0x1F
    #define _CSHLLINK_DUPEEX_PropertyStoreDataBlock 0x20
    #define _CSHLLINK_DUPEEX_ShimDataBlock 0x21
    #define _CSHLLINK_DUPEEX_SpecialFolderDataBlock 0x22
    #define _CSHLLINK_DUPEEX_TrackerDataBlock 0x23
    #define _CSHLLINK_DUPEEX_VistaAndAboveIDListDataBlock 0x24
    #define _CSHLLINK_ERRX_WRONGSIZE 0x25
    #define _CSHLLINK_ERRX_WRONGVERSION 0x26
    #define _CSHLLINK_ERR_NULLPEXTD 0x27
    #define _CSHLLINK_ERRX_NULLPSTRFNAME 0x28
    #define _CSHLLINK_ERRX_NULLPSTRDARDA 0x29
    #define _CSHLLINK_ERRX_NULLPSTRDARDU 0x2A
    #define _CSHLLINK_ERRX_NULLPSTRENVDA 0x2B
    #define _CSHLLINK_ERRX_NULLPSTRENVDU 0x2C
    #define _CSHLLINK_ERRX_NULLPSTRIENVDA 0x2D
    #define _CSHLLINK_ERRX_NULLPSTRIENVDU 0x2E
    #define _cshllink_errint(errorval) {cshllink_error=errorval; return -1;}

    /*
        SHLLINK Header

        - contains identification information, timestamps, and flags that specify the presence of optional structures
    */
    struct _cshllink_header{
        // The size, in bytes, of this structure. This value MUST be 0x0000004C
        uint32_t HeaderSize;

        // A class identifier (CLSID). This value MUST be 00021401-0000-0000-C000-000000000046
        uint64_t LinkCLSID_L;
        uint64_t LinkCLSID_H;

        // A LinkFlags structure (section 2.1.1) that specifies information about the shell link and the presence of optional portions of the structure
        uint32_t LinkFlags;

        // A FileAttributesFlags structure (section 2.1.2) that specifies information about the link target
        uint32_t FileAttributes;

        // A FILETIME structure ([MS-DTYP] section 2.3.3) that specifies the creation time of the link target in UTC (Coordinated Universal Time). If the value is zero, there is no creation time set on the link target
        uint64_t CreationTime;

        // A FILETIME structure ([MS-DTYP] section 2.3.3) that specifies the access time of the link target in UTC (Coordinated Universal Time). If the value is zero, there is no access time set on the link target
        uint64_t AccessTime;

        // A FILETIME structure ([MS-DTYP] section 2.3.3) that specifies the write time of the link target in UTC (Coordinated Universal Time). If the value is zero, there is no write time set on the link target
        uint64_t WriteTime;

        // A 32-bit unsigned integer that specifies the size, in bytes, of the link target. If the link target file is larger than 0xFFFFFFFF, this value specifies the least significant 32 bits of the link target file size 
        uint32_t FileSize;

        // A 32-bit signed integer that specifies the index of an icon within a given icon location
        uint32_t IconIndex;
        
        /* A 32-bit unsigned integer that specifies the expected window state of an application launched by the link. This value SHOULD be one of the following

            SW_SHOWNORMAL       0x00000001   The application is open and its window is open in a normal fashion.
            SW_SHOWMAXIMIZED    0x00000003   The application is open, and keyboard focus is given to the application, but its window is not shown.
            SW_SHOWMINNOACTIVE  0x00000007   The application is open, but its window is not shown. It is not given the keyboard focus

            All other values MUST be treated as SW_SHOWNORMAL
        */
        uint32_t ShowCommand;

        // A HotKeyFlags structure (section 2.1.3) that specifies the keystrokes used to launch the application referenced by the shortcut key. This value is assigned to the application after it is launched, so that pressing the key activates that application
        uint16_t HotKey;

        /*
            + 10 bytes NULL
        */
    };
    #define CSHLLINK_SW_SHOWNORMAL 0x00000001
    #define CSHLLINK_SW_SHOWMAXIMIZED 0x00000003
    #define CSHLLINK_SW_SHOWMINNOACTIVE 0x00000007
    #define CSHLLINK_HEADERSIZE 0x0000004C
    #define CSHLLINK_LINKCLSID 0x46000000000000C00000000000021401

    /*
        The shell link is saved with an item ID list (IDList). If this bit is set, a
        LinkTargetIDList structure (section 2.2) MUST follow the ShellLinkHeader.
        If this bit is not set, this structure MUST NOT be present
    */
    #define CSHLLINK_LF_HasLinkTargetIDList 1<<0
    /*
        The shell link is saved with link information. If this bit is set, a LinkInfo
        structure (section 2.3) MUST be present. If this bit is not set, this structure
        MUST NOT be present
    */
    #define CSHLLINK_LF_HasLinkInfo 1<<1
    /*
        The shell link is saved with a name string. If this bit is set, a
        NAME_STRING StringData structure (section 2.4) MUST be present. If
        this bit is not set, this structure MUST NOT be present
    */
    #define CSHLLINK_LF_HasName 1<<2
    /*
        The shell link is saved with a relative path string. If this bit is set, a
        RELATIVE_PATH StringData structure (section 2.4) MUST be present. If
        this bit is not set, this structure MUST NOT be present
    */
    #define CSHLLINK_LF_HasRelativePath 1<<3
    /*
        The shell link is saved with a working directory string. If this bit is set, a
        WORKING_DIR StringData structure (section 2.4) MUST be present. If
        this bit is not set, this structure MUST NOT be present
    */
    #define CSHLLINK_LF_HasWorkingDir 1<<4
    /*
        The shell link is saved with command line arguments. If this bit is set, a
        COMMAND_LINE_ARGUMENTS StringData structure (section 2.4) MUST
        be present. If this bit is not set, this structure MUST NOT be present
    */
    #define CSHLLINK_LF_HasArguments 1<<5
    /*
        The shell link is saved with an icon location string. If this bit is set, an
        ICON_LOCATION StringData structure (section 2.4) MUST be present. If
        this bit is not set, this structure MUST NOT be present
    */
    #define CSHLLINK_LF_HasIconLocation 1<<6
    /*
        The shell link contains Unicode encoded strings. This bit SHOULD be set. If
        this bit is set, the StringData section contains Unicode-encoded strings;
        otherwise, it contains strings that are encoded using the system default
        code page
    */
    #define CSHLLINK_LF_IsUnicode 1<<7
    /*
        The LinkInfo structure (section 2.3) is ignored
    */
    #define CSHLLINK_LF_ForceNoLinkInfo 1<<8
    /*
        The shell link is saved with an
        EnvironmentVariableDataBlock (section 2.5.4)
    */
    #define CSHLLINK_LF_HasExpString 1<<9
    /*
        The target is run in a separate virtual machine when launching a link
        target that is a 16-bit application
    */
    #define CSHLLINK_LF_RunInSeparateProcess 1<<10
    // unused 1<<11
    /*
        The shell link is saved with a DarwinDataBlock (section 2.5.3)
    */
    #define CSHLLINK_LF_HasDarwinID 1<<12
    /*
        The application is run as a different user when the target of the shell link is
        activated
    */
    #define CSHLLINK_LF_RunAsUser 1<<13
    /*
        The shell link is saved with an IconEnvironmentDataBlock (section 2.5.5)
    */
    #define CSHLLINK_LF_HasExpIcon 1<<14
    /*
        The file system location is represented in the shell namespace when the
        path to an item is parsed into an IDList
    */
    #define CSHLLINK_LF_NoPidlAlias 1<<15
    // unused 1<<16
    /*
        The shell link is saved with a ShimDataBlock (section 2.5.8)
    */
    #define CSHLLINK_LF_RunWithShimLayer 1<<17
    /*
        The TrackerDataBlock (section 2.5.10) is ignored
    */
    #define CSHLLINK_LF_ForceNoLinkTrack 1<<18
    /*
        The shell link attempts to collect target properties and store them in the
        PropertyStoreDataBlock (section 2.5.7) when the link target is set
    */
    #define CSHLLINK_LF_EnableTargetMetadata 1<<19
    /*
        The EnvironmentVariableDataBlock is ignored
    */
    #define CSHLLINK_LF_DisableLinkPathTracking 1<<20
    /*
        The SpecialFolderDataBlock (section 2.5.9) and the KnownFolderDataBlock (section 2.5.6) are ignored when loading the shell
        link. If this bit is set, these extra data blocks SHOULD NOT be saved when
        saving the shell link
    */
    #define CSHLLINK_LF_DisableKnownFolderTracking 1<<21
    /*
        If the link has a KnownFolderDataBlock (section 2.5.6), the unaliased form
        of the known folder IDList SHOULD be used when translating the target
        IDList at the time that the link is loaded
    */
    #define CSHLLINK_LF_DisableKnownFolderAlias 1<<22
    /*
        Creating a link that references another link is enabled. Otherwise,
        specifying a link as the target IDList SHOULD NOT be allowed
    */
    #define CSHLLINK_LF_AllowLinkToLink 1<<23
    /*
        When saving a link for which the target IDList is under a known folder,
        either the unaliased form of that known folder or the target IDList SHOULD
        be used
    */
    #define CSHLLINK_LF_UnaliasOnSave 1<<24
    /*
        The target IDList SHOULD NOT be stored; instead, the path specified in the
        EnvironmentVariableDataBlock (section 2.5.4) SHOULD be used to refer to
        the target
    */
    #define CSHLLINK_LF_PreferEnvironmentPath 1<<25
    /*
        When the target is a UNC name that refers to a location on a local
        machine, the local path IDList in the
        PropertyStoreDataBlock (section 2.5.7) SHOULD be stored, so it can be
        used when the link is loaded on the local machine
    */
    #define CSHLLINK_LF_KeepLocalIDListForUNCTarget 1<<26
    // 5 bits 0

    
    /*
        The file or directory is read-only. For a file, if this bit is set,
        applications can read the file but cannot write to it or delete
        it. For a directory, if this bit is set, applications cannot delete
        the directory
    */
    #define CSHLLINK_FA_READONLY 1<<0
    /*
        The file or directory is read-only. For a file, if this bit is set,
        applications can read the file but cannot write to it or delete
        it. For a directory, if this bit is set, applications cannot delete
        the directory
    */
    #define CSHLLINK_FA_HIDDEN 1<<1
    /*
        The file or directory is part of the operating system or is used
        exclusively by the operating system
    */
    #define CSHLLINK_FA_SYSTEM 1<<2
    // reserved 1<<3 MUST be zero
    /*
        The link target is a directory instead of a file
    */
    #define CSHLLINK_FA_DIRECTORY 1<<4
    /*
        The file or directory is an archive file. Applications use this
        flag to mark files for backup or removal
    */
    #define CSHLLINK_FA_ARCHIVE 1<<5
    // reserved 1<<6 MUST be zero
    /*
        The file or directory has no other flags set. If this bit is 1, all
        other bits in this structure MUST be clear
    */
    #define CSHLLINK_FA_NORMAL 1<<7
    /*
        The file is being used for temporary storage
    */
    #define CSHLLINK_FA_TEMPORARY 1<<8
    /*
        The file is a sparse file
    */
    #define CSHLLINK_FA_SPARSE_FILE 1<<9
    /*
        The file or directory has an associated reparse point
    */
    #define CSHLLINK_FA_REPARSE_POINT 1<<10
    /*
        The file or directory is compressed. For a file, this means that
        all data in the file is compressed. For a directory, this means
        that compression is the default for newly created files and
        subdirectories
    */
    #define CSHLLINK_FA_COMPRESSED 1<<11
    /*
        The data of the file is not immediately available
    */
    #define CSHLLINK_FA_OFFLINE 1<<12
    /*
        The contents of the file need to be indexed
    */
    #define CSHLLINK_FA_NOT_CONTENT_INDEXED 1<<13
    /*
        The file or directory is encrypted. For a file, this means that
        all data in the file is encrypted. For a directory, this means
        that encryption is the default for newly created files and
        subdirectories
    */
    #define CSHLLINK_FA_ENCRYPTED 1<<14
    // 16 bit 0


    #define CSHLLINK_HOTK_NONE 0x00
    #define CSHLLINK_HOTK_0 0x30
    #define CSHLLINK_HOTK_1 0x31
    #define CSHLLINK_HOTK_2 0x32
    #define CSHLLINK_HOTK_3 0x33
    #define CSHLLINK_HOTK_4 0x34
    #define CSHLLINK_HOTK_5 0x35
    #define CSHLLINK_HOTK_6 0x36
    #define CSHLLINK_HOTK_7 0x37
    #define CSHLLINK_HOTK_8 0x38
    #define CSHLLINK_HOTK_9 0x39
    #define CSHLLINK_HOTK_A 0x41
    #define CSHLLINK_HOTK_B 0x42
    #define CSHLLINK_HOTK_C 0x43
    #define CSHLLINK_HOTK_D 0x44
    #define CSHLLINK_HOTK_E 0x45
    #define CSHLLINK_HOTK_F 0x46
    #define CSHLLINK_HOTK_G 0x47
    #define CSHLLINK_HOTK_H 0x48
    #define CSHLLINK_HOTK_I 0x49
    #define CSHLLINK_HOTK_J 0x4A
    #define CSHLLINK_HOTK_K 0x4B
    #define CSHLLINK_HOTK_L 0x4C
    #define CSHLLINK_HOTK_M 0x4D
    #define CSHLLINK_HOTK_N 0x4E
    #define CSHLLINK_HOTK_O 0x4F
    #define CSHLLINK_HOTK_P 0x50
    #define CSHLLINK_HOTK_Q 0x51
    #define CSHLLINK_HOTK_R 0x52
    #define CSHLLINK_HOTK_S 0x53
    #define CSHLLINK_HOTK_T 0x54
    #define CSHLLINK_HOTK_U 0x55
    #define CSHLLINK_HOTK_V 0x56
    #define CSHLLINK_HOTK_W 0x57
    #define CSHLLINK_HOTK_X 0x58
    #define CSHLLINK_HOTK_Y 0x59
    #define CSHLLINK_HOTK_Z 0x5A
    #define CSHLLINK_HOTK_F1 0x70
    #define CSHLLINK_HOTK_F2 0x71
    #define CSHLLINK_HOTK_F3 0x72
    #define CSHLLINK_HOTK_F4 0x73
    #define CSHLLINK_HOTK_F5 0x74
    #define CSHLLINK_HOTK_F6 0x75
    #define CSHLLINK_HOTK_F7 0x76
    #define CSHLLINK_HOTK_F8 0x77
    #define CSHLLINK_HOTK_F9 0x78
    #define CSHLLINK_HOTK_F10 0x79
    #define CSHLLINK_HOTK_F11 0x7A
    #define CSHLLINK_HOTK_F12 0x7B
    #define CSHLLINK_HOTK_F13 0x7C
    #define CSHLLINK_HOTK_F14 0x7D
    #define CSHLLINK_HOTK_F15 0x7E
    #define CSHLLINK_HOTK_F16 0x7F
    #define CSHLLINK_HOTK_F17 0x80
    #define CSHLLINK_HOTK_F18 0x81
    #define CSHLLINK_HOTK_F19 0x82
    #define CSHLLINK_HOTK_F20 0x83
    #define CSHLLINK_HOTK_F21 0x84
    #define CSHLLINK_HOTK_F22 0x85
    #define CSHLLINK_HOTK_F23 0x86
    #define CSHLLINK_HOTK_F24 0x87
    #define CSHLLINK_HOTK_NUMLOCK 0x90
    #define CSHLLINK_HOTK_SCROLL 0x91

    #define CSHLLINK_HOTKF_NONE 0x00
    #define CSHLLINK_HOTKF_SHIFT 0x01
    #define CSHLLINK_HOTKF_CONTROL 0x02
    #define CSHLLINK_HOTKF_ALT 0x03

    /*
        SHLLINK LinkTargetIDList

        - specifies the target of the link. The presence of this structure is specified by the HasLinkTargetIDList bit (LinkFlags section 2.1.1) in the ShellLinkHeader
    */
            /*
                SHLLINK LinkTargetIDList - IDList - IDItem

                - The data stored in a given ItemID is defined by the source that corresponds to the location in the target namespace of the preceding ItemIDs. This data uniquely identifies the items in that part of the namespace
            */
            struct _cshllink_lnktidl_idl_item{
                // item size in bytes
                uint16_t item_size;
                // The shell data source-defined data that specifies an item
                uint8_t *item;
            };
        /*
            SHLLINK LinkTargetIDList - IDList (terminated with TerminalBlock (2 Bytes NULL))
        */
        struct _cshllink_lnktidl_idl{
            // num of elements
            uint8_t idl_inum;
            // An array of zero or more ItemID structures (section 2.2.2)
            struct _cshllink_lnktidl_idl_item *idl_item;
        };
    struct _cshllink_lnktidl{
        /*
            List of IDList elements
        */
        // size in bytes = SUM(_cshllink_lnktidl_idl_item.item_size)
        uint16_t idl_size;
        // A stored IDList structure (section 2.2.1), which contains the item ID list
        struct _cshllink_lnktidl_idl cshllink_lnktidl_idl;
    };
    /*
        SHLLINK LinkInfo

        - specifies information necessary to resolve the link target. The presence of this structure is specified by the HasLinkInfo bit (LinkFlags section 2.1.1) in the ShellLinkHeader
    */
        /*
            SHLLINK LinkInfo - VolumeID

            - specifies information about the volume that a link target was on when the link was created. This information is useful for resolving the link if the file is not found in its original location
        */
        struct _cshllink_lnkinfo_volid{
            // A 32-bit, unsigned integer that specifies the size, in bytes, of this structure. This value MUST be greater than 0x00000010. All offsets specified in this structure MUST be less than this value, and all strings contained in this structure MUST fit within the extent defined by this size
            uint32_t VolumeIDSize;
            // A 32-bit, unsigned integer that specifies the type of drive the link target is stored on
            /*
                DRIVE_UNKNOWN       0x00000000       The drive type cannot be determined.
                DRIVE_NO_ROOT_DIR   0x00000001       The root path is invalid; for example, there is no volume mounted at the path.
                DRIVE_REMOVABLE     0x00000002       The drive has removable media, such as a floppy drive, thumb drive, or flash card reader.
                DRIVE_FIXED         0x00000003       The drive has fixed media, such as a hard drive or flash drive.
                DRIVE_REMOTE        0x00000004       The drive is a remote (network) drive.
                DRIVE_CDROM         0x00000005       The drive is a CD-ROM drive.
                DRIVE_RAMDISK       0x00000006       The drive is a RAM disk.
            */
            uint32_t DriveType;
            // A 32-bit, unsigned integer that specifies the drive serial number of the volume the link target is stored on
            uint32_t DriveSerialNumber;
            // A 32-bit, unsigned integer that specifies the location of a string that contains the volume label of the drive that the link target is stored on. This value is an offset, in bytes, from the start of the VolumeID structure to a NULL-terminated string of characters, defined by the system default code page. The volume label string is located in the Data field of this structure. If the value of this field is 0x00000014, it MUST be ignored, and the value of the VolumeLabelOffsetUnicode field MUST be used to locate the volume label string
            uint32_t VolumeLabelOffset;
            // An optional, 32-bit, unsigned integer that specifies the location of a string that contains the volume label of the drive that the link target is stored on. This value is an offset, in bytes, from the start of the VolumeID structure to a NULL-terminated string of Unicode characters. The volume label string is located in the Data field of this structure. If the value of the VolumeLabelOffset field is not 0x00000014, this field MUST NOT be present; instead, the value of the VolumeLabelOffset field MUST be used to locate the volume label string
            uint32_t VolumeLabelOffsetUnicode;
            // A buffer of data that contains the volume label of the drive as a string defined by the system default code page or Unicode characters, as specified by preceding fields
            char *Data;
        };
        /*
        SHLLINK LinkInfo - CommonNetworkRelativeLink

        - specifies information about the network location where a link target is stored, including the mapped drive letter and the UNC path prefix. For details on UNC paths, see [MS-DFSNM] section 2.2.1.4
        */
        struct _cshllink_lnkinfo_cnetrlnk{
            // A 32-bit, unsigned integer that specifies the size, in bytes, of the CommonNetworkRelativeLink structure. This value MUST be greater than or equal to 0x00000014. All offsets specified in this structure MUST be less than this value, and all strings contained in this structure MUST fit within the extent defined by this size
            uint32_t CommonNetworkRelativeSize;
            // Flags that specify the contents of the DeviceNameOffset and NetProviderType fields
            uint32_t CommonNetworkRelativeLinkFlags;
            // A 32-bit, unsigned integer that specifies the location of the NetName field. This value is an offset, in bytes, from the start of the CommonNetworkRelativeLink structure
            uint32_t NetNameOffset;
            // A 32-bit, unsigned integer that specifies the location of the DeviceName field. If the ValidDevice flag is set, this value is an offset, in bytes, from the start of the CommonNetworkRelativeLink structure; otherwise, this value MUST be zero
            uint32_t DeviceNameOffset;
            // A 32-bit, unsigned integer that specifies the type of network provider. If the ValidNetType flag is set, this value MUST be one of the defined; otherwise, this value MUST be ignored
            uint32_t NetworkProviderType;
            // An optional, 32-bit, unsigned integer that specifies the location of the NetNameUnicode field. This value is an offset, in bytes, from the start of the CommonNetworkRelativeLink structure. This field MUST be present if the value of the NetNameOffset field is greater than 0x00000014; otherwise, this field MUST NOT be present
            uint32_t NetNameOffsetUnicode;
            // An optional, 32-bit, unsigned integer that specifies the location of the DeviceNameUnicode field. This value is an offset, in bytes, from the start of the CommonNetworkRelativeLink structure. This field MUST be present if the value of the NetNameOffset field is greater than 0x00000014; otherwise, this field MUST NOT be present
            uint32_t DeviceNameOffsetUnicode;
            // A NULL–terminated string, as defined by the system default code page, which specifies a server share path; for example, "\\server\share"
            char *NetName;
            // A NULL–terminated string, as defined by the system default code page, which specifies a device; for example, the drive letter "D:"
            char *DeviceName;
            // An optional, NULL–terminated, Unicode string that is the Unicode version of the NetName string. This field MUST be present if the value of the NetNameOffset field is greater than 0x00000014; otherwise, this field MUST NOT be present
            char *NetNameUnicode;
            // An optional, NULL–terminated, Unicode string that is the Unicode version of the DeviceName string. This field MUST be present if the value of the NetNameOffset field is greater than 0x00000014; otherwise, this field MUST NOT be present
            char *DeviceNameUnicode;
        };
    struct _cshllink_lnkinfo{
        // A 32-bit, unsigned integer that specifies the size, in bytes, of the LinkInfo structure. All offsets specified in this structure MUST be less than this value, and all strings contained in this structure MUST fit within the extent defined by this size
        uint32_t LinkInfoSize;
        // A 32-bit, unsigned integer that specifies the size, in bytes, of the LinkInfo header section, which is composed of the LinkInfoSize, LinkInfoHeaderSize, LinkInfoFlags VolumeIDOffset, LocalBasePathOffset, CommonNetworkRelativeLinkOffset, CommonPathSuffixOffset fields, and, if included, the LocalBasePathOffsetUnicode and CommonPathSuffixOffsetUnicode fields
        /*
        
        0x0000001C              Offsets to the optional fields are not specified.
        0x00000024 ≤ value      Offsets to the optional fields are specified

        In Windows, Unicode characters are stored in this structure if the data cannot be represented as ANSI characters due to truncation of the values. In this case, the value of the LinkInfoHeaderSize field is greater than or equal to 36
        */
        uint32_t LinkInfoHeaderSize;
        // Flags that specify whether the VolumeID, LocalBasePath, LocalBasePathUnicode, and CommonNetworkRelativeLink fields are present in this structure
        uint32_t LinkInfoFlags;
        // A 32-bit, unsigned integer that specifies the location of the VolumeID field. If the VolumeIDAndLocalBasePath flag is set, this value is an offset, in bytes, from the start of the LinkInfo structure; otherwise, this value MUST be zero
        uint32_t VolumeIDOffset;
        // A 32-bit, unsigned integer that specifies the location of the LocalBasePath field. If the VolumeIDAndLocalBasePath flag is set, this value is an offset, in bytes, from the start of the LinkInfo structure; otherwise, this value MUST be zero
        uint32_t LocalBasePathOffset;
        // A 32-bit, unsigned integer that specifies the location of the CommonNetworkRelativeLink field. If the CommonNetworkRelativeLinkAndPathSuffix flag is set, this value is an offset, in bytes, from the start of the LinkInfo structure; otherwise, this value MUST be zero
        uint32_t CommonNetworkRelativeLinkOffset;
        // A 32-bit, unsigned integer that specifies the location of the CommonPathSuffix field. This value is an offset, in bytes, from the start of the LinkInfo structure
        uint32_t CommonPathSuffixOffset;
        // An optional, 32-bit, unsigned integer that specifies the location of the LocalBasePathUnicode field. If the VolumeIDAndLocalBasePath flag is set, this value is an offset, in bytes, from the start of the LinkInfo structure; otherwise, this value MUST be zero. This field can be present only if the value of the LinkInfoHeaderSize field is greater than or equal to 0x00000024
        uint32_t LocalBasePathOffsetUnicode;
        // An optional, 32-bit, unsigned integer that specifies the location of the CommonPathSuffixUnicode field. This value is an offset, in bytes, from the start of the LinkInfo structure. This field can be present only if the value of the LinkInfoHeaderSize field is greater than or equal to 0x00000024
        uint32_t CommonPathSuffixOffsetUnicode;
        // An optional VolumeID structure (section 2.3.1) that specifies information about the volume that the link target was on when the link was created. This field is present if the VolumeIDAndLocalBasePath flag is set
        struct _cshllink_lnkinfo_volid cshllink_lnkinfo_volid;
        // An optional, NULL–terminated string, defined by the system default code page, which is used to construct the full path to the link item or link target by appending the string in the CommonPathSuffix field. This field is present if the VolumeIDAndLocalBasePath flag is set
        char *LocalBasePath;
        // An optional CommonNetworkRelativeLink structure (section 2.3.2) that specifies information about the network location where the link target is stored
        struct _cshllink_lnkinfo_cnetrlnk cshllink_lnkinfo_cnetrlnk;
        // A NULL–terminated string, defined by the system default code page, which is used to construct the full path to the link item or link target by being appended to the string in the LocalBasePath field
        char *CommonPathSuffix;
        // An optional, NULL–terminated, Unicode string that is used to construct the full path to the link item or link target by appending the string in the CommonPathSuffixUnicode field. This field can be present only if the VolumeIDAndLocalBasePath flag is set and the value of the LinkInfoHeaderSize field is greater than or equal to 0x00000024
        char16_t *LocalBasePathUnicode;
        // An optional, NULL–terminated, Unicode string that is used to construct the full path to the link item or link target by being appended to the string in the LocalBasePathUnicode field. This field can be present only if the value of the LinkInfoHeaderSize field is greater than or equal to 0x00000024
        char16_t *CommonPathSuffixUnicode;
    };

    /*
        If set, the VolumeID and LocalBasePath fields are present,
        and their locations are specified by the values of the
        VolumeIDOffset and LocalBasePathOffset fields,
        respectively. If the value of the LinkInfoHeaderSize field is
        greater than or equal to 0x00000024, the
        LocalBasePathUnicode field is present, and its location is
        specified by the value of the LocalBasePathOffsetUnicode
        field.
        If not set, the VolumeID, LocalBasePath, and
        LocalBasePathUnicode fields are not present, and the
        values of the VolumeIDOffset and LocalBasePathOffset
        fields are zero. If the value of the LinkInfoHeaderSize field
        is greater than or equal to 0x00000024, the value of the
        LocalBasePathOffsetUnicode field is zero
    */
    #define CSHLLINK_LIF_VolumeIDAndLocalBasePath 1<<0
    /*
        If set, the CommonNetworkRelativeLink field is present,
        and its location is specified by the value of the
        CommonNetworkRelativeLinkOffset field.
        If not set, the CommonNetworkRelativeLink field is not
        present, and the value of the
        CommonNetworkRelativeLinkOffset field is zero
    */
    #define CSHLLINK_LIF_CommonNetworkRelativeLinkAndPathSuffix 1<<1
    // 30 bit 0

    #define CSHLLINK_VIDS_DRIVE_UNKNOWN       0x00000000       //The drive type cannot be determined.
    #define CSHLLINK_VIDS_DRIVE_NO_ROOT_DIR   0x00000001       //The root path is invalid; for example, there is no volume mounted at the path.
    #define CSHLLINK_VIDS_DRIVE_REMOVABLE     0x00000002       //The drive has removable media, such as a floppy drive, thumb drive, or flash card reader.
    #define CSHLLINK_VIDS_DRIVE_FIXED         0x00000003       //The drive has fixed media, such as a hard drive or flash drive.
    #define CSHLLINK_VIDS_DRIVE_REMOTE        0x00000004       //The drive is a remote (network) drive.
    #define CSHLLINK_VIDS_DRIVE_CDROM         0x00000005       //The drive is a CD-ROM drive.
    #define CSHLLINK_VIDS_DRIVE_RAMDISK       0x00000006       //The drive is a RAM disk.


    /*
        If set, the DeviceNameOffset field contains an offset to the device name.
        If not set, the DeviceNameOffset field does not contain an offset to the device name, and
        its value MUST be zero
    */
    #define CSHLLINK_CNETRLNK_ValidDevice 1<<0
    /*
        If set, the NetProviderType field contains the network provider type.
        If not set, the NetProviderType field does not contain the network provider type, and its
        value MUST be zero
    */
    #define CSHLLINK_CNETRLNK_ValidNetType 1<<1
    // 30 bit 0

    #define CSHLLINK_NPT_WNNC_NET_AVID          0x001A0000
    #define CSHLLINK_NPT_WNNC_NET_DOCUSPACE     0x001B0000
    #define CSHLLINK_NPT_WNNC_NET_MANGOSOFT     0x001C0000
    #define CSHLLINK_NPT_WNNC_NET_SERNET        0x001D0000
    #define CSHLLINK_NPT_WNNC_NET_RIVERFRONT1   0X001E0000
    #define CSHLLINK_NPT_WNNC_NET_RIVERFRONT2   0x001F0000
    #define CSHLLINK_NPT_WNNC_NET_DECORB        0x00200000
    #define CSHLLINK_NPT_WNNC_NET_PROTSTOR      0x00210000
    #define CSHLLINK_NPT_WNNC_NET_FJ_REDIR      0x00220000
    #define CSHLLINK_NPT_WNNC_NET_DISTINCT      0x00230000
    #define CSHLLINK_NPT_WNNC_NET_TWINS         0x00240000
    #define CSHLLINK_NPT_WNNC_NET_RDR2SAMPLE    0x00250000
    #define CSHLLINK_NPT_WNNC_NET_CSC           0x00260000
    #define CSHLLINK_NPT_WNNC_NET_3IN1          0x00270000
    #define CSHLLINK_NPT_WNNC_NET_EXTENDNET     0x00290000
    #define CSHLLINK_NPT_WNNC_NET_STAC          0x002A0000
    #define CSHLLINK_NPT_WNNC_NET_FOXBAT        0x002B0000
    #define CSHLLINK_NPT_WNNC_NET_YAHOO         0x002C0000
    #define CSHLLINK_NPT_WNNC_NET_EXIFS         0x002D0000
    #define CSHLLINK_NPT_WNNC_NET_DAV           0x002E0000
    #define CSHLLINK_NPT_WNNC_NET_KNOWARE       0x002F0000
    #define CSHLLINK_NPT_WNNC_NET_OBJECT_DIRE   0x00300000
    #define CSHLLINK_NPT_WNNC_NET_MASFAX        0x00310000
    #define CSHLLINK_NPT_WNNC_NET_HOB_NFS       0x00320000
    #define CSHLLINK_NPT_WNNC_NET_SHIVA         0x00330000
    #define CSHLLINK_NPT_WNNC_NET_IBMAL         0x00340000
    #define CSHLLINK_NPT_WNNC_NET_LOCK          0x00350000
    #define CSHLLINK_NPT_WNNC_NET_TERMSRV       0x00360000
    #define CSHLLINK_NPT_WNNC_NET_SRT           0x00370000
    #define CSHLLINK_NPT_WNNC_NET_QUINCY        0x00380000
    #define CSHLLINK_NPT_WNNC_NET_OPENAFS       0x00390000
    #define CSHLLINK_NPT_WNNC_NET_AVID1         0X003A0000
    #define CSHLLINK_NPT_WNNC_NET_DFS           0x003B0000
    #define CSHLLINK_NPT_WNNC_NET_KWNP          0x003C0000
    #define CSHLLINK_NPT_WNNC_NET_ZENWORKS      0x003D0000
    #define CSHLLINK_NPT_WNNC_NET_DRIVEONWEB    0x003E0000
    #define CSHLLINK_NPT_WNNC_NET_VMWARE        0x003F0000
    #define CSHLLINK_NPT_WNNC_NET_RSFX          0x00400000
    #define CSHLLINK_NPT_WNNC_NET_MFILES        0x00410000
    #define CSHLLINK_NPT_WNNC_NET_MS_NFS        0x00420000
    #define CSHLLINK_NPT_WNNC_NET_GOOGLE        0x00430000

    /*
        SHLLINK StringData
    */
        /*
            SHLLINK StringData - Data
        */
       struct _cshllink_strdata_def{
           // A 16-bit, unsigned integer that specifies either the number of characters, defined by the system default code page, or the number of Unicode characters found in the String field. A value of zero specifies an empty string
           uint16_t CountCharacters;
           // An optional set of characters, defined by the system default code page, or a Unicode string with a length specified by the CountCharacters field. This string MUST NOT be NULL-terminated
           char16_t *UString;
       };
    struct _cshllink_strdata{
        // REFERED TO AS "Comment". An optional structure that specifies a description of the shortcut that is displayed to end users to identify the purpose of the shell link. This structure MUST be present if the HasName flag is set
        struct _cshllink_strdata_def NameString;
        // An optional structure that specifies the location of the link target relative to the file that contains the shell link. When specified, this string SHOULD be used when resolving the link. This structure MUST be present if the HasRelativePath flag is set
        struct _cshllink_strdata_def RelativePath;
        // An optional structure that specifies the file system path of the working directory to be used when activating the link target. This structure MUST be present if the HasWorkingDir flag is set
        struct _cshllink_strdata_def WorkingDir;
        // An optional structure that stores the command-line arguments that are specified when activating the link target. This structure MUST be present if the HasArguments flag is set
        struct _cshllink_strdata_def CommandLineArguments;
        // An optional structure that specifies the location of the icon to be used when displaying a shell link item in an icon view. This structure MUST be present if the HasIconLocation flag is set
        struct _cshllink_strdata_def IconLocation;
    };
            /*
            SHLLINK ExtraDataBlock - BlockInfo
            */
            struct _cshllink_extdatablk_blk_info{
                uint32_t BlockSize;
                uint32_t BlockSignature;
            };
        /*
            SHLLINK ExtraDataBlock - ConsoleDataBlock

            - specifies the display settings to use when a link target specifies an application that is run in a console window (In Windows environments, this is commonly known as a "command prompt" window)

        */
        #define _CSHLLINK_EDBLK_ConsoleDataBlockSiz 0x000000CC
        #define _CSHLLINK_EDBLK_ConsoleDataBlockSig 0xA0000002
        struct _cshllink_extdatablk_consdblk{
            // BlockSize MUST be 0x000000CC
            // BlockSignature MUST be 0xA0000002
            struct _cshllink_extdatablk_blk_info info;
            /*
                A 16-bit, unsigned integer that specifies the fill attributes that control the
                foreground and background text colors in the console window. The following bit definitions can be
                combined to specify 16 different values each for the foreground and background colors:

                FOREGROUND_BLUE         0x0001      The foreground text color contains blue.
                FOREGROUND_GREEN        0x0002      The foreground text color contains green.
                FOREGROUND_RED          0x0004      The foreground text color contains red.
                FOREGROUND_INTENSITY    0x0008      The foreground text color is intensified.
                BACKGROUND_BLUE         0x0010      The background text color contains blue.
                BACKGROUND_GREEN        0x0020      The background text color contains green.
                BACKGROUND_RED          0x0040      The background text color contains red.
                BACKGROUND_INTENSITY    0x0080      The background text color is intensified.

            */
            uint16_t FillAttributes;
            // A 16-bit, unsigned integer that specifies the fill attributes that control the foreground and background text color in the console window popup. The values are the same as for the FillAttributes field
            uint16_t PopupFillAttributes;
            // A 16-bit, signed integer that specifies the horizontal size (X axis), in characters, of the console window buffer
            uint16_t ScreenBufferSizeX;
            // A 16-bit, signed integer that specifies the vertical size (Y axis), in characters, of the console window buffer
            uint16_t ScreenBufferSizeY;
            // A 16-bit, signed integer that specifies the horizontal size (X axis), in characters, of the console window
            uint16_t WindowSizeX;
            // A 16-bit, signed integer that specifies the vertical size (Y axis), in characters, of the console window
            uint16_t WindowSizeY;
            // A 16-bit, signed integer that specifies the horizontal coordinate (X axis), in pixels, of the console window origin
            uint16_t WindowOriginX;
            // A 16-bit, signed integer that specifies the vertical coordinate (Y axis), in pixels, of the console window origin
            uint16_t WindowOriginY;
            /*8 bytes unused data*/
            // A 32-bit, unsigned integer that specifies the size, in pixels, of the font used in the console window. The two most significant bytes contain the font height and the two least significant bytes contain the font width. For vector fonts, the width is set to zero
            uint32_t FontSize;
            /*
                A 32-bit, unsigned integer that specifies the family of the font used in the
                console window. This value MUST be comprised of a font family and a font pitch. The values for
                the font family are shown in the following table:
                FF_DONTCARE     0x0000      The font family is unknown.
                FF_ROMAN        0x0010      The font is variable-width with serifs; for example, "Times New Roman".
                FF_SWISS        0x0020      The font is variable-width without serifs; for example, "Arial".
                FF_MODERN       0x0030      The font is fixed-width, with or without serifs; for example, "Courier New".
                FF_SCRIPT       0x0040      The font is designed to look like handwriting; for example, "Cursive".
                FF_DECORATIVE   0x0050      The font is a novelty font; for example, "Old English"

                A bitwise OR of one or more of the following font-pitch bits is added to the font family from the
                previous table:

                TMPF_NONE           0x0000      A font pitch does not apply.
                TMPF_FIXED_PITCH    0x0001      The font is a fixed-pitch font.
                TMPF_VECTOR         0x0002      The font is a vector font.
                TMPF_TRUETYPE       0x0004      The font is a true-type font.
                TMPF_DEVICE         0x0008      The font is specific to the device.
            */
            uint32_t FontFamily;
            // A 32-bit, unsigned integer that specifies the stroke weight of the font used in the console window
            /*
                700 ≤ value     A bold font.
                value < 700     A regular-weight font.
            */
            uint32_t FontWeight;
            // A 32-character Unicode string that specifies the face name of the font used in the console window
            char16_t *FaceName;
            // A 32-bit, unsigned integer that specifies the size of the cursor, in pixels, used in the console window
            /*
                value ≤ 25      A small cursor.
                26 — 50         A medium cursor.
                51 — 100        A large cursor.
            */
            uint32_t CursorSize;
            // A 32-bit, unsigned integer that specifies whether to open the console window in full-screen mode
            /*
                0x00000000              Full-screen mode is off.
                0x00000000 < value      Full-screen mode is on.
            */
            uint32_t FullScreen;
            // A 32-bit, unsigned integer that specifies whether to open the console window in QuikEdit mode. In QuickEdit mode, the mouse can be used to cut, copy, and paste text in the console window
            /*
                0x00000000              QuikEdit mode is off.
                0x00000000 < value      QuikEdit mode is on.
            */
            uint32_t QuickEdit;
            // A 32-bit, unsigned integer that specifies insert mode in the console window
            /*
                0x00000000              Insert mode is disabled.
                0x00000000 < value      Insert mode is enabled.
            */
            uint32_t InsertMode;
            // A 32-bit, unsigned integer that specifies auto-position mode of the console window
            /*
                0x00000000              The values of the WindowOriginX and WindowOriginY fields are used to position the console window.
                0x00000000 < value      The console window is positioned automatically
            */
            uint32_t AutoPosition;
            // A 32-bit, unsigned integer that specifies the size, in characters, of the buffer that is used to store a history of user input into the console window
            uint32_t HistoryBufferSize;
            // A 32-bit, unsigned integer that specifies the number of history buffers to use
            uint32_t NumberOfHistoryBuffers;
            // A 32-bit, unsigned integer that specifies whether to remove duplicates in the history buffer
            /*
                0x00000000              Duplicates are not allowed.
                0x00000000 < value      Duplicates are allowed.
            */
            uint32_t HistoryNoDup;
            // A table of 16 32-bit, unsigned integers specifying the RGB colors that are used for text in the console window. The values of the fill attribute fields FillAttributes and PopupFillAttributes are used as indexes into this table to specify the final foreground and background color for a character
            uint32_t ColorTable[16];
        };

        #define CSHLLINK_FILA_FOREGROUND_BLUE         0x0001      //The foreground text color contains blue.
        #define CSHLLINK_FILA_FOREGROUND_GREEN        0x0002      //The foreground text color contains green.
        #define CSHLLINK_FILA_FOREGROUND_RED          0x0004      //The foreground text color contains red.
        #define CSHLLINK_FILA_FOREGROUND_INTENSITY    0x0008      //The foreground text color is intensified.
        #define CSHLLINK_FILA_BACKGROUND_BLUE         0x0010      //The background text color contains blue.
        #define CSHLLINK_FILA_BACKGROUND_GREEN        0x0020      //The background text color contains green.
        #define CSHLLINK_FILA_BACKGROUND_RED          0x0040      //The background text color contains red.
        #define CSHLLINK_FILA_BACKGROUND_INTENSITY    0x0080      //The background text color is intensified.

        #define CSHLLINK_FONTF_FF_DONTCARE        0x0000      //The font family is unknown.
        #define CSHLLINK_FONTF_FF_ROMAN           0x0010      //The font is variable-width with serifs; for example, "Times New Roman".
        #define CSHLLINK_FONTF_FF_SWISS           0x0020      //The font is variable-width without serifs; for example, "Arial".
        #define CSHLLINK_FONTF_FF_MODERN          0x0030      //The font is fixed-width, with or without serifs; for example, "Courier New".
        #define CSHLLINK_FONTF_FF_SCRIPT          0x0040      //The font is designed to look like handwriting; for example, "Cursive".
        #define CSHLLINK_FONTF_FF_DECORATIVE      0x0050      //The font is a novelty font; for example, "Old English"

        #define CSHLLINK_FONTP_TMPF_NONE           0x0000      // A font pitch does not apply.
        #define CSHLLINK_FONTP_TMPF_FIXED_PITCH    0x0001      //The font is a fixed-pitch font.
        #define CSHLLINK_FONTP_TMPF_VECTOR         0x0002      //The font is a vector font.
        #define CSHLLINK_FONTP_TMPF_TRUETYPE       0x0004      //The font is a true-type font.
        #define CSHLLINK_FONTP_TMPF_DEVICE         0x0008      //The font is specific to the device.

        /*
            SHLLINK ExtraDataBlock - ConsoleFEDataBlock

            - The ConsoleFEDataBlock structure specifies the code page to use for displaying text when a link target specifies an application that is run in a console window
        */
        #define _CSHLLINK_EDBLK_ConsoleFEDataBlockSiz 0x0000000C
        #define _CSHLLINK_EDBLK_ConsoleFEDataBlockSig 0xA0000004
        struct _cshllink_extdatablk_consfdblk{
            // BlockSize MUST be 0x0000000C
            // BlockSignature MUST be 0xA0000004
            struct _cshllink_extdatablk_blk_info info;
            // A 32-bit, unsigned integer that specifies a code page language code identifier. For details concerning the structure and meaning of language code identifiers, see [MS-LCID]. For additional background information, see [MSCHARSET], [MSDN-CS], and [MSDOCS-CodePage]
            uint32_t CodePage;
        };
        /*
            SHLLINK ExtraDataBlock - DarwinDataBlock

            - The DarwinDataBlock structure specifies an application identifier that can be used instead of a link target IDList to install an application when a shell link is activated
        */
        #define _CSHLLINK_EDBLK_DarwinDataBlockSiz 0x00000314
        #define _CSHLLINK_EDBLK_DarwinDataBlockSig 0xA0000006
        struct _cshllink_extdatablk_darwdblk{
            // BlockSize MUST be 0x00000314
            // BlockSignature MUST be 0xA0000006
            struct _cshllink_extdatablk_blk_info info;
            // A NULL–terminated string, defined by the system default code page, which specifies an application identifier. This field SHOULD be ignored (260b)
            char *DarwinDataAnsi;
            // An optional, NULL–terminated, Unicode string that specifies an application identifier (In Windows, this is a Windows Installer (MSI) application descriptor. For more information, see [MSDN-MSISHORTCUTS]) (520b)
            char16_t *DarwinDataUnicode;
        };
        /*
            SHLLINK ExtraDataBlock - EnvironmentVariableDataBlock

            - The EnvironmentVariableDataBlock structure specifies a path to environment variable information when the link target refers to a location that has a corresponding environment variable
        */
        #define _CSHLLINK_EDBLK_EnvironmentVariableDataBlockSiz 0x00000314
        #define _CSHLLINK_EDBLK_EnvironmentVariableDataBlockSig 0xA0000001
        struct _cshllink_extdatablk_envdblk{
            // BlockSize MUST be 0x00000314
            // BlockSignature MUST be 0xA0000001
            struct _cshllink_extdatablk_blk_info info;
            // A NULL-terminated string, defined by the system default code page, which specifies a path to environment variable information (260b)
            char *TargetAnsi;
            // An optional, NULL-terminated, Unicode string that specifies a path to environment variable information (520b)
            char16_t *TargetUnicode;
        };
        /*
            SHLLINK ExtraDataBlock - IconEnvironmentDataBlock

            - The IconEnvironmentDataBlock structure specifies the path to an icon. The path is encoded using environment variables, which makes it possible to find the icon across machines where the locations vary but are expressed using environment variables
        */
        #define _CSHLLINK_EDBLK_IconEnvironmentDataBlockSiz 0x00000314
        #define _CSHLLINK_EDBLK_IconEnvironmentDataBlockSig 0xA0000007
        struct _cshllink_extdatablk_icoenvdblk{
            // BlockSize MUST be 0x00000314
            // BlockSignature MUST be 0xA0000007
            struct _cshllink_extdatablk_blk_info info;
            // A NULL-terminated string, defined by the system default code page, which specifies a path that is constructed with environment variables
            char *TargetAnsi;
            // An optional, NULL-terminated, Unicode string that specifies a path that is constructed with environment variables
            char16_t *TargetUnicode;
        };
        /*
            SHLLINK ExtraDataBlock - KnownFolderDataBlock

            - The KnownFolderDataBlock structure specifies the location of a known folder. This data can be used when a link target is a known folder to keep track of the folder so that the link target IDList can be translated when the link is loaded
        */
        #define _CSHLLINK_EDBLK_KnownFolderDataBlockSiz 0x0000001C
        #define _CSHLLINK_EDBLK_KnownFolderDataBlockSig 0xA000000B
        struct _cshllink_extdatablk_knownfdblk{
            // BlockSize MUST be 0x0000001C
            // BlockSignature MUST be 0xA000000B
            struct _cshllink_extdatablk_blk_info info;
            // A value in GUID packet representation ([MS-DTYP] section 2.3.4.2) that specifies the folder GUID ID
            uint8_t *KnownFolderID;
            // A 32-bit, unsigned integer that specifies the location of the ItemID of the first child segment of the IDList specified by KnownFolderID. This value is the offset, in bytes, into the link target IDList
            uint32_t Offset;
        };
        /*
            SHLLINK ExtraDataBlock - PropertyStoreDataBlock

            - A PropertyStoreDataBlock structure specifies a set of properties that can be used by applications to store extra data in the shell link
        */
        #define _CSHLLINK_EDBLK_PropertyStoreDataBlockSiz 0x0000000C
        #define _CSHLLINK_EDBLK_PropertyStoreDataBlockSig 0xA0000009
        struct _cshllink_extdatablk_propsdblk{
            // BlockSize MUST be >= 0x0000000C
            // BlockSignature MUST be 0xA0000009
            struct _cshllink_extdatablk_blk_info info;
            // A serialized property storage structure ([MS-PROPSTORE] section 2.2)
            uint8_t *PropertyStore;
        };
        /*
            SHLLINK ExtraDataBlock - ShimDataBlock

            - The ShimDataBlock structure specifies the name of a shim that can be applied when activating a link target
        */
        #define _CSHLLINK_EDBLK_ShimDataBlockSiz 0x00000088
        #define _CSHLLINK_EDBLK_ShimDataBlockSig 0xA0000008
        struct _cshllink_extdatablk_shimblk{
            // BlockSize MUST be >= 0x00000088
            // BlockSignature MUST be 0xA0000008
            struct _cshllink_extdatablk_blk_info info;
            // A Unicode string that specifies the name of a shim layer to apply to a link target when it is being activated
            char16_t *LayerName;
        };
        /*
            SHLLINK ExtraDataBlock - SpecialFolderDataBlock

            - The SpecialFolderDataBlock structure specifies the location of a special folder. This data can be used when a link target is a special folder to keep track of the folder, so that the link target IDList can be translated when the link is loaded
        */
        #define _CSHLLINK_EDBLK_SpecialFolderDataBlockSiz 0x00000010
        #define _CSHLLINK_EDBLK_SpecialFolderDataBlockSig 0xA0000005
        struct _cshllink_extdatablk_specfdblk{
            // BlockSize MUST be 0x00000010
            // BlockSignature MUST be 0xA0000005
            struct _cshllink_extdatablk_blk_info info;
            // A 32-bit, unsigned integer that specifies the folder integer ID
            uint32_t SpecialFolderID;
            // A 32-bit, unsigned integer that specifies the location of the ItemID of the first child segment of the IDList specified by SpecialFolderID. This value is the offset, in bytes, into the link target IDList
            uint32_t Offset;
        };
        /*
            SHLLINK ExtraDataBlock - TrackerDataBlock

            - The TrackerDataBlock structure specifies data that can be used to resolve a link target if it is not found in its original location when the link is resolved. This data is passed to the Link Tracking service [MS-DLTW] to find the link target
        */
        #define _CSHLLINK_EDBLK_TrackerDataBlockSiz 0x00000060
        #define _CSHLLINK_EDBLK_TrackerDataBlockSig 0xA0000003
        #define _CSHLLINK_EDBLK_TrackerDataBlockLen 0x00000058
        struct _cshllink_extdatablk_trackdblk{
            // BlockSize MUST be 0x00000060
            // BlockSignature MUST be 0xA0000003
            struct _cshllink_extdatablk_blk_info info;
            // A 32-bit, unsigned integer that specifies the size of the rest of the TrackerDataBlock structure, including this Length field. This value MUST be 0x00000058
            uint32_t Length;
            // A 32-bit, unsigned integer. This value MUST be 0x00000000
            uint32_t Version;
            // A NULL–terminated character string, as defined by the system default code page, which specifies the NetBIOS name of the machine where the link target was last known to reside
            char *MachineID;
            // Two values in GUID packet representation ([MS-DTYP] section 2.3.4.2) that are used to find the link target with the Link Tracking service, as described in [MS-DLTW]
            uint8_t *Droid;
            // Two values in GUID packet representation that are used to find the link target with the Link Tracking service
            uint8_t *DroidBirth;
        };
        /*
            SHLLINK ExtraDataBlock - VistaAndAboveIDListDataBlock

            - The VistaAndAboveIDListDataBlock structure specifies an alternate IDList that can be used instead of the LinkTargetIDList structure (section 2.2) on platforms that (The VistaAndAboveIDListDataBlock structure is not supported on Windows NT operating system, Windows 2000, Windows XP, or Windows Server 2003) support it
        */
        #define _CSHLLINK_EDBLK_VistaAndAboveIDListDataBlockSiz 0x0000000A
        #define _CSHLLINK_EDBLK_VistaAndAboveIDListDataBlockSig 0xA000000C
        struct _cshllink_extdatablk_viidldblk{
            // BlockSize MUST be >= 0x0000000A
            // BlockSignature MUST be 0xA000000C
            struct _cshllink_extdatablk_blk_info info;
            // An IDList structure (section 2.2.1)
            struct _cshllink_lnktidl_idl cshllink_lnktidl_idl;
        };
    /*
        SHLLINK ExtraDataBlock (terminated with TerminalBlock (4 Bytes NULL))
    */
    #define _CSHLLINK_EDBLK_NUM 11
    struct _cshllink_extdatablk{
        struct _cshllink_extdatablk_consdblk ConsoleDataBlock;
        struct _cshllink_extdatablk_consfdblk ConsoleFEDataBlock;
        struct _cshllink_extdatablk_darwdblk DarwinDataBlock;
        struct _cshllink_extdatablk_envdblk EnvironmentVariableDataBlock;
        struct _cshllink_extdatablk_icoenvdblk IconEnvironmentDataBlock;
        struct _cshllink_extdatablk_knownfdblk KnownFolderDataBlock;
        struct _cshllink_extdatablk_propsdblk PropertyStoreDataBlock;
        struct _cshllink_extdatablk_shimblk ShimDataBlock;
        struct _cshllink_extdatablk_specfdblk SpecialFolderDataBlock;
        struct _cshllink_extdatablk_trackdblk TrackerDataBlock;
        struct _cshllink_extdatablk_viidldblk VistaAndAboveIDListDataBlock;
    };


    /*
        SHLLINK Structure
    */
    typedef struct _cshllink{

        /*
            SHLLINK Header
        */
        struct _cshllink_header cshllink_header;
        /*
            SHLLINK LinkTargetIDList
        */
        struct _cshllink_lnktidl cshllink_lnktidl;
        /*
            SHLLINK LinkInfo
        */
        struct _cshllink_lnkinfo cshllink_lnkinfo;
        /*
        SHLLINK StringData
        */
        struct _cshllink_strdata cshllink_strdata;
        /*
        SHLLINK ExtraData
        */
        struct _cshllink_extdatablk cshllink_extdatablk;
    }cshllink;

    /*
        Functions
    */

    
    /*
        -> open file descriptor of type FILE (R mode)
        -> cshllink structure pointer containing the FILE content
        -- reads content, modifies cshllink structure
        <- on error this function will return -1 (the content of inputStruct is undefined), on success 0

        exact error codes are stored in cshllink_error
    */
    uint8_t cshllink_loadFile(FILE *fp, cshllink *inputStruct);
    
    /*
        Processes inputFile
    */
    uint8_t cshllink_loadFile_i(FILE *fp, cshllink *inputStruct);

    /*
        -> open file descriptor of type FILE (W mode)
        -> cshllink structure pointer containing the FILE content
        -- writes content to file
        <- on error this function will return -1 (the content of inputStruct is undefined), on success 0

        exact error codes are stored in cshllink_error
    */
    uint8_t cshllink_writeFile(FILE *fp, cshllink *inputStruct);
    
    /*
        Processes outputFile
    */
    uint8_t cshllink_writeFile_i(FILE *fp, cshllink *inputStruct);
    
    /*
        Converts little Endian to big Endian and vice versa
    */
    void cshllink_sEndian(void *inp, size_t size);
    
    /*
        read NULL terminated String
    */
    uint32_t cshllink_rNULLstr(char **dest, uint8_t errv1, uint8_t errv2, FILE *fp);
    uint32_t cshllink_rNULLwstr(char16_t **dest, uint8_t errv1, uint8_t errv2, FILE *fp);
    
    /*
        read String
    */
    uint32_t cshllink_rstr(char **dest, uint8_t errv1, uint8_t errv2, FILE *fp, size_t size);
    uint32_t cshllink_rwstr(char16_t **dest, uint8_t errv1, uint8_t errv2, FILE *fp, size_t size);

    /*
        write NULL terminated String
    */
    uint32_t cshllink_wNULLstr(char **dest, uint8_t errv1, uint8_t errv2, FILE *fp);
    uint32_t cshllink_wNULLwstr(char16_t **dest, uint8_t errv1, uint8_t errv2, FILE *fp);
    
    /*
        write String
    */
    uint32_t cshllink_wstr(char **dest, uint8_t errv1, uint8_t errv2, FILE *fp, size_t size);
    uint32_t cshllink_wwstr(char16_t **dest, uint8_t errv1, uint8_t errv2, FILE *fp, size_t size);

    /*
        Extra Data Block read functions
    */
    uint8_t _cshllink_readEConsoleDataBlock(cshllink **input, const struct _cshllink_extdatablk_blk_info info, FILE *fp);
    uint8_t _cshllink_readEConsoleFEDataBlock(cshllink **input, const struct _cshllink_extdatablk_blk_info info, FILE *fp);
    uint8_t _cshllink_readEDarwinDataBlock(cshllink **input, const struct _cshllink_extdatablk_blk_info info, FILE *fp);
    uint8_t _cshllink_readEEnvironmentVariableDataBlock(cshllink **input, const struct _cshllink_extdatablk_blk_info info, FILE *fp);
    uint8_t _cshllink_readEIconEnvironmentDataBlock(cshllink **input, const struct _cshllink_extdatablk_blk_info info, FILE *fp);
    uint8_t _cshllink_readEKnownFolderDataBlock(cshllink **input, const struct _cshllink_extdatablk_blk_info info, FILE *fp);
    uint8_t _cshllink_readEPropertyStoreDataBlock(cshllink **input, const struct _cshllink_extdatablk_blk_info info, FILE *fp);
    uint8_t _cshllink_readEShimDataBlock(cshllink **input, const struct _cshllink_extdatablk_blk_info info, FILE *fp);
    uint8_t _cshllink_readESpecialFolderDataBlock(cshllink **input, const struct _cshllink_extdatablk_blk_info info, FILE *fp);
    uint8_t _cshllink_readETrackerDataBlock(cshllink **input, const struct _cshllink_extdatablk_blk_info info, FILE *fp);
    uint8_t _cshllink_readEVistaAndAboveIDListDataBlock(cshllink **input, const struct _cshllink_extdatablk_blk_info info, FILE *fp);

    /*
        Extra Data Block write functions
    */
    uint8_t _cshllink_writeEConsoleDataBlock(cshllink **input, FILE *fp);
    uint8_t _cshllink_writeEConsoleFEDataBlock(cshllink **input, FILE *fp);
    uint8_t _cshllink_writeEDarwinDataBlock(cshllink **input, FILE *fp);
    uint8_t _cshllink_writeEEnvironmentVariableDataBlock(cshllink **input, FILE *fp);
    uint8_t _cshllink_writeEIconEnvironmentDataBlock(cshllink **input, FILE *fp);
    uint8_t _cshllink_writeEKnownFolderDataBlock(cshllink **input, FILE *fp);
    uint8_t _cshllink_writeEPropertyStoreDataBlock(cshllink **input, FILE *fp);
    uint8_t _cshllink_writeEShimDataBlock(cshllink **input, FILE *fp);
    uint8_t _cshllink_writeESpecialFolderDataBlock(cshllink **input, FILE *fp);
    uint8_t _cshllink_writeETrackerDataBlock(cshllink **input, FILE *fp);
    uint8_t _cshllink_writeEVistaAndAboveIDListDataBlock(cshllink **input, FILE *fp);
    
    /*
        frees whole structure
    */
    void cshllink_free(cshllink *inputStruct);


    //IDList (also for VistaAndAboveIDList -- param idl pointer)
        /*
            set idl item
            IN: pointer to IDL item; data for item; length in bytes of data
            OUT: 0 - Success
                 255 - Error
        */
        uint8_t cshllink_setIDListItem(struct _cshllink_lnktidl_idl_item *item, uint8_t *data, uint16_t len);
        /*
            add idl item
            IN: pointer to List of IDL items; data for new item; length in bytes of data
            OUT: 0 - Success
                 255 - Error
        */
        uint8_t cshllink_addIDListItem(struct _cshllink_lnktidl_idl *list, uint8_t *data, uint16_t len);
        /*
            remove idl item
            IN: pointer to List of IDL items; index to be removed
            OUT: 0 - Success
                 255 - Error
        */
        uint8_t cshllink_removeIDListItem(struct _cshllink_lnktidl_idl *list, uint8_t index);

    //VOLID and LocalBasePath
        /*
            enable VolumeID and LocalBasePath
        */
        uint8_t cshllink_enableVolumeIDAndLocalBasePath(cshllink *inputStruct);
        /*
            disable VolumeID and LocalBasePath
        */
        uint8_t cshllink_disableVolumeIDAndLocalBasePath(cshllink *inputStruct);
        /*
            set VolumeIDData
        */  
        uint8_t cshllink_setVolumeIDData(cshllink *inputStruct, char *data, uint32_t size);
        /*
            set LocalBasePath
        */
        uint8_t cshllink_setLocalBasePath(cshllink *inputStruct, char *data);
        /*
            set LocalBasePathUnicode
        */
        uint8_t cshllink_setLocalBasePathUnicode(cshllink *inputStruct, char16_t *data);

    //CommonNetworkRelativeLink
        /*
            set NetName
        */
        uint8_t cshllink_setNetName(cshllink *inputStruct, char *data); 
        /*
            set DeviceName
        */ 
        uint8_t cshllink_setDeviceName(cshllink *inputStruct, char *data); 
        /*
            set NetNameUnicode
        */ 
        uint8_t cshllink_setNetNameUnicode(cshllink *inputStruct, char16_t *data); 
        /*
            set DeviceNameUnicode
        */
        uint8_t cshllink_setDeviceNameUnicode(cshllink *inputStruct, char16_t *data); 


    //LNKINFO
        /*
            set CommonPathSuffix
        */
        uint8_t cshllink_setCommonPathSuffix(cshllink *inputStruct, char *data);
        /*
            set CommonPathSuffixUnicode
        */
        uint8_t cshllink_setCommonPathSuffixUnicode(cshllink *inputStruct, char16_t *data);


    //StringData
        /*
            set NameString
        */
        uint8_t cshllink_setNameString(cshllink *inputStruct, char16_t *data, uint16_t len);
        /*
            set RelativePath
        */
        uint8_t cshllink_setRelativePath(cshllink *inputStruct, char16_t *data, uint16_t len);
        /*
            set WorkingDir
        */
        uint8_t cshllink_setWorkingDir(cshllink *inputStruct, char16_t *data, uint16_t len);
        /*
            set CommandLineArguments
        */
        uint8_t cshllink_setCommandLineArguments(cshllink *inputStruct, char16_t *data, uint16_t len);
        /*
            set IconLocation
        */
        uint8_t cshllink_setIconLocation(cshllink *inputStruct, char16_t *data, uint16_t len);


    //EXTRA DATA
        /*
            disable EXTDB
        */
        uint8_t cshllink_disableConsoleDB(cshllink *inputStruct);
        uint8_t cshllink_disableConsoleFEDB(cshllink *inputStruct);
        uint8_t cshllink_disableDarwinDB(cshllink *inputStruct);
        uint8_t cshllink_disableEnvironmentVariableDB(cshllink *inputStruct);
        uint8_t cshllink_disableIconEnvironmentDB(cshllink *inputStruct);
        uint8_t cshllink_disableKnownFolderDB(cshllink *inputStruct);
        uint8_t cshllink_disablePropertyStoreDB(cshllink *inputStruct);
        uint8_t cshllink_disableShimDB(cshllink *inputStruct);
        uint8_t cshllink_disableTrackerDB(cshllink *inputStruct);
        uint8_t cshllink_disableVistaAndAboveIDListDB(cshllink *inputStruct);
        /*
            enable EXTDB
        */
        uint8_t cshllink_enableConsoleDB(cshllink *inputStruct);
        uint8_t cshllink_enableConsoleFEDB(cshllink *inputStruct);
        uint8_t cshllink_enableDarwinDB(cshllink *inputStruct);
        uint8_t cshllink_enableEnvironmentVariableDB(cshllink *inputStruct);
        uint8_t cshllink_enableIconEnvironmentDB(cshllink *inputStruct);
        uint8_t cshllink_enableKnownFolderDB(cshllink *inputStruct);
        uint8_t cshllink_enablePropertyStoreDB(cshllink *inputStruct);
        uint8_t cshllink_enableShimDB(cshllink *inputStruct);
        uint8_t cshllink_enableTrackerDB(cshllink *inputStruct);
        uint8_t cshllink_enableVistaAndAboveIDListDB(cshllink *inputStruct);
        
        //ConsoleDB
        /*
            set FontFaceName (32 char)
        */
        uint8_t cshllink_setFontFaceName(cshllink *inputStruct, char16_t *faceName);

        //DarwinDB
        /*
            set DarwinDataAnsi (260 byte)
        */
        uint8_t cshllink_setDarwinDataAnsi(cshllink *inputStruct, char16_t *data);
        /*
            set DarwinDataUnicode (520 byte)
        */
        uint8_t cshllink_setDarwinDataUnicode(cshllink *inputStruct, char16_t *data);

        //EnvironmentVariableDB
        /*
            set TargetAnsi (260 byte)
        */
        uint8_t cshllink_setEnvironmentVariableTargetAnsi(cshllink *inputStruct, char *data);
        /*
            set TargetUnicode (520 byte)
        */
        uint8_t cshllink_setEnvironmentVariableTargetUnicode(cshllink *inputStruct, char16_t *data);

        //IconEnvironmentDB
        /*
            set TargetAnsi (260 byte)
        */
        uint8_t cshllink_setIconEnvironmentTargetAnsi(cshllink *inputStruct, char *data);
        /*
            set TargetUnicode (520 byte)
        */
        uint8_t cshllink_setIconEnvironmentTargetUnicode(cshllink *inputStruct, char16_t *data);

        //KnownFolderDB
        /*
            set KnownFolderID (16 byte)
        */
        uint8_t cshllink_setKnownFolderID(cshllink *inputStruct, uint8_t *knownFolderID);

        //PropertyStoreDB
        /*
            set PropteryStore (min 4 bytes) - "size" in bytes
        */
        uint8_t cshllink_setPropertyStore(cshllink *inputStruct, uint8_t *propertyStore, uint32_t size);

        //ShimDB
        /*
            set LayerName (min 128 bytes) - "size" in bytes
        */
        uint8_t cshllink_setShimLayerName(cshllink *inputStruct, char16_t *layerName, uint32_t size);

        //TrackerDB
        /*
            set MachineID (16 bytes)
        */
        uint8_t cshllink_setTrackerMachineID(cshllink *inputStruct, char *machineID);
        /*
            set Droid (32 byte)
        */
        uint8_t cshllink_setTrackerDroi(cshllink *inputStruct, uint8_t *droid);
        /*
            set DroidBirth (32 byte)
        */
        uint8_t cshllink_setTrackerDroidBirth(cshllink *inputStruct, uint8_t *droidBirth);
    
        //VistaAndAboveIDListDB
        /*
            set idl item
            IN: pointer to IDL item; data for item; length in bytes of data
            OUT: 0 - Success
                 255 - Error
        */
        uint8_t cshllink_setVistaAndAboveIDListItem(struct _cshllink_lnktidl_idl_item *item, uint8_t *data, uint16_t len);
        /*
            add idl item
            IN: pointer to List of IDL items; data for new item; length in bytes of data
            OUT: 0 - Success
                 255 - Error
        */
        uint8_t cshllink_addVistaAndAboveIDListItem(struct _cshllink_lnktidl_idl *list, uint8_t *data, uint16_t len);
        /*
            remove idl item
            IN: pointer to List of IDL items; index to be removed
            OUT: 0 - Success
                 255 - Error
        */
        uint8_t cshllink_removeVistaAndAboveIDListItem(struct _cshllink_lnktidl_idl *list, uint8_t index);

    /*
        converts ansi to unicode
    */
    uint8_t cshllink_ansiToUni(char16_t *dest, char *src);

#endif