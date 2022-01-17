/*
    This library provides an interface for the MS Shell Link Binary File Format in C [MS-SHLLINK](https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-shllink/16cb4ca1-9339-4d0c-a68d-bf1d6cc0f943)

    

    Author: Felix Kröhnert
    License: GNU GPL V3 (refer to LICENSE for more information)
*/

#ifndef _CSHLLINK_H_
#define _CSHLLINK_H_

    #include <inttypes.h>

    /*
        SHLLINK Header

        - contains identification information, timestamps, and flags that specify the presence of optional structures
    */
    struct _cshllink_header{
        // The size, in bytes, of this structure. This value MUST be 0x0000004C
        uint32_t HeaderSize;

        // A class identifier (CLSID). This value MUST be 00021401-0000-0000-C000-000000000046
        uint64_t LinkCLSID;

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

            SW_SHOWNORMAL 0x00000001        The application is open and its window is open in a normal fashion.
            SW_SHOWMAXIMIZED 0x00000003     The application is open, and keyboard focus is given to the application, but its window is not shown.
            SW_SHOWMINNOACTIVE 0x00000007   The application is open, but its window is not shown. It is not given the keyboard focus

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
    #define CSHLLINK_LF_HasLinkTargetIDList 1<<31
    /*
        The shell link is saved with link information. If this bit is set, a LinkInfo
        structure (section 2.3) MUST be present. If this bit is not set, this structure
        MUST NOT be present
    */
    #define CSHLLINK_LF_HasLinkInfo 1<<30
    /*
        The shell link is saved with a name string. If this bit is set, a
        NAME_STRING StringData structure (section 2.4) MUST be present. If
        this bit is not set, this structure MUST NOT be present
    */
    #define CSHLLINK_LF_HasName 1<<29
    /*
        The shell link is saved with a relative path string. If this bit is set, a
        RELATIVE_PATH StringData structure (section 2.4) MUST be present. If
        this bit is not set, this structure MUST NOT be present
    */
    #define CSHLLINK_LF_HasRelativePath 1<<28
    /*
        The shell link is saved with a working directory string. If this bit is set, a
        WORKING_DIR StringData structure (section 2.4) MUST be present. If
        this bit is not set, this structure MUST NOT be present
    */
    #define CSHLLINK_LF_HasWorkingDir 1<<27
    /*
        The shell link is saved with command line arguments. If this bit is set, a
        COMMAND_LINE_ARGUMENTS StringData structure (section 2.4) MUST
        be present. If this bit is not set, this structure MUST NOT be present
    */
    #define CSHLLINK_LF_HasArguments 1<<26
    /*
        The shell link is saved with an icon location string. If this bit is set, an
        ICON_LOCATION StringData structure (section 2.4) MUST be present. If
        this bit is not set, this structure MUST NOT be present
    */
    #define CSHLLINK_LF_HasIconLocation 1<<25
    /*
        The shell link contains Unicode encoded strings. This bit SHOULD be set. If
        this bit is set, the StringData section contains Unicode-encoded strings;
        otherwise, it contains strings that are encoded using the system default
        code page
    */
    #define CSHLLINK_LF_IsUnicode 1<<24
    /*
        The LinkInfo structure (section 2.3) is ignored
    */
    #define CSHLLINK_LF_ForceNoLinkInfo 1<<23
    /*
        The shell link is saved with an
        EnvironmentVariableDataBlock (section 2.5.4)
    */
    #define CSHLLINK_LF_HasExpString 1<<22
    /*
        The target is run in a separate virtual machine when launching a link
        target that is a 16-bit application
    */
    #define CSHLLINK_LF_RunInSeparateProcess 1<<21
    // unused 1<<20
    /*
        The shell link is saved with a DarwinDataBlock (section 2.5.3)
    */
    #define CSHLLINK_LF_HasDarwinID 1<<19
    /*
        The application is run as a different user when the target of the shell link is
        activated
    */
    #define CSHLLINK_LF_RunAsUser 1<<18
    /*
        The shell link is saved with an IconEnvironmentDataBlock (section 2.5.5)
    */
    #define CSHLLINK_LF_HasExpIcon 1<<17
    /*
        The file system location is represented in the shell namespace when the
        path to an item is parsed into an IDList
    */
    #define CSHLLINK_LF_NoPidlAlias 1<<16
    // unused 1<<15
    /*
        The shell link is saved with a ShimDataBlock (section 2.5.8)
    */
    #define CSHLLINK_LF_RunWithShimLayer 1<<14
    /*
        The TrackerDataBlock (section 2.5.10) is ignored
    */
    #define CSHLLINK_LF_ForceNoLinkTrack 1<<13
    /*
        The shell link attempts to collect target properties and store them in the
        PropertyStoreDataBlock (section 2.5.7) when the link target is set
    */
    #define CSHLLINK_LF_EnableTargetMetadata 1<<13
    /*
        The EnvironmentVariableDataBlock is ignored
    */
    #define CSHLLINK_LF_DisableLinkPathTracking 1<<12
    /*
        The SpecialFolderDataBlock (section 2.5.9) and the KnownFolderDataBlock (section 2.5.6) are ignored when loading the shell
        link. If this bit is set, these extra data blocks SHOULD NOT be saved when
        saving the shell link
    */
    #define CSHLLINK_LF_DisableKnownFolderTracking 1<<11
    /*
        If the link has a KnownFolderDataBlock (section 2.5.6), the unaliased form
        of the known folder IDList SHOULD be used when translating the target
        IDList at the time that the link is loaded
    */
    #define CSHLLINK_LF_DisableKnownFolderAlias 1<<10
    /*
        Creating a link that references another link is enabled. Otherwise,
        specifying a link as the target IDList SHOULD NOT be allowed
    */
    #define CSHLLINK_LF_AllowLinkToLink 1<<9
    /*
        When saving a link for which the target IDList is under a known folder,
        either the unaliased form of that known folder or the target IDList SHOULD
        be used
    */
    #define CSHLLINK_LF_UnaliasOnSave 1<<8
    /*
        The target IDList SHOULD NOT be stored; instead, the path specified in the
        EnvironmentVariableDataBlock (section 2.5.4) SHOULD be used to refer to
        the target
    */
    #define CSHLLINK_LF_PreferEnvironmentPath 1<<7
    /*
        When the target is a UNC name that refers to a location on a local
        machine, the local path IDList in the
        PropertyStoreDataBlock (section 2.5.7) SHOULD be stored, so it can be
        used when the link is loaded on the local machine
    */
    #define CSHLLINK_LF_KeepLocalIDListForUNCTarget 1<<6
    // 5 bits 0

    
    /*
        The file or directory is read-only. For a file, if this bit is set,
        applications can read the file but cannot write to it or delete
        it. For a directory, if this bit is set, applications cannot delete
        the directory
    */
    #define CSHLLINK_FA_READONLY 1<<31
    /*
        The file or directory is read-only. For a file, if this bit is set,
        applications can read the file but cannot write to it or delete
        it. For a directory, if this bit is set, applications cannot delete
        the directory
    */
    #define CSHLLINK_FA_HIDDEN 1<<30
    /*
        The file or directory is part of the operating system or is used
        exclusively by the operating system
    */
    #define CSHLLINK_FA_SYSTEM 1<<29
    // reserved 1<<28 MUST be zero
    /*
        The link target is a directory instead of a file
    */
    #define CSHLLINK_FA_DIRECTORY 1<<27
    /*
        The file or directory is an archive file. Applications use this
        flag to mark files for backup or removal
    */
    #define CSHLLINK_FA_ARCHIVE 1<<26
    // reserved 1<<25 MUST be zero
    /*
        The file or directory has no other flags set. If this bit is 1, all
        other bits in this structure MUST be clear
    */
    #define CSHLLINK_FA_NORMAL 1<<24
    /*
        The file is being used for temporary storage
    */
    #define CSHLLINK_FA_TEMPORARY 1<<23
    /*
        The file is a sparse file
    */
    #define CSHLLINK_FA_SPARSE_FILE 1<<22
    /*
        The file or directory has an associated reparse point
    */
    #define CSHLLINK_FA_REPARSE_POINT 1<<21
    /*
        The file or directory is compressed. For a file, this means that
        all data in the file is compressed. For a directory, this means
        that compression is the default for newly created files and
        subdirectories
    */
    #define CSHLLINK_FA_COMPRESSED 1<<20
    /*
        The data of the file is not immediately available
    */
    #define CSHLLINK_FA_OFFLINE 1<<19
    /*
        The contents of the file need to be indexed
    */
    #define CSHLLINK_FA_NOT_CONTENT_INDEXED 1<<18
    /*
        The file or directory is encrypted. For a file, this means that
        all data in the file is encrypted. For a directory, this means
        that encryption is the default for newly created files and
        subdirectories
    */
    #define CSHLLINK_FA_ENCRYPTED 1<<17
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
            */
            struct _cshllink_lnktidl_idl_item{
                // item size in bytes
                uint16_t item_size;
                uint8_t *item;
            };
        /*
            SHLLINK LinkTargetIDList - IDList (terminated with TerminalBlock (2 Bytes NULL))
        */
        struct _cshllink_lnktidl_idl{
            // num of elements
            uint8_t idl_inum;
            struct _cshllink_lnktidl_idl_item *idl_item;
        };
    struct _cshllink_lnktidl{
        /*
            List of IDList elements
        */
        // size in bytes = _cshllink_lnktidl_idl_item.item_size
        uint16_t idl_size;
        struct _cshllink_lnktidl_idl cshllink_lnktidl_idl;
    };
    /*
        SHLLINK LinkInfo

        - specifies information necessary to resolve the link target. The presence of this structure is specified by the HasLinkInfo bit (LinkFlags section 2.1.1) in the ShellLinkHeader
    */
        /*
            SHLLINK LinkInfo - VolumeID
        */
        struct _cshllink_lnkinfo_volid{
            uint32_t VolumeIDSize;
            uint32_t DriveType;
            uint32_t DriveSerialNumber;
            uint32_t VolumeLabelOffset;
            uint32_t VolumeLabelOffsetUnicode;
            uint8_t *Data;
        };
        /*
        SHLLINK LinkInfo - CommonNetworkRelativeLink

        - are used to convey user interface and path identification information. The presence of these structures is specified by bits (LinkFlags section 2.1.1) in the ShellLinkHeader
        */
        struct _cshllink_lnkinfo_cnetrlnk{
            uint32_t CommonNetworkRelativeSize;
            uint32_t CommonNetworkRelativeLinkFlags;
            uint32_t NetNameOffset;
            uint32_t DeviceNameOffset;
            uint32_t NetworkProviderType;
            uint32_t NetNameOffsetUnicode;
            uint32_t DeviceNameOffsetUnicode;
            uint8_t *NetName;
            uint8_t *DeviceName;
            uint8_t *NetNameUnicode;
            uint8_t *DeviceNameUnicode;
        };
    struct _cshllink_lnkinfo{
        uint32_t LinkInfoSize;
        uint32_t LinkInfoHeaderSize;
        uint32_t LinkInfoFlags;
        uint32_t VolumeIDOffset;
        uint32_t LocalBasePathOffset;
        uint32_t CommonNetworkRelativeLinkOffset;
        uint32_t CommonPathSuffixOffset;
        uint32_t LocalBasePathOffsetUnicode;
        uint32_t CommonPathSuffixOffsetUnicode;
        struct _cshllink_lnkinfo_volid cshllink_lnkinfo_volid;
        uint8_t *LocalBasePath;
        struct _cshllink_lnkinfo_cnetrlnk cshllink_lnkinfo_cnetrlnk;
        uint8_t *CommonPathSuffix;
        uint8_t *LocalBasePathUnicode;
        uint8_t *CommonPathSuffixUnicode;
    };
    /*
        SHLLINK StringData
    */
        /*
            SHLLINK StringData - Data
        */
       struct _cshllink_strdata_def{
           uint16_t CountCharacters;
           uint8_t *String;
       };
    struct _cshllink_strdata{
        struct _cshllink_strdata_def NameString;
        struct _cshllink_strdata_def RelativePath;
        struct _cshllink_strdata_def WorkingDir;
        struct _cshllink_strdata_def CommandLineArguments;
        struct _cshllink_strdata_def IconLocation;
    };
    /*
        SHLLINK ExtraDataBlock (terminated with TerminalBlock (4 Bytes NULL))
    */
            /*
            SHLLINK ExtraDataBlock - BlockInfo
            */
            struct _cshllink_extdatablk_blk_info{
                uint32_t BlockSize;
                uint32_t BlockSignature;
            };
        /*
            SHLLINK ExtraDataBlock - ConsoleDataBlock
        */
        struct _cshllink_extdatablk_consdblk{
            struct _cshllink_extdatablk_blk_info info;
            uint16_t FillAttributes;
            uint16_t PopupFillAttributes;
            uint16_t ScreenBufferSizeX;
            uint16_t ScreenBufferSizeY;
            uint16_t WindowSizeX;
            uint16_t WindowSizeY;
            uint16_t WindowOriginX;
            uint16_t WindowOriginY;
            /*8 bytes unused data*/
            uint32_t FontSize;
            uint32_t FontFamily;
            uint32_t FontWeight;
            uint8_t FaceName[65];
            uint32_t CursorSize;
            uint32_t FullScreen;
            uint32_t QuickEdit;
            uint32_t InsertMode;
            uint32_t AutoPosition;
            uint32_t HistoryBufferSize;
            uint32_t NumberOfHistoryBuffers;
            uint32_t HistoryNoDup;
            uint32_t ColorTable[16];
        };
        /*
            SHLLINK ExtraDataBlock - ConsoleFEDataBlock
        */
        struct _cshllink_extdatablk_consfdblk{
            struct _cshllink_extdatablk_blk_info info;
            uint32_t CodePage;
        };
        /*
            SHLLINK ExtraDataBlock - DarwinDataBlock
        */
        struct _cshllink_extdatablk_darwdblk{
            struct _cshllink_extdatablk_blk_info info;
            uint8_t DarwinDataAnsi[260];
            uint8_t DarwinDataUnicode[520];
        };
        /*
            SHLLINK ExtraDataBlock - EnvironmentVariableDataBlock
        */
        struct _cshllink_extdatablk_envdblk{
            struct _cshllink_extdatablk_blk_info info;
            uint8_t TargetAnsi[260];
            uint8_t TargetUnicode[520];
        };
        /*
            SHLLINK ExtraDataBlock - IconEnvironmentDataBlock
        */
        struct _cshllink_extdatablk_icoenvdblk{
            struct _cshllink_extdatablk_blk_info info;
            uint8_t TargetAnsi[260];
            uint8_t TargetUnicode[520];
        };
        /*
            SHLLINK ExtraDataBlock - KnownFolderDataBlock
        */
        struct _cshllink_extdatablk_knownfdblk{
            struct _cshllink_extdatablk_blk_info info;
            uint8_t KnownFolderID[16];
            uint32_t Offset;
        };
        /*
            SHLLINK ExtraDataBlock - PropertyStoreDataBlock
        */
        struct _cshllink_extdatablk_propsdblk{
            struct _cshllink_extdatablk_blk_info info;
            uint8_t *PropertyStore;
        };
        /*
            SHLLINK ExtraDataBlock - ShimDataBlock
        */
        struct _cshllink_extdatablk_shimblk{
            struct _cshllink_extdatablk_blk_info info;
            uint8_t *LayerName;
        };
        /*
            SHLLINK ExtraDataBlock - SpecialFolderDataBlock
        */
        struct _cshllink_extdatablk_specfdblk{
            struct _cshllink_extdatablk_blk_info info;
            uint32_t SpecialFolderID;
            uint32_t Offset;
        };
        /*
            SHLLINK ExtraDataBlock - TrackerDataBlock
        */
        struct _cshllink_extdatablk_trackdblk{
            struct _cshllink_extdatablk_blk_info info;
            uint32_t Length;
            uint32_t Version;
            uint8_t MachineID[16];
            uint8_t Droid[32];
            uint8_t DroidBirth[32];
        };
        /*
            SHLLINK ExtraDataBlock - VistaAndAboveIDListDataBlock
        */
        struct _cshllink_extdatablk_viidldblk{
            struct _cshllink_extdatablk_blk_info info;
            struct _cshllink_lnktidl_idl cshllink_lnktidl_idl;
        };
    struct _cshllink_extdatablk{
        struct _cshllink_extdatablk_consdblk *ConsoleDataBlock;
        struct _cshllink_extdatablk_consfdblk *ConsoleFEDataBlock;
        struct _cshllink_extdatablk_darwdblk *DarwinDataBlock;
        struct _cshllink_extdatablk_envdblk *EnvironmentVariableDataBlock;
        struct _cshllink_extdatablk_icoenvdblk *IconEnvironmentDataBlock;
        struct _cshllink_extdatablk_knownfdblk *KnownFolderDataBlock;
        struct _cshllink_extdatablk_propsdblk *PropertyStoreDataBlock;
        struct _cshllink_extdatablk_shimblk *ShimDataBlock;
        struct _cshllink_extdatablk_specfdblk *SpecialFolderDataBlock;
        struct _cshllink_extdatablk_trackdblk *TrackerDataBlock;
        struct _cshllink_extdatablk_viidldblk *VistaAndAboveIDListDataBlock;
    };


    /*
        SHLLINK Structure
    */
    struct _cshllink {

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
    };

#endif