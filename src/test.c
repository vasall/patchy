#include "patchy.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void clear(int *buf, int size)
{
        int i;

        for(i = 0; i < size; i++)
                buf[i] = 0;
}

static s8 print_entry(void *ent, s16 idx, void *pass)
{
        int val = *(int *)ent;

        PA_IGNORE(pass);

        printf("%02d: %d\n", idx, val);
        return 0;
}


int main(void)
{
        struct pa_document document;
        struct pa_list list;
        void *buf;

        int i;
        int tmp;

        int tmp_buf[10];

        clear(tmp_buf, 10);

        printf("Starting the program...\n");


        paInit(&document);

        buf = malloc(sizeof(int) * 20);
        paInitListFixed(&list, sizeof(int), 20, buf);

        for(i = 9; i >= 0; i--) {
                paPushList(&list, &i, 1);
        }
        
        tmp = 9;
        for(i = 9; i >= 0; i--) {
                paPushList(&list, &tmp, 1);
        }

        printf("List contents:\n");
        paApplyList(&list, &print_entry, NULL, PA_FORWARD);

        printf("Start shifting 5:\n");
        paShiftList(&list, tmp_buf, 5);

        for(i = 0; i < 5; i++)
                printf("%d\n", tmp_buf[i]);

        printf("List contents:\n");
        paApplyList(&list, &print_entry, NULL, PA_FORWARD);

        paUnshiftList(&list, tmp_buf, 3);

        printf("List contents:\n");
        paApplyList(&list, &print_entry, NULL, PA_FORWARD);

        tmp = 0;
        paInsertList(&list, &tmp, 10, 1);

        printf("List contents:\n");
        paApplyList(&list, &print_entry, NULL, PA_FORWARD);

        paGetList(&list, tmp_buf, 2, 8);
        for(i = 0; i < 8; i++) {
                printf("%d  ", tmp_buf[i]);
        }
        printf("\n");

        printf("List contents:\n");
        paApplyList(&list, &print_entry, NULL, PA_FORWARD);

        printf("Shutting down the program!\n");

        paDestroyList(&list);
        paQuit(&document);
        free(buf);
        return 0;
}
