#include "patchy.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(void)
{
        struct pa_Document document;

        printf("Starting the program...\n");


        paInit(&document);


        paQuit(&document);
        return 0;
}
