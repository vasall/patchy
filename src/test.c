#include "patchy.h"
#include "patchy_internal.h"

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
        struct pa_table table;
        void *ptr = NULL;

        char write_buf[32];

#define BUF_SIZE        512
        unsigned char buf[BUF_SIZE];

        printf("Init...\n");
        paInit(&document);
        paInitTableFixed(&table, buf, BUF_SIZE, sizeof(int), 32);
        printf("done!\n");

        

        printf("Shutdown...\n");
        paDestroyTable(&table);
        paQuit(&document);
        printf("done!\n");

        return 0;
}

