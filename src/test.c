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
        struct pa_flex flex;
        struct pa_flex_token token;

#define BUF_SIZE        128
        unsigned char buf[BUF_SIZE];

#define SWP_SIZE        128
        unsigned char swp[SWP_SIZE];

#define STK_SIZE        128
        unsigned char stk[STK_SIZE];

        printf("Init...\n");
        paInit(&document);
        paInitFlexFixed(&flex, buf, BUF_SIZE, swp, SWP_SIZE, stk, STK_SIZE);
        printf("done!\n");

        printf("Flex-Alloc: %d\n", flex.tokens.alloc);


        printf("Shutdown...\n");
        paDestroyFlex(&flex);
        paQuit(&document);
        printf("done!\n");

        return 0;
}
