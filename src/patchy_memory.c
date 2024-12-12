#include "patchy.h"

#include <stdlib.h>


PA_LIB void *pa_mem_alloc(void *p, s32 size)
{
        if(p) {
                return realloc(p, size);
        }

        return malloc(size);
}

PA_LIB void pa_mem_free(void *p)
{
        free(p);
}

PA_LIB void pa_mem_set(void *p, u8 b, s32 size)
{
        memset(p, b, size);
}


PA_LIB void  pa_mem_copy(void *dst, void *src, s32  size)
{
        memcpy(dst, src, size);
}
