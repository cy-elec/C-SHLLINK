/*
    This library provides an interface for the MS Shell Link Binary File Format in C [MS-SHLLINK](https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-shllink/16cb4ca1-9339-4d0c-a68d-bf1d6cc0f943)


    This library is completely based on [MS-SHLLINK](https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-shllink/16cb4ca1-9339-4d0c-a68d-bf1d6cc0f943)
    The descriptions of the variables are taken directly from the official documentation.

    Author: Felix Kröhnert
    License: GNU GPL V3 (refer to LICENSE for more information)
*/

#include "cshllink.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

    uint8_t cshllink_error=0;
    /*
        -> open file descriptor of type FILE (R mode)
        -> cshllink structure pointer containing the FILE content
        -- reads content, modifies cshllink structure
        <- on error this function will return -1 (the content of inputStruct is undefined), on success 0

        exact error codes are stored in cshllink_error
    */
    #pragma region loadFile
    uint8_t cshllink_loadFile(FILE *fp, cshllink *inputStruct) {

        // test if FILE is open
        if(fp==NULL) 
            _cshllink_errint(_CSHLLINK_ERR_FCL);

        // test if FILE is in READ mode
        fseek(fp, 0, SEEK_SET);
        {
            char tmp;
            if(fread(&tmp, 1, 1, fp)!=1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
        }

        // clear inputStruct
        if(inputStruct==NULL)
            _cshllink_errint(_CSHLLINK_ERR_NULLPA)
        {
            cshllink tmp = {0};
            *inputStruct = tmp;
        }

        // read FILE
        return cshllink_loadFile_i(fp, inputStruct);
    }
    
    /*
        Processes inputFile
    */
    uint8_t cshllink_loadFile_i(FILE *fp, cshllink *inputStruct) {
        
        fseek(fp, 0, SEEK_SET);

        /*
            HEADER
        */
       {
            //HeaderSize
            if(fread(&inputStruct->cshllink_header.HeaderSize, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
            if(inputStruct->cshllink_header.HeaderSize!=0x4c)
                _cshllink_errint(_CSHLLINK_ERR_WHEADS);
            
            //LinkCLSID
            if(fread(&inputStruct->cshllink_header.LinkCLSID_L, 16, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
            cshllink_sEndian(&inputStruct->cshllink_header.LinkCLSID_L, 16);
            if(inputStruct->cshllink_header.LinkCLSID_H!=0x0114020000000000)
                _cshllink_errint(_CSHLLINK_ERR_WCLSIDS);
            if(inputStruct->cshllink_header.LinkCLSID_L!=0xC000000000000046)
                _cshllink_errint(_CSHLLINK_ERR_WCLSIDS);

            //LinkFlags
            if(fread(&inputStruct->cshllink_header.LinkFlags, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
            
            //FileAttributes
            if(fread(&inputStruct->cshllink_header.FileAttributes, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
            
            //CreationTime
            if(fread(&inputStruct->cshllink_header.CreationTime, 8, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
            //AccessTime
            if(fread(&inputStruct->cshllink_header.AccessTime, 8, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
            //WriteTime
            if(fread(&inputStruct->cshllink_header.WriteTime, 8, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);

            //FileSize
            if(fread(&inputStruct->cshllink_header.FileSize, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);

            //IconIndex
            if(fread(&inputStruct->cshllink_header.IconIndex, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
            
            //ShowCommand
            if(fread(&inputStruct->cshllink_header.ShowCommand, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);

            //HotKey
            if(fread(&inputStruct->cshllink_header.HotKey, 2, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);

            fseek(fp, 10, SEEK_CUR);
        }

        /*
            LinkTargetIDList
        */
        if(inputStruct->cshllink_header.LinkFlags&CSHLLINK_LF_HasLinkTargetIDList) {
            //IDList size
            if(fread(&inputStruct->cshllink_lnktidl.idl_size, 2, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);

            int tmpS=inputStruct->cshllink_lnktidl.idl_size-2;
            while(tmpS>0) {
                //realloc element
                inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_item = realloc(inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_item, inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_inum+1);
                if(inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_item==NULL) 
                    _cshllink_errint(_CSHLLINK_ERR_NULLPIDL);
                //Element size
                if(fread(&inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_item[inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_inum].item_size, 2, 1, fp) != 1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);
                //Element (size -2 as struct contains one 2byte var)
                inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_item[inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_inum].item = malloc(inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_item[inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_inum].item_size-2);
                if(inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_item[inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_inum].item==NULL)
                    _cshllink_errint(_CSHLLINK_ERR_NULLPIDLM);
                if(fread(&inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_item[inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_inum].item, inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_item[inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_inum].item_size-2, 1, fp) != 1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);
                
                tmpS-=inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_item[inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_inum].item_size;
                inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_inum++;
            }
            uint16_t nullb;
            if(fread(&nullb, 2, 1, fp)!=1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
            if(tmpS<0||nullb!=0)
                _cshllink_errint(_CSHLLINK_ERR_INVIDL);   
        }

        /*
            LinkInfo
        */
        if(inputStruct->cshllink_header.LinkFlags&CSHLLINK_LF_HasLinkInfo) {
            //LinkInfoSize
            if(fread(&inputStruct->cshllink_lnkinfo.LinkInfoSize, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
            
            //LinkInfoHeaderSize
            if(fread(&inputStruct->cshllink_lnkinfo.LinkInfoHeaderSize, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
            if(inputStruct->cshllink_lnkinfo.LinkInfoHeaderSize!=0x0000001C && inputStruct->cshllink_lnkinfo.LinkInfoHeaderSize<0x00000024)
                _cshllink_errint(_CSHLLINK_ERR_INVLIHS);
            
            //LinkInfoFlags
            if(fread(&inputStruct->cshllink_lnkinfo.LinkInfoFlags, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);

            //VolumeIDOffset
            if(fread(&inputStruct->cshllink_lnkinfo.VolumeIDOffset, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
            
            //LocalBasePathOffset
            if(fread(&inputStruct->cshllink_lnkinfo.LocalBasePathOffset, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);

            //CommonNetworkRelativeLinkOffset
            if(fread(&inputStruct->cshllink_lnkinfo.CommonNetworkRelativeLinkOffset, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);

            //CommonPathSuffixOffset
            if(fread(&inputStruct->cshllink_lnkinfo.CommonPathSuffixOffset, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);

            //only present if LinkInfoHeaderSize >= 0x00000024
            if(inputStruct->cshllink_lnkinfo.LinkInfoHeaderSize>=0x00000024) {
                //LocalBasePathOffsetUnicode
                if(fread(&inputStruct->cshllink_lnkinfo.LocalBasePathOffsetUnicode, 4, 1, fp) != 1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);

                //CommonPathSuffixOffsetUnicode
                if(fread(&inputStruct->cshllink_lnkinfo.CommonPathSuffixOffsetUnicode, 4, 1, fp) != 1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);
            }

            /*
                VolumeID
            */
            if(inputStruct->cshllink_lnkinfo.LinkInfoFlags&CSHLLINK_LIF_VolumeIDAndLocalBasePath) {
                int VtmpSize=16;
                //VolumeIDSize
                if(fread(&inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeIDSize, 4, 1, fp) != 1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);

                //DriveType
                if(fread(&inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.DriveType, 4, 1, fp) != 1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);
                
                //DriveSerialNumber
                if(fread(&inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.DriveSerialNumber, 4, 1, fp) != 1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);

                //VolumeLabelOffset
                if(fread(&inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeLabelOffset, 4, 1, fp) != 1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);
                
                //only present if VolumeLabelOffset == 0x00000014
                if(inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeLabelOffset==0x00000014) {
                    //VolumeLabelOffsetUnicode
                    if(fread(&inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeLabelOffsetUnicode, 4, 1, fp) != 1)
                        _cshllink_errint(_CSHLLINK_ERR_FIO);
                    VtmpSize+=4;
                }

                //DATA
                if(inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeIDSize<=VtmpSize)
                    _cshllink_errint(_CSHLLINK_ERR_VIDSLOW);
                inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.Data = malloc(inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeIDSize-VtmpSize);
                if(inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.Data==NULL)
                    _cshllink_errint(_CSHLLINK_ERR_NULLPVIDD);
                //Data
                if(fread(inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.Data, inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeIDSize-VtmpSize, 1, fp) != 1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);

                //LocalBasePath
                if(cshllink_rNULLstr(&inputStruct->cshllink_lnkinfo.LocalBasePath, _CSHLLINK_ERR_NULLPLBP, _CSHLLINK_ERR_FIO, fp))
                    return -1;
            }
            /*
                CommonNetworkRelativeLink
            */
            if(inputStruct->cshllink_lnkinfo.LinkInfoFlags&CSHLLINK_LIF_CommonNetworkRelativeLinkAndPathSuffix) {
                //CommonNetworkRelativeSize
                if(fread(&inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_cnetrlnk.CommonNetworkRelativeSize, 4, 1, fp) != 1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);

                //CommonNetworkRelativeLinkFlags
                if(fread(&inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_cnetrlnk.CommonNetworkRelativeLinkFlags, 4, 1, fp) != 1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);

                //NetNameOffset
                if(fread(&inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_cnetrlnk.NetNameOffset, 4, 1, fp) != 1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);

                //DeviceNameOffset
                if(fread(&inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_cnetrlnk.DeviceNameOffset, 4, 1, fp) != 1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);
                
                //NetworkProviderType
                if(fread(&inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_cnetrlnk.NetworkProviderType, 4, 1, fp) != 1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);
                
                if(inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_cnetrlnk.NetNameOffset>0x00000014) {
                    //NetNameOffsetUnicode
                    if(fread(&inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_cnetrlnk.NetNameOffsetUnicode, 4, 1, fp) != 1)
                        _cshllink_errint(_CSHLLINK_ERR_FIO);

                    //DeviceNameOffsetUnicode
                    if(fread(&inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_cnetrlnk.DeviceNameOffsetUnicode, 4, 1, fp) != 1)
                        _cshllink_errint(_CSHLLINK_ERR_FIO);
                }

                //NetName
                if(cshllink_rNULLstr(&inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_cnetrlnk.NetName, _CSHLLINK_ERR_NULLPNETN, _CSHLLINK_ERR_FIO, fp))
                        return -1;
                
                //DeviceName
                if(cshllink_rNULLstr(&inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_cnetrlnk.DeviceName, _CSHLLINK_ERR_NULLPDEVN, _CSHLLINK_ERR_FIO, fp))
                        return -1;

                if(inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_cnetrlnk.NetNameOffset>0x00000014) {
                    //NetNameUnicode
                    if(cshllink_rNULLstr(&inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_cnetrlnk.NetNameUnicode, _CSHLLINK_ERR_NULLPNNU, _CSHLLINK_ERR_FIO, fp))
                            return -1;

                    //DeviceNameUnicode
                    if(cshllink_rNULLstr(&inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_cnetrlnk.DeviceNameUnicode, _CSHLLINK_ERR_NULLPDNU, _CSHLLINK_ERR_FIO, fp))
                            return -1;
                }
            }

            //CommonPathSuffix
            if(cshllink_rNULLstr(&inputStruct->cshllink_lnkinfo.CommonPathSuffix, _CSHLLINK_ERR_NULLPCPS, _CSHLLINK_ERR_FIO, fp))
                    return -1;

            if(inputStruct->cshllink_lnkinfo.LinkInfoHeaderSize>=0x00000024) {
                //LocalBasePathUnicode
                if(inputStruct->cshllink_lnkinfo.LinkInfoFlags&CSHLLINK_LIF_VolumeIDAndLocalBasePath) {
                    if(cshllink_rNULLstr(&inputStruct->cshllink_lnkinfo.LocalBasePathUnicode, _CSHLLINK_ERR_NULLPLBPU, _CSHLLINK_ERR_FIO, fp))
                        return -1;
                }
                //CommonPathSuffixUnicode
                if(cshllink_rNULLstr(&inputStruct->cshllink_lnkinfo.CommonPathSuffixUnicode, _CSHLLINK_ERR_NULLPCPSU, _CSHLLINK_ERR_FIO, fp))
                    return -1;
            }
        
        }

        /*
            StringData (all unicode 2 bytes)
        */
            if(inputStruct->cshllink_header.LinkFlags&CSHLLINK_LF_HasName) {
                //NameString
                if(fread(&inputStruct->cshllink_strdata.NameString.CountCharacters, 2, 1, fp)!=1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);
                if(cshllink_rwstr(&inputStruct->cshllink_strdata.NameString.UString, _CSHLLINK_ERR_NULLPSTRDNAME, _CSHLLINK_ERR_FIO, fp, inputStruct->cshllink_strdata.NameString.CountCharacters))
                    return -1;
            }
            if(inputStruct->cshllink_header.LinkFlags&CSHLLINK_LF_HasRelativePath) {
                //RelativePath
                if(fread(&inputStruct->cshllink_strdata.RelativePath.CountCharacters, 2, 1, fp)!=1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);
                if(cshllink_rwstr(&inputStruct->cshllink_strdata.RelativePath.UString, _CSHLLINK_ERR_NULLPSTRDRPATH, _CSHLLINK_ERR_FIO, fp, inputStruct->cshllink_strdata.RelativePath.CountCharacters))
                    return -1;
            }
            if(inputStruct->cshllink_header.LinkFlags&CSHLLINK_LF_HasWorkingDir) {
                //WorkingDir
                if(fread(&inputStruct->cshllink_strdata.WorkingDir.CountCharacters, 2, 1, fp)!=1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);
                if(cshllink_rwstr(&inputStruct->cshllink_strdata.WorkingDir.UString, _CSHLLINK_ERR_NULLPSTRDWDIR, _CSHLLINK_ERR_FIO, fp, inputStruct->cshllink_strdata.WorkingDir.CountCharacters))
                    return -1;
            }
            if(inputStruct->cshllink_header.LinkFlags&CSHLLINK_LF_HasArguments) {
                //CommandLineArguments
                if(fread(&inputStruct->cshllink_strdata.CommandLineArguments.CountCharacters, 2, 1, fp)!=1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);
                if(cshllink_rwstr(&inputStruct->cshllink_strdata.CommandLineArguments.UString, _CSHLLINK_ERR_NULLPSTRDARG, _CSHLLINK_ERR_FIO, fp, inputStruct->cshllink_strdata.CommandLineArguments.CountCharacters))
                    return -1;
            }
            if(inputStruct->cshllink_header.LinkFlags&CSHLLINK_LF_HasIconLocation) {
                //IconLocation
                if(fread(&inputStruct->cshllink_strdata.IconLocation.CountCharacters, 2, 1, fp)!=1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);
                if(cshllink_rwstr(&inputStruct->cshllink_strdata.IconLocation.UString, _CSHLLINK_ERR_NULLPSTRDICO, _CSHLLINK_ERR_FIO, fp, inputStruct->cshllink_strdata.IconLocation.CountCharacters))
                    return -1;
            }
        /*
            ExtraDataBlock
        */
        {
            long int cpos=ftell(fp);
            fseek(fp, 0, SEEK_END);
            long int epos=ftell(fp)-4;      //TerminalBlock
            fseek(fp, cpos, SEEK_SET);

            for(int i=0; i<_CSHLLINK_EDBLK_NUM; i++) {
                cpos=ftell(fp);
                if(cpos>=epos) break;
                
                struct _cshllink_extdatablk_blk_info info={0};
                if(fread(&info.BlockSize, 4, 1, fp)!=1) 
                    _cshllink_errint(_CSHLLINK_ERR_FIO);
                if(fread(&info.BlockSignature, 4, 1, fp)!=1) 
                    _cshllink_errint(_CSHLLINK_ERR_FIO);

                switch(info.BlockSignature) {
                    case _CSHLLINK_EDBLK_ConsoleDataBlockSig:
                        if(_cshllink_readEConsoleDataBlock(inputStruct, info, fp)) return -1;
                        break;
                    case _CSHLLINK_EDBLK_ConsoleFEDataBlockSig: 
                        if(_cshllink_readEConsoleFEDataBlock(inputStruct, info, fp)) return -1;
                        break;
                    case _CSHLLINK_EDBLK_DarwinDataBlockSig: 
                        if(_cshllink_readEDarwinDataBlock(inputStruct, info, fp)) return -1;
                        break;
                    case _CSHLLINK_EDBLK_EnvironmentVariableDataBlockSig: 
                        if(_cshllink_readEEnvironmentVariableDataBlock(inputStruct, info, fp)) return -1;
                        break;
                    case _CSHLLINK_EDBLK_IconEnvironmentDataBlockSig: 
                        if(_cshllink_readEIconEnvironmentDataBlock(inputStruct, info, fp)) return -1;
                        break;
                    case _CSHLLINK_EDBLK_KnownFolderDataBlockSig: 
                        if(_cshllink_readEKnownFolderDataBlock(inputStruct, info, fp)) return -1;
                        break;
                    case _CSHLLINK_EDBLK_PropertyStoreDataBlockSig: 
                        if(_cshllink_readEPropertyStoreDataBlock(inputStruct, info, fp)) return -1;
                        break;
                    case _CSHLLINK_EDBLK_ShimDataBlockSig: 
                        if(_cshllink_readEShimDataBlock(inputStruct, info, fp)) return -1;
                        break;
                    case _CSHLLINK_EDBLK_SpecialFolderDataBlockSig: 
                        if(_cshllink_readESpecialFolderDataBlock(inputStruct, info, fp)) return -1;
                        break;
                    case _CSHLLINK_EDBLK_TrackerDataBlockSig: 
                        if(_cshllink_readETrackerDataBlock(inputStruct, info, fp)) return -1;
                        break;
                    case _CSHLLINK_EDBLK_VistaAndAboveIDListDataBlockSig: 
                        if(_cshllink_readEVistaAndAboveIDListDataBlock(inputStruct, info, fp)) return -1;
                        break;
                    default: _cshllink_errint(_CSHLLINK_ERR_UNKEDBSIG);
                }

            }

        }

        return 0;
    }


    /*
        -> open file descriptor of type FILE (W mode)
        -> cshllink structure pointer containing the FILE content
        -- writes content to file
        <- on error this function will return -1 (the content of inputStruct is undefined), on success 0

        exact error codes are stored in cshllink_error
    */
    uint8_t cshllink_writeFile(FILE *fp, cshllink *inputStruct) {

        return 0;
    }

    /*
        Converts little Endian to big Endian and vice versa (size in bytes)
    */
    void cshllink_sEndian(void *inp, size_t size) {
        for(int i=0; i<size/2; i++) {
            uint8_t t = ((uint8_t*)inp)[size-1-i];
            ((uint8_t*)inp)[size-1-i] = ((uint8_t*)inp)[i];
            ((uint8_t*)inp)[i] = t;
        }
    }

    /*
        read NULL terminated String
    */
    uint32_t cshllink_rNULLstr(char **dest, uint8_t errv1, uint8_t errv2, FILE *fp) {
        char tmpC=1;
        uint32_t tmpS=0;
        while(tmpC!=0) {
            *dest = realloc(*dest, tmpS+1);
            if(*dest==NULL)
                _cshllink_errint(errv1);
            if(fread(&(*dest)[tmpS], 1, 1, fp) != 1)
                _cshllink_errint(errv2);
            tmpC = (*dest)[tmpS];
            tmpS++;
        }
        return 0;
    }
    uint32_t cshllink_rNULLwstr(char16_t **dest, uint8_t errv1, uint8_t errv2, FILE *fp) {
        char16_t tmpC=1;
        uint32_t tmpS=0;
        while(tmpC!=0) {
            *dest = realloc(*dest, (tmpS+1)*sizeof(char16_t));
            if(*dest==NULL)
                _cshllink_errint(errv1);
            if(fread(&(*dest)[tmpS], sizeof(char16_t), 1, fp) != 1)
                _cshllink_errint(errv2);
            tmpC = (*dest)[tmpS];
            tmpS++;
        }
        return 0;
    }

    /*
        read String
    */
    uint32_t cshllink_rstr(char **dest, uint8_t errv1, uint8_t errv2, FILE *fp, size_t size) {
        if(size==0) _cshllink_errint(errv1);
        *dest = malloc(size);
        if(*dest==NULL)
            _cshllink_errint(errv1);
        if(fread(*dest, 1, size, fp) != size)
            _cshllink_errint(errv2);
        return 0;
    }
    uint32_t cshllink_rwstr(char16_t **dest, uint8_t errv1, uint8_t errv2, FILE *fp, size_t size) {
        if(size==0) _cshllink_errint(errv1);
        *dest = malloc(size*sizeof(char16_t));
        if(*dest==NULL)
            _cshllink_errint(errv1);
        if(fread(*dest, sizeof(char16_t), size, fp) != size)
            _cshllink_errint(errv2);
        return 0;
    }

    /*
        Extra Data Block read functions
    */
    uint8_t _cshllink_readEConsoleDataBlock(cshllink *input, const struct _cshllink_extdatablk_blk_info info, FILE *fp) {
        if(input->cshllink_extdatablk.ConsoleDataBlock.info.BlockSignature!=0)
            _cshllink_errint(_CSHLLINK_DUPEEX_ConsoleDataBlock);

        if(info.BlockSize!=_CSHLLINK_EDBLK_ConsoleDataBlockSiz)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);
        input->cshllink_extdatablk.ConsoleDataBlock.info.BlockSize=info.BlockSize;

        //FillAttributes
        if(fread(&input->cshllink_extdatablk.ConsoleDataBlock.FillAttributes, 2, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //PopupFillAttributes
        if(fread(&input->cshllink_extdatablk.ConsoleDataBlock.PopupFillAttributes, 2, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //ScreenBufferSizeX
        if(fread(&input->cshllink_extdatablk.ConsoleDataBlock.ScreenBufferSizeX, 2, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //ScreenBufferSizeY
        if(fread(&input->cshllink_extdatablk.ConsoleDataBlock.ScreenBufferSizeY, 2, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);
        
        //WindowSizeX
        if(fread(&input->cshllink_extdatablk.ConsoleDataBlock.WindowSizeX, 2, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);
        
        //WindowSizeY
        if(fread(&input->cshllink_extdatablk.ConsoleDataBlock.WindowSizeY, 2, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //WindowOriginX
        if(fread(&input->cshllink_extdatablk.ConsoleDataBlock.WindowOriginX, 2, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //WindowOriginY
        if(fread(&input->cshllink_extdatablk.ConsoleDataBlock.WindowOriginY, 2, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);
    
        fseek(fp, 8, SEEK_CUR);

        //FontSize
        if(fread(&input->cshllink_extdatablk.ConsoleDataBlock.FontSize, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);
    
        //FontFamily
        if(fread(&input->cshllink_extdatablk.ConsoleDataBlock.FontFamily, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //FontWeight
        if(fread(&input->cshllink_extdatablk.ConsoleDataBlock.FontWeight, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //FaceName
        if(cshllink_rwstr(&input->cshllink_extdatablk.ConsoleDataBlock.FaceName, _CSHLLINK_ERRX_NULLPSTRFNAME, _CSHLLINK_ERR_FIO, fp, 32))
            return -1;

        //CursorSize
        if(fread(&input->cshllink_extdatablk.ConsoleDataBlock.CursorSize, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //FullScreen
        if(fread(&input->cshllink_extdatablk.ConsoleDataBlock.FullScreen, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);
        
        //QuickEdit
        if(fread(&input->cshllink_extdatablk.ConsoleDataBlock.QuickEdit, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //InsertMode
        if(fread(&input->cshllink_extdatablk.ConsoleDataBlock.InsertMode, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //AutoPosition
        if(fread(&input->cshllink_extdatablk.ConsoleDataBlock.AutoPosition, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //HistoryBufferSize
        if(fread(&input->cshllink_extdatablk.ConsoleDataBlock.HistoryBufferSize, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //NumberOfHistoryBuffers
        if(fread(&input->cshllink_extdatablk.ConsoleDataBlock.NumberOfHistoryBuffers, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //HistoryNoDup
        if(fread(&input->cshllink_extdatablk.ConsoleDataBlock.HistoryNoDup, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //AutoPosition
        if(fread(&input->cshllink_extdatablk.ConsoleDataBlock.AutoPosition, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);
       
        //ColorTable
        for(int i=0; i<16; i++)
            if(fread(&input->cshllink_extdatablk.ConsoleDataBlock.ColorTable[i], 4, 1, fp)!=1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);

        return 0;
    }

    uint8_t _cshllink_readEConsoleFEDataBlock(cshllink *input, const struct _cshllink_extdatablk_blk_info info, FILE *fp) {
        if(input->cshllink_extdatablk.ConsoleFEDataBlock.info.BlockSignature!=0)
            _cshllink_errint(_CSHLLINK_DUPEEX_ConsoleFEDataBlock);
        
        if(info.BlockSize!=_CSHLLINK_EDBLK_ConsoleFEDataBlockSiz)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);
        input->cshllink_extdatablk.ConsoleFEDataBlock.info.BlockSize=info.BlockSize;

        //CodePage
        if(fread(&input->cshllink_extdatablk.ConsoleFEDataBlock.CodePage, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        return 0;
    }

    uint8_t _cshllink_readEDarwinDataBlock(cshllink *input, const struct _cshllink_extdatablk_blk_info info, FILE *fp) {
        if(input->cshllink_extdatablk.DarwinDataBlock.info.BlockSignature!=0)
            _cshllink_errint(_CSHLLINK_DUPEEX_DarwinDataBlock);
        
        if(info.BlockSize!=_CSHLLINK_EDBLK_DarwinDataBlockSiz)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);
        input->cshllink_extdatablk.DarwinDataBlock.info.BlockSize=info.BlockSize;

        //DarwinDataAnsi
        if(cshllink_rstr(&input->cshllink_extdatablk.DarwinDataBlock.DarwinDataAnsi, _CSHLLINK_ERRX_NULLPSTRDARDA, _CSHLLINK_ERR_FIO, fp, 260))
            return -1;
        
        //DarwinDataUnicode
        if(cshllink_rwstr(&input->cshllink_extdatablk.DarwinDataBlock.DarwinDataUnicode, _CSHLLINK_ERRX_NULLPSTRDARDU, _CSHLLINK_ERR_FIO, fp, 520))
            return -1;

        return 0;
    }

    uint8_t _cshllink_readEEnvironmentVariableDataBlock(cshllink *input, const struct _cshllink_extdatablk_blk_info info, FILE *fp) {
        if(input->cshllink_extdatablk.EnvironmentVariableDataBlock.info.BlockSignature!=0)
            _cshllink_errint(_CSHLLINK_DUPEEX_EnvironmentVariableDataBlock);
        
        if(info.BlockSize!=_CSHLLINK_EDBLK_EnvironmentVariableDataBlockSiz)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);
        input->cshllink_extdatablk.EnvironmentVariableDataBlock.info.BlockSize=info.BlockSize;

        //EnvironmentVariableDataAnsi
        if(cshllink_rstr(&input->cshllink_extdatablk.EnvironmentVariableDataBlock.TargetAnsi, _CSHLLINK_ERRX_NULLPSTRENVDA, _CSHLLINK_ERR_FIO, fp, 260))
            return -1;
        
        //EnvironmentVariableDataUnicode
        if(cshllink_rwstr(&input->cshllink_extdatablk.EnvironmentVariableDataBlock.TargetUnicode, _CSHLLINK_ERRX_NULLPSTRENVDU, _CSHLLINK_ERR_FIO, fp, 520))
            return -1;

        return 0;
    }

    uint8_t _cshllink_readEIconEnvironmentDataBlock(cshllink *input, const struct _cshllink_extdatablk_blk_info info, FILE *fp) {
        if(input->cshllink_extdatablk.IconEnvironmentDataBlock.info.BlockSignature!=0)
            _cshllink_errint(_CSHLLINK_DUPEEX_IconEnvironmentDataBlock);
        
        if(info.BlockSize!=_CSHLLINK_EDBLK_IconEnvironmentDataBlockSiz)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);
        input->cshllink_extdatablk.IconEnvironmentDataBlock.info.BlockSize=info.BlockSize;
        
        //IconEnvironmentDataAnsi
        if(cshllink_rstr(&input->cshllink_extdatablk.IconEnvironmentDataBlock.TargetAnsi, _CSHLLINK_ERRX_NULLPSTRIENVDA, _CSHLLINK_ERR_FIO, fp, 260))
            return -1;

        //IconEnvironmentDataUnicode
        if(cshllink_rwstr(&input->cshllink_extdatablk.IconEnvironmentDataBlock.TargetUnicode, _CSHLLINK_ERRX_NULLPSTRIENVDU, _CSHLLINK_ERR_FIO, fp, 520))
            return -1;

        return 0;
    }

    uint8_t _cshllink_readEKnownFolderDataBlock(cshllink *input, const struct _cshllink_extdatablk_blk_info info, FILE *fp) {
        if(input->cshllink_extdatablk.KnownFolderDataBlock.info.BlockSignature!=0)
            _cshllink_errint(_CSHLLINK_DUPEEX_KnownFolderDataBlock);
        
        if(info.BlockSize!=_CSHLLINK_EDBLK_KnownFolderDataBlockSiz)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);
        input->cshllink_extdatablk.KnownFolderDataBlock.info.BlockSize=info.BlockSize;

        //KnownFolderID
        if(cshllink_rstr((char **)&input->cshllink_extdatablk.KnownFolderDataBlock.KnownFolderID, _CSHLLINK_ERR_NULLPEXTD, _CSHLLINK_ERR_FIO, fp, 16))
            return -1;
        
        //Offset
        if(fread(&input->cshllink_extdatablk.KnownFolderDataBlock.Offset, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        return 0;
    }

    uint8_t _cshllink_readEPropertyStoreDataBlock(cshllink *input, const struct _cshllink_extdatablk_blk_info info, FILE *fp) {
        if(input->cshllink_extdatablk.PropertyStoreDataBlock.info.BlockSignature!=0)
            _cshllink_errint(_CSHLLINK_DUPEEX_PropertyStoreDataBlock);
        
        if(info.BlockSize!=_CSHLLINK_EDBLK_PropertyStoreDataBlockSiz)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);
        input->cshllink_extdatablk.PropertyStoreDataBlock.info.BlockSize=info.BlockSize;

        //PropertyStore
        if(cshllink_rstr((char **)&input->cshllink_extdatablk.KnownFolderDataBlock.KnownFolderID, _CSHLLINK_ERR_NULLPEXTD, _CSHLLINK_ERR_FIO, fp, input->cshllink_extdatablk.PropertyStoreDataBlock.info.BlockSize-8))
            return -1;

        return 0;
    }

    uint8_t _cshllink_readEShimDataBlock(cshllink *input, const struct _cshllink_extdatablk_blk_info info, FILE *fp) {
        if(input->cshllink_extdatablk.ShimDataBlock.info.BlockSignature!=0)
            _cshllink_errint(_CSHLLINK_DUPEEX_ShimDataBlock);
        
        if(info.BlockSize!=_CSHLLINK_EDBLK_ShimDataBlockSiz)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);
        input->cshllink_extdatablk.ShimDataBlock.info.BlockSize=info.BlockSize;

        //LayerName
        if(cshllink_rwstr(&input->cshllink_extdatablk.ShimDataBlock.LayerName, _CSHLLINK_ERR_NULLPEXTD, _CSHLLINK_ERR_FIO, fp, input->cshllink_extdatablk.ShimDataBlock.info.BlockSize-8))
            return -1;

        return 0;
    }

    uint8_t _cshllink_readESpecialFolderDataBlock(cshllink *input, const struct _cshllink_extdatablk_blk_info info, FILE *fp) {
        if(input->cshllink_extdatablk.SpecialFolderDataBlock.info.BlockSignature!=0)
            _cshllink_errint(_CSHLLINK_DUPEEX_SpecialFolderDataBlock);
        
        if(info.BlockSize!=_CSHLLINK_EDBLK_SpecialFolderDataBlockSiz)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);
        input->cshllink_extdatablk.SpecialFolderDataBlock.info.BlockSize=info.BlockSize;

        //SpecialFolderID
        if(fread(&input->cshllink_extdatablk.SpecialFolderDataBlock.SpecialFolderID, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //Offset
        if(fread(&input->cshllink_extdatablk.SpecialFolderDataBlock.Offset, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        return 0;
    }

    uint8_t _cshllink_readETrackerDataBlock(cshllink *input, const struct _cshllink_extdatablk_blk_info info, FILE *fp) {
        if(input->cshllink_extdatablk.TrackerDataBlock.info.BlockSignature!=0)
            _cshllink_errint(_CSHLLINK_DUPEEX_TrackerDataBlock);
        
        printf("0x%lx:\t0x%x -- 0x%x\n", ftell(fp), info.BlockSize, _CSHLLINK_EDBLK_TrackerDataBlockSiz);

        if(info.BlockSize!=_CSHLLINK_EDBLK_TrackerDataBlockSiz)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);
        input->cshllink_extdatablk.TrackerDataBlock.info.BlockSize=info.BlockSize;

        //Length
        if(fread(&input->cshllink_extdatablk.TrackerDataBlock.Length, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);
        if(input->cshllink_extdatablk.TrackerDataBlock.Length!=_CSHLLINK_EDBLK_TrackerDataBlockLen)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);

        //Version
        if(fread(&input->cshllink_extdatablk.TrackerDataBlock.Length, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);
        if(input->cshllink_extdatablk.TrackerDataBlock.Length!=0)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGVERSION);
        
        //MachineID
        if(cshllink_rstr(&input->cshllink_extdatablk.TrackerDataBlock.MachineID, _CSHLLINK_ERR_NULLPEXTD, _CSHLLINK_ERR_FIO, fp, 16))
            return -1;

        //Droid
        if(cshllink_rstr((char **)&input->cshllink_extdatablk.TrackerDataBlock.Droid, _CSHLLINK_ERR_NULLPEXTD, _CSHLLINK_ERR_FIO, fp, 32))
            return -1;

        //DroidBirth
        if(cshllink_rstr((char **)&input->cshllink_extdatablk.TrackerDataBlock.DroidBirth, _CSHLLINK_ERR_NULLPEXTD, _CSHLLINK_ERR_FIO, fp, 32))
            return -1;

        return 0;
    }

    uint8_t _cshllink_readEVistaAndAboveIDListDataBlock(cshllink *input, const struct _cshllink_extdatablk_blk_info info, FILE *fp) {
        if(input->cshllink_extdatablk.VistaAndAboveIDListDataBlock.info.BlockSignature!=0)
            _cshllink_errint(_CSHLLINK_DUPEEX_VistaAndAboveIDListDataBlock);
        
        if(info.BlockSize!=_CSHLLINK_EDBLK_VistaAndAboveIDListDataBlockSiz)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);
        input->cshllink_extdatablk.VistaAndAboveIDListDataBlock.info.BlockSize=info.BlockSize;

        int tmpS=input->cshllink_extdatablk.VistaAndAboveIDListDataBlock.info.BlockSize=info.BlockSize-10;
        while(tmpS>0) {
            //realloc element
            input->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_item = realloc(input->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_item, input->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_inum+1);
            if(input->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_item==NULL) 
                _cshllink_errint(_CSHLLINK_ERR_NULLPIDL);
            //Element size
            if(fread(&input->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_item[input->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_inum].item_size, 2, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
            //Element (size -2 as struct contains one 2byte var)
            input->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_item[input->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_inum].item = malloc(input->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_item[input->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_inum].item_size-2);
            if(input->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_item[input->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_inum].item==NULL)
                _cshllink_errint(_CSHLLINK_ERR_NULLPIDLM);
            if(fread(&input->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_item[input->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_inum].item, input->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_item[input->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_inum].item_size-2, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
            
            tmpS-=input->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_item[input->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_inum].item_size;
            input->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_inum++;
        }
        uint16_t nullb;
        if(fread(&nullb, 2, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);
        if(tmpS<0||nullb!=0)
            _cshllink_errint(_CSHLLINK_ERR_INVIDL);  

        return 0;
    }

    #pragma endregion

    #pragma region util

    /*
        frees whole structure
    */
    void cshllink_free(cshllink *inputStruct) {      
        free(inputStruct->cshllink_extdatablk.ConsoleDataBlock.FaceName);
        free(inputStruct->cshllink_extdatablk.DarwinDataBlock.DarwinDataAnsi);
        free(inputStruct->cshllink_extdatablk.DarwinDataBlock.DarwinDataUnicode);
        free(inputStruct->cshllink_extdatablk.EnvironmentVariableDataBlock.TargetAnsi);
        free(inputStruct->cshllink_extdatablk.EnvironmentVariableDataBlock.TargetUnicode);
        free(inputStruct->cshllink_extdatablk.IconEnvironmentDataBlock.TargetAnsi);
        free(inputStruct->cshllink_extdatablk.IconEnvironmentDataBlock.TargetUnicode);
        free(inputStruct->cshllink_extdatablk.KnownFolderDataBlock.KnownFolderID);
        free(inputStruct->cshllink_extdatablk.PropertyStoreDataBlock.PropertyStore);
        free(inputStruct->cshllink_extdatablk.ShimDataBlock.LayerName);
        free(inputStruct->cshllink_extdatablk.TrackerDataBlock.Droid);
        free(inputStruct->cshllink_extdatablk.TrackerDataBlock.DroidBirth);
        free(inputStruct->cshllink_extdatablk.TrackerDataBlock.MachineID);
        free(inputStruct->cshllink_lnkinfo.CommonPathSuffix);
        free(inputStruct->cshllink_lnkinfo.CommonPathSuffixUnicode);
        free(inputStruct->cshllink_lnkinfo.LocalBasePath);
        free(inputStruct->cshllink_lnkinfo.LocalBasePathUnicode);
        free(inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_item);
        free(inputStruct->cshllink_strdata.CommandLineArguments.UString);
        free(inputStruct->cshllink_strdata.IconLocation.UString);
        free(inputStruct->cshllink_strdata.NameString.UString);
        free(inputStruct->cshllink_strdata.RelativePath.UString);
        free(inputStruct->cshllink_strdata.WorkingDir.UString);
    }

    #pragma endregion