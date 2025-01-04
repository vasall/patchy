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
        struct pa_flex_helper helper;
        struct pa_flex flex;
        struct pa_flex_token token;
        void *ptr = NULL;

#define BUF_SIZE        512
        unsigned char buf[BUF_SIZE];

#define SWP_SIZE        512
        unsigned char swp[SWP_SIZE];

#define VAL_SIZE        512
        unsigned char val[VAL_SIZE];

#define FLX_SIZE        512
        unsigned char flex_buf[FLX_SIZE];

        struct pa_flex_reference ref = {200, 12};
        s32 value;

        char *expression = "((2 + (3em + 25pct)) * 4)/3.2";

        printf("Init...\n");
        paInit(&document);
        paInitFlexHelperFixed(&helper, buf, BUF_SIZE, swp, SWP_SIZE, val, VAL_SIZE);
        paInitFlexFixed(&flex, &helper, flex_buf, FLX_SIZE);
        printf("done!\n");

        printf("Flex-Alloc: %d\n", flex.tokens.alloc);

        paParseFlex(&flex, expression);
       
        while((ptr = paIterateList(&flex.tokens, ptr))) {
                token = *(struct pa_flex_token *)ptr;

                printf("code: %02x, value: %f\n", token.code, token.value);
        }

        value = paProcessFlex(&flex, &ref);
        printf("Resulting value: %d\n", value);

        printf("Shutdown...\n");
        paDestroyFlex(&flex);
        paDestroyFlexHelper(&helper);
        paQuit(&document);
        printf("done!\n");

        return 0;
}

