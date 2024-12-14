#include "patchy.h"

#include <stdlib.h>
#include <string.h>

PA_LIB void *pa_mem_alloc(struct pa_memory *mem, void *p, s32 size)
{
        if(p) {
                return realloc(p, size);
        }

        return malloc(size);
}

PA_LIB void pa_mem_free(struct pa_memory *mem, void *p)
{
        free(p);
}

PA_LIB void pa_mem_set(void *p, u8 v, s32 size)
{
        memset(p, v, size);
}

PA_LIB void pa_mem_zero(void *p, s32 size)
{
        memset(p, 0, size);
}


PA_LIB void pa_mem_copy(void *dst, void *src, s32 size)
{
        memcpy(dst, src, size);
}

PA_LIB void pa_mem_move(void *dst, void *src, s32 size)
{
        memmove(dst, src, size);
}
