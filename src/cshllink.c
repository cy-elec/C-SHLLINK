/*
    This library provides an interface for the Shell Link Binary File Format in C [MS-SHLLINK](https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-shllink/16cb4ca1-9339-4d0c-a68d-bf1d6cc0f943)



    Author: Felix Kröhnert
    License: GNU GPL V3 (refer to LICENSE for more information)
*/

#include "cshllink.h"
#include <stdio.h>

/*
        SHLLINK Header
*/
struct cshllink_header {
    int dummy;
};
/*
    SHLLINK LinkTargetIDList
*/
struct cshllink_lnktidl {
    int dummy;
};
/*
    SHLLINK LinkInfo
*/
struct cshllink_lnkinfo {
    int dummy;
};
