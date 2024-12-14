#ifndef _PATCHY_INTERNAL_H
#define _PATCHY_INTERNAL_H

#include "patchy.h"


/* 
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 *
 *              COMPONENTS
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 */


/*
 * Create a new string object and preallocate the given number of bytes.
 *
 * @alloc: The number of bytes to preallocate
 *
 * Returns: Either a pointer to the newly created string object or NULL if an
 *          error occurred
 */
PA_API struct pa_string *pa_str_create(s16 alloc);


/*
 * Destroy a string and free up the allocated memory.
 *
 * @str: Pointer to the string object
 */
PA_API void pa_str_destroy(struct pa_string *str);


/*
 * Clear the string and reset it's attributes to zero.
 *
 * @str: Pointer to the string object
 */
PA_API void pa_str_clear(struct pa_string *str);


/*
 * Get the characters from the string object as a regular C-string.
 *
 * @str: Pointer to the string object
 *
 * Returns: The C-string
 */
PA_API char *pa_str_get(struct pa_string *str);


/*
 * Combine two strings to one new one, by attaching strings 2 to string 1.
 * This will not destroy the two input strings, so if you don't need them
 * anymore, remember to clear both of them.
 *
 * @str1: The first string
 * @str2: The second string appended to the first one
 *
 * Returns: Either a new string or NULL if an error occurred
 */
PA_API struct pa_string *pa_str_combine(struct pa_string *str1,
                struct pa_string *str2);


/*
 * This function will write the given characters to the string. If you specify
 * an offset in the middle of the string, the trailing characters will be
 * overwritten. If you want to just add the new characters, use the extend
 * function instead.
 *
 * @str: Pointer to the string object
 * @off: The offset to write the new characters to
 * @txt: The new characters to write to the string
 * @[len]: The size of the string in bytes
 *
 * Returns: 0 if successfull or -1 if an error occurred
 */
PA_API s8 pa_str_write(struct pa_string *str, s16 off, char *txt, s16 len);


/*
 * Append characters to the string. If you specify an offset, the trailing
 * characters will be moved back to fit the incoming ones, so nothing will be
 * overwritten.
 *
 * @str: Pointer to the string object
 * @off: The offset to place the new characters into in characters
 * @[len]: The size of the string in bytes
 *
 * Returns: 0 on success or -1 if an error occurred
 */
PA_API s8 pa_str_extend(struct pa_string *str, s16 off, char *txt, s16 len);


/*
 * Crop a string to the given limits.
 *
 * @str: Pointer to the string object
 * @start: The starting character to start cropping from
 * @end: The end character to crop to
 *
 * Returns: 0 on success or -1 if an error occurred
 */
PA_API s8 pa_str_crop(struct pa_string *str, s16 start, s16 end);


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
