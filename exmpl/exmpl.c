#include <stdio.h>
#include <stdlib.h>
#include <cshllink.h>
#include <string.h>

struct Test {
    int val;
    char str[10];
};

int main(void) {

    struct Test *t = malloc(2*sizeof(struct Test));

    (t+sizeof(struct Test))->val=2;
    strcpy((t+sizeof(struct Test))->str, "Elem1");
    t->val=41;
    strcpy(t->str, "Elem2");

    printf("%s: %d\n%s: %d\n", t->str, t->val, (t+sizeof(struct Test))->str, (t+sizeof(struct Test))->val);

    cshllink LNK = {0};
    LNK.cshllink_header.HotKey=0xff00;
    FILE *fp = fopen("../exmpl/test.lnk", "rb");

    int rval=cshllink_loadFile(fp, &LNK);
    printf("%d - 0x%x: 0x%I64x%I64x\n", rval, cshllink_error, LNK.cshllink_header.LinkCLSID_H, LNK.cshllink_header.LinkCLSID_L);

    return 0;
}