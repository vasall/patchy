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


/* 
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 *
 *              General Enumeration
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 */

typedef enum {
        PA_STATIC,
        PA_DYNAMIC
} PAenum;

/* 
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 *
 *              MEMORY
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 */

PA_LIB void *pa_mem_alloc(void *p, s32 size);
PA_LIB void  pa_mem_free(void *p);
PA_LIB void  pa_mem_set(void *p, u8 b, s32 size);
PA_LIB void  pa_mem_copy(void *dst, void *src, s32  size);

/* 
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 *
 *              COMPONENTS
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 */

#define PA_STRING_GROWTH        1.5

#define PA_STRING_START           0
#define PA_STRING_END            -1

#define PA_STRING_AUTO           -1

/*
 * 
 */
typedef struct {
        s16 length;    /* The number of characters/glyphs */

        s16 size;      /* The number of used bytes */
        s16 alloc;     /* The number of allocated bytes */

        char *buffer;   /* The buffer containing the string */
} pa_String;


typedef struct {
        PAenum mode;        

        s16 entry_size; /* The size of a slot in bytes */

        s16 count;  /* Number of used slots */
        s16 alloc;  /* Number of allocated slots */

        u8 *data;
} pa_List;

/*
 * Initialize the list and preallocate the memory for the entries.
 *
 * @lst: Pointer to the list
 * @memmode: The mode to use for this list(PA_STATIC, PA_DYNAMIC)
 * @size: The size of a single entry in bytes
 * @alloc: The number of slots to preallocate
 *
 * Returns: 0 on success or -1 if an error occurred
 */
PA_LIB s8 paInitList(pa_List *lst, PAenum mode, s16 size, s16 alloc);

/*
 * Destroy a list and free the allocated memory.
 *
 * @lst: Pointer to the list
 */
PA_LIB void paDestroyList(pa_List *lst);

/*
 * Remove all entries from the list and reset it's attributes. This will not
 * free memory.
 *
 * @lst: Pointer to the list
 */
PA_LIB void paClearList(pa_List *lst);

/*
 * Append entries to the end of the list. If the list is configured as static
 * and the limit is reached, no more entries will be written. If the list is
 * configured as dynamic, more memory will be allocated to fit all entries.
 *
 * @lst: Pointer to the list
 * @ptr: Pointer to the data to write to the list
 * @num: The number of entries to push to the list
 *
 * Returns: The number of entries written to the list or -1 if an error occurred
 */
PA_LIB s16 paPushList(pa_List *lst, void *ptr, s16 num);

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
PA_LIB s16 paPopList(pa_List *lst, void *out, s16 num);

/*
 * Add entries to the beginning of the list. If the list is configured as
 * static and the limit has been reached, no more entries will be added. If the
 * list is configured as dynamic, the necessary memory to fit all entries will
 * be allocated.
 *
 * @lst: Pointer to the list
 * @ptr: A pointer to copy the entries from
 * @num: The number of entries to write to the list
 *
 * Returns: The number of entries added to the list or -1 if an error occurred
 */
PA_LIB s16 paUnshiftList(pa_List *lst, void *ptr, s16 num);

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
PA_LIB s16 paShiftList(pa_List *lst, void *out, s16 num);

/*
 * Insert entries into the list
 */
PA_LIB s16 paInsertList(paList *lst, void *ptr, s16 start, s16 num);

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
PA_LIB s16 paPeekList(pa_List *lst, void *out, s16 start, s16 num);

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
PA_LIB s16 paGetList(pa_List *lst, void *out, s16 start, s16 num);

/*
 * Megre two lists into one. First copy the entries from the first list and then
 * the second. This function will initialize the resulting list and allocate the
 * necessary space. If either one of the lists are marked as static, the new
 * list will also be marked as static. After using remember to destroy the input
 * lists if you don't need them anymore.
 *
 * @in1: The first list
 * @in2: The second list
 * @out: The new list containing both lists
 *
 * Returns: The number of elements in the new list or -1 if an error occurred
 */
PA_LIB s16 paMergeList(pa_List *in1, pa_List *in2, pa_List *out);

/*
 * Create a new list and copy over entries from the source list. This will not
 * modify the source in any way.
 *
 * @src: Pointer to the source list 
 * @out: The pointer to the new list
 * @start: The starting offset to start copying from
 * @num: The number of elements to copy
 *
 * Returns: The number of entries in the new list or -1 if an error occurred
 */
PA_LIB s16 paDeriveList(pa_List *src, pa_List *out, s16 start, s16 num);


struct pa_Table {

};

struct pa_Dictionary {

};

struct pa_Flex {

};


/* 
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 *
 *              RESOURCES
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 */ 


enum pa_eResourceType {
        PA_RES_IMAGE        = 1,
        PA_RES_ICON         = 2,
        PA_RES_FONT         = 3
};


struct pa_Texture {

};

struct pa_Image {

};

struct pa_ImageAtlas {

};

struct pa_Icon {

};

struct pa_IconAtlas {

};

struct pa_Font {

};

struct pa_FontAtlas {

};


/*
 * Contains all resources for the patchy-instance like textures, fonts and
 * icons.
 */
struct pa_Context {

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
typedef struct {
        enum wut_eSheetAttribId 	id;

        enum wut_eSheetDatatype 	type;

        union {
                struct {
                        struct wut_Flex         *pointer;
                } flex;

                struct {
                        u32                     code;
                        u32 _pad;
                } hexcode;

                struct {
                        u8                      code;
                        u8 _pad[7];
                } keyword;
        } value;      
} pa_StyleAttribute;


/*
 * 
 */
struct pa_Stylesheet {

};


/*
 * 
 */
struct pa_Style {

};


/*
 * A class is a bundle of different style-attributes applies to all elements
 * tagged with the class-name.
 */
struct pa_Class {

};

/*
 * The typeclass is a bundle of different style-attributes applied to all
 * elements with a given type.
 */
struct pa_TypeClass {

};

/* 
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 *
 *              EVENT
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 */

/*  */
typedef struct {

} pa_Event;

typedef s8 (*pa_event_callback)(struct pa_Event evt, void *data);

/*  */
typedef struct {

} pa_EventHandler;


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


enum pa_eTag {
        PA_UNDEF        = -1,
        PA_BODY         =  0,
        PA_BLOCK        =  1,
        PA_TEXT         =  2,
        PA_INPUT        =  3,
        PA_IMAGE        =  4,
        PA_CUSTOM       =  5
};

/*
 * 
 */
typedef struct {
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
} pa_Element;


#define PA_ELEMENT_TREE_MIN     128

/*
 * 
 */
typedef struct {
        /* A list containing all elements */
        struct pa_Element       *elements;
        s16                     element_number;

        /* The z-index pipeline to handle overlap */
        s16                     pipe_start;
} pa_ElementTree;


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
struct pa_Patch {

};

/*
 * A batch contains multiple patches using the same material, ie. the same
 * transparency/opaquenes, same resources, same color, etc.
 */
struct pa_Batch {

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
struct pa_Document {
        pa_ElementTree   element_tree; 
};

/*
 * Initialize the document and prepare everything.
 *
 * @doc: Pointer to the document
 *
 * Returns: 0 on success or -1 if an error occurred
 */
PA_API s8 paInit(struct pa_Document *doc);


/*
 * Shut everything down, free all allocated memory and cleanup.
 *
 * @doc: Pointer to the document
 */
PA_API void paQuit(struct pa_Document *doc);

#endif /* _PATCHY_H */
