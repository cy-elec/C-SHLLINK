#include <stdio.h>
#include <stdlib.h>
#include <cshllink.h>
#include <string.h>

struct Test {
    int val;
    char str[10];
    char16_t *wstr;
};



int main(void) {

    struct Test *t = malloc(2*sizeof(struct Test));

    (t+1)->val=2;
    strcpy((t+1)->str, "Elem1");
    t->val=41;
    strcpy(t->str, "Elem2");

    printf("%s: %d\n%s: %d\n", t->str, t->val, (t+1)->str, (t+1)->val);
    fflush(stdout);

    cshllink LNK = {0};
    FILE *fp = fopen("../exmpl/test2.lnk", "rb");
    FILE *fw = fopen("../exmpl/test2_custom.lnk", "wb+");

    int rval=cshllink_loadFile(fp, &LNK);
    printf("rval: %d - ERRCODE: 0x%x\n", rval, cshllink_error);
    if(rval) return 1;

    rval=cshllink_writeFile(fw, &LNK);
    printf("Writing... rval: %d - ERRCODE: 0x%x\n", rval, cshllink_error);
    if(rval) return 1;
    
    fclose(fp);
    fclose(fw);

    cshllink_free(&LNK);

    return 0;
}