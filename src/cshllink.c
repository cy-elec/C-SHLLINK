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