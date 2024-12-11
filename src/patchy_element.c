#include "patchy.h"
#include "patchy_internal.h"


PA_LIB s8 pa_element_tree_init(struct pa_element_tree *tree)
{
        s16 size;

        if(!tree)
                return -1;


        /* Preallocate memory for minimum element count */
        size = sizeof(struct pa_element) * PA_ELEMENT_TREE_MIN;
        if(!(tree->elements = pa_malloc(size))) {
                return -1;
        }

        /* Reset the attributes */
        tree->element_number = 0;
        tree->pipe_start = -1;

        return 0;
}


PA_LIB void pa_element_tree_destroy(struct pa_element_tree *tree)
{
        if(!tree)
                return;

        pa_free(tree->elements);
}
