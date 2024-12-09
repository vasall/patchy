#ifndef _PATCHY_H
#define _PATCHY_H

/*
 * COMPONENTS
 *
 * Abstract constructs formatting data in a certain way.
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
 * Contains all resources for the patchy-instance like textures, fonts and
 * icons.
 */
struct pa_context {
        
};


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
 * General wrapper for a patchy-instance containing all necessary modules and
 * parts.
 * Use one per window.
 */
struct pa_document {
        
};


#endif /* _PATCHY_H */
