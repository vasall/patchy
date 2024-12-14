#ifndef _PATCHY_INTERNAL_H
#define _PATCHY_INTERNAL_H

#include "patchy.h"


/* 
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 *
 *              UTF8
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 */

/* 
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 *
 *              ELEMENT
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 */

/*
 * Initialize the element-tree with the minimum of slots.
 *
 * @tree: Pointer to the tree struct
 *
 * Returns: 0 on success or -1 if an error occurred
 */
PA_LIB s8 pa_etr_init(struct pa_element_tree *tree);


/*
 * Free the allocated memory and destroy the element-tree.
 *
 * @tree: Pointer to the element-tree
 */
PA_LIB void pa_etr_destroy(struct pa_element_tree *tree);


/*
 * Reserve memory for a new element. Nothing will be initialized.
 *
 * @doc: Pointer to the document
 *
 * Returns: Pointer to the memory space for the new element or NULL if an error
 *          occurred
 */
PA_LIB struct pa_Element *pa_ele_new(struct pa_document *doc);


/*
 * Get a pointer to an element through the index.
 *
 * @doc: Pointer to the document
 *
 * Returns: Either a pointer to the element at the given index or NULL if an
 *          error occurred
 */
PA_LIB struct pa_Element *pa_ele_by_index(struct pa_document *doc, s16 idx);


#endif /* _PATCHY_INTERNAL_H */
