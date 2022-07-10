#include <stdio.h>
#include <stdlib.h>
#include "cshllink.h"
#include <string.h>

struct Test {
    int val;
    char str[10];
    char16_t *wstr;
};



int main(void) {

    struct Test *t = malloc(2*sizeof(struct Test));

    (t+1)->val=2;
    memcpy((t+1)->str, "Elem1", 6);
    t->val=41;
    memcpy(t->str, "Elem2\0", 6);

    printf("%s: %d\n%s: %d\n", t->str, t->val, (t+1)->str, (t+1)->val);

    cshllink LNK1 = {0}, LNK2 = {0};
    FILE *fp1 = fopen("../exmpl/test1.lnk", "rb+");
    FILE *fp2 = fopen("../exmpl/test2.lnk", "rb+");

    if(cshllink_loadFile(fp1, &LNK1)==255) {
        printf("ERR READ 1 0x%x\n", cshllink_error);
        return 0;
    }
    if(cshllink_loadFile(fp2, &LNK2)==255) {
        printf("ERR READ 2 0x%x\n", cshllink_error);
        return 0;
    }

    char16_t *tmpIconLocation = malloc(LNK1.cshllink_strdata.IconLocation.CountCharacters);
    uint16_t len = LNK1.cshllink_strdata.IconLocation.CountCharacters;
    memcpy(tmpIconLocation, LNK1.cshllink_strdata.IconLocation.UString, LNK1.cshllink_strdata.IconLocation.CountCharacters*2);

    cshllink_setIconLocation(&LNK1, LNK2.cshllink_strdata.IconLocation.UString, LNK2.cshllink_strdata.IconLocation.CountCharacters);
    cshllink_setIconLocation(&LNK2, tmpIconLocation, len);

    char *tmpIconEnvironmentTargetAnsi = malloc(260);
    char16_t *tmpIconEnvironmentTargetUnicode = malloc(520);
    if(tmpIconEnvironmentTargetAnsi==NULL || tmpIconEnvironmentTargetUnicode==NULL) {
        printf("NULLP\n");
        return 0;
    }
    int sig = LNK1.cshllink_extdatablk.IconEnvironmentDataBlock.info.BlockSignature;
    if(sig!=0) {
        memcpy(tmpIconEnvironmentTargetAnsi, LNK1.cshllink_extdatablk.IconEnvironmentDataBlock.TargetAnsi, strlen(LNK1.cshllink_extdatablk.IconEnvironmentDataBlock.TargetAnsi));
        memcpy(tmpIconEnvironmentTargetUnicode, LNK1.cshllink_extdatablk.IconEnvironmentDataBlock.TargetUnicode, cshllink_strlen16(LNK1.cshllink_extdatablk.IconEnvironmentDataBlock.TargetUnicode)*2);
    }
    else {
        memset(tmpIconEnvironmentTargetAnsi, 0, 260);
        memset(tmpIconEnvironmentTargetUnicode, 0, 520);   
    }

    if(LNK2.cshllink_extdatablk.IconEnvironmentDataBlock.info.BlockSignature==0)
        cshllink_disableIconEnvironmentDB(&LNK1);
    else {
        cshllink_enableIconEnvironmentDB(&LNK1);
        cshllink_setIconEnvironmentTargetAnsi(&LNK1, LNK2.cshllink_extdatablk.IconEnvironmentDataBlock.TargetAnsi);
        cshllink_setIconEnvironmentTargetUnicode(&LNK1, LNK2.cshllink_extdatablk.IconEnvironmentDataBlock.TargetUnicode);
    }
    if(sig==0)
        cshllink_disableIconEnvironmentDB(&LNK2);
    else {
        cshllink_enableIconEnvironmentDB(&LNK2);
        cshllink_setIconEnvironmentTargetAnsi(&LNK2, tmpIconEnvironmentTargetAnsi);
        cshllink_setIconEnvironmentTargetUnicode(&LNK2, tmpIconEnvironmentTargetUnicode);
    }

    //Name String
    

    if(cshllink_writeFile(fp1, &LNK1)==255) {
        printf("ERR WRITE 1 0x%x\n", cshllink_error);
        return 0;
    }
    if(cshllink_writeFile(fp2, &LNK2)==255) {
        printf("ERR WRITE 2 0x%x\n", cshllink_error);
        return 0;
    }

    fclose(fp1);
    fclose(fp2);

    cshllink_free(&LNK1);
    cshllink_free(&LNK2);

    return 0;
}