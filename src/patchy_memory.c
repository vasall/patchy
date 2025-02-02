#include "patchy.h"
#include "patchy_internal.h"

#include <stdlib.h>
#include <string.h>


PA_LIB s8 pa_mem_init_default(struct pa_memory *mem)
{
        mem->mode = PA_DYNAMIC;
        mem->space = NULL;

        mem->allocator.alloc = &malloc;
        mem->allocator.realloc = &realloc;
        mem->allocator.free = &free;
        
        return 0;
}

PA_LIB void *pa_mem_alloc(struct pa_memory *mem, void *p, s32 size)
{
        if(p) {
                return mem->allocator.realloc(p, size);
        }

        return mem->allocator.alloc(size);
}

PA_LIB void pa_mem_free(struct pa_memory *mem, void *p)
{
        mem->allocator.free(p);
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

PA_LIB s8 pa_mem_compare(void *ptr1, void *ptr2, s32 size)
{
        if(memcmp(ptr1, ptr2, size) == 0)
                return 1;

        return 0;
}
