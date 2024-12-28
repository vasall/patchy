#ifndef _PATCHY_INTERNAL_H
#define _PATCHY_INTERNAL_H

#include "patchy.h"


/* 
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 *
 *              MEMORY
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 */

/*
 * Initialize the memory-manager and attach the default allocation functions to
 * it. The memory manager will therefore also be configured as dynamic.
 *
 * @mem: Pointer to the memory-manager
 *
 * Returns: 0 on success or -1 if an error occurred
 */
PA_LIB s8 pa_mem_init_default(struct pa_memory *mem);

/*
 * Allocate memory to fit the given number of bytes. If some memory has already
 * been allocated and is given through the pointer-parameter, the memory will
 * instead be reallocated. In case reallocation fails, the existing memory will
 * not be corrupted, but NULL will be returned.
 *
 * @mem: Poiner to the memory-manager
 * @[p]: Current memory pointer
 * @size: The requested number of bytes
 *
 * Returns: A pointer to the memory or NULL if an error occurred
 */
PA_LIB void *pa_mem_alloc(struct pa_memory *mem, void *p, s32 size);

/*
 * Free the allocated memory if the mode is set to dynamic. In case of static
 * memory nothing will happen.
 *
 * @mem: Pointer to the memory-manager
 * @p: Pointer to the memory-space to free
 */
PA_LIB void pa_mem_free(struct pa_memory *mem, void *p);

/*
 * Set the all bytes in the given memory-space.
 *
 * @p: Pointer to the memory-space
 * @v: The bytes
 * @size: The number of bytes to set
 */
PA_LIB void pa_mem_set(void *p, u8 v, s32 size);

/*
 * Zero all bytes in a given memory-space.
 *
 * @p: Pointer to the memory-space
 * @size: The number of bytes to zero out
 */
PA_LIB void pa_mem_zero(void *p, s32 size);

/*
 * Copy over memory. If the source and destination overlap, this function can
 * overwrite the source while still reading from it, so watch out for this.
 *
 * @dst: Pointer to the destination
 * @src: Pointer to the source
 * @size: The number of bytes to copy
 */
PA_LIB void pa_mem_copy(void *dst, void *src, s32 size);

/*
 * This will move memory while checking that the source will not be overwritten
 * while reading from it.
 *
 * @dst: Pointer to the destination
 * @src: Pointer to the source
 * @size: The number of bytes to move
 */
PA_LIB void pa_mem_move(void *dst, void *src, s32 size);

/* 
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 *
 *              STRING-HELPER
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 */

/*
 * Returns 1 for ASCII-space-character.
 * Otherwise returns 0.
 */
PA_LIB s8 pa_isspace(char c);

/*
 * Returns 1 for ASCII-characters A..Z, a..z.
 * Otherwise returns 0.
 */
PA_LIB s8 pa_isletter(char c);

/*
 * Returns 1 for ASCII-characters 0..9.
 * Otherwise returns 0.
 */
PA_LIB s8 pa_isdigit(char c);

/*
 * Returns 1 for ASCII-characters 0..9, A..Z, a..z.
 * Otherwise returns 0.
 */
PA_LIB s8 pa_ishexcode(char c);

/*
 * A wrapper for the clib-strlen function.
 */
PA_LIB s32 pa_strlen(char *s);

/*
 * A wrapper for the clib-strcpy function.
 */
PA_LIB void pa_strcpy(char *dst, char *src);

/*
 * A wrapper for the clib-strcmp function.
 */
PA_LIB s16 pa_strcmp(char *str1, char *str2);

/*
 * Trim the leading and trailing spaces from a string by moving the string
 * pointer to the first character and write a null-terminator after the last
 * character. This function will not move the characters to the beginning of the
 * string-buffer, so watch out to not lose the pointer for the original memory.
 *
 * @s: Pointer to the null-terminated string to trim
 *
 * Returns: The updated pointer to the string 
 */
PA_LIB char *pa_trim(char *str);

/*
 * Convert the input string containing ASCII-encoded numbers into an usable
 * integer-value.
 *
 * @s: The null-terminated string containing the ASCII-encoded integer-value
 *
 * Returns: The resulting integer value
 */
PA_LIB s64 pa_atoi(char *s);

/*
 * Convert the input string containing an ASCII-encoded floating-point value
 * into a usable float.
 *
 * @s: The null-terminated string containing the ASCII-encoded float-value
 *
 * Returns: The resulting float value
 */
PA_LIB f64 pa_atof(char *s);

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
