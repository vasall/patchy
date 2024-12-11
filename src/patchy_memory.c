#include "patchy.h"

#include <stdlib.h>


PA_LIB void *pa_malloc(s32 size)
{
        return malloc(size);
}


PA_LIB void *pa_realloc(void *p, s32 size)
{
        return realloc(p, size);
}


PA_LIB void pa_free(void *p)
{
        free(p);
}
