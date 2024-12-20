#include "patchy.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(void)
{
        struct pa_document document;

        printf("Init...\n");
        paInit(&document);
        printf("Done!\n");


        printf("Shutdown...\n");
        paQuit(&document);
        printf("Done!\n");

        return 0;
}
