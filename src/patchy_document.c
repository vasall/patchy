#include "patchy.h"
#include "patchy_internal.h"

#include <stdlib.h>


PA_API s8 paInit(struct pa_document *doc)
{
        if(!doc)
                return -1;

        /* Initialize the element-tree */
        if(pa_element_tree_init(&doc->element_tree) < 0) {
                return -1;
        }

        return 0;

}


PA_API void paQuit(struct pa_document *doc)
{
        if(!doc)
                return;

        pa_element_tree_destroy(&doc->element_tree);
}


