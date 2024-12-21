#include "patchy.h"
#include "patchy_internal.h"


/* 
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 *
 *              STRING-HELPER
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 */

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
