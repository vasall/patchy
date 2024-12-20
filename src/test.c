#include "patchy.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(void)
{
        struct pa_document document;
        struct pa_string str;

        #define BUFFER_SIZE 24
        char buffer[BUFFER_SIZE];

        #define OUTPUT_SIZE 128
        char output_buffer[OUTPUT_SIZE];
        int read;

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

        read = paCopyString(&str, output_buffer, 1, PA_ALL, OUTPUT_SIZE);
        printf("Read(%d, %d): %s\n", read, strlen(output_buffer), output_buffer);
        printf("String(%d chars, %d bytes): %s\n", str.length, str.size, str.buffer); 
        *output_buffer = 0;

        read = paReadString(&str, output_buffer, 1, 2, OUTPUT_SIZE);
        printf("Read(%d, %d): %s\n", read, strlen(output_buffer), output_buffer);
        printf("String(%d chars, %d bytes): %s\n", str.length, str.size, str.buffer); 
        *output_buffer = 0;

        printf("Shutdown...\n");
        paDestroyString(&str);
        paQuit(&document);
        printf("Done!\n");

        return 0;
}
