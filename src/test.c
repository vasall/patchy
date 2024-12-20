#include "patchy.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char **argv)
{
        struct pa_document document;
        struct pa_string str;

        #define BUFFER_SIZE 24
        char buffer[BUFFER_SIZE];
        
        char *string1 = "HelloWorld";           /* 10 characters, 10 bytes */
        char *string2 = "一二三四五六七八九十"; /* 10 characters, 30 bytes */
        char *string3 = "wo ai ni";             /*  8 characters,  8 bytes */
        char *string4 = "hello my friends";    /*  16 characters, 16 bytes */

        printf("Init...\n");
        paInit(&document);
        paInitStringFixed(&str, buffer, BUFFER_SIZE);
        printf("Done!\n");

        
        /* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

        printf("Write string1(%d bytes): %s\n", strlen(string1), string1);
        paWriteString(&str, string1, PA_START, PA_ALL);
        printf("String(%d chars, %d bytes): %s\n", str.length, str.size, str.buffer); 

        printf("Write string2(%d bytes): %s\n", strlen(string2), string2);
        paWriteString(&str, string2, PA_START, PA_ALL);
        printf("String(%d chars, %d bytes): %s\n", str.length, str.size, str.buffer); 

        printf("InsertString3(%d bytes): %s\n", strlen(string3), string3);
        paInsertString(&str, string3, 1, 3);
        printf("String(%d chars, %d bytes): %s\n", str.length, str.size, str.buffer); 

        printf("WriteString4(%d bytes): %s\n", strlen(string4), string4);
        paWriteString(&str, string4, 0, 7);
        printf("String(%d chars, %d bytes): %s\n", str.length, str.size, str.buffer); 

        printf("Write string2(%d bytes): %s\n", strlen(string2), string2);
        paWriteString(&str, string2, PA_START, 5);
        printf("String(%d chars, %d bytes): %s\n", str.length, str.size, str.buffer); 

        /* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */


        printf("Shutdown...\n");
        paDestroyString(&str);
        paQuit(&document);
        printf("Done!\n");

        return 0;
}
