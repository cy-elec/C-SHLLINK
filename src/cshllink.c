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
        cshllink_free(inputStruct);

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
            inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_inum=0;

            //realloc enough mem
            inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_item = realloc(inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_item, (tmpS)* sizeof *inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_item);
            if(inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_item==NULL) 
                _cshllink_errint(_CSHLLINK_ERR_NULLPIDL);

            while(tmpS>0) {
            
                //Element size
                if(fread(&inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_item[inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_inum].item_size, 2, 1, fp) != 1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);
                //Element (size -2 as struct contains one 2byte var)
                inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_item[inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_inum].item = malloc((inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_item[inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_inum].item_size-2)* sizeof *inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_item[inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_inum].item);
                if(inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_item[inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_inum].item==NULL)
                    _cshllink_errint(_CSHLLINK_ERR_NULLPIDLM);
                if(fread(inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_item[inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_inum].item, inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_item[inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_inum].item_size-2, 1, fp) != 1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);

                tmpS-=inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_item[inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_inum].item_size;
                inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_inum+=1;
            }
            //free unneccessary mem
            inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_item = realloc(inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_item, (inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_inum)* sizeof *inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_item);
            if(inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_item==NULL) 
                _cshllink_errint(_CSHLLINK_ERR_NULLPIDL);

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
                if(inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeLabelOffset==0x00000014) {
                    if(fread(inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.Data, sizeof(char16_t), (inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeIDSize-VtmpSize)/2, fp) != (inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeIDSize-VtmpSize)/2)
                        _cshllink_errint(_CSHLLINK_ERR_FIO);
                }
                else {
                    if(fread(inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.Data, sizeof(char), inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeIDSize-VtmpSize, fp) != inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeIDSize-VtmpSize)
                        _cshllink_errint(_CSHLLINK_ERR_FIO);
                }

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
                    if(cshllink_rNULLwstr(&inputStruct->cshllink_lnkinfo.LocalBasePathUnicode, _CSHLLINK_ERR_NULLPLBPU, _CSHLLINK_ERR_FIO, fp))
                        return -1;
                }
                //CommonPathSuffixUnicode
                if(cshllink_rNULLwstr(&inputStruct->cshllink_lnkinfo.CommonPathSuffixUnicode, _CSHLLINK_ERR_NULLPCPSU, _CSHLLINK_ERR_FIO, fp))
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
                if(cshllink_rwstr(&inputStruct->cshllink_strdata.NameString.UString, _CSHLLINK_ERR_NULLPSTRDNAME, _CSHLLINK_ERR_FIO, fp, inputStruct->cshllink_strdata.NameString.CountCharacters*2))
                    return -1;
            }
            if(inputStruct->cshllink_header.LinkFlags&CSHLLINK_LF_HasRelativePath) {
                //RelativePath
                if(fread(&inputStruct->cshllink_strdata.RelativePath.CountCharacters, 2, 1, fp)!=1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);
                if(cshllink_rwstr(&inputStruct->cshllink_strdata.RelativePath.UString, _CSHLLINK_ERR_NULLPSTRDRPATH, _CSHLLINK_ERR_FIO, fp, inputStruct->cshllink_strdata.RelativePath.CountCharacters*2))
                    return -1;
            }
            if(inputStruct->cshllink_header.LinkFlags&CSHLLINK_LF_HasWorkingDir) {
                //WorkingDir
                if(fread(&inputStruct->cshllink_strdata.WorkingDir.CountCharacters, 2, 1, fp)!=1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);
                if(cshllink_rwstr(&inputStruct->cshllink_strdata.WorkingDir.UString, _CSHLLINK_ERR_NULLPSTRDWDIR, _CSHLLINK_ERR_FIO, fp, inputStruct->cshllink_strdata.WorkingDir.CountCharacters*2))
                    return -1;
            }
            if(inputStruct->cshllink_header.LinkFlags&CSHLLINK_LF_HasArguments) {
                //CommandLineArguments
                if(fread(&inputStruct->cshllink_strdata.CommandLineArguments.CountCharacters, 2, 1, fp)!=1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);
                if(cshllink_rwstr(&inputStruct->cshllink_strdata.CommandLineArguments.UString, _CSHLLINK_ERR_NULLPSTRDARG, _CSHLLINK_ERR_FIO, fp, inputStruct->cshllink_strdata.CommandLineArguments.CountCharacters*2))
                    return -1;
            }
            if(inputStruct->cshllink_header.LinkFlags&CSHLLINK_LF_HasIconLocation) {
                //IconLocation
                if(fread(&inputStruct->cshllink_strdata.IconLocation.CountCharacters, 2, 1, fp)!=1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);
                if(cshllink_rwstr(&inputStruct->cshllink_strdata.IconLocation.UString, _CSHLLINK_ERR_NULLPSTRDICO, _CSHLLINK_ERR_FIO, fp, inputStruct->cshllink_strdata.IconLocation.CountCharacters*2))
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
                        if(_cshllink_readEConsoleDataBlock(&inputStruct, info, fp)) {
                            inputStruct->cshllink_extdatablk.ConsoleDataBlock.info.BlockSignature=0;
                            return -1;
                            }
                        break;
                    case _CSHLLINK_EDBLK_ConsoleFEDataBlockSig: 
                        if(_cshllink_readEConsoleFEDataBlock(&inputStruct, info, fp)) {
                            inputStruct->cshllink_extdatablk.ConsoleFEDataBlock.info.BlockSignature=0;
                            return -1;
                            }
                        break;
                    case _CSHLLINK_EDBLK_DarwinDataBlockSig: 
                        if(_cshllink_readEDarwinDataBlock(&inputStruct, info, fp)) {
                            inputStruct->cshllink_extdatablk.DarwinDataBlock.info.BlockSignature=0;
                            return -1;
                            }
                        break;
                    case _CSHLLINK_EDBLK_EnvironmentVariableDataBlockSig: 
                        if(_cshllink_readEEnvironmentVariableDataBlock(&inputStruct, info, fp)) {
                            inputStruct->cshllink_extdatablk.EnvironmentVariableDataBlock.info.BlockSignature=0;
                            return -1;
                            }
                        break;
                    case _CSHLLINK_EDBLK_IconEnvironmentDataBlockSig: 
                        if(_cshllink_readEIconEnvironmentDataBlock(&inputStruct, info, fp)) {
                            inputStruct->cshllink_extdatablk.IconEnvironmentDataBlock.info.BlockSignature=0;
                            return -1;
                            }
                        break;
                    case _CSHLLINK_EDBLK_KnownFolderDataBlockSig: 
                        if(_cshllink_readEKnownFolderDataBlock(&inputStruct, info, fp)) {
                            inputStruct->cshllink_extdatablk.KnownFolderDataBlock.info.BlockSignature=0;
                            return -1;
                            }
                        break;
                    case _CSHLLINK_EDBLK_PropertyStoreDataBlockSig: 
                        if(_cshllink_readEPropertyStoreDataBlock(&inputStruct, info, fp)) {
                            inputStruct->cshllink_extdatablk.PropertyStoreDataBlock.info.BlockSignature=0;
                            return -1;
                            }
                        break;
                    case _CSHLLINK_EDBLK_ShimDataBlockSig: 
                        if(_cshllink_readEShimDataBlock(&inputStruct, info, fp)) {
                            inputStruct->cshllink_extdatablk.ShimDataBlock.info.BlockSignature=0;
                            return -1;
                            }
                        break;
                    case _CSHLLINK_EDBLK_SpecialFolderDataBlockSig: 
                        if(_cshllink_readESpecialFolderDataBlock(&inputStruct, info, fp)) {
                            inputStruct->cshllink_extdatablk.SpecialFolderDataBlock.info.BlockSignature=0;
                            return -1;
                            }
                        break;
                    case _CSHLLINK_EDBLK_TrackerDataBlockSig: 
                        if(_cshllink_readETrackerDataBlock(&inputStruct, info, fp)) {
                            inputStruct->cshllink_extdatablk.TrackerDataBlock.info.BlockSignature=0;
                            return -1;
                            }
                        break;
                    case _CSHLLINK_EDBLK_VistaAndAboveIDListDataBlockSig: 
                        if(_cshllink_readEVistaAndAboveIDListDataBlock(&inputStruct, info, fp)) {
                            inputStruct->cshllink_extdatablk.VistaAndAboveIDListDataBlock.info.BlockSignature=0;
                            return -1;
                            }
                        break;
                    default: _cshllink_errint(_CSHLLINK_ERR_UNKEDBSIG);
                }

            }

        }

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
        size/=2;
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
    uint8_t _cshllink_readEConsoleDataBlock(cshllink **input, const struct _cshllink_extdatablk_blk_info info, FILE *fp) {
        if((*input)->cshllink_extdatablk.ConsoleDataBlock.info.BlockSignature!=0)
            _cshllink_errint(_CSHLLINK_DUPEEX_ConsoleDataBlock);

        (*input)->cshllink_extdatablk.ConsoleDataBlock.info.BlockSignature = _CSHLLINK_EDBLK_ConsoleDataBlockSig;

        if(info.BlockSize!=_CSHLLINK_EDBLK_ConsoleDataBlockSiz)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);
        (*input)->cshllink_extdatablk.ConsoleDataBlock.info.BlockSize=info.BlockSize;

        //FillAttributes
        if(fread(&(*input)->cshllink_extdatablk.ConsoleDataBlock.FillAttributes, 2, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //PopupFillAttributes
        if(fread(&(*input)->cshllink_extdatablk.ConsoleDataBlock.PopupFillAttributes, 2, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //ScreenBufferSizeX
        if(fread(&(*input)->cshllink_extdatablk.ConsoleDataBlock.ScreenBufferSizeX, 2, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //ScreenBufferSizeY
        if(fread(&(*input)->cshllink_extdatablk.ConsoleDataBlock.ScreenBufferSizeY, 2, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);
        
        //WindowSizeX
        if(fread(&(*input)->cshllink_extdatablk.ConsoleDataBlock.WindowSizeX, 2, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);
        
        //WindowSizeY
        if(fread(&(*input)->cshllink_extdatablk.ConsoleDataBlock.WindowSizeY, 2, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //WindowOriginX
        if(fread(&(*input)->cshllink_extdatablk.ConsoleDataBlock.WindowOriginX, 2, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //WindowOriginY
        if(fread(&(*input)->cshllink_extdatablk.ConsoleDataBlock.WindowOriginY, 2, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);
    
        fseek(fp, 8, SEEK_CUR);

        //FontSize
        if(fread(&(*input)->cshllink_extdatablk.ConsoleDataBlock.FontSize, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);
    
        //FontFamily
        if(fread(&(*input)->cshllink_extdatablk.ConsoleDataBlock.FontFamily, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //FontWeight
        if(fread(&(*input)->cshllink_extdatablk.ConsoleDataBlock.FontWeight, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //FaceName
        if(cshllink_rwstr(&(*input)->cshllink_extdatablk.ConsoleDataBlock.FaceName, _CSHLLINK_ERRX_NULLPSTRFNAME, _CSHLLINK_ERR_FIO, fp, 32))
            return -1;

        //CursorSize
        if(fread(&(*input)->cshllink_extdatablk.ConsoleDataBlock.CursorSize, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //FullScreen
        if(fread(&(*input)->cshllink_extdatablk.ConsoleDataBlock.FullScreen, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);
        
        //QuickEdit
        if(fread(&(*input)->cshllink_extdatablk.ConsoleDataBlock.QuickEdit, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //InsertMode
        if(fread(&(*input)->cshllink_extdatablk.ConsoleDataBlock.InsertMode, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //AutoPosition
        if(fread(&(*input)->cshllink_extdatablk.ConsoleDataBlock.AutoPosition, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //HistoryBufferSize
        if(fread(&(*input)->cshllink_extdatablk.ConsoleDataBlock.HistoryBufferSize, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //NumberOfHistoryBuffers
        if(fread(&(*input)->cshllink_extdatablk.ConsoleDataBlock.NumberOfHistoryBuffers, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //HistoryNoDup
        if(fread(&(*input)->cshllink_extdatablk.ConsoleDataBlock.HistoryNoDup, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //AutoPosition
        if(fread(&(*input)->cshllink_extdatablk.ConsoleDataBlock.AutoPosition, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);
       
        //ColorTable
        for(int i=0; i<16; i++)
            if(fread(&(*input)->cshllink_extdatablk.ConsoleDataBlock.ColorTable[i], 4, 1, fp)!=1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);

        return 0;
    }

    uint8_t _cshllink_readEConsoleFEDataBlock(cshllink **input, const struct _cshllink_extdatablk_blk_info info, FILE *fp) {
        if((*input)->cshllink_extdatablk.ConsoleFEDataBlock.info.BlockSignature!=0)
            _cshllink_errint(_CSHLLINK_DUPEEX_ConsoleFEDataBlock);
        
        (*input)->cshllink_extdatablk.ConsoleFEDataBlock.info.BlockSignature = _CSHLLINK_EDBLK_ConsoleFEDataBlockSig;

        if(info.BlockSize!=_CSHLLINK_EDBLK_ConsoleFEDataBlockSiz)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);
        (*input)->cshllink_extdatablk.ConsoleFEDataBlock.info.BlockSize=info.BlockSize;

        //CodePage
        if(fread(&(*input)->cshllink_extdatablk.ConsoleFEDataBlock.CodePage, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        return 0;
    }

    uint8_t _cshllink_readEDarwinDataBlock(cshllink **input, const struct _cshllink_extdatablk_blk_info info, FILE *fp) {
        if((*input)->cshllink_extdatablk.DarwinDataBlock.info.BlockSignature!=0)
            _cshllink_errint(_CSHLLINK_DUPEEX_DarwinDataBlock);
        
        (*input)->cshllink_extdatablk.DarwinDataBlock.info.BlockSignature = _CSHLLINK_EDBLK_DarwinDataBlockSig;

        if(info.BlockSize!=_CSHLLINK_EDBLK_DarwinDataBlockSiz)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);
        (*input)->cshllink_extdatablk.DarwinDataBlock.info.BlockSize=info.BlockSize;

        //DarwinDataAnsi
        if(cshllink_rstr(&(*input)->cshllink_extdatablk.DarwinDataBlock.DarwinDataAnsi, _CSHLLINK_ERRX_NULLPSTRDARDA, _CSHLLINK_ERR_FIO, fp, 260))
            return -1;
        
        //DarwinDataUnicode
        if(cshllink_rwstr(&(*input)->cshllink_extdatablk.DarwinDataBlock.DarwinDataUnicode, _CSHLLINK_ERRX_NULLPSTRDARDU, _CSHLLINK_ERR_FIO, fp, 520))
            return -1;

        return 0;
    }

    uint8_t _cshllink_readEEnvironmentVariableDataBlock(cshllink **input, const struct _cshllink_extdatablk_blk_info info, FILE *fp) {
        if((*input)->cshllink_extdatablk.EnvironmentVariableDataBlock.info.BlockSignature!=0)
            _cshllink_errint(_CSHLLINK_DUPEEX_EnvironmentVariableDataBlock);
        
        (*input)->cshllink_extdatablk.EnvironmentVariableDataBlock.info.BlockSignature = _CSHLLINK_EDBLK_EnvironmentVariableDataBlockSig;

        if(info.BlockSize!=_CSHLLINK_EDBLK_EnvironmentVariableDataBlockSiz)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);
        (*input)->cshllink_extdatablk.EnvironmentVariableDataBlock.info.BlockSize=info.BlockSize;

        //EnvironmentVariableDataAnsi
        if(cshllink_rstr(&(*input)->cshllink_extdatablk.EnvironmentVariableDataBlock.TargetAnsi, _CSHLLINK_ERRX_NULLPSTRENVDA, _CSHLLINK_ERR_FIO, fp, 260))
            return -1;
        
        //EnvironmentVariableDataUnicode
        if(cshllink_rwstr(&(*input)->cshllink_extdatablk.EnvironmentVariableDataBlock.TargetUnicode, _CSHLLINK_ERRX_NULLPSTRENVDU, _CSHLLINK_ERR_FIO, fp, 520))
            return -1;

        return 0;
    }

    uint8_t _cshllink_readEIconEnvironmentDataBlock(cshllink **input, const struct _cshllink_extdatablk_blk_info info, FILE *fp) {
        if((*input)->cshllink_extdatablk.IconEnvironmentDataBlock.info.BlockSignature!=0)
            _cshllink_errint(_CSHLLINK_DUPEEX_IconEnvironmentDataBlock);
        
        (*input)->cshllink_extdatablk.IconEnvironmentDataBlock.info.BlockSignature = _CSHLLINK_EDBLK_IconEnvironmentDataBlockSig;

        if(info.BlockSize!=_CSHLLINK_EDBLK_IconEnvironmentDataBlockSiz)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);
        (*input)->cshllink_extdatablk.IconEnvironmentDataBlock.info.BlockSize=info.BlockSize;
        //IconEnvironmentDataAnsi
        if(cshllink_rstr(&(*input)->cshllink_extdatablk.IconEnvironmentDataBlock.TargetAnsi, _CSHLLINK_ERRX_NULLPSTRIENVDA, _CSHLLINK_ERR_FIO, fp, 260))
            return -1;

        //IconEnvironmentDataUnicode
        if(cshllink_rwstr(&(*input)->cshllink_extdatablk.IconEnvironmentDataBlock.TargetUnicode, _CSHLLINK_ERRX_NULLPSTRIENVDU, _CSHLLINK_ERR_FIO, fp, 520))
            return -1;

        return 0;
    }

    uint8_t _cshllink_readEKnownFolderDataBlock(cshllink **input, const struct _cshllink_extdatablk_blk_info info, FILE *fp) {
        if((*input)->cshllink_extdatablk.KnownFolderDataBlock.info.BlockSignature!=0)
            _cshllink_errint(_CSHLLINK_DUPEEX_KnownFolderDataBlock);
        
        (*input)->cshllink_extdatablk.KnownFolderDataBlock.info.BlockSignature = _CSHLLINK_EDBLK_KnownFolderDataBlockSig;

        if(info.BlockSize!=_CSHLLINK_EDBLK_KnownFolderDataBlockSiz)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);
        (*input)->cshllink_extdatablk.KnownFolderDataBlock.info.BlockSize=info.BlockSize;

        //KnownFolderID
        if(cshllink_rstr((char **)&(*input)->cshllink_extdatablk.KnownFolderDataBlock.KnownFolderID, _CSHLLINK_ERR_NULLPEXTD, _CSHLLINK_ERR_FIO, fp, 16))
            return -1;
        
        //Offset
        if(fread(&(*input)->cshllink_extdatablk.KnownFolderDataBlock.Offset, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        return 0;
    }

    uint8_t _cshllink_readEPropertyStoreDataBlock(cshllink **input, const struct _cshllink_extdatablk_blk_info info, FILE *fp) {
        if((*input)->cshllink_extdatablk.PropertyStoreDataBlock.info.BlockSignature!=0)
            _cshllink_errint(_CSHLLINK_DUPEEX_PropertyStoreDataBlock);
        
        (*input)->cshllink_extdatablk.PropertyStoreDataBlock.info.BlockSignature = _CSHLLINK_EDBLK_PropertyStoreDataBlockSig;

        if(info.BlockSize<_CSHLLINK_EDBLK_PropertyStoreDataBlockSiz)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);
        (*input)->cshllink_extdatablk.PropertyStoreDataBlock.info.BlockSize=info.BlockSize;

        //PropertyStore
        if(cshllink_rstr((char **)&(*input)->cshllink_extdatablk.KnownFolderDataBlock.KnownFolderID, _CSHLLINK_ERR_NULLPEXTD, _CSHLLINK_ERR_FIO, fp, (*input)->cshllink_extdatablk.PropertyStoreDataBlock.info.BlockSize-8))
            return -1;

        return 0;
    }

    uint8_t _cshllink_readEShimDataBlock(cshllink **input, const struct _cshllink_extdatablk_blk_info info, FILE *fp) {
        if((*input)->cshllink_extdatablk.ShimDataBlock.info.BlockSignature!=0)
            _cshllink_errint(_CSHLLINK_DUPEEX_ShimDataBlock);
        
        (*input)->cshllink_extdatablk.ShimDataBlock.info.BlockSignature = _CSHLLINK_EDBLK_ShimDataBlockSig;

        if(info.BlockSize<_CSHLLINK_EDBLK_ShimDataBlockSiz)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);
        (*input)->cshllink_extdatablk.ShimDataBlock.info.BlockSize=info.BlockSize;

        //LayerName
        if(cshllink_rwstr(&(*input)->cshllink_extdatablk.ShimDataBlock.LayerName, _CSHLLINK_ERR_NULLPEXTD, _CSHLLINK_ERR_FIO, fp, (*input)->cshllink_extdatablk.ShimDataBlock.info.BlockSize-8))
            return -1;

        return 0;
    }

    uint8_t _cshllink_readESpecialFolderDataBlock(cshllink **input, const struct _cshllink_extdatablk_blk_info info, FILE *fp) {
        if((*input)->cshllink_extdatablk.SpecialFolderDataBlock.info.BlockSignature!=0)
            _cshllink_errint(_CSHLLINK_DUPEEX_SpecialFolderDataBlock);
        
        (*input)->cshllink_extdatablk.SpecialFolderDataBlock.info.BlockSignature = _CSHLLINK_EDBLK_SpecialFolderDataBlockSig;

        if(info.BlockSize!=_CSHLLINK_EDBLK_SpecialFolderDataBlockSiz)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);
        (*input)->cshllink_extdatablk.SpecialFolderDataBlock.info.BlockSize=info.BlockSize;

        //SpecialFolderID
        if(fread(&(*input)->cshllink_extdatablk.SpecialFolderDataBlock.SpecialFolderID, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //Offset
        if(fread(&(*input)->cshllink_extdatablk.SpecialFolderDataBlock.Offset, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        return 0;
    }

    uint8_t _cshllink_readETrackerDataBlock(cshllink **input, const struct _cshllink_extdatablk_blk_info info, FILE *fp) {
        if((*input)->cshllink_extdatablk.TrackerDataBlock.info.BlockSignature!=0)
            _cshllink_errint(_CSHLLINK_DUPEEX_TrackerDataBlock);
        (*input)->cshllink_extdatablk.TrackerDataBlock.info.BlockSignature = _CSHLLINK_EDBLK_TrackerDataBlockSig;

        if(info.BlockSize!=_CSHLLINK_EDBLK_TrackerDataBlockSiz)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);
        (*input)->cshllink_extdatablk.TrackerDataBlock.info.BlockSize=info.BlockSize;

        //Length
        if(fread(&(*input)->cshllink_extdatablk.TrackerDataBlock.Length, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);
        if((*input)->cshllink_extdatablk.TrackerDataBlock.Length!=_CSHLLINK_EDBLK_TrackerDataBlockLen)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);

        //Version
        if(fread(&(*input)->cshllink_extdatablk.TrackerDataBlock.Version, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);
        if((*input)->cshllink_extdatablk.TrackerDataBlock.Version!=0)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGVERSION);
        
        //MachineID
        if(cshllink_rstr(&(*input)->cshllink_extdatablk.TrackerDataBlock.MachineID, _CSHLLINK_ERR_NULLPEXTD, _CSHLLINK_ERR_FIO, fp, 16))
            return -1;

        //Droid
        if(cshllink_rstr((char **)&(*input)->cshllink_extdatablk.TrackerDataBlock.Droid, _CSHLLINK_ERR_NULLPEXTD, _CSHLLINK_ERR_FIO, fp, 32))
            return -1;

        //DroidBirth
        if(cshllink_rstr((char **)&(*input)->cshllink_extdatablk.TrackerDataBlock.DroidBirth, _CSHLLINK_ERR_NULLPEXTD, _CSHLLINK_ERR_FIO, fp, 32))
            return -1;


        return 0;
    }

    uint8_t _cshllink_readEVistaAndAboveIDListDataBlock(cshllink **input, const struct _cshllink_extdatablk_blk_info info, FILE *fp) {
        if((*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.info.BlockSignature!=0)
            _cshllink_errint(_CSHLLINK_DUPEEX_VistaAndAboveIDListDataBlock);
        
        (*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.info.BlockSignature = _CSHLLINK_EDBLK_VistaAndAboveIDListDataBlockSig;

        if(info.BlockSize<_CSHLLINK_EDBLK_VistaAndAboveIDListDataBlockSiz)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);
        (*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.info.BlockSize=info.BlockSize;

        int tmpS=(*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.info.BlockSize=info.BlockSize-10;

        //realloc enough mem
        (*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_item = realloc((*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_item, (tmpS)* sizeof *(*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_item);
        if((*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_item==NULL) 
            _cshllink_errint(_CSHLLINK_ERR_NULLPIDL);

        while(tmpS>0) {
            
            //Element size
            if(fread(&(*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_item[(*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_inum].item_size, 2, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
            //Element (size -2 as struct contains one 2byte var)
            (*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_item[(*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_inum].item = malloc(((*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_item[(*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_inum].item_size-2) * sizeof *(*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_item[(*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_inum].item);
            if((*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_item[(*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_inum].item==NULL)
                _cshllink_errint(_CSHLLINK_ERR_NULLPIDLM);
            if(fread((*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_item[(*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_inum].item, (*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_item[(*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_inum].item_size-2, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
            
            tmpS-=(*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_item[(*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_inum].item_size;
            (*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_inum++;
        }
        //free unneccessary mem
        (*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_item = realloc((*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_item, ((*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_inum)* sizeof *(*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_item);
        if((*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_item==NULL) 
            _cshllink_errint(_CSHLLINK_ERR_NULLPIDL);

        uint16_t nullb;
        if(fread(&nullb, 2, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);
        if(tmpS<0||nullb!=0)
            _cshllink_errint(_CSHLLINK_ERR_INVIDL);  

        return 0;
    }

    #pragma endregion

    #pragma region writeFile

    /*
        -> open file descriptor of type FILE (W mode)
        -> cshllink structure pointer containing the FILE content
        -- writes content to file
        <- on error this function will return -1 (the content of inputStruct is undefined), on success 0

        exact error codes are stored in cshllink_error
    */
    uint8_t cshllink_writeFile(FILE *fp, cshllink *inputStruct) {

        // test if FILE is open
        if(fp==NULL) 
            _cshllink_errint(_CSHLLINK_ERR_FCL);

        // test if FILE is in WRITE mode
        fseek(fp, 0, SEEK_SET);
        {
            char tmp=1;
            if(fwrite(&tmp, 1, 1, fp)!=1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
        }

        // write FILE
        return cshllink_writeFile_i(fp, inputStruct);
    }

    /*
        Processes outputFile
    */
    uint8_t cshllink_writeFile_i(FILE *fp, cshllink *inputStruct) {
        
        fseek(fp, 0, SEEK_SET);

        /*
            HEADER
        */
       {
            //HeaderSize
            if(inputStruct->cshllink_header.HeaderSize!=0x4c)
                _cshllink_errint(_CSHLLINK_ERR_WHEADS);
            if(fwrite(&inputStruct->cshllink_header.HeaderSize, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
            
            //LinkCLSID
            if(inputStruct->cshllink_header.LinkCLSID_H!=0x0114020000000000)
                _cshllink_errint(_CSHLLINK_ERR_WCLSIDS);
            if(inputStruct->cshllink_header.LinkCLSID_L!=0xC000000000000046)
                _cshllink_errint(_CSHLLINK_ERR_WCLSIDS);
            cshllink_sEndian(&inputStruct->cshllink_header.LinkCLSID_L, 16);
            if(fwrite(&inputStruct->cshllink_header.LinkCLSID_L, 16, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);

            //LinkFlags
            if(fwrite(&inputStruct->cshllink_header.LinkFlags, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
            
            //FileAttributes
            if(fwrite(&inputStruct->cshllink_header.FileAttributes, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
            
            //CreationTime
            if(fwrite(&inputStruct->cshllink_header.CreationTime, 8, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
            //AccessTime
            if(fwrite(&inputStruct->cshllink_header.AccessTime, 8, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
            //WriteTime
            if(fwrite(&inputStruct->cshllink_header.WriteTime, 8, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);

            //FileSize
            if(fwrite(&inputStruct->cshllink_header.FileSize, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);

            //IconIndex
            if(fwrite(&inputStruct->cshllink_header.IconIndex, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
            
            //ShowCommand
            if(fwrite(&inputStruct->cshllink_header.ShowCommand, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);

            //HotKey
            if(fwrite(&inputStruct->cshllink_header.HotKey, 2, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);

            //10 bytes null
            char tmp[]="\0\0\0\0\0\0\0\0\0\0";
            if(fwrite(&tmp, 1, 10, fp) != 10)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
        }

        /*
            LinkTargetIDList
        */
        if(inputStruct->cshllink_header.LinkFlags&CSHLLINK_LF_HasLinkTargetIDList) {
            //IDList size
            if(fwrite(&inputStruct->cshllink_lnktidl.idl_size, 2, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);

            if(inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_item==NULL) 
                _cshllink_errint(_CSHLLINK_ERR_NULLPIDL);
            for(int i=0; i<inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_inum; i++) {
                //Element size
                if(fwrite(&inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_item[i].item_size, 2, 1, fp) != 1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);
                //Element (size -2 as struct contains one 2byte var)
                if(inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_item[i].item==NULL)
                    _cshllink_errint(_CSHLLINK_ERR_NULLPIDLM);
                if(fwrite(inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_item[i].item, inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_item[i].item_size-2, 1, fp) != 1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);
            }
            //TerminalBlock
            uint16_t nullb=0;
            if(fwrite(&nullb, 2, 1, fp)!=1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
        }

        /*
            LinkInfo
        */
        if(inputStruct->cshllink_header.LinkFlags&CSHLLINK_LF_HasLinkInfo) {
            //LinkInfoSize
            if(fwrite(&inputStruct->cshllink_lnkinfo.LinkInfoSize, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
            
            //LinkInfoHeaderSize
            if(inputStruct->cshllink_lnkinfo.LinkInfoHeaderSize!=0x0000001C && inputStruct->cshllink_lnkinfo.LinkInfoHeaderSize<0x00000024)
                _cshllink_errint(_CSHLLINK_ERR_INVLIHS);
            if(fwrite(&inputStruct->cshllink_lnkinfo.LinkInfoHeaderSize, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
            
            //LinkInfoFlags
            if(fwrite(&inputStruct->cshllink_lnkinfo.LinkInfoFlags, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);

            //VolumeIDOffset
            if(fwrite(&inputStruct->cshllink_lnkinfo.VolumeIDOffset, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
            
            //LocalBasePathOffset
            if(fwrite(&inputStruct->cshllink_lnkinfo.LocalBasePathOffset, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);

            //CommonNetworkRelativeLinkOffset
            if(fwrite(&inputStruct->cshllink_lnkinfo.CommonNetworkRelativeLinkOffset, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);

            //CommonPathSuffixOffset
            if(fwrite(&inputStruct->cshllink_lnkinfo.CommonPathSuffixOffset, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);

            //only present if LinkInfoHeaderSize >= 0x00000024
            if(inputStruct->cshllink_lnkinfo.LinkInfoHeaderSize>=0x00000024) {
                //LocalBasePathOffsetUnicode
                if(fwrite(&inputStruct->cshllink_lnkinfo.LocalBasePathOffsetUnicode, 4, 1, fp) != 1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);

                //CommonPathSuffixOffsetUnicode
                if(fwrite(&inputStruct->cshllink_lnkinfo.CommonPathSuffixOffsetUnicode, 4, 1, fp) != 1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);
            }

            /*
                VolumeID
            */
            if(inputStruct->cshllink_lnkinfo.LinkInfoFlags&CSHLLINK_LIF_VolumeIDAndLocalBasePath) {
                int VtmpSize=16;
                //VolumeIDSize
                if(fwrite(&inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeIDSize, 4, 1, fp) != 1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);

                //DriveType
                if(fwrite(&inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.DriveType, 4, 1, fp) != 1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);
                
                //DriveSerialNumber
                if(fwrite(&inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.DriveSerialNumber, 4, 1, fp) != 1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);

                //VolumeLabelOffset
                if(fwrite(&inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeLabelOffset, 4, 1, fp) != 1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);
                
                //only present if VolumeLabelOffset == 0x00000014
                if(inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeLabelOffset==0x00000014) {
                    //VolumeLabelOffsetUnicode
                    if(fwrite(&inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeLabelOffsetUnicode, 4, 1, fp) != 1)
                        _cshllink_errint(_CSHLLINK_ERR_FIO);
                    VtmpSize+=4;
                }

                //DATA
                if(inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeIDSize<=VtmpSize)
                    _cshllink_errint(_CSHLLINK_ERR_VIDSLOW);
                if(inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.Data==NULL)
                    _cshllink_errint(_CSHLLINK_ERR_NULLPVIDD);
                //Data
                if(inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeLabelOffset==0x00000014) {
                    if(fwrite(inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.Data, sizeof(char16_t), (inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeIDSize-VtmpSize)/2, fp) != (inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeIDSize-VtmpSize)/2)
                        _cshllink_errint(_CSHLLINK_ERR_FIO);
                }
                else {
                    if(fwrite(inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.Data, sizeof(char), inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeIDSize-VtmpSize, fp) != inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeIDSize-VtmpSize)
                        _cshllink_errint(_CSHLLINK_ERR_FIO);
                }

                //LocalBasePath
                if(cshllink_wNULLstr(&inputStruct->cshllink_lnkinfo.LocalBasePath, _CSHLLINK_ERR_NULLPLBP, _CSHLLINK_ERR_FIO, fp))
                    return -1;
            }
            /*
                CommonNetworkRelativeLink
            */
            if(inputStruct->cshllink_lnkinfo.LinkInfoFlags&CSHLLINK_LIF_CommonNetworkRelativeLinkAndPathSuffix) {
                //CommonNetworkRelativeSize
                if(fwrite(&inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_cnetrlnk.CommonNetworkRelativeSize, 4, 1, fp) != 1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);

                //CommonNetworkRelativeLinkFlags
                if(fwrite(&inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_cnetrlnk.CommonNetworkRelativeLinkFlags, 4, 1, fp) != 1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);

                //NetNameOffset
                if(fwrite(&inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_cnetrlnk.NetNameOffset, 4, 1, fp) != 1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);

                //DeviceNameOffset
                if(fwrite(&inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_cnetrlnk.DeviceNameOffset, 4, 1, fp) != 1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);
                
                //NetworkProviderType
                if(fwrite(&inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_cnetrlnk.NetworkProviderType, 4, 1, fp) != 1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);
                
                if(inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_cnetrlnk.NetNameOffset>0x00000014) {
                    //NetNameOffsetUnicode
                    if(fwrite(&inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_cnetrlnk.NetNameOffsetUnicode, 4, 1, fp) != 1)
                        _cshllink_errint(_CSHLLINK_ERR_FIO);

                    //DeviceNameOffsetUnicode
                    if(fwrite(&inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_cnetrlnk.DeviceNameOffsetUnicode, 4, 1, fp) != 1)
                        _cshllink_errint(_CSHLLINK_ERR_FIO);
                }

                //NetName
                if(cshllink_wNULLstr(&inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_cnetrlnk.NetName, _CSHLLINK_ERR_NULLPNETN, _CSHLLINK_ERR_FIO, fp))
                        return -1;
                
                //DeviceName
                if(cshllink_wNULLstr(&inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_cnetrlnk.DeviceName, _CSHLLINK_ERR_NULLPDEVN, _CSHLLINK_ERR_FIO, fp))
                        return -1;

                if(inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_cnetrlnk.NetNameOffset>0x00000014) {
                    //NetNameUnicode
                    if(cshllink_wNULLstr(&inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_cnetrlnk.NetNameUnicode, _CSHLLINK_ERR_NULLPNNU, _CSHLLINK_ERR_FIO, fp))
                            return -1;

                    //DeviceNameUnicode
                    if(cshllink_wNULLstr(&inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_cnetrlnk.DeviceNameUnicode, _CSHLLINK_ERR_NULLPDNU, _CSHLLINK_ERR_FIO, fp))
                            return -1;
                }
            }

            //CommonPathSuffix
            if(cshllink_wNULLstr(&inputStruct->cshllink_lnkinfo.CommonPathSuffix, _CSHLLINK_ERR_NULLPCPS, _CSHLLINK_ERR_FIO, fp))
                    return -1;

            if(inputStruct->cshllink_lnkinfo.LinkInfoHeaderSize>=0x00000024) {
                //LocalBasePathUnicode
                if(inputStruct->cshllink_lnkinfo.LinkInfoFlags&CSHLLINK_LIF_VolumeIDAndLocalBasePath) {
                    if(cshllink_wNULLwstr(&inputStruct->cshllink_lnkinfo.LocalBasePathUnicode, _CSHLLINK_ERR_NULLPLBPU, _CSHLLINK_ERR_FIO, fp))
                        return -1;
                }
                //CommonPathSuffixUnicode
                if(cshllink_wNULLwstr(&inputStruct->cshllink_lnkinfo.CommonPathSuffixUnicode, _CSHLLINK_ERR_NULLPCPSU, _CSHLLINK_ERR_FIO, fp))
                    return -1;
            }
        
        }

        /*
            StringData (all unicode 2 bytes)
        */
            if(inputStruct->cshllink_header.LinkFlags&CSHLLINK_LF_HasName) {
                //NameString
                if(fwrite(&inputStruct->cshllink_strdata.NameString.CountCharacters, 2, 1, fp)!=1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);
                if(cshllink_wwstr(&inputStruct->cshllink_strdata.NameString.UString, _CSHLLINK_ERR_NULLPSTRDNAME, _CSHLLINK_ERR_FIO, fp, inputStruct->cshllink_strdata.NameString.CountCharacters*2))
                    return -1;
            }
            if(inputStruct->cshllink_header.LinkFlags&CSHLLINK_LF_HasRelativePath) {
                //RelativePath
                if(fwrite(&inputStruct->cshllink_strdata.RelativePath.CountCharacters, 2, 1, fp)!=1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);
                if(cshllink_wwstr(&inputStruct->cshllink_strdata.RelativePath.UString, _CSHLLINK_ERR_NULLPSTRDRPATH, _CSHLLINK_ERR_FIO, fp, inputStruct->cshllink_strdata.RelativePath.CountCharacters*2))
                    return -1;
            }
            if(inputStruct->cshllink_header.LinkFlags&CSHLLINK_LF_HasWorkingDir) {
                //WorkingDir
                if(fwrite(&inputStruct->cshllink_strdata.WorkingDir.CountCharacters, 2, 1, fp)!=1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);
                if(cshllink_wwstr(&inputStruct->cshllink_strdata.WorkingDir.UString, _CSHLLINK_ERR_NULLPSTRDWDIR, _CSHLLINK_ERR_FIO, fp, inputStruct->cshllink_strdata.WorkingDir.CountCharacters*2))
                    return -1;
            }
            if(inputStruct->cshllink_header.LinkFlags&CSHLLINK_LF_HasArguments) {
                //CommandLineArguments
                if(fwrite(&inputStruct->cshllink_strdata.CommandLineArguments.CountCharacters, 2, 1, fp)!=1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);
                if(cshllink_wwstr(&inputStruct->cshllink_strdata.CommandLineArguments.UString, _CSHLLINK_ERR_NULLPSTRDARG, _CSHLLINK_ERR_FIO, fp, inputStruct->cshllink_strdata.CommandLineArguments.CountCharacters*2))
                    return -1;
            }
            if(inputStruct->cshllink_header.LinkFlags&CSHLLINK_LF_HasIconLocation) {
                //IconLocation
                if(fwrite(&inputStruct->cshllink_strdata.IconLocation.CountCharacters, 2, 1, fp)!=1)
                    _cshllink_errint(_CSHLLINK_ERR_FIO);
                if(cshllink_wwstr(&inputStruct->cshllink_strdata.IconLocation.UString, _CSHLLINK_ERR_NULLPSTRDICO, _CSHLLINK_ERR_FIO, fp, inputStruct->cshllink_strdata.IconLocation.CountCharacters*2))
                    return -1;
            }
        /*
            ExtraDataBlock
        */
        {

            if(_cshllink_writeEConsoleDataBlock(&inputStruct, fp)) return -1;
            if(_cshllink_writeEConsoleFEDataBlock(&inputStruct, fp)) return -1;
            if(_cshllink_writeEDarwinDataBlock(&inputStruct, fp)) return -1;
            if(_cshllink_writeEEnvironmentVariableDataBlock(&inputStruct, fp)) return -1;
            if(_cshllink_writeEIconEnvironmentDataBlock(&inputStruct, fp)) return -1;
            if(_cshllink_writeEKnownFolderDataBlock(&inputStruct, fp)) return -1;
            if(_cshllink_writeEPropertyStoreDataBlock(&inputStruct, fp)) return -1;
            if(_cshllink_writeEShimDataBlock(&inputStruct, fp)) return -1;
            if(_cshllink_writeESpecialFolderDataBlock(&inputStruct, fp)) return -1;
            if(_cshllink_writeETrackerDataBlock(&inputStruct, fp)) return -1;
            if(_cshllink_writeEVistaAndAboveIDListDataBlock(&inputStruct, fp)) return -1;

            //TerminalBlock
            char tmp[]="\0\0\0\0";
            if(fwrite(&tmp, 1, 4, fp) != 4)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
        }

        return 0;
    }

    /*
        write NULL terminated String
    */
    uint32_t cshllink_wNULLstr(char **dest, uint8_t errv1, uint8_t errv2, FILE *fp) {
        char tmpC=1;
        uint32_t index=0;
        while(tmpC!=0) {
            if(*dest==NULL)
                _cshllink_errint(errv1);
            if(fwrite(&(*dest)[index], 1, 1, fp) != 1)
                _cshllink_errint(errv2);
            tmpC = (*dest)[index];
            index++;
        }
        return 0;
    }
    uint32_t cshllink_wNULLwstr(char16_t **dest, uint8_t errv1, uint8_t errv2, FILE *fp) {
        char16_t tmpC=1;
        uint32_t index=0;
        while(tmpC!=0) {
            if(*dest==NULL)
                _cshllink_errint(errv1);
            if(fwrite(&(*dest)[index], sizeof(char16_t), 1, fp) != 1)
                _cshllink_errint(errv2);
            tmpC = (*dest)[index];
            index++;
        }
        return 0;
    }

    /*
        write String
    */
    uint32_t cshllink_wstr(char **dest, uint8_t errv1, uint8_t errv2, FILE *fp, size_t size) {
        if(size==0) _cshllink_errint(errv1);
        if(*dest==NULL)
            _cshllink_errint(errv1);
        if(fwrite(*dest, 1, size, fp) != size)
            _cshllink_errint(errv2);
        return 0;
    }
    uint32_t cshllink_wwstr(char16_t **dest, uint8_t errv1, uint8_t errv2, FILE *fp, size_t size) {
        size/=2;
        if(size==0) _cshllink_errint(errv1);
        if(*dest==NULL)
            _cshllink_errint(errv1);
        if(fwrite(*dest, sizeof(char16_t), size, fp) != size)
            _cshllink_errint(errv2);
        return 0;
    }

    /*
        Extra Data Block write functions
    */
    uint8_t _cshllink_writeEConsoleDataBlock(cshllink **input, FILE *fp) {
        if((*input)->cshllink_extdatablk.ConsoleDataBlock.info.BlockSignature!=_CSHLLINK_EDBLK_ConsoleDataBlockSig) return 0;

        if((*input)->cshllink_extdatablk.ConsoleDataBlock.info.BlockSize!=_CSHLLINK_EDBLK_ConsoleDataBlockSiz)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);

        //Size and Signature
        if(fwrite(&(*input)->cshllink_extdatablk.ConsoleDataBlock.info.BlockSize, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
        if(fwrite(&(*input)->cshllink_extdatablk.ConsoleDataBlock.info.BlockSignature, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);

        //FillAttributes
        if(fwrite(&(*input)->cshllink_extdatablk.ConsoleDataBlock.FillAttributes, 2, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //PopupFillAttributes
        if(fwrite(&(*input)->cshllink_extdatablk.ConsoleDataBlock.PopupFillAttributes, 2, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //ScreenBufferSizeX
        if(fwrite(&(*input)->cshllink_extdatablk.ConsoleDataBlock.ScreenBufferSizeX, 2, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //ScreenBufferSizeY
        if(fwrite(&(*input)->cshllink_extdatablk.ConsoleDataBlock.ScreenBufferSizeY, 2, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);
        
        //WindowSizeX
        if(fwrite(&(*input)->cshllink_extdatablk.ConsoleDataBlock.WindowSizeX, 2, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);
        
        //WindowSizeY
        if(fwrite(&(*input)->cshllink_extdatablk.ConsoleDataBlock.WindowSizeY, 2, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //WindowOriginX
        if(fwrite(&(*input)->cshllink_extdatablk.ConsoleDataBlock.WindowOriginX, 2, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //WindowOriginY
        if(fwrite(&(*input)->cshllink_extdatablk.ConsoleDataBlock.WindowOriginY, 2, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);
    
        //8 bytes NULL
            char tmp[]="\0\0\0\0\0\0\0\0";
            if(fwrite(&tmp, 1, 8, fp) != 8)
                _cshllink_errint(_CSHLLINK_ERR_FIO);

        //FontSize
        if(fwrite(&(*input)->cshllink_extdatablk.ConsoleDataBlock.FontSize, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);
    
        //FontFamily
        if(fwrite(&(*input)->cshllink_extdatablk.ConsoleDataBlock.FontFamily, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //FontWeight
        if(fwrite(&(*input)->cshllink_extdatablk.ConsoleDataBlock.FontWeight, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //FaceName
        if(cshllink_wwstr(&(*input)->cshllink_extdatablk.ConsoleDataBlock.FaceName, _CSHLLINK_ERRX_NULLPSTRFNAME, _CSHLLINK_ERR_FIO, fp, 32))
            return -1;

        //CursorSize
        if(fwrite(&(*input)->cshllink_extdatablk.ConsoleDataBlock.CursorSize, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //FullScreen
        if(fwrite(&(*input)->cshllink_extdatablk.ConsoleDataBlock.FullScreen, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);
        
        //QuickEdit
        if(fwrite(&(*input)->cshllink_extdatablk.ConsoleDataBlock.QuickEdit, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //InsertMode
        if(fwrite(&(*input)->cshllink_extdatablk.ConsoleDataBlock.InsertMode, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //AutoPosition
        if(fwrite(&(*input)->cshllink_extdatablk.ConsoleDataBlock.AutoPosition, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //HistoryBufferSize
        if(fwrite(&(*input)->cshllink_extdatablk.ConsoleDataBlock.HistoryBufferSize, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //NumberOfHistoryBuffers
        if(fwrite(&(*input)->cshllink_extdatablk.ConsoleDataBlock.NumberOfHistoryBuffers, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //HistoryNoDup
        if(fwrite(&(*input)->cshllink_extdatablk.ConsoleDataBlock.HistoryNoDup, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //AutoPosition
        if(fwrite(&(*input)->cshllink_extdatablk.ConsoleDataBlock.AutoPosition, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);
       
        //ColorTable
        for(int i=0; i<16; i++)
            if(fwrite(&(*input)->cshllink_extdatablk.ConsoleDataBlock.ColorTable[i], 4, 1, fp)!=1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);

        return 0;
    }

    uint8_t _cshllink_writeEConsoleFEDataBlock(cshllink **input, FILE *fp) {
        if((*input)->cshllink_extdatablk.ConsoleFEDataBlock.info.BlockSignature!=_CSHLLINK_EDBLK_ConsoleFEDataBlockSig)
            return 0;
        
        if((*input)->cshllink_extdatablk.ConsoleFEDataBlock.info.BlockSize!=_CSHLLINK_EDBLK_ConsoleFEDataBlockSiz)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);

        //Size and Signature
        if(fwrite(&(*input)->cshllink_extdatablk.ConsoleFEDataBlock.info.BlockSize, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
        if(fwrite(&(*input)->cshllink_extdatablk.ConsoleFEDataBlock.info.BlockSignature, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);

        //CodePage
        if(fwrite(&(*input)->cshllink_extdatablk.ConsoleFEDataBlock.CodePage, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        return 0;
    }

    uint8_t _cshllink_writeEDarwinDataBlock(cshllink **input, FILE *fp) {
        if((*input)->cshllink_extdatablk.DarwinDataBlock.info.BlockSignature!=_CSHLLINK_EDBLK_DarwinDataBlockSig)
            return 0;
        
        if((*input)->cshllink_extdatablk.DarwinDataBlock.info.BlockSize!=_CSHLLINK_EDBLK_DarwinDataBlockSiz)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);

        //Size and Signature
        if(fwrite(&(*input)->cshllink_extdatablk.DarwinDataBlock.info.BlockSize, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
        if(fwrite(&(*input)->cshllink_extdatablk.DarwinDataBlock.info.BlockSignature, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);

        //DarwinDataAnsi
        if(cshllink_wstr(&(*input)->cshllink_extdatablk.DarwinDataBlock.DarwinDataAnsi, _CSHLLINK_ERRX_NULLPSTRDARDA, _CSHLLINK_ERR_FIO, fp, 260))
            return -1;
        
        //DarwinDataUnicode
        if(cshllink_wwstr(&(*input)->cshllink_extdatablk.DarwinDataBlock.DarwinDataUnicode, _CSHLLINK_ERRX_NULLPSTRDARDU, _CSHLLINK_ERR_FIO, fp, 520))
            return -1;

        return 0;
    }

    uint8_t _cshllink_writeEEnvironmentVariableDataBlock(cshllink **input, FILE *fp) {
        if((*input)->cshllink_extdatablk.EnvironmentVariableDataBlock.info.BlockSignature!=_CSHLLINK_EDBLK_EnvironmentVariableDataBlockSig)
            return 0;
        
        if((*input)->cshllink_extdatablk.EnvironmentVariableDataBlock.info.BlockSize!=_CSHLLINK_EDBLK_EnvironmentVariableDataBlockSiz)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);

        //Size and Signature
        if(fwrite(&(*input)->cshllink_extdatablk.EnvironmentVariableDataBlock.info.BlockSize, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
        if(fwrite(&(*input)->cshllink_extdatablk.EnvironmentVariableDataBlock.info.BlockSignature, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);

        //EnvironmentVariableDataAnsi
        if(cshllink_wstr(&(*input)->cshllink_extdatablk.EnvironmentVariableDataBlock.TargetAnsi, _CSHLLINK_ERRX_NULLPSTRENVDA, _CSHLLINK_ERR_FIO, fp, 260))
            return -1;
        
        //EnvironmentVariableDataUnicode
        if(cshllink_wwstr(&(*input)->cshllink_extdatablk.EnvironmentVariableDataBlock.TargetUnicode, _CSHLLINK_ERRX_NULLPSTRENVDU, _CSHLLINK_ERR_FIO, fp, 520))
            return -1;

        return 0;
    }

    uint8_t _cshllink_writeEIconEnvironmentDataBlock(cshllink **input, FILE *fp) {
        if((*input)->cshllink_extdatablk.IconEnvironmentDataBlock.info.BlockSignature!=_CSHLLINK_EDBLK_IconEnvironmentDataBlockSig)
            return 0;
        
        if((*input)->cshllink_extdatablk.IconEnvironmentDataBlock.info.BlockSize!=_CSHLLINK_EDBLK_IconEnvironmentDataBlockSiz)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);
        
        //Size and Signature
        if(fwrite(&(*input)->cshllink_extdatablk.IconEnvironmentDataBlock.info.BlockSize, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
        if(fwrite(&(*input)->cshllink_extdatablk.IconEnvironmentDataBlock.info.BlockSignature, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);

        //IconEnvironmentDataAnsi
        if(cshllink_wstr(&(*input)->cshllink_extdatablk.IconEnvironmentDataBlock.TargetAnsi, _CSHLLINK_ERRX_NULLPSTRIENVDA, _CSHLLINK_ERR_FIO, fp, 260))
            return -1;

        //IconEnvironmentDataUnicode
        if(cshllink_wwstr(&(*input)->cshllink_extdatablk.IconEnvironmentDataBlock.TargetUnicode, _CSHLLINK_ERRX_NULLPSTRIENVDU, _CSHLLINK_ERR_FIO, fp, 520))
            return -1;

        return 0;
    }

    uint8_t _cshllink_writeEKnownFolderDataBlock(cshllink **input, FILE *fp) {
        if((*input)->cshllink_extdatablk.KnownFolderDataBlock.info.BlockSignature!=_CSHLLINK_EDBLK_KnownFolderDataBlockSig)
            return 0;
        
        if((*input)->cshllink_extdatablk.KnownFolderDataBlock.info.BlockSize!=_CSHLLINK_EDBLK_KnownFolderDataBlockSiz)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);

        //Size and Signature
        if(fwrite(&(*input)->cshllink_extdatablk.KnownFolderDataBlock.info.BlockSize, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
        if(fwrite(&(*input)->cshllink_extdatablk.KnownFolderDataBlock.info.BlockSignature, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);

        //KnownFolderID
        if(cshllink_wstr((char **)&(*input)->cshllink_extdatablk.KnownFolderDataBlock.KnownFolderID, _CSHLLINK_ERR_NULLPEXTD, _CSHLLINK_ERR_FIO, fp, 16))
            return -1;
        
        //Offset
        if(fwrite(&(*input)->cshllink_extdatablk.KnownFolderDataBlock.Offset, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        return 0;
    }

    uint8_t _cshllink_writeEPropertyStoreDataBlock(cshllink **input, FILE *fp) {
        if((*input)->cshllink_extdatablk.PropertyStoreDataBlock.info.BlockSignature!=_CSHLLINK_EDBLK_PropertyStoreDataBlockSig)
            return 0;
        
        if((*input)->cshllink_extdatablk.PropertyStoreDataBlock.info.BlockSize<_CSHLLINK_EDBLK_PropertyStoreDataBlockSiz)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);

        //Size and Signature
        if(fwrite(&(*input)->cshllink_extdatablk.PropertyStoreDataBlock.info.BlockSize, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
        if(fwrite(&(*input)->cshllink_extdatablk.PropertyStoreDataBlock.info.BlockSignature, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);

        //PropertyStore
        if(cshllink_wstr((char **)&(*input)->cshllink_extdatablk.KnownFolderDataBlock.KnownFolderID, _CSHLLINK_ERR_NULLPEXTD, _CSHLLINK_ERR_FIO, fp, (*input)->cshllink_extdatablk.PropertyStoreDataBlock.info.BlockSize-8))
            return -1;

        return 0;
    }

    uint8_t _cshllink_writeEShimDataBlock(cshllink **input, FILE *fp) {
        if((*input)->cshllink_extdatablk.ShimDataBlock.info.BlockSignature!=_CSHLLINK_EDBLK_ShimDataBlockSig)
            return 0;
        
        if((*input)->cshllink_extdatablk.ShimDataBlock.info.BlockSize<_CSHLLINK_EDBLK_ShimDataBlockSiz)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);

        //Size and Signature
        if(fwrite(&(*input)->cshllink_extdatablk.ShimDataBlock.info.BlockSize, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
        if(fwrite(&(*input)->cshllink_extdatablk.ShimDataBlock.info.BlockSignature, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);

        //LayerName
        if(cshllink_wwstr(&(*input)->cshllink_extdatablk.ShimDataBlock.LayerName, _CSHLLINK_ERR_NULLPEXTD, _CSHLLINK_ERR_FIO, fp, (*input)->cshllink_extdatablk.ShimDataBlock.info.BlockSize-8))
            return -1;

        return 0;
    }

    uint8_t _cshllink_writeESpecialFolderDataBlock(cshllink **input, FILE *fp) {
        if((*input)->cshllink_extdatablk.SpecialFolderDataBlock.info.BlockSignature!=_CSHLLINK_EDBLK_SpecialFolderDataBlockSig)
            return 0;
        
        if((*input)->cshllink_extdatablk.SpecialFolderDataBlock.info.BlockSize!=_CSHLLINK_EDBLK_SpecialFolderDataBlockSiz)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);

        //Size and Signature
        if(fwrite(&(*input)->cshllink_extdatablk.SpecialFolderDataBlock.info.BlockSize, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
        if(fwrite(&(*input)->cshllink_extdatablk.SpecialFolderDataBlock.info.BlockSignature, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);

        //SpecialFolderID
        if(fwrite(&(*input)->cshllink_extdatablk.SpecialFolderDataBlock.SpecialFolderID, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //Offset
        if(fwrite(&(*input)->cshllink_extdatablk.SpecialFolderDataBlock.Offset, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        return 0;
    }

    uint8_t _cshllink_writeETrackerDataBlock(cshllink **input, FILE *fp) {
        if((*input)->cshllink_extdatablk.TrackerDataBlock.info.BlockSignature!=_CSHLLINK_EDBLK_TrackerDataBlockSig)
            return 0;

        if((*input)->cshllink_extdatablk.TrackerDataBlock.info.BlockSize!=_CSHLLINK_EDBLK_TrackerDataBlockSiz)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);



        //Size and Signature
        if(fwrite(&(*input)->cshllink_extdatablk.TrackerDataBlock.info.BlockSize, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
        if(fwrite(&(*input)->cshllink_extdatablk.TrackerDataBlock.info.BlockSignature, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);

        //Length
        if((*input)->cshllink_extdatablk.TrackerDataBlock.Length!=_CSHLLINK_EDBLK_TrackerDataBlockLen)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);
        if(fwrite(&(*input)->cshllink_extdatablk.TrackerDataBlock.Length, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

        //Version
        if((*input)->cshllink_extdatablk.TrackerDataBlock.Version!=0)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGVERSION);
        if(fwrite(&(*input)->cshllink_extdatablk.TrackerDataBlock.Version, 4, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);
        
        //MachineID
        if(cshllink_wstr(&(*input)->cshllink_extdatablk.TrackerDataBlock.MachineID, _CSHLLINK_ERR_NULLPEXTD, _CSHLLINK_ERR_FIO, fp, 16))
            return -1;

        //Droid
        if(cshllink_wstr((char **)&(*input)->cshllink_extdatablk.TrackerDataBlock.Droid, _CSHLLINK_ERR_NULLPEXTD, _CSHLLINK_ERR_FIO, fp, 32))
            return -1;

        //DroidBirth
        if(cshllink_wstr((char **)&(*input)->cshllink_extdatablk.TrackerDataBlock.DroidBirth, _CSHLLINK_ERR_NULLPEXTD, _CSHLLINK_ERR_FIO, fp, 32))
            return -1;

        return 0;
    }

    uint8_t _cshllink_writeEVistaAndAboveIDListDataBlock(cshllink **input, FILE *fp) {
        if((*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.info.BlockSignature!=_CSHLLINK_EDBLK_VistaAndAboveIDListDataBlockSig)
            return 0;
        
        if((*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.info.BlockSize<_CSHLLINK_EDBLK_VistaAndAboveIDListDataBlockSiz)
            _cshllink_errint(_CSHLLINK_ERRX_WRONGSIZE);

        //Size and Signature
        if(fwrite(&(*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.info.BlockSize, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
        if(fwrite(&(*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.info.BlockSignature, 4, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);

        if((*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_item==NULL) 
                _cshllink_errint(_CSHLLINK_ERR_NULLPIDL);
        for(int i=0; i<(*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_inum; i++) {
            //Element size
            if(fwrite(&(*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_item[i].item_size, 2, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
            //Element (size -2 as struct contains one 2byte var)
            if((*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_item[i].item==NULL)
                _cshllink_errint(_CSHLLINK_ERR_NULLPIDLM);
            if(fwrite((*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_item[i].item, (*input)->cshllink_extdatablk.VistaAndAboveIDListDataBlock.cshllink_lnktidl_idl.idl_item[i].item_size-2, 1, fp) != 1)
                _cshllink_errint(_CSHLLINK_ERR_FIO);
        }

        //TerminalBlock
        uint16_t nullb=0;
        if(fwrite(&nullb, 2, 1, fp)!=1)
            _cshllink_errint(_CSHLLINK_ERR_FIO);

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
        for(int i=0; i<inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_inum; i++) {
            if(inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_item[i].item!=NULL)
                free(inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_item[i].item);
        }
        free(inputStruct->cshllink_lnktidl.cshllink_lnktidl_idl.idl_item);
        free(inputStruct->cshllink_strdata.CommandLineArguments.UString);
        free(inputStruct->cshllink_strdata.IconLocation.UString);
        free(inputStruct->cshllink_strdata.NameString.UString);
        free(inputStruct->cshllink_strdata.RelativePath.UString);
        free(inputStruct->cshllink_strdata.WorkingDir.UString);
    }

    //IDList (also for VistaAndAboveIDList -- param idl pointer)
        /*
            set idl item
            IN: pointer to IDL item; data for item; length in bytes of data;
            OUT: 0 - Success
                 255 - Error
        */
        uint8_t cshllink_setIDListItem(struct _cshllink_lnktidl *list, uint8_t *data, uint16_t size, uint8_t index) {
            if(index>=list->cshllink_lnktidl_idl.idl_inum)
                _cshllink_errint(_CSHLLINK_ERR_NULLPIDL);
            //set new size
            if(list->cshllink_lnktidl_idl.idl_item[index].item_size>size)
                list->idl_size-= (list->cshllink_lnktidl_idl.idl_item[index].item_size - size);
            else if(list->cshllink_lnktidl_idl.idl_item[index].item_size<size)
                list->idl_size-= (list->cshllink_lnktidl_idl.idl_item[index].item_size - size);
            
            if(_cshllink_setIDListItem(&list->cshllink_lnktidl_idl.idl_item[index], data, size))
                return -1;

            return 0;
        }
        /*
            add idl item
            IN: pointer to List of IDL items; data for new item; length in bytes of data
            OUT: 0 - Success
                 255 - Error
        */
        uint8_t cshllink_addIDListItem(struct _cshllink_lnktidl *list, uint8_t *data, uint16_t size) {
            //set new size
            list->idl_size += size + 2;

            if(_cshllink_addIDListItem(&list->cshllink_lnktidl_idl, data, size))
                return -1;

            return 0;
        }
        /*
            remove idl item
            IN: pointer to List of IDL items; index to be removed
            OUT: 0 - Success
                 255 - Error
        */
        uint8_t cshllink_removeIDListItem(struct _cshllink_lnktidl *list, uint8_t index) {
            if(index>=list->cshllink_lnktidl_idl.idl_inum)
                _cshllink_errint(_CSHLLINK_ERR_NULLPIDL);
            
            if(index>=list->cshllink_lnktidl_idl.idl_inum)
                _cshllink_errint(_CSHLLINK_ERR_NULLPIDL);

            //set new size
            list->idl_size -= (list->cshllink_lnktidl_idl.idl_item[index].item_size+2);

            if(_cshllink_removeIDListItem(&list->cshllink_lnktidl_idl, index))
                return -1;

            return 0;
        }

            //IDList integ. (also for VistaAndAboveIDList -- param idl pointer)
            /*
                set idl item
                IN: pointer to IDL item; data for item; length in bytes of data;
                OUT: 0 - Success
                    255 - Error
            */
            uint8_t _cshllink_setIDListItem(struct _cshllink_lnktidl_idl_item *item, uint8_t *data, uint16_t size) {
                
                item->item_size = size;
                item->item=realloc(item->item, sizeof *item->item * size);
                if(item->item==NULL) 
                    _cshllink_errint(_CSHLLINK_ERR_NULLPIDLM);

                memcpy(item->item, data, size);

                return 0;

            }
            /*
                add idl item
                IN: pointer to List of IDL items; data for new item; length in bytes of data
                OUT: 0 - Success
                    255 - Error
            */
            uint8_t _cshllink_addIDListItem(struct _cshllink_lnktidl_idl *list, uint8_t *data, uint16_t size) {

                //realloc mem
                list->idl_item = realloc(list->idl_item, (size+2)* sizeof *list->idl_item);
                if(list->idl_item==NULL) 
                    _cshllink_errint(_CSHLLINK_ERR_NULLPIDL);

                if(_cshllink_setIDListItem(&list->idl_item[list->idl_inum], data, size))
                    return -1;

                list->idl_inum+=1;

                return 0;
            }
            /*
                remove idl item
                IN: pointer to List of IDL items; index to be removed
                OUT: 0 - Success
                    255 - Error
            */
            uint8_t _cshllink_removeIDListItem(struct _cshllink_lnktidl_idl *list, uint8_t index) {
                if(index>=list->idl_inum)
                    _cshllink_errint(_CSHLLINK_ERR_NULLPIDL);

                int size = list->idl_item[index].item_size+2;
                free(list->idl_item[index].item);
                
                for(int i=index+1; i<list->idl_inum; i++) {
                    list->idl_item[i-1] = list->idl_item[i];
                }

                //realloc mem
                list->idl_item = realloc(list->idl_item, (size)* sizeof *list->idl_item);
                if(list->idl_item==NULL) 
                    _cshllink_errint(_CSHLLINK_ERR_NULLPIDL);
                
                list->idl_inum-=1;

                return 0;
            }

    //VOLID and LocalBasePath
        /*
            enable VolumeID and LocalBasePath
        */
        uint8_t cshllink_enableVolumeIDAndLocalBasePath(cshllink *inputStruct) {
            
            
            if(inputStruct->cshllink_lnkinfo.LinkInfoFlags&CSHLLINK_LIF_VolumeIDAndLocalBasePath)
                return 0;
            
            inputStruct->cshllink_lnkinfo.LinkInfoFlags|=CSHLLINK_LIF_VolumeIDAndLocalBasePath;
            

            //VolumeIDSize
            inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeIDSize = 17;

            //DriveType (unknown)
            inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.DriveType = 0;
            
            //DriveSerialNumber (unknown)
            inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.DriveSerialNumber = 0;

            //VolumeLabelOffset
            inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeLabelOffset = 0x00000016;

            //DATA
            inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.Data = realloc(inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.Data, 1);
            if(inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.Data==NULL)
                _cshllink_errint(_CSHLLINK_ERR_NULLPVIDD);
            //Data
            *inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.Data = 0;

            inputStruct->cshllink_lnkinfo.LocalBasePathOffset = inputStruct->cshllink_lnkinfo.LinkInfoHeaderSize + inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeIDSize;

            //LocalBasePath
            inputStruct->cshllink_lnkinfo.LocalBasePath = realloc(inputStruct->cshllink_lnkinfo.LocalBasePath, 1);
            if(inputStruct->cshllink_lnkinfo.LocalBasePath==NULL)
                _cshllink_errint(_CSHLLINK_ERR_NULLPVIDD);
            //LocalBasePath
            *inputStruct->cshllink_lnkinfo.LocalBasePath = 0;
            
            //set size of link info
            inputStruct->cshllink_lnkinfo.LinkInfoHeaderSize = 28;
            inputStruct->cshllink_lnkinfo.LinkInfoSize = 28 + inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeIDSize + 1;

            return 0;
        }
        /*
            disable VolumeID and LocalBasePath
        */
        uint8_t cshllink_disableVolumeIDAndLocalBasePath(cshllink *inputStruct) {
            if(!(inputStruct->cshllink_lnkinfo.LinkInfoFlags&CSHLLINK_LIF_VolumeIDAndLocalBasePath))
                return 0;
            
            inputStruct->cshllink_lnkinfo.LinkInfoFlags&=~CSHLLINK_LIF_VolumeIDAndLocalBasePath;

            free(inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.Data);

            inputStruct->cshllink_lnkinfo.LocalBasePathOffset = 0;
            inputStruct->cshllink_lnkinfo.LinkInfoSize -= inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeIDSize + strlen(inputStruct->cshllink_lnkinfo.LocalBasePath) + 1;

            if(inputStruct->cshllink_lnkinfo.LinkInfoHeaderSize>=0x00000024) {
                inputStruct->cshllink_lnkinfo.LinkInfoHeaderSize -= 4;
                inputStruct->cshllink_lnkinfo.LinkInfoSize -= 4 + cshllink_strlen16(inputStruct->cshllink_lnkinfo.LocalBasePathUnicode) + 1; 
            }

            return 0;
        }
        /*
            set VolumeIDData
        */  
        uint8_t cshllink_setVolumeIDDataAnsi(cshllink *inputStruct, char *data, uint32_t size) {

            inputStruct->cshllink_lnkinfo.LinkInfoSize += -inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeIDSize + 16 + size;
            inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeIDSize= 16 + size;

            if(inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeLabelOffsetUnicode!=0) {
                inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeLabelOffsetUnicode = 0;
            }
            inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeLabelOffset = 0x00000010;

            inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.Data = realloc(inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.Data, sizeof *inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.Data * size);
            if(inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.Data==NULL)
                _cshllink_errint(_CSHLLINK_ERR_NULLPVIDD);
            
            memcpy(inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.Data, data, size);

            return 0;
        }
        uint8_t cshllink_setVolumeIDDataUnicode(cshllink *inputStruct, char16_t *data, uint32_t size) {

            inputStruct->cshllink_lnkinfo.LinkInfoSize += -inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeIDSize + 20 + size;
            inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeIDSize= 20 + size;

            if(inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeLabelOffset!=0x00000014) {
                inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeLabelOffset = 0x00000014;
            }
            inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.VolumeLabelOffsetUnicode = 0x00000014;

            inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.Data = realloc(inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.Data, sizeof *inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.Data * size);
            if(inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.Data==NULL)
                _cshllink_errint(_CSHLLINK_ERR_NULLPVIDD);

            memcpy(inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_volid.Data, data, size);

            return 0;
        }
        /*
            set LocalBasePath
        */
        uint8_t cshllink_setLocalBasePath(cshllink *inputStruct, char *data) {

            if(!inputStruct->cshllink_lnkinfo.LinkInfoFlags&CSHLLINK_LIF_VolumeIDAndLocalBasePath) return -1;

            if(inputStruct->cshllink_lnkinfo.LinkInfoHeaderSize>=0x00000024) {
                inputStruct->cshllink_lnkinfo.LinkInfoHeaderSize -= 4;
                inputStruct->cshllink_lnkinfo.LinkInfoSize -= 4 + cshllink_strlen16(inputStruct->cshllink_lnkinfo.LocalBasePathUnicode) + 1;
            }

            memcpy(inputStruct->cshllink_lnkinfo.LocalBasePath, data, strlen(data)+1);

            return 0;
        }
        /*
            set LocalBasePathUnicode
        */
        uint8_t cshllink_setLocalBasePathUnicode(cshllink *inputStruct, char16_t *data) {
            if(!inputStruct->cshllink_lnkinfo.LinkInfoFlags&CSHLLINK_LIF_VolumeIDAndLocalBasePath) return -1;

            if(inputStruct->cshllink_lnkinfo.LinkInfoHeaderSize<0x00000024) {
                inputStruct->cshllink_lnkinfo.LinkInfoHeaderSize += 4;
                inputStruct->cshllink_lnkinfo.LinkInfoSize += 4 - strlen(inputStruct->cshllink_lnkinfo.LocalBasePath) + 1;
            }

            memcpy(inputStruct->cshllink_lnkinfo.LocalBasePath, data, cshllink_strlen16(data)+1);

            return 0;
        }

    //CommonNetworkRelativeLink
        /*
            set NetName
        */
        uint8_t cshllink_setNetName(cshllink *inputStruct, char *data) {
            return 0;
        } 
        /*
            set DeviceName
        */ 
        uint8_t cshllink_setDeviceName(cshllink *inputStruct, char *data) {
            return 0;
        } 
        /*
            set NetNameUnicode
        */ 
        uint8_t cshllink_setNetNameUnicode(cshllink *inputStruct, char16_t *data) {
            return 0;
        } 
        /*
            set DeviceNameUnicode
        */
        uint8_t cshllink_setDeviceNameUnicode(cshllink *inputStruct, char16_t *data) {
            return 0;
        } 


    //LNKINFO
        /*
            set CommonPathSuffix
        */
        uint8_t cshllink_setCommonPathSuffix(cshllink *inputStruct, char *data) {
            return 0;
        }
        /*
            set CommonPathSuffixUnicode
        */
        uint8_t cshllink_setCommonPathSuffixUnicode(cshllink *inputStruct, char16_t *data) {
            return 0;
        }


    //StringData
        /*
            set NameString
        */
        uint8_t cshllink_setNameString(cshllink *inputStruct, char16_t *data, uint16_t len) {
            inputStruct->cshllink_strdata.NameString.CountCharacters=len;
            if(len==0) {
                free(inputStruct->cshllink_strdata.NameString.UString);
                return 0;
            }
            inputStruct->cshllink_strdata.NameString.UString = realloc(inputStruct->cshllink_strdata.NameString.UString, sizeof *inputStruct->cshllink_strdata.NameString.UString * len);
            if(inputStruct->cshllink_strdata.NameString.UString==NULL)
                _cshllink_errint(_CSHLLINK_ERR_NULLPSTRDNAME);
            memcpy(inputStruct->cshllink_strdata.NameString.UString, data, len*sizeof(char16_t));

            return 0;
        }
        /*
            set RelativePath
        */
        uint8_t cshllink_setRelativePath(cshllink *inputStruct, char16_t *data, uint16_t len) {
            inputStruct->cshllink_strdata.RelativePath.CountCharacters=len;
            if(len==0) {
                free(inputStruct->cshllink_strdata.RelativePath.UString);
                return 0;
            }
            inputStruct->cshllink_strdata.RelativePath.UString = realloc(inputStruct->cshllink_strdata.RelativePath.UString, sizeof *inputStruct->cshllink_strdata.RelativePath.UString * len);
            if(inputStruct->cshllink_strdata.RelativePath.UString==NULL)
                _cshllink_errint(_CSHLLINK_ERR_NULLPSTRDNAME);
            memcpy(inputStruct->cshllink_strdata.RelativePath.UString, data, len*sizeof(char16_t));

            return 0;
        }
        /*
            set WorkingDir
        */
        uint8_t cshllink_setWorkingDir(cshllink *inputStruct, char16_t *data, uint16_t len) {
            inputStruct->cshllink_strdata.WorkingDir.CountCharacters=len;
            if(len==0) {
                free(inputStruct->cshllink_strdata.WorkingDir.UString);
                return 0;
            }
            inputStruct->cshllink_strdata.WorkingDir.UString = realloc(inputStruct->cshllink_strdata.WorkingDir.UString, sizeof *inputStruct->cshllink_strdata.WorkingDir.UString * len);
            if(inputStruct->cshllink_strdata.WorkingDir.UString==NULL)
                _cshllink_errint(_CSHLLINK_ERR_NULLPSTRDNAME);
            memcpy(inputStruct->cshllink_strdata.WorkingDir.UString, data, len*sizeof(char16_t));

            return 0;
        }
        /*
            set CommandLineArguments
        */
        uint8_t cshllink_setCommandLineArguments(cshllink *inputStruct, char16_t *data, uint16_t len) {
            inputStruct->cshllink_strdata.CommandLineArguments.CountCharacters=len;
            if(len==0) {
                free(inputStruct->cshllink_strdata.CommandLineArguments.UString);
                return 0;
            }
            inputStruct->cshllink_strdata.CommandLineArguments.UString = realloc(inputStruct->cshllink_strdata.CommandLineArguments.UString, sizeof *inputStruct->cshllink_strdata.CommandLineArguments.UString * len);
            if(inputStruct->cshllink_strdata.CommandLineArguments.UString==NULL)
                _cshllink_errint(_CSHLLINK_ERR_NULLPSTRDNAME);
            memcpy(inputStruct->cshllink_strdata.CommandLineArguments.UString, data, len*sizeof(char16_t));

            return 0;
        }
        /*
            set IconLocation
        */
        uint8_t cshllink_setIconLocation(cshllink *inputStruct, char16_t *data, uint16_t len) {
            

            inputStruct->cshllink_strdata.IconLocation.CountCharacters=len;
            if(len==0) {
                free(inputStruct->cshllink_strdata.IconLocation.UString);
                return 0;
            }
            inputStruct->cshllink_strdata.IconLocation.UString = realloc(inputStruct->cshllink_strdata.IconLocation.UString, sizeof *inputStruct->cshllink_strdata.IconLocation.UString * len);
            if(inputStruct->cshllink_strdata.IconLocation.UString==NULL)
                _cshllink_errint(_CSHLLINK_ERR_NULLPSTRDNAME);
            memcpy(inputStruct->cshllink_strdata.IconLocation.UString, data, len*sizeof(char16_t));

            return 0;
        }


    //EXTRA DATA
        /*
            disable EXTDB
        */
        uint8_t cshllink_disableConsoleDB(cshllink *inputStruct) {
            return 0;
        }
        uint8_t cshllink_disableConsoleFEDB(cshllink *inputStruct) {
            return 0;
        }
        uint8_t cshllink_disableDarwinDB(cshllink *inputStruct) {
            return 0;
        }
        uint8_t cshllink_disableEnvironmentVariableDB(cshllink *inputStruct) {
            return 0;
        }
        uint8_t cshllink_disableIconEnvironmentDB(cshllink *inputStruct) {
            return 0;
        }
        uint8_t cshllink_disableKnownFolderDB(cshllink *inputStruct) {
            return 0;
        }
        uint8_t cshllink_disablePropertyStoreDB(cshllink *inputStruct) {
            return 0;
        }
        uint8_t cshllink_disableShimDB(cshllink *inputStruct) {
            return 0;
        }
        uint8_t cshllink_disableTrackerDB(cshllink *inputStruct) {
            return 0;
        }
        uint8_t cshllink_disableVistaAndAboveIDListDB(cshllink *inputStruct) {
            return 0;
        }
        /*
            enable EXTDB
        */
        uint8_t cshllink_enableConsoleDB(cshllink *inputStruct) {
            return 0;
        }
        uint8_t cshllink_enableConsoleFEDB(cshllink *inputStruct) {
            return 0;
        }
        uint8_t cshllink_enableDarwinDB(cshllink *inputStruct) {
            return 0;
        }
        uint8_t cshllink_enableEnvironmentVariableDB(cshllink *inputStruct) {
            return 0;
        }
        uint8_t cshllink_enableIconEnvironmentDB(cshllink *inputStruct) {
            return 0;
        }
        uint8_t cshllink_enableKnownFolderDB(cshllink *inputStruct) {
            return 0;
        }
        uint8_t cshllink_enablePropertyStoreDB(cshllink *inputStruct) {
            return 0;
        }
        uint8_t cshllink_enableShimDB(cshllink *inputStruct) {
            return 0;
        }
        uint8_t cshllink_enableTrackerDB(cshllink *inputStruct) {
            return 0;
        }
        uint8_t cshllink_enableVistaAndAboveIDListDB(cshllink *inputStruct) {
            return 0;
        }
        
        //ConsoleDB
        /*
            set FontFaceName (32 char)
        */
        uint8_t cshllink_setFontFaceName(cshllink *inputStruct, char16_t *faceName) {
            return 0;
        }

        //DarwinDB
        /*
            set DarwinDataAnsi (260 byte)
        */
        uint8_t cshllink_setDarwinDataAnsi(cshllink *inputStruct, char *data) {
            
            size_t len = strlen(data);
            inputStruct->cshllink_extdatablk.DarwinDataBlock.DarwinDataAnsi = realloc(inputStruct->cshllink_extdatablk.DarwinDataBlock.DarwinDataAnsi, 260);
            if(inputStruct->cshllink_extdatablk.DarwinDataBlock.DarwinDataAnsi==NULL)
                _cshllink_errint(_CSHLLINK_ERRX_NULLPSTRDARDA);
            memcpy(inputStruct->cshllink_extdatablk.DarwinDataBlock.DarwinDataAnsi, data, len);
            len--;
            while(len++<259) inputStruct->cshllink_extdatablk.DarwinDataBlock.DarwinDataAnsi[len]=0;

            return 0;
        }
        /*
            set DarwinDataUnicode (520 byte)
        */
        uint8_t cshllink_setDarwinDataUnicode(cshllink *inputStruct, char16_t *data) {
            
            size_t len = cshllink_strlen16(data);
            inputStruct->cshllink_extdatablk.DarwinDataBlock.DarwinDataUnicode = realloc(inputStruct->cshllink_extdatablk.DarwinDataBlock.DarwinDataUnicode, 520);
            if(inputStruct->cshllink_extdatablk.DarwinDataBlock.DarwinDataUnicode==NULL)
                _cshllink_errint(_CSHLLINK_ERRX_NULLPSTRIENVDU);
            memcpy(inputStruct->cshllink_extdatablk.DarwinDataBlock.DarwinDataUnicode, data, len*2);
            len--;
            while(len++<259) inputStruct->cshllink_extdatablk.DarwinDataBlock.DarwinDataUnicode[len]=0;

            return 0;
        }

        //EnvironmentVariableDB
        /*
            set TargetAnsi (260 byte)
        */
        uint8_t cshllink_setEnvironmentVariableTargetAnsi(cshllink *inputStruct, char *data) {
            
            size_t len = strlen(data);
            inputStruct->cshllink_extdatablk.EnvironmentVariableDataBlock.TargetAnsi = realloc(inputStruct->cshllink_extdatablk.EnvironmentVariableDataBlock.TargetAnsi, 260);
            if(inputStruct->cshllink_extdatablk.EnvironmentVariableDataBlock.TargetAnsi==NULL)
                _cshllink_errint(_CSHLLINK_ERRX_NULLPSTRENVDA);
            memcpy(inputStruct->cshllink_extdatablk.EnvironmentVariableDataBlock.TargetAnsi, data, len);
            len--;
            while(len++<259) inputStruct->cshllink_extdatablk.EnvironmentVariableDataBlock.TargetAnsi[len]=0;

            return 0;
        }
        /*
            set TargetUnicode (520 byte)
        */
        uint8_t cshllink_setEnvironmentVariableTargetUnicode(cshllink *inputStruct, char16_t *data) {
            
            size_t len = cshllink_strlen16(data);
            inputStruct->cshllink_extdatablk.EnvironmentVariableDataBlock.TargetUnicode = realloc(inputStruct->cshllink_extdatablk.EnvironmentVariableDataBlock.TargetUnicode, 520);
            if(inputStruct->cshllink_extdatablk.EnvironmentVariableDataBlock.TargetUnicode==NULL)
                _cshllink_errint(_CSHLLINK_ERRX_NULLPSTRIENVDU);
            memcpy(inputStruct->cshllink_extdatablk.EnvironmentVariableDataBlock.TargetUnicode, data, len*2);
            len--;
            while(len++<259) inputStruct->cshllink_extdatablk.EnvironmentVariableDataBlock.TargetUnicode[len]=0;

            return 0;
        }

        //IconEnvironmentDB
        /*
            set TargetAnsi (260 byte)
        */
        uint8_t cshllink_setIconEnvironmentTargetAnsi(cshllink *inputStruct, char *data) {

            size_t len = strlen(data);
            inputStruct->cshllink_extdatablk.IconEnvironmentDataBlock.TargetAnsi = realloc(inputStruct->cshllink_extdatablk.IconEnvironmentDataBlock.TargetAnsi, 260);
            if(inputStruct->cshllink_extdatablk.IconEnvironmentDataBlock.TargetAnsi==NULL)
                _cshllink_errint(_CSHLLINK_ERRX_NULLPSTRIENVDA);
            memcpy(inputStruct->cshllink_extdatablk.IconEnvironmentDataBlock.TargetAnsi, data, len);
            len--;
            while(len++<259) inputStruct->cshllink_extdatablk.IconEnvironmentDataBlock.TargetAnsi[len]=0;

            return 0;
        }
        /*
            set TargetUnicode (520 byte)
        */
        uint8_t cshllink_setIconEnvironmentTargetUnicode(cshllink *inputStruct, char16_t *data) {

            size_t len = cshllink_strlen16(data);
            inputStruct->cshllink_extdatablk.IconEnvironmentDataBlock.TargetUnicode = realloc(inputStruct->cshllink_extdatablk.IconEnvironmentDataBlock.TargetUnicode, 520);
            if(inputStruct->cshllink_extdatablk.IconEnvironmentDataBlock.TargetUnicode==NULL)
                _cshllink_errint(_CSHLLINK_ERRX_NULLPSTRIENVDU);
            memcpy(inputStruct->cshllink_extdatablk.IconEnvironmentDataBlock.TargetUnicode, data, len*2);
            len--;
            while(len++<259) {
                inputStruct->cshllink_extdatablk.IconEnvironmentDataBlock.TargetUnicode[len]=0;
            }
            return 0;
        }

        //KnownFolderDB
        /*
            set KnownFolderID (16 byte)
        */
        uint8_t cshllink_setKnownFolderID(cshllink *inputStruct, uint8_t *knownFolderID) {
            return 0;
        }

        //PropertyStoreDB
        /*
            set PropteryStore (min 4 bytes) - "size" in bytes
        */
        uint8_t cshllink_setPropertyStore(cshllink *inputStruct, uint8_t *propertyStore, uint32_t size) {
            return 0;
        }

        //ShimDB
        /*
            set LayerName (min 128 bytes) - "size" in bytes
        */
        uint8_t cshllink_setShimLayerName(cshllink *inputStruct, char16_t *layerName, uint32_t size) {
            return 0;
        }

        //TrackerDB
        /*
            set MachineID (16 bytes)
        */
        uint8_t cshllink_setTrackerMachineID(cshllink *inputStruct, char *machineID) {
            return 0;
        }
        /*
            set Droid (32 byte)
        */
        uint8_t cshllink_setTrackerDroi(cshllink *inputStruct, uint8_t *droid) {
            return 0;
        }
        /*
            set DroidBirth (32 byte)
        */
        uint8_t cshllink_setTrackerDroidBirth(cshllink *inputStruct, uint8_t *droidBirth) {
            return 0;
        }
    
        //VistaAndAboveIDListDB
        /*
            set idl item
            IN: pointer to IDL item; data for item; length in bytes of data
            OUT: 0 - Success
                 255 - Error
        */
        uint8_t cshllink_setVistaAndAboveIDListItem(struct _cshllink_extdatablk_viidldblk *viaail, uint8_t *data, uint16_t size, uint8_t index) {
            return 0;
        }
        /*
            add idl item
            IN: pointer to List of IDL items; data for new item; length in bytes of data
            OUT: 0 - Success
                 255 - Error
        */
        uint8_t cshllink_addVistaAndAboveIDListItem(struct _cshllink_extdatablk_viidldblk *viaail, uint8_t *data, uint16_t size) {
            return 0;
        }
        /*
            remove idl item
            IN: pointer to List of IDL items; index to be removed
            OUT: 0 - Success
                 255 - Error
        */
        uint8_t cshllink_removeVistaAndAboveIDListItem(struct _cshllink_extdatablk_viidldblk *viaail, uint8_t index) {
            return 0;
        }

    /*
        converts ansi to unicode
    */
    uint8_t cshllink_ansiToUni(char16_t *dest, char *src) {
        
        int i=0;
        while(src[i]!=0) {
            dest[i]=src[i];
            i++;
        }
        dest[i]=0;
        return i;

    }

    /*
        strlen char16
    */
    size_t cshllink_strlen16(char16_t *src) {
        size_t size=0;

        while(*src!=0) {
            src++;
            size++;
        }

        return size;
    }

    #pragma endregion