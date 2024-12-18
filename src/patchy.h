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

/*
 * Redefine NULL as it's not builtin-type for whatever reason. :(
 */
#ifndef NULL
#define NULL ((void *)0)
#endif


typedef unsigned char           u8;
typedef unsigned short          u16;
typedef unsigned int            u32;
typedef unsigned long           u64;

typedef signed char             s8;
typedef signed short            s16;
typedef signed int              s32;
typedef signed long             s64;

typedef float                   f32;
typedef double                  f64;


#define PA_IGNORE(x)           (void)(x)

#define PA_MIN(a, b)            ((a > b) ? b : a)
#define PA_MAX(a, b)            ((a > b) ? a : b)

#define PA_OVERLAP(min1, max1, min2, max2) \
        (PA_MAX(0, PA_MIN(max1, max2) - PA_MAX(min1, min2)))

#define PA_START                  0
#define PA_END                   -1

#define PA_ALL                   -1

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
        void *(*alloc)(u64 size);
        void *(*realloc)(void *p, u64 size);
        void (*free)(void *p);
};

struct pa_memory {
        enum pa_memory_mode     mode;
        void                    *space;
        struct pa_allocator     allocator;
};

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

/* Check if byte is start of utf8-sequence */
#define PA_ISUTF(c) (((c) & 0xC0) != 0x80)

/*
 * 
 */
struct pa_string {
        struct pa_memory *memory;
        enum pa_memory_mode mode;

        char *buffer;   /* The buffer containing the string */

        s16 length;     /* The number of characters in the string */
        s32 size;       /* The number of used bytes excl. null-terminator */
        s32 alloc;      /* The number of allocated bytes */
};

/*
 * Initialize the string with dynamic memory which will be both used to
 * preallocate memory during initialization and to ensure new characters will
 * fit in the character-buffer.
 *
 * @str: Pointer to the string
 * @mem: Pointer to the memory-manager
 *
 * Returns: 0 on success or -1 if an error occurred
 */
PA_API s8 paInitString(struct pa_string *str, struct pa_memory *mem);

/*
 * Initialize the string using fixed memory for the character-buffer. This will
 * mean the string will not scale to fit new characters if the limit has been
 * reached.
 *
 * @str: Pointer to the string
 * @buf: Pointer to the memory-buffer
 * @alloc: The size of the memory-buffer in bytes
 *
 * Returns: 0 on success or -1 if an error occurred
 */
PA_API s8 paInitStringFixed(struct pa_string *str, void *buf, s32 alloc);

/*
 * Write as many UTF8-characters to the string at the given character-offset as
 * possible. If the string is configured as dynamic, the string will scale to
 * fit all new characters.
 * This function will overwrite the current content of the string.
 * The characters in the source-buffer need to already be UTF8-encoded and if
 * the given number is set to PA_ALL, the string needs to be null-terminated.
 *
 * @str: Pointer to the string
 * @src: The source-buffer to copy from
 * @off: The offset-position in the string
 * @num: The number of characters to write to the string
 *
 * Returns: The number of written characters or -1 if an error occurred
 */
PA_API s16 paWriteString(struct pa_string *str, char *src, s16 off, s16 num);

/*
 * Insert UTF8-encoded characters from the source-buffer to the string-buffer at
 * the requested offset and move the trailing block back as to not overwrite and
 * current characters. This function will only copy over full-characters and
 * do that until either the requested number of characters has been inserted or
 * the string-buffer limit has been reached.
 *
 * @str: Pointer to the string
 * @src: The source-buffer to copy from
 * @off: The offset-position in the string
 * @num: The number of characters to insert into the string
 *
 * Returns: The number of written characters or -1 if an error occurred
 */
PA_API s16 paInsertString(struct pa_string *str, char *src, s16 off, s16 num);

/*
 * Copy characters from the string without removing them. The destination-buffer
 * has to be preallocated. The written string will be null-terminated. The
 * function will write as many characters into the destination-buffer until
 * either the limit or the requested character-number has been reached. Only
 * full character-sequences are written.
 *
 * @str: Pointer to the string
 * @dst: A pointer to the destination buffer
 * @off: The character to start copying from
 * @num: The number of characters to read from the string
 * @lim: The size of the destination-buffer in bytes with the null-terminator
 *
 * Returns: The number of copied characters or -1 if an error occurred
 */
PA_API s16 paCopyString(struct pa_string *str, char *dst, s16 off, 
                s16 num, s32 lim);

/*
 * Remove characters from the string-buffer and write them to the
 * destination-buffer. The destination-buffer has to be preallocated
 * to fit the requested characters. The written character-string will
 * be null-terminated. This function will only write the characters into the
 * destionation-buffer until the limit has been reached. Only full
 * character-sequences are written.
 *
 * @str: Pointer to the string
 * @dst: A pointer to the destination buffer
 * @off: The character to start reading from
 * @num: The number of charaters to read from the string
 * @lim: The size of the destination-buffer in bytes with the null-terminator
 *
 * Returns: The number of read characters or -1 if an error occurred
 */
PA_API s16 paReadString(struct pa_string *str, char *dst, s16 off,
                s16 num, s32 lim);

/*
 * Get the character-number in the string from the given byte-offset.
 * byte offset => character number
 *
 * @str: Pointer to the string
 * @off: The byte-offset
 *
 * Returns: The character-number in the string or -1 if an error occurred
 */
PA_API s16 paGetStringCharacter(struct pa_string *str, s32 off);

/*
 * Get the byte-offset from the character-number in the string.
 * character number => byte offset
 *
 * @str: Pointer to the string
 * @cnum: The character-number in the string
 *
 * Returns: The byte-offset in the string-buffer
 */
PA_API s32 paGetStringOffset(struct pa_string *str, s16 cnum);

/*
 * Get the next character in the string encoded in 4 bytes and update the
 * iterator handle.
 *
 * @str: Pointer to the string
 * @off: The current byte-offset in the string
 *
 * Returns: The next character in the string encoded in 4 bytes
 */
PA_API u32 paNextStringChar(struct pa_string *str, s32 *off);

/*
 * Get the pointer for the next character in the string.
 * You can use the function like this to iterate through the string:
 * ...
 * struct pa_string str;
 * void *chr = NULL;
 * ...
 * while((chr = paIterateString(&str, chr))) {
 *      ..Do something...
 * }
 * ...
 *
 * @str: Pointer to the string
 * @chr: Pointer to the current character in the string
 *
 * Returns: The pointer to the next character in the string or NULL if there are
 *          no more characters
 */
PA_API char *paIterateString(struct pa_string *str, char *chr);


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
        s16 inuse;  /* The total size of all used slots in bytes */
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
 * Get a pointer to the next entry in the list. For the first step pass NULL for
 * ptr.
 * Example on how to iterate through the whole list:
 * ...
 * struct pa_list list;
 * void *ptr = NULL;
 * ...
 * while((ptr = paIterateList(&lst, ptr))) {
 *      ..Do something..
 * }
 * ...
 */
PA_API void *paIterateList(struct pa_list *lst, void *ptr);

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
