#include "patchy.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static s8 print_entry(struct pa_handle *hdl, void *pass)
{
        s32 num = *(s32 *)hdl->pointer;
        s32 index = hdl->index;

        printf("%d: %d\n", index, num);
        return 0;
}

int main(int argc, char **argv)
{
        struct pa_document document;

        paInit(&document);



        paQuit(&document);
        return 0;
}
