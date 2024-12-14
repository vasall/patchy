#include "patchy.h"
#include "patchy_internal.h"

#include <stdlib.h>


PA_API s8 paInit(struct pa_document *doc)
{
        if(!doc)
                return -1;

        pa_mem_init_default(&doc->memory);

        return 0;

}


PA_API void paQuit(struct pa_document *doc)
{
        if(!doc)
                return;

}


