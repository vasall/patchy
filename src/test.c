#include "patchy.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char **argv)
{
        struct pa_document document;
        struct pa_string str;

        #define ALLOC_SIZE 32
        void *p = malloc(ALLOC_SIZE);

        #define BUFFER_SIZE 7
        char buffer[BUFFER_SIZE];
        int tmp;

        char *insert = "一二三四五六七八九";

        paInit(&document);

        paInitStringFixed(&str, p, ALLOC_SIZE);

        if(argc > 1) {
                printf("Input size: %d\n", strlen(argv[1]));
                tmp = paWriteString(&str, argv[1], 0, PA_ALL);
                printf("String InUse: %d\n", str.size);
                printf("StringContent(%d): %s\n", tmp, str.buffer);
        }
               
        tmp = paWriteString(&str, insert, 4, PA_ALL);
        printf("StringContent(%d): %s\n", tmp, str.buffer);

        tmp = paReadString(&str, buffer, 0, 12, BUFFER_SIZE - 1);
        printf("Output buffer(%d): %s\n", tmp, buffer);

        free(p);
        paQuit(&document);
        return 0;
}
