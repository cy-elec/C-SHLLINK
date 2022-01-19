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
                if(cshllink_rNULLstr(inputStruct->cshllink_lnkinfo.LocalBasePath, _CSHLLINK_ERR_NULLPLBP, _CSHLLINK_ERR_FIO, fp))
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
                if(cshllink_rNULLstr(inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_cnetrlnk.NetName, _CSHLLINK_ERR_NULLPNETN, _CSHLLINK_ERR_FIO, fp))
                        return -1;
                
                //DeviceName
                if(cshllink_rNULLstr(inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_cnetrlnk.DeviceName, _CSHLLINK_ERR_NULLPDEVN, _CSHLLINK_ERR_FIO, fp))
                        return -1;

                if(inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_cnetrlnk.NetNameOffset>0x00000014) {
                    //NetNameUnicode
                    if(cshllink_rNULLstr(inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_cnetrlnk.NetNameUnicode, _CSHLLINK_ERR_NULLPNNU, _CSHLLINK_ERR_FIO, fp))
                            return -1;

                    //DeviceNameUnicode
                    if(cshllink_rNULLstr(inputStruct->cshllink_lnkinfo.cshllink_lnkinfo_cnetrlnk.DeviceNameUnicode, _CSHLLINK_ERR_NULLPDNU, _CSHLLINK_ERR_FIO, fp))
                            return -1;
                }
            }

            //CommonPathSuffix
            if(cshllink_rNULLstr(inputStruct->cshllink_lnkinfo.CommonPathSuffix, _CSHLLINK_ERR_NULLPCPS, _CSHLLINK_ERR_FIO, fp))
                    return -1;

            if(inputStruct->cshllink_lnkinfo.LinkInfoHeaderSize>=0x00000024) {
                //LocalBasePathUnicode
                if(inputStruct->cshllink_lnkinfo.LinkInfoFlags&CSHLLINK_LIF_VolumeIDAndLocalBasePath) {
                    if(cshllink_rNULLstr(inputStruct->cshllink_lnkinfo.LocalBasePathUnicode, _CSHLLINK_ERR_NULLPLBPU, _CSHLLINK_ERR_FIO, fp))
                        return -1;
                }
                //CommonPathSuffixUnicode
                if(cshllink_rNULLstr(inputStruct->cshllink_lnkinfo.CommonPathSuffixUnicode, _CSHLLINK_ERR_NULLPCPSU, _CSHLLINK_ERR_FIO, fp))
                    return -1;
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
    uint8_t cshllink_rNULLstr(char *dest, uint8_t errv1, uint8_t errv2, FILE *fp) {
        char tmpC=1;
        uint32_t tmpS=0;
        while(tmpC!=0) {
            dest = realloc(dest, tmpS+1);
            if(dest==NULL)
                _cshllink_errint(errv1);
            if(fread(&dest[tmpS], 1, 1, fp) != 1)
                _cshllink_errint(errv2);
            tmpC = dest[tmpS];
            tmpS++;
        }
        return 0;
    }