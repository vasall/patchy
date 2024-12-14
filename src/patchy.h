#ifndef _PATCHY_H
#define _PATCHY_H


#define PA_INTERN       static
#define PA_LIB          extern
#define PA_API          extern

/* 
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 *
 *              HELPERS
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 */

#include <stdint.h>

typedef uint8_t                 u8;
typedef uint16_t                u16;
typedef uint32_t                u32;
typedef uint64_t                u64;

typedef int8_t                  s8;
typedef int16_t                 s16;
typedef int32_t                 s32;
typedef int64_t                 s64;

typedef float                   f32;
typedef double                  f64;

/*
 * The size of the datatypes in bytes.
 */
#define U8_S			1
#define U16_S			2
#define U32_S			4
#define U64_S			8

#define S8_S			1
#define S16_S			2
#define S32_S			4
#define S64_S			8

#define F32_S			4
#define F64_S			8



/*
 * The limits for the data types.
 */

#define U8_MIN                 8
#define U8_MAX                 255
#define U16_MIN                0
#define U16_MAX                65535
#define U32_MIN                0
#define U32_MAX                4294967295
#define U64_MIN                0
#define U64_MAX                18446744073709551615

#define S8_MIN                 -128
#define S8_MAX                 127
#define S16_MIN                32768
#define S16_MAX                32767
#define S32_MIN                -2147483648
#define S32_MAX                2147483647
#define S64_MIN                -9223372036854775808
#define S64_MAX                9223372036854775807


#define PA_IGNORE(x)           (void)(x)

/* 
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 *
 *              General Enumeration
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 */

enum pa_memory_mode {
        PA_DYNAMIC      = 0,
        PA_FIXED        = 1
};

enum pa_tag_type {
        PA_UNDEF        = 0,
        PA_BODY         = 1,
        PA_BLOCK        = 2,
        PA_TEXT         = 3,
        PA_INPUT        = 4,
        PA_IMAGE        = 5,
        PA_CUSTOM       = 6
};

/* 
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 *
 *              PREDEFINE ALL API STRUCTS
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 */

struct pa_allocator;
struct pa_memory;

struct pa_list;
struct pa_string;


/* 
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 *
 *              MEMORY
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 */

struct pa_allocator {
        void *(*alloc)(s32 size);
        void *(*realloc)(void *p, s32 size);
        void (*free)(void *p);
};

struct pa_memory {
        enum pa_memory_mode     mode;
        void                    *space;
        struct pa_allocator     allocator;
};

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
 *              COMPONENTS
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 */

/*
 * -----------------------------------------------------------------------------
 *
 *      STRING
 *
 * TODO:
 * - Implement UTF8-support
 *
 */

#define PA_STRING_INITIAL_SIZE  128

#define PA_START                  0
#define PA_END                   -1

#define PA_ALL                   -1

/*
 * 
 */
struct pa_string {
        struct pa_memory *memory;
        enum pa_memory_mode mode;

        char *buffer;   /* The buffer containing the string */

        s16 length;     /* The number of characters in the string */
        s32 size;       /* The number of used bytes */
        s32 alloc;      /* The number of allocated bytes */
};

/*
 * 
 */
PA_API s8 paInitString(struct pa_string *str, struct pa_memory *mem);

/*
 * 
 */
PA_API s8 paInitStringFixed(struct pa_string *str, void *buf, s32 size);

/*
 * Write UTF8-characters to the string at the given offset-position. Both the
 * offset aswell as the character number are defined as per-character-numbers.
 * For example "I love you" would be 10 characters and "我爱你" would be
 * 3 characters.
 *
 * @str: Pointer to the string
 * @src: The source-buffer to copy from containing UTF8-formatted characters
 * @off: The offset-position in characters
 * @num: The number of characters to write to the string
 *
 * Returns: The number of written characters or -1 if an error occurred
 */
PA_API s16 paWriteString(struct pa_string *str, void *src, s16 off, s16 num);

/*
 * Copy characters from the string without removing them. The destination-buffer
 * has to be preallocated to fit all the requested characters. The written
 * string will be null-terminated.
 *
 * @str: Pointer to the string
 * @dst: A pointer to the destination buffer
 * @off: The character to start copying from
 * @num: The number of characters to read from the string
 *
 * Returns: The number of copied characters or -1 if an error occurred
 */
PA_API s16 paCopyString(struct pa_string *str, void *dst, s16 off, s16 num);

/*
 * Read characters from the string while also removing them. The
 * destination-buffer has to be preallocated to fit the requested characters.
 * The written character-string will be null-terminated.
 *
 * @str: Pointer to the string
 * @dst: A pointer to the destination buffer
 * @off: The character to start reading from
 *
 * Returns: The number of read characters or -1 if an error occurred
 */
PA_API s16 paReadString(struct pa_string *str, void *dst, s16 off, s16 num);

/*
 * Get the pointer for the next character in the string.
 * You can use the function like this to iterate through the string:
 * ...
 * struct pa_string str;
 * void *chr = NULL;
 * ...
 * while((chr = paIterateString(&str, chr))) {
 *      ..Do something
 * }
 * ...
 *
 * @str: Pointer to the string
 * @chr: Pointer to the current character in the string
 *
 * Returns: The pointer to the next character in the string or NULL if there are
 *          no more characters
 */
PA_API s16 paIterateString(struct pa_string *str, void *chr);

/*
 * Get the character in the string from the given byte-offset.
 *
 * @str: Pointer to the string
 * @off: The byte-offset
 *
 * Returns: The character-number in the string or -1 if an error occurred
 */

/*
 * -----------------------------------------------------------------------------
 *
 *      LIST
 *
 */

struct pa_handle {
        void    *pointer;
        s32     index;
};

struct pa_list {
        struct pa_memory *memory;
        enum pa_memory_mode mode;

        u8 *data;

        s16 entry_size; /* The size of a slot in bytes */

        s16 count;  /* Number of used slots */
        s16 alloc;  /* Number of allocated slots */
};

typedef s8 (*pa_list_func)(struct pa_handle *hdl, void *data);

/*
 * Initialize the list. The memory will be allocated during initialization and
 * and will scale to fit all new entries. After use call paDestroyList() to
 * prevvent memory leaks.
 *
 * @lst: Pointer to the list
 * @mem: Pointer to the memory-manager
 * @size: The size of a single entry in bytes
 * @alloc: The initial number of slots to preallocate
 *
 * Returns: 0 on success or -1 if an error occurred
 */
PA_API s8 paInitList(struct pa_list *lst, struct pa_memory *mem,
                s16 size, s16 alloc);

/*
 * Create a static list onto a buffer. This list will only operate on the given
 * memory and will not adjust size to fit new elements if the limits has been
 * reached. Note that the memory-buffer has to be big enough to fit the given
 * number of slots for the entries set through the alloc-parameters. After use
 * call paDestroyList() and then free the memory yourself.
 *
 * Example on how to store 20 integers:
 * ...
 * struct pa_list list;
 * int numbers = 20;
 * void *buf = malloc(numbers * sizeof(int));
 * paInitListFixed(&lst, sizeof(int), numbers, buf);
 * ...
 *
 * @lst: Pointer to the list
 * @size: The size of a single entry in bytes
 * @alloc: The number of slots
 * @buf: The buffer to store the entries in
 *
 * Returns: 0 on success or -1 if an error occurred
 */
PA_API s8 paInitListFixed(struct pa_list *lst, s16 size, s16 alloc, void *buf);

/*
 * Destroy a list and free the allocated memory.
 *
 * @lst: Pointer to the list
 */
PA_API void paDestroyList(struct pa_list *lst);

/*
 * Remove all entries from the list and reset it's attributes. This will not
 * free memory.
 *
 * @lst: Pointer to the list
 */
PA_API void paClearList(struct pa_list *lst);

/*
 * Append entries to the end of the list. If the list is configured as static
 * and the limit is reached, no more entries will be written. If the list is
 * configured as dynamic, more memory will be allocated to fit all entries.
 *
 * @lst: Pointer to the list
 * @src: Pointer to the data to write to the list
 * @num: The number of entries to push to the list
 *
 * Returns: The number of entries written to the list or -1 if an error occurred
 */
PA_API s16 paPushList(struct pa_list *lst, void *src, s16 num);

/*
 * Pop entries from the end of the list and write them to the given pointer.
 *
 * @lst: Pointer to the list
 * @out: A pointer to write the entries to
 * @num: The number of entries to pop from the list
 *
 * Returns: The number of entries popped from the list or -1 if an error
 *          occurred
 */
PA_API s16 paPopList(struct pa_list *lst, void *dst, s16 num);

/*
 * Add entries to the beginning of the list. If the list is configured as
 * static and the limit has been reached, no more entries will be added. If the
 * list is configured as dynamic, the necessary memory to fit all entries will
 * be allocated.
 *
 * @lst: Pointer to the list
 * @src: A pointer to copy the entries from
 * @num: The number of entries to write to the list
 *
 * Returns: The number of entries added to the list or -1 if an error occurred
 */
PA_API s16 paUnshiftList(struct pa_list *lst, void *src, s16 num);

/*
 * Get entries from the beginning of the list and write to the output-pointer.
 *
 * @lst: Pointer to the list
 * @out: A pointer to write the entries to
 * @num: The number of entries to take from the list
 *
 * Returns: The number of entries written to the output-pointer or -1 if an
 *          error occurred
 */
PA_API s16 paShiftList(struct pa_list *lst, void *dst, s16 num);

/*
 * Insert entries into the list at a certain position.
 *
 * @lst: Pointer to the list
 * @src: Pointer to copy the entries from
 * @start: The starting index to insert the entries to
 * @num: The number of entries to insert
 *
 * Returns: The number of entries written to the list or -1 if an error occurred
 */
PA_API s16 paInsertList(struct pa_list *lst, void *src, s16 start, s16 num);

/*
 * Copy the entries from the list without removing them.
 *
 * @lst: Pointer to the list
 * @out: Pointer to write the entries to
 * @start: The starting index to copy from
 * @num: The number of elements to copy
 *
 * Returns: The number of written entries or -1 if an error occurred
 */
PA_API s16 paPeekList(struct pa_list *lst, void *dst, s16 start, s16 num);

/*
 * Extract elements from the list from the starting index.
 *
 * @lst: Pointer to the list
 * @out: Pointer to write the entries to
 * @start: The starting offset
 * @num: The number of entries to retrieve
 *
 * Returns: The number of retrieved elements or -1 if an error occurred
 */
PA_API s16 paGetList(struct pa_list *lst, void *dst, s16 start, s16 num);

/*
 * Call a callback-function on every entry in the list. If the callback-function
 * returns 1 the loop will stop. Otherwise the callback-function should always
 * return 0.
 *
 * @lst: Pointer to the list
 * @fnc: The callback function
 * @pass: A pointer that will be passed onto every function-call
 */
PA_API void paApplyList(struct pa_list *lst, pa_list_func fnc, void *pass);

/*
 * Call a callback-function from the last entry to the first. If the
 * callback-function returns 1 the loop will stop. Otherwise the
 * callback-fcuntion should always return 0.
 *
 * @lst: Pointer to the list
 * @fnc: The callback function
 * @pass: A pointer that will be passed onto every function-call
 */
PA_API void paApplyListBack(struct pa_list *lst, pa_list_func fnc, void *pass);


struct pa_Dictionary {

};

/*
 * ----  CODE-TABLE  ----
 *
 * OPERATOR
 * 	
 * 	0x01	(	
 * 	0x02	)
 * 	0x03	*
 * 	0x04	+
 * 	0x05	-
 * 	0x06	/
 *
 * OPERAND
 *
 * 	0x11    const	     12	  Constant value
 * 	0x12	PIXEL	    9px	  Pixel value, works like const
 * 	0x13	PERCENT	  10pct	  Pct. used to scale with size value
 * 	0x14	EM	    4em	  Faktor multp. with text size
 *
 * ----------------------
 */
struct pa_flex_token {
        u8                      code;
        f32                     value;
};

struct pa_flex {
        struct pa_list         tokens;
};

/*
 * 
 */
PA_LIB s8 wut_flx_init(struct pa_flex *flx, s16 tokens);

/*
 * 
 */
PA_LIB void wut_flx_destroy(struct pa_flex *flx);

/*
 * 
 */
PA_LIB void wut_flx_parse(struct pa_flex *flx, char *str);


/* 
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 *
 *              RESOURCES
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 */ 


/*
 * Contains all resources for the patchy-instance like textures, fonts and
 * icons.
 */
struct pa_context {

};


/* 
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 *
 *              STYLE
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 */

/*
 * A single style-attribute.
 */
struct pa_style_attribute {
              
};


/*
 * 
 */
struct pa_stylesheet {

};


/*
 * 
 */
struct pa_style {

};


/*
 * A class is a bundle of different style-attributes applies to all elements
 * tagged with the class-name.
 */
struct pa_class_style {

};

/*
 * The typeclass is a bundle of different style-attributes applied to all
 * elements with a given type.
 */
struct pa_type_style {

};

/* 
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 *
 *              EVENT
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 */

/*  */
struct pa_event {

};

typedef s8 (*pa_event_callback)(struct pa_event evt, void *data);

/*  */
struct pa_event_handler {

};


PA_API void paEventBegin(void);
PA_API void paEventEnd(void);

PA_API void paEventKey(void);
PA_API void paEventMotion(void);
PA_API void paEventButton(void);
PA_API void paEventScroll(void);

/* 
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 *
 *              ELEMENT
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 */

#define PA_ELEMENT_NAME_MAX     128

/*
 * 
 */
struct pa_element {
        /*  */
        char    name[PA_ELEMENT_NAME_MAX];

        /*  */
        s16     parent;

        /*  */
        s16     sibling_next;

        /*  */
        s16     children_number;
        s16     children_start;

        /*  */
        s16     broadsearch_next;

        /*  */
        s16     pipe_next;
};


#define PA_ELEMENT_TREE_MIN     128

/*
 * 
 */
struct pa_element_tree {
        /* A list containing all elements */
        struct pa_list          elements;

        /* The z-index pipeline to handle overlap */
        s16                     pipe_start;
};


/* 
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 *
 *              BATCHING
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 */

/*
 * A patch is a representation of the final rendering instructions and contains
 * information about size, position and possible references to required
 * resources from the context.
 */
struct pa_patch {

};

/*
 * A batch contains multiple patches using the same material, ie. the same
 * transparency/opaquenes, same resources, same color, etc.
 */
struct pa_batch {

};


/* 
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 *
 *              DOCUMENT
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 */

/*
 * General wrapper for a patchy-instance containing all necessary modules and
 * parts.
 * Use one per window.
 */
struct pa_document {
        /*
         * Memory manager to handle alloc, realloc, free, etc.
         * For now it relies on the default allocation functions, but in the
         * future it will be adapted to work with static memory and custom
         * allocation functions.
         */
        struct pa_memory        memory;

        struct pa_element_tree  element_tree; 
};

/*
 * Initialize the document and prepare everything.
 *
 * @doc: Pointer to the document
 *
 * Returns: 0 on success or -1 if an error occurred
 */
PA_API s8 paInit(struct pa_document *doc);


/*
 * Shut everything down, free all allocated memory and cleanup.
 *
 * @doc: Pointer to the document
 */
PA_API void paQuit(struct pa_document *doc);

#endif /* _PATCHY_H */
