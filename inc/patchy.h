#ifndef _PATCHY_H
#define _PATCHY_H

/* 
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 *
 *              COMPONENTS
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 */

/*
 * 
 */
struct pa_string {

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
        PA_IMAGE        = 1,
        PA_ICON         = 2,
        PA_FONT         = 3,
        PA_TILE         = 4
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

/*
 * 
 */
struct pa_element {

};


/*
 * 
 */
struct pa_element_tree {

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
        
};


#endif /* _PATCHY_H */
