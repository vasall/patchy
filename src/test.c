#include "patchy.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct test {
        int a;
        int b;
};

int main(void)
{
        struct pa_document document;

        printf("Init...\n");
        paInit(&document);
        printf("done!\n");


        printf("Shutdown...\n");
        paQuit(&document);
        printf("done!\n");

        return 0;
}
