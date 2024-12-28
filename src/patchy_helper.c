#include "patchy.h"
#include "patchy_internal.h"


/* 
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 *
 *              STRING-HELPER
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 */

PA_LIB s8 pa_isspace(char c)
{
        return c == 0x20;
}

PA_LIB s8 pa_isletter(char c)
{
        /* Uppercase letters A..Z */
        if(c >= 0x41 && c <= 0x5A)
                return 1;

        /* Lowercase letters a..z */
        if(c >= 0x61 && c <= 0x7A)
                return 1;

        return 0;
}

PA_LIB s8 pa_isdigit(char c)
{
        if(c >= 0x30 && c <= 0x39)
                return 1;

        return 0;
}

PA_LIB s8 pa_ishexcode(char c)
{
        return (pa_isletter(c) || pa_isdigit(c));
}

PA_LIB s32 pa_strlen(char *s)
{
        s32 c = 0;
        while(*(s++)) c++;
        return c;
}

PA_LIB void pa_strcpy(char *dst, char *src)
{
        while((*(dst++) = *(src++)));
}

PA_LIB s16 pa_strcmp(char *str1, char *str2)
{
        s16 i;

        if(pa_strlen(str1) != pa_strlen(str2))
                return 1;

        for(i = 0; i < pa_strlen(str1); i++) {
                if(str1[i] != str2[i])
                        return 1;
        }

        return 0;
}

PA_LIB char *pa_trim(char *str)
{
        char *end;

        /* Trim leading space */
        while(pa_isspace(*str)) str++;

        /* All spaces? */
        if (*str == 0)
                return str;

        /* Trim trailing space */
        end = str + pa_strlen(str) - 1;
        while (end > str && pa_isspace(*end)) end--;

        /* Write new null terminator */
        *(end + 1) = 0;

        return str;
}

PA_LIB s64 pa_atoi(char *s)
{
        char *run = s + pa_strlen(s) - 1;
        char c;
        s64 value = 0;
        s64 factor = 1;
        s8 read = 1;

        while(run >= s && read) {
                c = *run;
                /* Handle numbers 0..9 */
                if(pa_isdigit(c)) {
                        value += (c - 0x30) * factor;
                        factor *= 10;
                }
                /* Handle the minus-sign */
                else if(c == 0x2D) {
                        read = 0;
                        value *= -1;
                }
                run--;
        }

        return value;
}

/* Copied from https://github.com/GaloisInc/minlibc/ */
PA_LIB f64 pa_atof(char *s)
{
        f64 a = 0.0;
        s32 e = 0;
        s32 c;
        while ((c = *s++) && pa_isdigit(c)) {
                a = a * 10.0 + (c - '0');
        }
        if(c == '.') {
                while((c = *s++) && pa_isdigit(c)) {
                        a = a * 10.0 + (c - '0');
                        e = e - 1;
                }
        }
        if(c == 'e' || c == 'E') {
                s32 sign = 1;
                s32 i = 0;
                c = *s++;
                if(c == '+')
                        c = *s++;
                else if(c == '-') {
                        c = *s++;
                        sign = -1;
                }
                while(pa_isdigit(c)) {
                        i = i * 10 + (c - '0');
                        c = *s++;
                }
                e += i * sign;
        }
        while(e > 0) {
                a *= 10.0;
                e--;
        }
        while(e < 0) {
                a *= 0.1;
                e++;
        }
        return a;
}
