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
 *              MEMORY
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 */

PA_LIB void *pa_malloc(s32 size);
PA_LIB void *pa_realloc(void *p, s32 size);
PA_LIB void pa_free(void *p);


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
struct pa_string {
        s16     length; /* The number of characters/glyphs */

        s16     size;   /* The number of used bytes */
        s16     alloc;  /* The number of allocated bytes */

        char    *buf;   /* The buffer containing the string */
};

struct pa_tree {

};

struct pa_list {

};

struct pa_table {

};

struct pa_dictionary {

};


/* 
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 *
 *              RESOURCES
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 */ 


enum pa_resource_type {
        PA_RES_IMAGE        = 1,
        PA_RES_ICON         = 2,
        PA_RES_FONT         = 3,
        PA_RES_TILE         = 4
};


struct pa_texture {

};

struct pa_image {

};

struct pa_image_atlas {

};

struct pa_icon {

};

struct pa_icon_atlas {

};

struct pa_font {

};

struct pa_font_atlas {

};

struct pa_tile {

};

struct pa_tile_atlas {

};

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
 * 
 */
struct pa_stylesheet {

};


/*
 * A single style-attribute.
 */
struct pa_style_attrib {
        
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
struct pa_class {

};

/*
 * The typeclass is a bundle of different style-attributes applied to all
 * elements with a given type.
 */
struct pa_type_class {

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


/* 
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 *
 *              ELEMENT
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 */

#define PA_ELEMENT_NAME_MAX     128


enum pa_tag {
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
        struct pa_element       *elements;
        s16                     element_number;

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
       struct pa_element_tree   element_tree; 
};

/*
 * Initialize the document and prepare everything.
 *
 * @doc: Pointer to the document
 *
 * Returns: 0 on success or -1 if an error occurred
 */
PA_API s8 pa_init(struct pa_document *doc);


/*
 * Shut everything down, free all allocated memory and cleanup.
 *
 * @doc: Pointer to the document
 */
PA_API void pa_quit(struct pa_document *doc);

#endif /* _PATCHY_H */
