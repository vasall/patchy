/*
*/
#ifndef PA_SINGLE_FILE
#define PA_SINGLE_FILE
#endif

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
#define PA_CEIL(x)              ((x-(s32)(x)) > 0 ? (s32)(x+1) : (s32)(x))


#define PA_OVERLAP(min1, max1, min2, max2) \
        (PA_MAX(0, PA_MIN(max1, max2) - PA_MAX(min1, min2)))

#define PA_START                  0
#define PA_END                   -1

#define PA_ALL                   -1

#define PA_NOLIM                 -1

/* 
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 *
 *              General Enumeration
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 */

enum pa_memory_mode {
        PA_MEM_UDEF     = -1,
        PA_DYNAMIC      =  0,
        PA_FIXED        =  1
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
 * Destroy the string, reset all attributes and free any allocated memory if the
 * string is configured as static. If the string is static, the provided memory
 * has to be freed manually after use.
 *
 * @str: Pointer to the string
 */
PA_API void paDestroyString(struct pa_string *str);

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
        s16 alloc;  /* Number of allocated slots */

        s32 alloc_size; /* The size of allocated buffer in bytes */
        s32 limit;  /* The absolute limit for the size in bytes */
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
 * @lim: The number of bytes to allocate at max, use -1 for no limit
 *
 * Returns: 0 on success or -1 if an error occurred
 */
PA_API s8 paInitList(struct pa_list *lst, struct pa_memory *mem,
                s32 size, s16 alloc, s32 lim);

/*
 * Create a static list onto a buffer. This list will only operate on the given
 * memory and will not adjust its size to fit new elements once the limits has
 * been reached. The function will also calculate the number of usable slots in
 * the given buffer.
 * After use call paDestroyList() and then free the memory yourself.
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
 * @buf: The buffer to store the entries in
 * @buf_sz: The size of the given buffer in bytes
 *
 * Returns: 0 on success or -1 if an error occurred
 */
PA_API s8 paInitListFixed(struct pa_list *lst, s16 size, void *buf, s32 buf_sz);

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
 * Copy entries from one list to another. Note that both lists have to be
 * initialized already, with the same entry-size! The requested entries will
 * be copied over from the source to the destination. If the destination-list is
 * configured as static, only as many entries will be copied as will fit into
 * the given memory-space. If the list is configured as dynamic, it will scale
 * to fit all entries.
 *
 * @dst: Pointer to the destination-list to copy to
 * @src: Pointer to the source-list to copy from
 * @dst_off: The starting offset for the destination-list to start copying to
 * @src_off: The starting offset for the source-list to start copying from
 * @num: The number of entries to copy over
 *
 * Returns: The number of copied entries or -1 if an error occurred
 */
PA_API s16 paCopyList(struct pa_list *dst, struct pa_list *src,
                s16 dst_off, s16 src_off, s16 num);

/*
 * Get a pointer to the next entry in the list. For the first step pass NULL for
 * ptr.
 * Example on how to iterate through the whole list:
 * ...
 * struct pa_list list;
 * void *ptr = NULL;
 * ...
 * while((ptr = paIterateList(&lst, ptr))) {
 *      ..Do something with ptr..
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

/*
 * -----------------------------------------------------------------------------
 *
 *      Dictionary
 *
 * When pushing a new key-value-pair into the dictionary, the key is hashed and
 * the new entry consisting of the key and value memory is pushed with a next
 * value into the dictionary-buffer. The entry will then be put into one of the
 * buckets depending on the hash and attached to the last entry in the bucket by
 * updating it's next value.
 *
 * The dictionary-buffer looks like this:
 *
 *   ... <next><hash><key><value> <next><hash><key><value> ...
 *
 * The next-value is a short, the hash is an unsigned short, the key is defined
 * via PA_DICT_KEY_SIZE and the value-size is set during initialization.
 *
 * To mark a entry-slot as not-used the next is set to -1. To identify the
 * end of the bucket, the next is set to -(bucket_number + 2).
 *
 * To search for an entry in the dictionary, we again hash the key, determine
 * the right bucket and then jump from entry to entry to find the right one like
 * in a linked list.
 */

#define PA_DICT_NEXT_SIZE 2
#define PA_DICT_HASH_SIZE 2
#define PA_DICT_KEY_SIZE  32
#define PA_DICT_HEAD_SIZE (PA_DICT_NEXT_SIZE+PA_DICT_HASH_SIZE+PA_DICT_KEY_SIZE)
#define PA_DICT_BUCKETS   8

struct pa_dictionary {
        struct pa_memory *memory;
        enum pa_memory_mode mode;

        s32 value_size; /* The size of the value-part in bytes */
        s32 entry_size; /* The size of the header and value */

        s16 number;   /* Number of active entries in the dictionary */
        s16 alloc;    /* Number of slots of entries in the dictionary */  
        u8 *buffer;   /* Memory-buffer to store the entries */

        /* 
         * All buckets containing the index for their first entry in the
         * dictionary-buffer.
         */
        s16 buckets[PA_DICT_BUCKETS];
};

struct pa_dictionary_entry {
        char key[PA_DICT_KEY_SIZE];
        void *value;
};

/*
 * Initialize the dictionary, link to the memory-manager and preallocate the
 * specified number of entries.
 *
 * @dct: Pointer to the dictionary
 * @mem: Pointer to the memory-manager
 * @size: The size of the value-part in bytes
 * @alloc: The number of entries to preallocate
 *
 * Returns: 0 on success or -1 if an error occurred
 */
PA_API s8 paInitDictionary(struct pa_dictionary *dct, struct pa_memory *mem,
                s32 size, s16 alloc);

/*
 * Initialize the dictionary using static memory, and prepare the
 * dictionary-buffer.
 *
 * @dic: Pointer to the dictionary
 * @buffer: The buffer to use to store the entries
 * @buf_sz: The size of the buffer in bytes
 * @value_sz: The size of the value-part in bytes
 *
 * Returns: 0 on success or -1 if an error occurred
 */
PA_API s8 paInitDictionaryFixed(struct pa_dictionary *dct, void *buffer,
                s32 buf_sz, s32 value_sz);

/*
 * Destroy the dictionary, reset all attributes and if configured as dynamic,
 * free the allocated memory.
 *
 * @dct: Pointer to the dictionary
 */
PA_API void paDestroyDictionary(struct pa_dictionary *dct);

/*
 * Set a key-value-pair in the dictionary. If the key already exists then
 * overwrite it. If it does not yet exist, create it. This function will copy
 * over the memory into the buffer.
 *
 * @dct: Pointer to the dictionary
 * @key: Pointer to the key
 * @value: Pointer to the value
 *
 * Returns: Either 0 on success or -1 if an error occurred
 */
PA_API s8 paSetDictionary(struct pa_dictionary *dct, char *key, void *value);

/*
 * Retrieve an entry from the dictionary by searching for the key. To do so, the
 * key will be hashed and the proper bucket determined. Then the function will
 * go through all entries in the bucked until the requested entry is found.
 *
 * @dct: Pointer to the dictionary
 * @key: Pointer to the key to look for
 * @dst: A pointer to write the entry for the key to
 *
 * Returns: 1 if the entry has been found, 0 if not and -1 if an error occurred
 */
PA_API s8 paGetDictionary(struct pa_dictionary *dct, char *key, void *out);

/*
 * Remove an entry from the dictionary and open up the slot. The memory in the
 * entry will be lost!
 *
 * @dct: Pointer to the dictionary
 * @key: The key of the entry
 */
PA_API void paRemoveDictionary(struct pa_dictionary *dct, char *key);

/*
 * This function allows the iteration of every entry in every bucket. The basic
 * principal is passing a pointer to the current entry and getting the
 * entry-data and pointer for the next entry. Note that this function goes from
 * bucket to bucket, so it will not return the entries in respect to when they
 * have been added.
 *
 * Here's an example of how to use the function:
 * ...
 * struct pa_dictionary dict;
 * struct pa_dictionary_entry ent;
 * void *ptr = NULL;
 * ...
 * while((ptr = paIterateDictionary(&dict, ptr, &ent))) {
 *      ..Do something with the entry...
 * }
 * ...
 *
 * @dct: Pointer to the dictionary
 * @ptr: The runing pointer used to iterate(pass NULL at the start)
 * @ent: A pointer to write the entry data to
 *
 * Returns: The pointer for the current entry in the dictionary-buffer or NULL
 *          if this is the last entry in the dictionary
 */
PA_API void *paIterateDictionary(struct pa_dictionary *dct, void *ptr, 
                struct pa_dictionary_entry *ent);

/*
 * Iterate through all entries in a bucket.
 *
 * Here's an example of how to use the function to iterate through bucket 2:
 * ...
 * struct pa_dictionary dict;
 * struct pa_dictionary_entry ent;
 * void *ptr = NULL;
 * ...
 * while((ptr = paIterateDictionaryBucket(&dict, 2, ptr, &ent))) {
 *      ..Do something with the entry...
 * }
 * ...
 *
 * @dct: Pointer to the dictionary
 * @bucket: The bucket to iterate through
 * @ptr: The running pointer used to iterate(pass NULL at start)
 * @ent: A pointer to write the entry data to
 *
 * Returns: The pointer for the current entry in the dictionary-buffer or NULL
 *          if this is the last entry in the bucket
 */
PA_API void *paIterateDictionaryBucket(struct pa_dictionary *dct, s16 bucket,
                void *ptr, struct pa_dictionary_entry *ent);

/*
 * -----------------------------------------------------------------------------
 *
 *      TABLE
 *
 * When pushing a new key-value-pair into the table, the key is hashed and
 * the new entry consisting of the key and value memory is pushed with a next
 * value into the table-buffer. The entry will then be put into one of the
 * buckets depending on the hash and attached to the last entry in the bucket by
 * updating it's next value.
 *
 * The table-buffer looks like this:
 *
 *   ... <next><hash><key><value> <next><hash><key><value> ...
 *
 * The next-value is a short, the hash is an unsigned short, the key and the 
 * value-size is set during initialization.
 *
 * To mark a entry-slot as not-used the next is set to -1. To identify the
 * end of the bucket, the next is set to -(bucket_number + 2).
 *
 * To search for an entry in the dictionary, we again hash the key, determine
 * the right bucket and then jump from entry to entry to find the right one like
 * in a linked list.
 */


#define PA_TBL_NEXT_SIZE 2
#define PA_TBL_HASH_SIZE 2
#define PA_TBL_HEAD_SIZE (PA_TBL_NEXT_SIZE+PA_TBL_HASH_SIZE)
#define PA_TBL_BUCKETS   8

struct pa_table {
        struct pa_memory *memory;
        enum pa_memory_mode mode;

        s32 key_size;   /* The size of the key in bytes */
        s32 value_size; /* The size of the value-part in bytes */
        s32 entry_size; /* The size of the header, key and value in bytes */

        s16 number;   /* Number of active entries in the table */
        s16 alloc;    /* Number of slots of entries in the table */  
        u8 *buffer;   /* Memory-buffer to store the entries */

        /* 
         * All buckets containing the index for their first entry in the
         * table-buffer.
         */
        s16 buckets[PA_TBL_BUCKETS];
};

struct pa_table_entry {
        void *key;
        void *value;
};

/*
 * Initialize the table, link to the memory-manager and preallocate the
 * specified number of entries.
 *
 * @tbl: Pointer to the table
 * @mem: Pointer to the memory-manager
 * @key_sz: The size of the key in bytes
 * @value_sz: The size of the value in bytes
 * @alloc: The number of entries to preallocate
 *
 * Returns: 0 on success or -1 if an error occurred
 */
PA_API s8 paInitTable(struct pa_table *tbl, struct pa_memory *mem,
                s32 key_sz, s32 value_sz, s16 alloc);

/*
 * Initialize the table using static memory, and prepare the
 * table-buffer.
 *
 * @dic: Pointer to the table
 * @buffer: The buffer to use to store the entries
 * @buf_sz: The size of the buffer in bytes
 * @value_sz: The size of the value-part in bytes
 *
 * Returns: 0 on success or -1 if an error occurred
 */
PA_API s8 paInitTableFixed(struct pa_table *tbl, void *buffer,
                s32 buf_sz, s32 key_sz, s32 value_sz);

/*
 * Destroy the table, reset all attributes and if configured as dynamic,
 * free the allocated memory.
 *
 * @tbl: Pointer to the table
 */
PA_API void paDestroyTable(struct pa_table *tbl);

/*
 * Set a key-value-pair in the table. If the key already exists then
 * overwrite it. If it does not yet exist, create it. This function will copy
 * over the memory into the buffer.
 *
 * @tbl: Pointer to the table
 * @key: Pointer to the key
 * @value: Pointer to the value
 *
 * Returns: Either 0 on success or -1 if an error occurred
 */
PA_API s8 paSetTable(struct pa_table *tbl, void *key, void *value);

/*
 * Retrieve an entry from the table by searching for the key. To do so, the
 * key will be hashed and the proper bucket determined. Then the function will
 * go through all entries in the bucked until the requested entry is found.
 *
 * @tbl: Pointer to the table
 * @key: Pointer to the buffer containing the key
 * @dst: A pointer to write the entry for the key to
 *
 * Returns: 1 if the entry has been found, 0 if not and -1 if an error occurred
 */
PA_API s8 paGetTable(struct pa_table *tbl, void *key, void *out);

/*
 * Remove an entry from the table and open up the slot. The memory in the
 * entry will be lost!
 *
 * @tbl: Pointer to the table
 * @key: Pointer to a buffer containing the key
 */
PA_API void paRemoveTable(struct pa_table *tbl, void *key);

/*
 * This function allows the iteration of every entry in every bucket. The basic
 * principal is passing a pointer to the current entry and getting the
 * entry-data and pointer for the next entry. Note that this function goes from
 * bucket to bucket, so it will not return the entries in respect to when they
 * have been added.
 *
 * Here's an example of how to use the function:
 * ...
 * struct pa_table tbl;
 * struct pa_table_entry ent;
 * void *ptr = NULL;
 * ...
 * while((ptr = paIterateTable(&tbl, ptr, &ent))) {
 *      ..Do something with the entry...
 * }
 * ...
 *
 * @tbl: Pointer to the table
 * @ptr: The runing pointer used to iterate(pass NULL at the start)
 * @ent: A pointer to write the entry data to
 *
 * Returns: The pointer for the current entry in the table-buffer or NULL
 *          if this is the last entry in the table
 */
PA_API void *paIterateTable(struct pa_table *tbl, void *ptr, 
                struct pa_table_entry *ent);

/*
 * Iterate through all entries in a bucket.
 *
 * Here's an example of how to use the function to iterate through bucket 2:
 * ...
 * struct pa_table tbl;
 * struct pa_table_entry ent;
 * void *ptr = NULL;
 * ...
 * while((ptr = paIterateTableBucket(&tbl, 2, ptr, &ent))) {
 *      ..Do something with the entry...
 * }
 * ...
 *
 * @tbl: Pointer to the table
 * @bucket: The bucket to iterate through
 * @ptr: The running pointer used to iterate(pass NULL at start)
 * @ent: A pointer to write the entry data to
 *
 * Returns: The pointer for the current entry in the table-buffer or NULL
 *          if this is the last entry in the bucket
 */
PA_API void *paIterateTableBucket(struct pa_table *tbl, s16 bucket,
                void *ptr, struct pa_table_entry *ent);

/*
 * -----------------------------------------------------------------------------
 *
 *      FLEX
 *
 * The flex-helper is used to parse mathematical expressions for size like for
 * example "3px * 4 - 5em" and create flex-terms. A single parser is needed to
 * parse and process any amount of terms as it's just used for providing the
 * necessary resources for parsing and processing. A single flex-term is just a
 * container for an instance of a size-expression.
 */

#define PA_FLX_READ_BUFFER_SIZE        64

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

struct pa_flex_helper {
        struct pa_list          tokens;
        struct pa_list          swap;
        struct pa_list          values;
};

struct pa_flex {
        struct pa_flex_helper   *helper;
        struct pa_list          tokens;
};

struct pa_flex_reference {
        s32 relative;      /* Width/Height of the reference */
        s32 font;          /* Font-Size */
};

/*
 * Initialize a flex-helper using the dynamic funcitionalities of the
 * framework and preallocate the requeste number of tokens.
 *
 * @hlp: Pointer to the flex-helper
 * @mem: Pointer to the memory-manager
 * @token: The initial number of token to preallocate
 *
 * Returns: 0 on success or -1 if an error occurred
 */
PA_API s8 paInitFlexHelper(struct pa_flex_helper *hlp, struct pa_memory *mem,
                s16 tokens);

/*
 * Initialize a flex-helper using a static buffer. The flex-helper will then
 * calculate the maximum number of tokens that can be stored in the given
 * buffer. Any attempt to push tokens into the flex-helper when it has already
 * reached its limit will be ignored. Usually the token- and swap-buffers should
 * be the same size!
 *
 * @hlp: Pointer to the flex-helper
 * @tok_buf: Pointer to the buffer to use for storing tokens
 * @tok_buf_sz: The size of the given buffer
 * @swp_buf: The swap-buffer used when parsing the input-string
 * @swp_buf_sz: The size of the swap-buffer in bytes
 * @val_buf: The value-buffer used when processing a flex-term
 * @val_buf_sz: The size of the value-buffer in bytes
 *
 * Returns: 0 on success or -1 if an error occurred
 */
PA_API s8 paInitFlexHelperFixed(struct pa_flex_helper *hlp, 
                void *tok_buf, s32 tok_buf_sz, 
                void *swp_buf, s32 swp_buf_sz, 
                void *val_buf, s32 val_buf_sz);

/*
 * Destroy a flex-helper, reset all attributes and, if configured as dynamic,
 * free. the allocated memory. Use this function after use, even if the
 * flex-helper is configured as static.
 *
 * @hlp: Pointer to the flex-helper
 */
PA_API void paDestroyFlexHelper(struct pa_flex_helper *hlp);

/*
 * Clear all contents of a flex-helper and reset all internal lists. Everything
 * in the flex-helper will be lost!
 *
 * @hlp: Pointer to the flex-helper
 */
PA_API void paClearFlexHelper(struct pa_flex_helper *hlp);

/*
 * Dynamically initialize an expression into the flex-term using the
 * flex-helper. The requested number of token slots will be allocated during
 * initialization and if necessary the flex-term will scale to fit new tokens
 * beyond its original limits.
 *
 * @flx: Pointer to the flex-term
 * @hlp: Pointer to the flex-helper using for parsing and processing
 * @mem: Pointer to the memory-manager
 * @tokens: The number of token-slots to preallocate during initialization
 *
 * Returns: 0 on success or -1 if an error occurred
 */
PA_API s8 paInitFlex(struct pa_flex *flx, struct pa_flex_helper *hlp,
                struct pa_memory *mem, s16 tokens);

/*
 * Statically create a flex-term on top of the given memory buffer. The maximum
 * number of tokens is calculated from the size of the given buffer. If the
 * limit has been reached, all new incoming tokens will be ignored.
 *
 * @flx: Pointer to the flex-term
 * @hlp: Pointer to the flex-helper using for parsing and processing
 * @tok_buf: The token-buffer to store the tokens in
 * @tok_buf_sz: The size of the token-buffer in bytes
 *
 * Returns: 0 on success or -1 if an error occurred
 */
PA_API s8 paInitFlexFixed(struct pa_flex *flx, struct pa_flex_helper *hlp,
                void *tok_buf, s32 tok_buf_sz);

/*
 * Destroy a flex-term and if allocated dynamically free the allocated memory.
 * Call this function even if the flex-term has been created with static memory,
 * but don't forget to free the provided buffer yourself.
 *
 * @flx: Pointer to the flex-term
 */
PA_API void paDestroyFlex(struct pa_flex *flx);

/*
 * Clear the internal memory of the flex-term without freeing it. This will lose
 * all data contained in the flex-term, so use with caution.
 *
 * @flx: Pointer to the flex-term
 */
PA_API void paClearFlex(struct pa_flex *flx);

/*
 * IMPORTANT: Using this function will overwrite everything already in the
 * flex-term! Also check that both the parser and the term have both already
 * been initialized!
 *
 *
 * Parse an input-term by first tokenizing the input and then converting it to
 * postfix-notation using Dijkstra's shunting yard algortihm.
 * If the flex-handler is configured as static, the function will only write as
 * many tokens to the flex-handler, as the limit allows. If the flex-handler is
 * configured as dynamic, the token-list will be scaled according the the
 * incoming number of tokens.
 *
 * @hlp: Pointer to the the flex-helper
 * @str: A null-terminated string containing the size-expression
 * @flx: Pointer to the flex-term to parse into
 *
 * Returns: The number of written tokens or -1 if an error occurred
 */
PA_API s16 paParseFlex(struct pa_flex *flx, char *str);

/*
 * Process the flex-term using the given set of references.
 *
 * @flx: Pointer to the flex
 * @ref: Pointer to the set of references
 *
 * Returns: The resulting value of the expression
 */
PA_API s32 paProcessFlex(struct pa_flex *flx, struct pa_flex_reference *ref);

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
        char placeholder;
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
        char placeholder; 
};


/*
 * 
 */
struct pa_stylesheet {
        char placeholder;
};


/*
 * 
 */
struct pa_style {
        char placeholder;
};


/*
 * A class is a bundle of different style-attributes applies to all elements
 * tagged with the class-name.
 */
struct pa_class_style {
        char placeholder;
};

/*
 * The typeclass is a bundle of different style-attributes applied to all
 * elements with a given type.
 */
struct pa_type_style {
        char placeholder;
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
        char placeholder;
};

typedef s8 (*pa_event_callback)(struct pa_event evt, void *data);

/*  */
struct pa_event_handler {
        char placeholder;
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
        char placeholder;
};

/*
 * A batch contains multiple patches using the same material, ie. the same
 * transparency/opaquenes, same resources, same color, etc.
 */
struct pa_batch {
        char placeholder;
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

#ifdef PA_IMPLEMENTATION

#ifndef _PATCHY_INTERNAL_H
#define _PATCHY_INTERNAL_H




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
 * Compare two memory-buffers byte-by-byte.
 *
 * @ptr1: Pointer for the first memory-buffer
 * @ptr2: Pointer to the second memory-buffer
 *
 * Returns: 1 if the buffers are equal and 0 if not
 */
PA_LIB s8 pa_mem_compare(void *ptr1, void *ptr2, s32 size);

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
 * Remove all characters from the string, move all characters so they're
 * consecutive and update the null-terminator. All modifications will be applied
 * to the given string-buffer. To preserve the original state, duplicate the
 * buffer. The original pointer for the string-buffer will not be lost.
 *
 * @str: Pointer to the null-terminated string to strip
 *
 * Returns: A pointer to the updated string
 */
PA_LIB char *pa_strip(char *str);

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




#include <stdio.h>

/*
 * -----------------------------------------------------------------------------
 *
 *      STRING
 *
 */

/*
 * The number of trailing bytes for every byte in a UTF8-encoded sequence.
 * To get the total size of a sequence, get the trailing bytes and add one.
 *
 * Binary    Hex         Comments
 * 0xxxxxxx  0x00..0x7F  Only byte of a 1-byte character encoding
 * 10xxxxxx  0x80..0xBF  Continuation byte: one of 1-3 bytes following the first
 * 110xxxxx  0xC0..0xDF  First byte of a 2-byte character encoding
 * 1110xxxx  0xE0..0xEF  First byte of a 3-byte character encoding
 * 11110xxx  0xF0..0xF7  First byte of a 4-byte character encoding
 */
PA_INTERN const char str_trailing_bytes[256] = {
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};

PA_INTERN const u32 str_offset_utf8[6] = {
        0x00000000UL, 0x00003080UL, 0x000E2080UL,
        0x03C82080UL, 0xFA082080UL, 0x82082080UL
};

PA_INTERN s16 str_sequence_size(char *s)
{
        return str_trailing_bytes[*(u8 *)s] + 1;
}

PA_INTERN u32 str_next(char *s, s32 *off)
{
        u32 ch = 0;
        s32 sz = 0;

        do {
                ch <<= 6;
                ch += (u8)s[(*off)++];
                sz++;
        } while(s[*off] && !PA_ISUTF(s[*off]));

        ch -= str_offset_utf8[sz-1];
        return ch;
}

PA_INTERN s16 str_length(char *s)
{
        s16 count = 0;
        s32 i = 0;

        if(s == NULL || pa_strlen(s) == 0)
                return 0;

        while(str_next(s, &i) != 0)
                count++;

        return count;
}

/* charnum => byte offset */
PA_INTERN s32 str_offset(char *s, s16 charnum)
{
        s16 offs = 0;

        while(charnum > 0 && s[offs]) {
                (void)(PA_ISUTF(s[++offs]) || PA_ISUTF(s[++offs]) ||
                                PA_ISUTF(s[++offs]) || ++offs);
                charnum--;
        }

        return offs;
}

/* byte offset => charnum */
PA_INTERN s16 str_charnum(char *s, s32 offset)
{
        s16 charnum = 0;
        s32 offs = 0;

        while(offs < offset && s[offs]) {
                (void)(PA_ISUTF(s[++offs]) || PA_ISUTF(s[++offs]) ||
                                PA_ISUTF(s[++offs]) || ++offs);
                charnum++;
        }

        return charnum;
}

PA_INTERN void str_inc(char *s, int *i)
{
        (void)(PA_ISUTF(s[++(*i)]) || PA_ISUTF(s[++(*i)]) ||
                        PA_ISUTF(s[++(*i)]) || ++(*i));
}

PA_INTERN void str_dec(char *s, int *i)
{
        (void)(PA_ISUTF(s[--(*i)]) || PA_ISUTF(s[--(*i)]) || 
                        PA_ISUTF(s[--(*i)]) || --(*i));
}

PA_INTERN void str_ensure_fit(struct pa_string *str, s32 size)
{
        s32 new_alloc;
        void *p;

        if(size < 1)
                return;

        if(str->size + size + 1 > str->alloc && str->mode == PA_DYNAMIC) {
                new_alloc = (str->size + size) * 1.5;
                if(!(p = pa_mem_alloc(str->memory, str->buffer, new_alloc))) {
                        return;
                }
                str->buffer = p;
                str->alloc = new_alloc;
        }
}



PA_API s8 paInitString(struct pa_string *str, struct pa_memory *mem)
{
        str->memory = mem;
        str->mode = PA_DYNAMIC;

        str->length = 0;
        str->size = 0;
        str->alloc = PA_STRING_INITIAL_SIZE;

        if(!(str->buffer = pa_mem_alloc(str->memory, NULL, str->alloc))) {
                return -1;
        }
        *str->buffer = 0;

        return 0;
}

PA_API s8 paInitStringFixed(struct pa_string *str, void *buf, s32 alloc)
{
        str->memory = NULL;
        str->mode = PA_FIXED;

        str->buffer = buf;
        *str->buffer = 0;

        str->length = 0;
        str->size = 0;
        str->alloc = alloc;
        return 0;
}

PA_API void paDestroyString(struct pa_string *str)
{
        if(str->mode == PA_DYNAMIC) {
                pa_mem_free(str->memory, str->buffer);
        }

        str->buffer = NULL;
        str->length = 0;
        str->size = 0;
        str->alloc = 0;
}

PA_API s16 paWriteString(struct pa_string *str, char *src, s16 off, s16 num)
{
        s32 free_size;
        s32 read_sz;
        s32 write_off;
        s32 write_sz;
        s32 run;
        s16 count;

        s16 overlap;
        s32 overlap_sz;

        s32 trail_sz;

        /* First we figure out how many bytes should be read from the source */
        if(num == PA_ALL) {
                read_sz = pa_strlen(src);
                num = str_charnum(src, read_sz);
        }
        else {
                read_sz = str_offset(src, num);
        }

        /* Second, we figure out what the offset should be */
        off = off == PA_END ? str->length : off;
        write_off = paGetStringOffset(str, off);

        /* Validate input parameters */
        if(read_sz < 1) return 0;
        if(off < 0 || off > str->length) return 0;

        /* Next we figure out the overlap */
        overlap = PA_OVERLAP(0, str->length, off, num);
        overlap_sz = str_offset(str->buffer + write_off, overlap);  

        /* Scale the string-buffer to fit the new characters */
        str_ensure_fit(str, read_sz - overlap_sz);

        /* 
         * Determine how much free memory is left (subtract 1 for the
         * null-terminator).
         */
        free_size = str->alloc - str->size - 1 + overlap_sz;


        /* Now we have to figure out how many characters can actually we written
         * to the string buffer.
         */
        run = 0;
        count = 0;
        write_sz = 0;
        while(str_next(src, &run) && run <= free_size && run <= read_sz) {
                write_sz = run;
                count++;
        }
        num = count;

        /* 
         * Before we can actually copy to the string buffer, we have to move the
         * trailing characters.
         */
        trail_sz = str->size - (write_off + overlap_sz);
        if(trail_sz > 0) {
                pa_mem_move(str->buffer + write_off + write_sz, 
                                str->buffer + (str->size - trail_sz),
                                trail_sz);
        }

        /* Copy from source to the string buffer */
        pa_mem_copy(str->buffer + write_off, src, write_sz);

        /* Update string byte-size and set null-terminator */
        str->size += write_sz - overlap_sz;
        str->length += num - overlap;
        str->buffer[str->size] = 0;

        /* Return number of written bytes */
        return num;
}

PA_API s16 paInsertString(struct pa_string *str, char *src, s16 off, s16 num)
{
        s32 free_size;
        s16 read_num;
        s32 read_sz;
        s32 write_off;
        s32 write_sz;
        s32 run;
        s32 move_off;
        s32 move_sz;
        s16 count;

        /* First we figure out how many bytes should be read from the source */
        if(num == PA_ALL) {
                read_sz = pa_strlen(src);
                read_num = str_charnum(src, read_sz);
        }
        else {
                read_sz = str_offset(src, num);
                read_num = num;
        }

        /* Second, we figure out what the offset should be */
        off = off == PA_END ? str->length : off;
        write_off = str_offset(str->buffer, off);

        /* If the string is dynamic, scale the buffer to fit new characters */
        str_ensure_fit(str, read_sz);

        /* 
         * Then we determine how much free memory is actually left in the
         * string(also subtract 1 for the null-terminator).
         */
        free_size = str->alloc - str->size - 1;

        /* Now we have to figure out how many characters can actually we written
         * to the string buffer.
         */
        run = 0;
        count = 0;
        while(str_next(src, &run) && run <= free_size && run <= read_sz) {
                write_sz = run;
                count++;
        }
        read_num = count;

        /* 
         * Determine the size of the trailing block which has to be moved and
         * move it.
         */
        move_sz = str->size - write_off;
        if(move_sz > 0) {
                move_off = write_off + write_sz;
                pa_mem_move(str->buffer + move_off, str->buffer + write_off,
                                move_sz);
        }

        /* Copy from source to the string buffer */
        pa_mem_copy(str->buffer + write_off, src, write_sz);

        /* Update string byte-size and set null-terminator */
        str->size += write_sz;
        str->length += read_num;
        str->buffer[str->size] = 0;

        /* Return number of written bytes */
        return num;
}

PA_API s16 paCopyString(struct pa_string *str, char *dst, s16 off,
                s16 num, s32 lim)
{
        s32 trail;
        s32 copy_sz;
        s32 copy_off;
        s16 count;

        if(off >= str->length) return 0;

        /* Resolve input parameters */
        trail = str->length - off;
        num = num == PA_ALL ? trail : num;
        num = num > trail ? trail : num;

        /* Calculate the size and offset for the characters in the string */
        copy_off = str_offset(str->buffer, off);
        copy_sz = str_offset(str->buffer + copy_off, num);

        /* Reduce character count to keep in the limit */
        count = 0;
        while(copy_sz > lim) {
                str_dec(str->buffer, &copy_sz);
                count++;
        }
        num -= count;

        /* Copy over the characters and set null-terminator in output-buffer */
        pa_mem_copy(dst, str->buffer + copy_off, copy_sz);
        dst[copy_sz] = 0;

        /* Return the number of copied characters */
        return num;
}

PA_API s16 paReadString(struct pa_string *str, char *dst, s16 off,
                s16 num, s32 lim)
{
        s16 trail;
        s32 read_off;
        s32 read_sz;
        s32 move_off;
        s32 move_sz;
        s16 count;

        if(off >= str->length) return 0;

        /* Resolve input parameters */
        trail = str->length - off;
        num = num == PA_ALL ? trail : num;
        num = num > trail ? trail : num;

        /* Calculate the size and offset for the characters in the string */
        read_off = str_offset(str->buffer, off);
        read_sz = str_offset(str->buffer + read_off, num);

        /* Reduce character count to keep in the limit */
        count = 0;
        while(read_sz > lim) {
                str_dec(str->buffer, &read_sz);
                count++;
        }
        num -= count;

        /* Copy over the characters and set null-terminator in output-buffer */
        pa_mem_copy(dst, str->buffer + read_off, read_sz);
        dst[read_sz] = 0;

        /* Calculate the size and new offset for the trailing block */
        move_sz = str->size - read_off;
        move_off = read_off + read_sz;

        /* Move the trailing block forward to fill the gap */
        pa_mem_move(str->buffer + read_off, str->buffer + move_off,
                        move_sz);

        /* Update string byte-size and set null-terminator */
        str->size -= read_sz;
        str->length -= num;
        str->buffer[str->size] = 0;

        /* Return the number of read bytes */
        return num;
}

PA_API s16 paGetStringCharacter(struct pa_string *str, s32 off)
{
        s16 charnum = 0;
        s16 offs = 0;
        char *s = str->buffer;

        while(offs < off && s[offs]) {
                (void)(PA_ISUTF(s[++offs]) || PA_ISUTF(s[++offs]) ||
                                PA_ISUTF(s[++offs]) || ++offs);
                charnum++;
        }

        return charnum;
}

PA_API s32 paGetStringOffset(struct pa_string *str, s16 cnum)
{
        s32 offs = 0;
        char *s = str->buffer;

        while(cnum > 0 && s[offs]) {
                (void)(PA_ISUTF(s[++offs]) || PA_ISUTF(s[++offs]) ||
                                PA_ISUTF(s[++offs]) || ++offs);
                cnum--;
        }

        return offs;
}

PA_API u32 paNextStringChar(struct pa_string *str, s32 *off)
{
        return str_next(str->buffer, off);
}

PA_API char *paIterateString(struct pa_string *str, char *chr)
{
        if(chr == NULL)
                return str->buffer;

        if(*chr == 0)
                return NULL;

        return chr + str_sequence_size(chr);
}

/*
 * -----------------------------------------------------------------------------
 *
 *      LIST
 *
 */

PA_INTERN void lst_ensure_fit(struct pa_list *lst, s32 num)
{
        s32 new_num = lst->count + num;
        s32 new_alloc;
        s32 new_size;
        void *p;

        if(lst->mode != PA_DYNAMIC)
                return;

        if(new_num < lst->alloc)
                return;

        new_alloc = new_num * 1.5; 
        new_size = new_alloc * lst->entry_size;

        if(lst->limit > 0) {
                new_size = PA_MIN(new_size, lst->limit);

                if(new_size == lst->alloc_size)
                        return;
        }


        if(!(p = pa_mem_alloc(lst->memory, lst->data, new_size)))
                return;

        lst->data = p;
        lst->alloc = new_alloc;
        lst->alloc_size = new_size;
}

PA_LIB s8 paInitList(struct pa_list *lst, struct pa_memory *mem, 
                s32 size, s16 alloc, s32 lim)
{
        lst->memory = mem;
        lst->mode = PA_DYNAMIC;
        lst->entry_size = size;
        lst->count = 0;
        lst->alloc = alloc;
        lst->alloc_size = lst->alloc * lst->entry_size;
        lst->limit = lim;

        if(!(lst->data = pa_mem_alloc(lst->memory, NULL, lst->alloc_size))) {
                return -1;
        }

        paClearList(lst);
        return 0;
}

PA_LIB s8 paInitListFixed(struct pa_list *lst, s16 size, void *buf, s32 buf_sz)
{
        lst->memory = NULL;
        lst->mode = PA_FIXED;
        lst->entry_size = size;
        lst->count = 0;
        lst->alloc = buf_sz / size; /* Calculate the number of usable slots */
        lst->alloc_size = buf_sz;
        lst->data = buf;
        lst->limit = PA_NOLIM;

        paClearList(lst);
        return 0;
}

PA_LIB void paDestroyList(struct pa_list *lst)
{
        if(lst->mode == PA_DYNAMIC) {
                pa_mem_free(lst->memory, lst->data);
        }
}

PA_LIB void paClearList(struct pa_list *lst)
{
        pa_mem_set(lst->data, 0, lst->alloc * lst->entry_size);

        lst->count = 0;
}

PA_LIB s16 paPushList(struct pa_list *lst, void *src, s16 num)
{
        s16 open_slots;
        s16 entry_number;
        s32 offset;
        s32 size;

        /* If configured as dynamic, scale to fit new entries */
        lst_ensure_fit(lst, num);

        /* Figure out how many entries can be written to the list */
        open_slots = lst->alloc - lst->count;
        entry_number = num > open_slots ? open_slots : num;

        /* Copy entries to list */
        offset = lst->count * lst->entry_size;
        size = entry_number * lst->entry_size;
        pa_mem_copy(lst->data + offset, src, size);

        /* Update number of entries in list and return number of written */
        lst->count += entry_number;
        return entry_number;
}

PA_LIB s16 paPopList(struct pa_list *lst, void *dst, s16 num)
{
        s16 entry_number;
        s32 size;
        s32 offset;

        /* Figure out how many entries can be returned */
        entry_number = num > lst->count ? lst->count : num;

        /* Copy entries from list */
        size = entry_number * lst->entry_size;
        offset = (lst->count * lst->entry_size) - size;
        pa_mem_copy(dst, lst->data + offset, size);

        /* Update the number of entries in the list and return */
        lst->count -= entry_number;
        return entry_number;
}

PA_LIB s16 paUnshiftList(struct pa_list *lst, void *src, s16 num)
{
        s16 open_slots;
        s16 entry_number;
        s32 size;
        s32 move_sz;

        /* If configured as dynamic, scale to fit new entries */
        lst_ensure_fit(lst, num);

        /* Figure out how many entries can be written to the list */
        open_slots = lst->alloc - lst->count;
        entry_number = num > open_slots ? open_slots : num;
        size = entry_number * lst->entry_size;

        /* Move all entries back to make space at the beginning */
        move_sz = lst->count * lst->entry_size;
        pa_mem_move(lst->data + size, lst->data, move_sz);

        /* Now copy over the data from the source */
        pa_mem_copy(lst->data, src, size);

        /* Update the number of entries in the list and return */
        lst->count += entry_number;
        return entry_number;
}

PA_LIB s16 paShiftList(struct pa_list *lst, void *dst, s16 num)
{
        s16 entry_number;
        s16 entry_left;
        s32 size;
        s32 offset;

        /* Figure out how many entries can be returned */
        entry_number = num > lst->count ? lst->count : num;

        /* Copy entries from list */
        size = entry_number * lst->entry_size;
        pa_mem_copy(dst, lst->data, size);

        /* Move the rest of the entries forward to close the gap */
        entry_left = lst->count - entry_number;
        if(entry_left > 0) {
                offset = size;
                size = entry_left * lst->entry_size;
                pa_mem_move(lst->data, lst->data + offset, size);
        }

        /* Update the number of entries in the list and return */
        lst->count -= entry_number;
        return entry_number;
}

PA_LIB s16 paInsertList(struct pa_list *lst, void *src, s16 start, s16 num)
{
        s16 open_slots;
        s16 entry_number;
        s32 offset;
        s32 size;
        s32 move_off;
        s32 move_sz;

        if(start == PA_END) {
                start = lst->count;
        }

        /* If configured as dynamic, scale to fit new entries */
        lst_ensure_fit(lst, num);

        /* Figure out how many entries can be inserted into the list */
        open_slots = lst->alloc - lst->count;
        entry_number = num > open_slots ? open_slots : num;
        size = entry_number * lst->entry_size;
        offset = start * lst->entry_size;

        /* Move entries back to make space */
        move_sz = (lst->count - start) * lst->entry_size;
        move_off = (lst->count * lst->entry_size) - move_sz;
        pa_mem_move(lst->data + move_off, lst->data + offset, move_sz);

        /* Copy over the entries from the source */
        pa_mem_copy(lst->data + offset, src, size);

        /* Update the number of entries in the list and return */
        lst->count += entry_number;
        return entry_number;
}

PA_LIB s16 paPeekList(struct pa_list *lst, void *dst, s16 start, s16 num)
{
        s16 entry_number;
        s16 entry_left;
        s32 offset;
        s32 size;

        /* Figure out how many entries can actually be returned */
        entry_left = lst->count - start;
        entry_number = num > entry_left ? entry_left : num;

        /* Copy over the entries */
        offset = start * lst->entry_size;
        size = entry_number * lst->entry_size;
        pa_mem_copy(dst, lst->data + offset, size);

        /* Return the number of returned entries */
        return entry_number;
}

PA_LIB s16 paGetList(struct pa_list *lst, void *dst, s16 start, s16 num)
{
        s16 entry_left;
        s16 entry_number;
        s32 offset;
        s32 size;
        s32 move_off;
        s32 move_sz;

        /* Figure out how many entries can actually be returned */
        entry_left = lst->count - start;
        entry_number = num > entry_left ? entry_left : num;

        /* Copy over the entries */
        offset = start * lst->entry_size;
        size = entry_number * lst->entry_size;
        pa_mem_copy(dst, lst->data + offset, size);

        /* Move back the entries to fill the gap */
        move_off = offset + size;
        move_sz = (lst->count - (start + entry_number)) * lst->entry_size;
        pa_mem_move(lst->data + offset, lst->data + move_off, move_sz);

        /* Update the number of entries in the list and return */
        lst->count -= entry_number;
        return entry_number;
}

PA_API s16 paCopyList(struct pa_list *dst, struct pa_list *src,
                s16 dst_off, s16 src_off, s16 num)
{
        s16 read_num;
        s16 write_num;
        s32 read_off;
        s32 write_free;
        s32 write_off;
        s32 size;
        s16 overlap;

        /* Validate input parameters */
        if(src_off < 0 || src_off >= src->count) return -1;
        if(dst_off < -1 || dst_off > dst->count) return -1;
        if(src->entry_size != dst->entry_size) return -1;

        /* Resolve parameters */
        dst_off = dst_off == PA_END ? dst->count : dst_off;
        num = num == PA_ALL ? (src->count - src_off) : num;

        /* Figure out the potential overlap in the destination-list */
        overlap = PA_OVERLAP(0, dst->count, dst_off, num);

        /* Scale to fit new entries */
        lst_ensure_fit(dst, num - overlap);

        /* Calculate how many entries can be actually be copied */
        read_num = src->count - src_off;
        write_free = (dst->alloc - dst->count) + overlap;
        num = PA_MIN(num, PA_MIN(read_num, write_free));

        /* Copy over the entries to the source */
        size = src->entry_size * num;
        read_off = src_off * src->entry_size;
        write_off = dst_off * src->entry_size;
        pa_mem_copy(dst->data + write_off, src->data + read_off, size);

        /* Update the number of entries in the destionation-list and return */
        dst->count += num - overlap;
        return num;
}

PA_API void *paIterateList(struct pa_list *lst, void *ptr)
{
        /* First step, set pointer to beginning of data buffer */
        if(ptr == NULL) {
                return lst->data;
        }

        /* Increment the pointer */
        ptr = (void *)((u8 *)ptr + lst->entry_size);

        /* Check if the pointer has reached the end of the line */
        if((u8 *)ptr >= lst->data + (lst->count * lst->entry_size)) {
                return NULL;
        }

        return ptr;
}

PA_API void paApplyList(struct pa_list *lst, pa_list_func fnc, void *pass)
{
        s32 i;
        struct pa_handle hdl;


        for(i = 0; i < lst->count; i++) {
                hdl.pointer = lst->data + (i * lst->entry_size);
                hdl.index = i;
                if(fnc(&hdl, pass)) return;
        }
}

PA_API void paApplyListBack(struct pa_list *lst, pa_list_func fnc, void *pass)
{
        s32 i;
        struct pa_handle hdl;


        for(i = lst->count - 1; i >= 0; i--) {
                hdl.pointer = lst->data + (i * lst->entry_size);
                hdl.index = i;
                if(fnc(&hdl, pass)) return;
        }
}

/*
 * -----------------------------------------------------------------------------
 *
 *      DICTIONARY
 *
 */

/* Source: https://stackoverflow.com/a/21001712 */
PA_INTERN u16 dct_hash(char *key)
{
        u32 byte;
        u32 crc;
        u32 mask;
        s32 i = 0;
        s32 j;

        crc = 0xFFFFFFFF;
        while(key[i] != 0) {
                byte = key[i];
                crc = crc ^ byte;
                for (j = 7; j >= 0; j--) {
                        mask = -(crc & 1);
                        crc = (crc >> 1) ^ (0xEDB88320 & mask);
                }
                i++;
        }
        return (~crc) % 0xFFFF;
}

PA_INTERN s16 dct_find_open(struct pa_dictionary *dct)
{
        s16 i;

        for(i = 0; i < dct->alloc; i++) {
                if(*(s16 *)(dct->buffer + i * dct->entry_size) == -1)
                        return i;
        }

        return -1;
}

PA_INTERN u8 *dct_next_bucket(struct pa_dictionary *dct, s16 bucket, u8 *ptr)
{
        s16 next_idx = -1;

        if(dct->buckets[bucket] < 0)
                return NULL;

        /*
         * On the first call ptr will be NULL, so we have to find the first
         * entry in the dictionary.
         */
        if(!ptr) {
                return dct->buffer + (dct->buckets[bucket] * dct->entry_size);
        }

        /*
         * Otherwise we have to get a pointer to the next entry.
         */
        if((next_idx = *(s16 *)ptr) < 0) {
                return NULL;
        }

        /* Return a pointer to the entry */
        return dct->buffer + (next_idx * dct->entry_size);


}

PA_INTERN u8 *dct_next(struct pa_dictionary *dct, u8 *ptr)
{
        s16 idx = -1;
        s16 next_idx = -1;
        s16 bucket_num;

        /*
         * On the first call ptr will be NULL, so we have to find the first
         * entry in the dictionary.
         */
        if(!ptr) {
                bucket_num = 0;
                while(bucket_num < PA_DICT_BUCKETS) {
                        if(dct->buckets[bucket_num] >= 0) {
                                idx = dct->buckets[bucket_num];
                                ptr = dct->buffer + (idx * dct->entry_size);
                                break;
                        }
                        bucket_num++;
                }

                return ptr;
        }

        /*
         * Otherwise we have to get a pointer to the next entry.
         */
        next_idx = *(s16 *)ptr;
        if(next_idx <= -2) {
                bucket_num = (-1 * next_idx) - 2 + 1;
                while(bucket_num < PA_DICT_BUCKETS) {
                        if(dct->buckets[bucket_num] >= 0) {
                                next_idx = dct->buckets[bucket_num];
                                break;
                        }
                        bucket_num++;
                }

                /* No more entries left in the dictionary */
                if(next_idx <= -2)
                        return NULL;
        }

        /* Return a pointer to the entry */
        return dct->buffer + (next_idx * dct->entry_size);
}

PA_INTERN u8 *dct_find_key(struct pa_dictionary *dct, char *key, u8 **prev,
                s16 *bucket_out)
{
        u16 hash = dct_hash(key);
        s16 bucket = hash % PA_DICT_BUCKETS;
        u8 *ptr = NULL;
        s16 off = PA_DICT_NEXT_SIZE + PA_DICT_HASH_SIZE;

        if(dct->buckets[bucket] < 0)
                return NULL;

        if(bucket_out) *bucket_out = bucket;

        if(prev) *prev = NULL;
        while((ptr = dct_next_bucket(dct, bucket, ptr))) {
                /* Compare the hashes */
                if(hash == *(u16 *)(ptr + PA_DICT_NEXT_SIZE)) {
                        if(pa_strcmp(key, (char *)(ptr + off)) == 0) {
                                return ptr;
                        }
                }
                if(prev) *prev = ptr;
        }

        return NULL;
}

PA_API s8 paInitDictionary(struct pa_dictionary *dct, struct pa_memory *mem,
                s32 size, s16 alloc)
{
        s32 alloc_sz;
        s16 i;

        dct->memory = mem;
        dct->mode = PA_DYNAMIC;

        dct->value_size = size;
        dct->entry_size = PA_DICT_HEAD_SIZE + size;
        dct->number = 0;
        dct->alloc = alloc;

        alloc_sz = dct->entry_size * dct->alloc;
        if(!(dct->buffer = pa_mem_alloc(dct->memory, NULL, alloc_sz))) {
                return -1;
        }

        /* Reset all memory slots in the buffer */
        for(i = 0; i < dct->alloc; i++) {
                *(s16 *)(dct->buffer + i * dct->entry_size) = -1;
        }

        /* Reset all buckets */
        for(i = 0; i < PA_DICT_BUCKETS; i++) {
                dct->buckets[i] = -1;
        }

        return 0;
}

PA_API s8 paInitDictionaryFixed(struct pa_dictionary *dct, void *buffer,
                s32 buf_sz, s32 value_sz)
{
        s16 i;

        dct->memory = NULL;
        dct->mode = PA_FIXED;

        dct->value_size = value_sz;
        dct->entry_size = PA_DICT_HEAD_SIZE + value_sz;
        dct->number = 0;
        dct->buffer = buffer;

        dct->alloc = buf_sz / dct->entry_size;

        /* Reset all entry-slots in the buffer */
        for(i = 0; i < dct->alloc; i++) {
                *(s16 *)(dct->buffer + i * dct->entry_size) = -1;
        }

        /* Reset all buckets */
        for(i = 0; i < PA_DICT_BUCKETS; i++) {
                dct->buckets[i] = -1;
        }

        return 0;
}

PA_API void paDestroyDictionary(struct pa_dictionary *dct)
{
        if(dct->mode == PA_DYNAMIC) {
                pa_mem_free(dct->memory, dct->buffer);
        }

        dct->memory = 0;
        dct->mode = PA_MEM_UDEF;

        dct->value_size = 0;
        dct->entry_size = 0;
        dct->number = 0;
        dct->alloc = 0;
        dct->buffer = 0;
}

PA_API s8 paSetDictionary(struct pa_dictionary *dct, char *key, void *value)
{
        s16 slot;
        u16 hash;
        s16 bucket;
        u8 *ptr;
        s32 tmp;
        s16 next_idx;


        /* Validate the input parameters */
        if(pa_strlen(key) > 32)
                return -1;

        /*
         * If there is already an entry with the keyword in the dictionary, we
         * can just overwrite it's value and return.
         */
        if((ptr = dct_find_key(dct, key, NULL, NULL))) {
                /* Copy over the content */
                pa_mem_copy(ptr + PA_DICT_HEAD_SIZE, value, dct->value_size);
                return 0;

        }

        /* Get an open slot in the dictionary-buffer */
        if((slot = dct_find_open(dct)) < 0)
                return -1;

        /* Hash the key and determine the bucket  */
        hash = dct_hash(key); 
        bucket = hash % PA_DICT_BUCKETS;

        /* 
         * Write the copy over the data to the dictionary-buffer.
         */

        /* Set the next index */
        ptr = dct->buffer + (slot * dct->entry_size);
        *(s16 *)ptr = -(bucket + 2);
        ptr += PA_DICT_NEXT_SIZE;

        /* Copy the hash */
        *(u16 *)ptr = hash;
        ptr += PA_DICT_HASH_SIZE;

        /* Copy over the keyword */
        pa_strcpy((char *)ptr, key);
        ptr += PA_DICT_KEY_SIZE;

        /* Copy over the content */
        pa_mem_copy(ptr, value, dct->value_size);

        /*
         * Attach the entry to the bucket.
         */
        if(dct->buckets[bucket] < 0) {
                dct->buckets[bucket] = slot;
        }
        else {
                tmp = dct->buckets[bucket] * dct->entry_size;
                ptr = dct->buffer + tmp;
                while((next_idx = *(s16 *)ptr) >= 0) {
                        ptr = dct->buffer + next_idx * dct->entry_size;
                }
                *(s16 *)ptr = slot;
        }

        dct->number++;
        return 0;
}

PA_API s8 paGetDictionary(struct pa_dictionary *dct, char *key, void *out)
{
        u8 *ptr;

        if(!(ptr = dct_find_key(dct, key, NULL, NULL)))
                return 0;

        pa_mem_copy(out, ptr + PA_DICT_HEAD_SIZE, dct->entry_size);
        return 1;
}

PA_API void paRemoveDictionary(struct pa_dictionary *dct, char *key)
{
        s16 bucket;
        u8 *ptr;
        u8 *prev;
        s16 next_idx;

        if((ptr = dct_find_key(dct, key, &prev, &bucket))) {
                next_idx = *(s16 *)ptr;

                if(!prev && next_idx < 0) {
                        dct->buckets[bucket] = -1;
                }
                else if(!prev && next_idx >= 0) {
                        dct->buckets[bucket] = next_idx;
                }
                else if(prev) {
                        *(s16 *)prev = *(s16 *)ptr;
                }

                *(s16 *)ptr = -1;
                dct->number--;
        }
}

PA_API void *paIterateDictionary(struct pa_dictionary *dct, void *ptr, 
                struct pa_dictionary_entry *ent)
{
        char *tmp;

        /* Check if the dictionary is empty */
        if(dct->number < 1)
                return NULL;

        /* Go to the next entry in the dictionary */
        if(!(ptr = dct_next(dct, ptr)))
                return NULL;

        /* Return the entry-data for the current entry */
        if(ent) {
                /* Copy over the key */
                tmp = (char *)ptr + PA_DICT_NEXT_SIZE + PA_DICT_HASH_SIZE;
                pa_strcpy(ent->key, tmp);

                /* Set the pointer for the data */
                ent->value = tmp + PA_DICT_KEY_SIZE;
        }

        return ptr;
}

PA_API void *paIterateDictionaryBucket(struct pa_dictionary *dct, s16 bucket,
                void *ptr, struct pa_dictionary_entry *ent)
{
        char *tmp;

        /* Check if the dictionary is empty */
        if(dct->number < 1)
                return NULL;

        if(!(ptr = dct_next_bucket(dct, bucket, ptr)))
                return NULL;

        /* Return the data for the current entry */
        if(ent) {
                /* Copy over the key */
                tmp = (char *)ptr + PA_DICT_NEXT_SIZE + PA_DICT_HASH_SIZE;
                pa_strcpy(ent->key, tmp);

                /* Set the pointer for the data */
                ent->value = tmp + PA_DICT_KEY_SIZE;
        }

        return ptr;
}

/*
 * -----------------------------------------------------------------------------
 *
 *      TABLE
 *
 */

/* Source: https://stackoverflow.com/a/21001712 */
PA_INTERN u16 tbl_hash(u8 *key, s32 key_sz)
{
        u32 byte;
        u32 crc;
        u32 mask;
        s32 i = 0;
        s32 j;

        crc = 0xFFFFFFFF;
        while(i < key_sz && key[i] != 0) {
                byte = key[i];
                crc = crc ^ byte;
                for (j = 7; j >= 0; j--) {
                        mask = -(crc & 1);
                        crc = (crc >> 1) ^ (0xEDB88320 & mask);
                }
                i++;
        }
        return (~crc) % 0xFFFF;
}

PA_INTERN s16 tbl_find_open(struct pa_table *tbl)
{
        s16 i;

        for(i = 0; i < tbl->alloc; i++) {
                if(*(s16 *)(tbl->buffer + i * tbl->entry_size) == -1)
                        return i;
        }

        return -1;
}

PA_INTERN u8 *tbl_next_bucket(struct pa_table *tbl, s16 bucket, u8 *ptr)
{
        s16 next_idx = -1;

        if(tbl->buckets[bucket] < 0)
                return NULL;

        /*
         * On the first call ptr will be NULL, so we have to find the first
         * entry in the table.
         */
        if(!ptr) {
                return tbl->buffer + (tbl->buckets[bucket] * tbl->entry_size);
        }

        /*
         * Otherwise we have to get a pointer to the next entry.
         */
        if((next_idx = *(s16 *)ptr) < 0) {
                return NULL;
        }

        /* Return a pointer to the entry */
        return tbl->buffer + (next_idx * tbl->entry_size);
}

PA_INTERN u8 *tbl_next(struct pa_table *tbl, u8 *ptr)
{
        s16 idx = -1;
        s16 next_idx = -1;
        s16 bucket_num;

        /*
         * On the first call ptr will be NULL, so we have to find the first
         * entry in the table.
         */
        if(!ptr) {
                bucket_num = 0;
                while(bucket_num < PA_TBL_BUCKETS) {
                        if(tbl->buckets[bucket_num] >= 0) {
                                idx = tbl->buckets[bucket_num];
                                ptr = tbl->buffer + (idx * tbl->entry_size);
                                break;
                        }
                        bucket_num++;
                }

                return ptr;
        }

        /*
         * Otherwise we have to get a pointer to the next entry.
         */
        next_idx = *(s16 *)ptr;
        if(next_idx <= -2) {
                bucket_num = (-1 * next_idx) - 2 + 1;
                while(bucket_num < PA_TBL_BUCKETS) {
                        if(tbl->buckets[bucket_num] >= 0) {
                                next_idx = tbl->buckets[bucket_num];
                                break;
                        }
                        bucket_num++;
                }

                /* No more entries left in the table */
                if(next_idx <= -2)
                        return NULL;
        }

        /* Return a pointer to the entry */
        return tbl->buffer + (next_idx * tbl->entry_size);
}

PA_INTERN u8 *tbl_find_key(struct pa_table *tbl, u8 *key, u8 **prev,
                s16 *bucket_out)
{
        u16 hash = tbl_hash(key, tbl->key_size);
        s16 bucket = hash % PA_TBL_BUCKETS;
        u8 *ptr = NULL;
        s16 off = PA_TBL_NEXT_SIZE + PA_TBL_HASH_SIZE;

        if(tbl->buckets[bucket] < 0)
                return NULL;

        if(bucket_out) *bucket_out = bucket;

        if(prev) *prev = NULL;
        while((ptr = tbl_next_bucket(tbl, bucket, ptr))) {
                /* Compare the hashes */
                if(hash == *(u16 *)(ptr + PA_TBL_NEXT_SIZE)) {
                        if(pa_mem_compare(key, ptr + off, tbl->key_size)) {
                                return ptr;
                        }
                }
                if(prev) *prev = ptr;
        }

        return NULL;
}

PA_API s8 paInitTable(struct pa_table *tbl, struct pa_memory *mem,
                s32 key_sz, s32 value_sz, s16 alloc)
{
        s32 alloc_sz;
        s16 i;

        tbl->memory = mem;
        tbl->mode = PA_DYNAMIC;

        tbl->key_size = key_sz;
        tbl->value_size = value_sz;
        tbl->entry_size = PA_TBL_HEAD_SIZE + key_sz + value_sz;
        tbl->number = 0;
        tbl->alloc = alloc;

        alloc_sz = tbl->entry_size * tbl->alloc;
        if(!(tbl->buffer = pa_mem_alloc(tbl->memory, NULL, alloc_sz))) {
                return -1;
        }

        /* Reset all memory slots in the buffer */
        for(i = 0; i < tbl->alloc; i++) {
                *(s16 *)(tbl->buffer + i * tbl->entry_size) = -1;
        }

        /* Reset all buckets */
        for(i = 0; i < PA_TBL_BUCKETS; i++) {
                tbl->buckets[i] = -1;
        }

        return 0;
}

PA_API s8 paInitTableFixed(struct pa_table *tbl, void *buffer,
                s32 buf_sz, s32 key_sz, s32 value_sz)
{
        s16 i;

        tbl->memory = NULL;
        tbl->mode = PA_FIXED;

        tbl->key_size = key_sz;
        tbl->value_size = value_sz;
        tbl->entry_size = PA_TBL_HEAD_SIZE + value_sz;
        tbl->number = 0;
        tbl->buffer = buffer;

        tbl->alloc = buf_sz / tbl->entry_size;

        /* Reset all entry-slots in the buffer */
        for(i = 0; i < tbl->alloc; i++) {
                *(s16 *)(tbl->buffer + i * tbl->entry_size) = -1;
        }

        /* Reset all buckets */
        for(i = 0; i < PA_TBL_BUCKETS; i++) {
                tbl->buckets[i] = -1;
        }

        return 0;
}

PA_API void paDestroyTable(struct pa_table *tbl)
{
        if(tbl->mode == PA_DYNAMIC) {
                pa_mem_free(tbl->memory, tbl->buffer);
        }

        tbl->memory = 0;
        tbl->mode = PA_MEM_UDEF;

        tbl->value_size = 0;
        tbl->entry_size = 0;
        tbl->number = 0;
        tbl->alloc = 0;
        tbl->buffer = 0;
}

PA_API s8 paSetTable(struct pa_table *tbl, void *key, void *value)
{
        s16 slot;
        u16 hash;
        s16 bucket;
        u8 *ptr;
        s32 tmp;
        s16 next_idx;

        s16 key_off = PA_TBL_HEAD_SIZE;
        s16 val_off = key_off + tbl->key_size;
                
        /*
         * If there is already an entry with the keyword in the table, we
         * can just overwrite it's value and return.
         */
        if((ptr = tbl_find_key(tbl, key, NULL, NULL))) {
                /* Copy over the content */
                pa_mem_copy(ptr + val_off, value, tbl->value_size);
                return 0;

        }

        /* Get an open slot in the table-buffer */
        if((slot = tbl_find_open(tbl)) < 0)
                return -1;

        /* Hash the key and determine the bucket  */
        hash = tbl_hash(key, tbl->key_size); 
        bucket = hash % PA_TBL_BUCKETS;

        /* 
         * Write the copy over the data to the table-buffer.
         */

        /* Set the next index */
        ptr = tbl->buffer + (slot * tbl->entry_size);
        *(s16 *)ptr = -(bucket + 2);
        ptr += PA_TBL_NEXT_SIZE;

        /* Copy the hash */
        *(u16 *)ptr = hash;
        ptr += PA_TBL_HASH_SIZE;

        /* Copy over the keyword */
        pa_mem_copy(ptr, key, tbl->key_size);
        ptr += tbl->key_size;

        /* Copy over the content */
        pa_mem_copy(ptr, value, tbl->value_size);

        /*
         * Attach the entry to the bucket.
         */
        if(tbl->buckets[bucket] < 0) {
                tbl->buckets[bucket] = slot;
        }
        else {
                tmp = tbl->buckets[bucket] * tbl->entry_size;
                ptr = tbl->buffer + tmp;
                while((next_idx = *(s16 *)ptr) >= 0) {
                        ptr = tbl->buffer + next_idx * tbl->entry_size;
                }
                *(s16 *)ptr = slot;
        }

        tbl->number++;
        return 0;
}

PA_API s8 paGetTable(struct pa_table *tbl, void *key, void *out)
{
        u8 *ptr;
        s16 off = PA_TBL_HEAD_SIZE + tbl->key_size;

        if(!(ptr = tbl_find_key(tbl, key, NULL, NULL)))
                return 0;

        pa_mem_copy(out, ptr + off, tbl->entry_size);
        return 1;
}

PA_API void paRemoveTable(struct pa_table *tbl, void *key)
{
        s16 bucket;
        u8 *ptr;
        u8 *prev;
        s16 next_idx;

        if((ptr = tbl_find_key(tbl, key, &prev, &bucket))) {
                next_idx = *(s16 *)ptr;

                if(!prev && next_idx < 0) {
                        tbl->buckets[bucket] = -1;
                }
                else if(!prev && next_idx >= 0) {
                        tbl->buckets[bucket] = next_idx;
                }
                else if(prev) {
                        *(s16 *)prev = *(s16 *)ptr;
                }

                *(s16 *)ptr = -1;
                tbl->number--;
        }
}

PA_API void *paIterateTable(struct pa_table *tbl, void *ptr, 
                struct pa_table_entry *ent)
{
        char *tmp;

        /* Check if the table is empty */
        if(tbl->number < 1)
                return NULL;

        /* Go to the next entry in the table */
        if(!(ptr = tbl_next(tbl, ptr)))
                return NULL;

        /* Return the entry-data for the current entry */
        if(ent) {
                /* Copy over the key */
                tmp = (char *)ptr + PA_TBL_NEXT_SIZE + PA_TBL_HASH_SIZE;

                /* Set the pointer for the data */
                ent->key = tmp;
                ent->value = tmp + tbl->key_size;
        }

        return ptr;
}

PA_API void *paIterateTableBucket(struct pa_table *tbl, s16 bucket,
                void *ptr, struct pa_table_entry *ent)
{
        char *tmp;

        /* Check if the table is empty */
        if(tbl->number < 1)
                return NULL;

        if(!(ptr = tbl_next_bucket(tbl, bucket, ptr)))
                return NULL;

        /* Return the data for the current entry */
        if(ent) {
                /* Copy over the key */
                tmp = (char *)ptr + PA_TBL_NEXT_SIZE + PA_TBL_HASH_SIZE;

                /* Set the pointer for the data */
                ent->key = tmp;
                ent->value = tmp + tbl->key_size;
        }

        return ptr;
}

/*
 * -----------------------------------------------------------------------------
 *
 *      FLEX
 *
 */

/*
 * Return 1 if the given character is in range defined by low and high.
 * Otherwise return 0.
 */
PA_INTERN s8 flx_inrange(char c, s8 low, s8 high)
{
        return ((c >= low) && (c <= high));
}

/*
 * Return 1 if character is one of the following:
 * 
 *   - numbers: 0..9
 *   - spec. char: .
 */
PA_INTERN s8 flx_is_operand(char c)
{

        return (flx_inrange(c, 0x30, 0x39) || c == 0x2E);
}

/*
 * Return 1 if character is one of the following:
 *
 *   - letters: a..z and A..Z
 *   - spec. char: %
 */
PA_INTERN s8 flx_is_unit(char c)
{
        return (flx_inrange(c, 0x61, 0x7A) || flx_inrange(c, 0x41, 0x5A) ||
                        c == 0x25);
}

/*
 * Return 1 if character is one of the following:
 *
 *   -spec. char: + - * / ( )
 */
PA_INTERN s8 flx_is_operator(char c)
{
        return (c == 0x2B || c == 0x2D || c == 0x2A || c == 0x2F || c == 0x28 ||
                        c == 0x29);
}

/* 
 * input string <s> -> output token <tok> 
 * 
 * @opt: Type of token to parse(Operator/Operand)
 * @s: The null-terminated string containing the token
 * @tok: A pointer to write the token to
 *
 * Returns: 0 on success or -1 if an error occurred
 */
PA_INTERN s8 flx_parse_token(u8 opt, char *s, struct pa_flex_token *tok)
{
        char buf[64];
        char *c;
        u8 tail = 0;

        /* 		OPERAND	         	 */
        if(opt == 1) {
                c = s;
                while(*c && (flx_inrange(*c, 0x30, 0x39) || *c == 0x2E))
                        buf[tail++] = *c++;
                buf[tail] = 0;

                /* No digit detected */
                if(tail < 1) return -1;

                tok->code = 0x11;	/* const */
                tok->value = (f32)pa_atof(buf);

                tail = 0;
                while(*c && flx_is_unit(*c))
                        buf[tail++] = *c++;
                buf[tail] = 0;

                if(tail != 0) {
                        if(pa_strcmp(buf, "px") == 0) {
                                /* A pixel-value has to be an integer */
                                tok->value = (s32)tok->value;
                                tok->code = 0x12;
                        }
                        else if(pa_strcmp(buf, "pct") == 0) {
                                tok->value /= 100.0;

                                tok->code = 0x13;
                        }
                        else if(pa_strcmp(buf, "em") == 0) {
                                tok->code = 0x14;
                        }
                        else {
                                /* Unit invalid */
                                return -1;
                        }
                }

        }
        /* 		OPERATOR		 */
        else if(opt == 3) {
                tok->value = 0;

                switch(*s) {
                        case 0x28: tok->code = 0x01; break; /* ( */
                        case 0x29: tok->code = 0x02; break; /* ) */
                        case 0x2A: tok->code = 0x03; break; /* * */
                        case 0x2B: tok->code = 0x04; break; /* + */
                        case 0x2D: tok->code = 0x05; break; /* - */
                        case 0x2F: tok->code = 0x06; break; /* / */
                }
        }

        return 0;
}

/*
 * Go through the null-terminated string and tokeninze the different expressions.
 * Input-string <str> -> Token-List
 *
 * The tokenizing works through a state-machine where depending on the
 * reading-state, the characters will be inserted into the reading-buffer.
 *
 * read-states:
 *   0: Don't read to buffer
 *   1: Read operand(ie. 12px, 3, 4pct)
 *   2: Read operator(ie. + - / *)
 *
 */
PA_INTERN void flx_tokenize(struct pa_flex_helper *hlp, char *str)
{
        struct pa_flex_token tok;

        char *c;
        char buf[PA_FLX_READ_BUFFER_SIZE];
        u8 buf_tail;
        u8 read;	
        u8 fin;

        c = str;
        buf_tail = 0;

        read = 0;
        fin = 0;

        do {
                /* Null-terminator, Stop */
                if(*c == 0x00) {
                        fin = read;
                }
                /* 		SPACE		 */
                else if(*c == 0x20) {
                        /* Do nothing */
                }
                /* 		OPERAND		 */
                else if(flx_is_operand(*c) || flx_is_unit(*c)) {
                        /* Start reading operand */
                        if(read == 0) {
                                read = 1;
                                buf_tail = 0;
                        }
                        else if(read != 1) {
                                fin = read;
                                read = 1;

                        }
                }
                /* 		OPERATOR	 */
                else if(flx_is_operator(*c)) {
                        /* Start reading operator */
                        if(read == 0) {
                                read = 3;
                                buf_tail = 0;
                        }
                        else {
                                fin = read;
                                read = 3;
                        }
                }
                else {
                        /* Invalid character */
                        return;
                }


                if(fin) {
                        buf[buf_tail] = 0;
                        pa_strip(buf);

                        if(flx_parse_token(fin, buf, &tok) < 0) {
                                /* Invalid expression */
                                return;
                        }
                        
                        if(paPushList(&hlp->tokens, &tok, 1) < 0) {
                                /* Failed to add token to list */
                                return;
                        }

                        buf_tail = 0;
                        fin = 0;
                }


                if(read) {
                        buf[buf_tail] = *c;
                        buf_tail++;
                }
        } while(*(c++));
}

PA_INTERN s8 flx_operator_prio(struct pa_flex_token *tok)
{
        switch(tok->code) {
                case 0x03: return 0x03;	/* * */
                case 0x04: return 0x02;	/* + */
                case 0x05: return 0x02;	/* - */
                case 0x06: return 0x03;	/* / */
        }
        return 0x00;
}

PA_INTERN s8 flx_shunting_yard(struct pa_flex_helper *hlp, struct pa_flex *flx)
{
        struct pa_list *tokens;
        struct pa_list *operators;
        struct pa_list *output;

        struct pa_flex_token tok;
        struct pa_flex_token tok_swp;

        u8 prio[2];
        u8 check;
        u8 opensign = 0; /* 0: positive, 1: negative */

        /* Assign shortcuts for the lists */
        tokens = &hlp->tokens;
        operators = &hlp->swap;
        output = &flx->tokens;

        while(paShiftList(tokens, &tok, 1)) {
                /* Push operand to output */
                if(tok.code > 0x06) {
                        /* To indicate the first operand is negative */
                        if(opensign) {
                                tok.value *= -1.0;
                                opensign = 0;
                        }

                        paPushList(output, &tok, 1);
                }
                /* Push operator into operator-stack */
                else if(tok.code >= 0x03 && tok.code <= 0x06) {
                        if(output->count < 1) {
                                if(tok.code != 0x04 && tok.code != 0x05) {
                                        /* Missing operand */
                                        goto err_return;
                                }
                                /*
                                 * Hotfix, so that the initial operand can be
                                 * negative.
                                 */
                                else if(tok.code == 0x05) {
                                        opensign = 1;
                                }

                                continue;
                        }

                        while(paPopList(operators, &tok_swp, 1)) {
                                prio[0] = flx_operator_prio(&tok);
                                prio[1] = flx_operator_prio(&tok_swp);

                                if(tok.code == 0x01 || prio[0] > prio[1]) {
                                        paPushList(operators, &tok_swp, 1);
                                        break;
                                }

                                paPushList(output, &tok_swp, 1);
                        }

                        paPushList(operators, &tok, 1);
                }
                /* Handle opening-bracket '(' */
                else if(tok.code == 0x01) {
                        paPushList(operators, &tok, 1);
                }
                /* Handle closing-bracket ')' */
                else if(tok.code == 0x02) {
                        if(operators->count < 1) {
                                /* Missing opening bracket */
                                goto err_return;
                        }

                        check = 0;
                        while(paPopList(operators, &tok_swp, 1)) {
                                if(tok_swp.code == 0x01) {
                                        check = 1;
                                        break;
                                }
                                paPushList(output, &tok_swp, 1);
                        }
                        if(!check) {
                                /* Missing opening bracket */
                                goto err_return;
                        }
                }
        }

        /* Copy over all operators to the output and then we're done :) */
        while(paPopList(operators, &tok, 1))
                paPushList(output, &tok, 1);

        /* Now just cleanup the parser and return */
        paClearFlexHelper(hlp);
        return 0;

err_return:
        paClearFlexHelper(hlp);
        paClearFlex(flx);
        return -1;
}

PA_API s8 paInitFlexHelper(struct pa_flex_helper *hlp, struct pa_memory *mem,
                s16 tokens)
{
        s32 toksize = sizeof(struct pa_flex_token);

        /* Create the swap-list */
        if(paInitList(&hlp->swap, mem, toksize, tokens, PA_NOLIM) < 0)
                return -1;

        /* Create the value-list */
        if(paInitList(&hlp->values, mem, sizeof(f32), tokens, PA_NOLIM) < 0)
                goto err_destroy_swp;

        /* Create the token-list */
        if(paInitList(&hlp->tokens, mem, toksize, tokens, PA_NOLIM) < 0)
                goto err_destroy_val;

        return 0;

err_destroy_val:
        paDestroyList(&hlp->values);

err_destroy_swp:
        paDestroyList(&hlp->swap);

        return -1;
}

PA_API s8 paInitFlexHelperFixed(struct pa_flex_helper *hlp,
                void *tok_buf, s32 tok_buf_sz, 
                void *swp_buf, s32 swp_buf_sz,
                void *val_buf, s32 val_buf_sz)
{
        s32 toksize = sizeof(struct pa_flex_token);

        /* Create the swap-list */
        if(paInitListFixed(&hlp->swap, toksize, swp_buf, swp_buf_sz) < 0)
                return -1;

        if(paInitListFixed(&hlp->values, sizeof(f32), val_buf, val_buf_sz) < 0)
                goto err_destroy_swp;

        /* Create the token-list */
        if(paInitListFixed(&hlp->tokens, toksize, tok_buf, tok_buf_sz) < 0)
                goto err_destroy_val;

        return 0;

err_destroy_val:
        paDestroyList(&hlp->values);

err_destroy_swp:
        paDestroyList(&hlp->swap);

        return -1;
}

PA_API void paDestroyFlexHelper(struct pa_flex_helper *hlp)
{
        paDestroyList(&hlp->swap);
        paDestroyList(&hlp->values);
        paDestroyList(&hlp->tokens);
}

PA_API void paClearFlexHelper(struct pa_flex_helper *hlp)
{
        paClearList(&hlp->tokens);
        paClearList(&hlp->swap);
        paClearList(&hlp->values);
}

PA_API s8 paInitFlex(struct pa_flex *flx, struct pa_flex_helper *hlp,
                struct pa_memory *mem, s16 tokens)
{
        s32 toksize = sizeof(struct pa_flex_token);
        flx->helper = hlp;
        return paInitList(&flx->tokens, mem, toksize, tokens, PA_NOLIM);
}

PA_API s8 paInitFlexFixed(struct pa_flex *flx, struct pa_flex_helper *hlp,
                void *tok_buf, s32 tok_buf_sz)
{
        s32 toksize = sizeof(struct pa_flex_token);
        flx->helper = hlp;
        return paInitListFixed(&flx->tokens, toksize, tok_buf, tok_buf_sz);
}

PA_API void paDestroyFlex(struct pa_flex *flx)
{
        paDestroyList(&flx->tokens);
}

PA_API void paClearFlex(struct pa_flex *flx)
{
        paClearList(&flx->tokens);
}

PA_API s16 paParseFlex(struct pa_flex *flx, char *str)
{
        /* Firs we tokenize the input and write the tokens to the parser */
        flx_tokenize(flx->helper, str);

        flx_shunting_yard(flx->helper, flx);

        return 0;
}

PA_API s32 paProcessFlex(struct pa_flex *flx, struct pa_flex_reference *ref)
{
        struct pa_flex_token tok;
        struct pa_list *values;
        f32 value;
        f32 opd[2];
        u16 i = 0;

        values = &flx->helper->values;

        while(paPeekList(&flx->tokens, &tok, i++, 1)) {
                /*  Handle operands  */
                if(tok.code > 0x06) {
                        switch(tok.code) {
                                case 0x11: 
                                        value = tok.value;
                                        break;
                                case 0x12: 
                                        value = tok.value;
                                        break;
                                case 0x13: 
                                        value = tok.value * ref->relative;
                                        break;
                                case 0x14:
                                        value = tok.value * ref->font;
                                        break;
                                default: 
                                        value = tok.value;
                                        break;
                        }

                        paPushList(values, &value, 1);
                }
                /* Handle operators */
                else {
                        paPopList(values, &opd[0], 1);
                        paPopList(values, &opd[1], 1);

                        switch(tok.code) {
                                case 0x03: value = opd[1] * opd[0]; break;
                                case 0x04: value = opd[1] + opd[0]; break;
                                case 0x05: value = opd[1] - opd[0]; break;
                                case 0x06: value = opd[1] / opd[0]; break;
                        }

                        paPushList(values, &value, 1);
                }
        }

        paPopList(values, &value, 1);
        paClearList(values);

        return (s32)value;
}




#include <stdlib.h>


PA_API s8 paInit(struct pa_document *doc)
{
        if(!doc)
                return -1;

        pa_mem_init_default(&doc->memory);

        return 0;

}


PA_API void paQuit(struct pa_document *doc)
{
        if(!doc)
                return;

}







PA_LIB s8 pa_element_tree_init(struct pa_element_tree *tree)
{
        s16 size;

        return 0;
}


PA_LIB void pa_element_tree_destroy(struct pa_element_tree *tree)
{
        if(!tree)
                return;

}





/* 
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 *
 *              STRING-HELPER
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 */

PA_LIB s8 pa_isspace(char c)
{
        return c == 0x20;
}

PA_LIB s8 pa_isletter(char c)
{
        /* Uppercase letters A..Z */
        if(c >= 0x41 && c <= 0x5A)
                return 1;

        /* Lowercase letters a..z */
        if(c >= 0x61 && c <= 0x7A)
                return 1;

        return 0;
}

PA_LIB s8 pa_isdigit(char c)
{
        if(c >= 0x30 && c <= 0x39)
                return 1;

        return 0;
}

PA_LIB s8 pa_ishexcode(char c)
{
        return (pa_isletter(c) || pa_isdigit(c));
}

PA_LIB s32 pa_strlen(char *s)
{
        s32 c = 0;
        while(*(s++)) c++;
        return c;
}

PA_LIB void pa_strcpy(char *dst, char *src)
{
        while((*(dst++) = *(src++)));
}

PA_LIB s16 pa_strcmp(char *str1, char *str2)
{
        s16 i;

        if(pa_strlen(str1) != pa_strlen(str2))
                return 1;

        for(i = 0; i < pa_strlen(str1); i++) {
                if(str1[i] != str2[i])
                        return 1;
        }

        return 0;
}

PA_LIB char *pa_trim(char *str)
{
        char *end;

        /* Trim leading space */
        while(pa_isspace(*str)) str++;

        /* All spaces? */
        if (*str == 0)
                return str;

        /* Trim trailing space */
        end = str + pa_strlen(str) - 1;
        while (end > str && pa_isspace(*end)) end--;

        /* Write new null terminator */
        *(end + 1) = 0;

        return str;
}


PA_LIB char *pa_strip(char *str)
{
        char *d = str;
        char *p = str;

        do {
                while(*d == 0x20)
                        d++;

        } while((*(str++) = *(d++)));

        return p;
}

PA_LIB s64 pa_atoi(char *s)
{
        char *run = s + pa_strlen(s) - 1;
        char c;
        s64 value = 0;
        s64 factor = 1;
        s8 read = 1;

        while(run >= s && read) {
                c = *run;
                /* Handle numbers 0..9 */
                if(pa_isdigit(c)) {
                        value += (c - 0x30) * factor;
                        factor *= 10;
                }
                /* Handle the minus-sign */
                else if(c == 0x2D) {
                        read = 0;
                        value *= -1;
                }
                run--;
        }

        return value;
}

/* Copied from https://github.com/GaloisInc/minlibc/ */
PA_LIB f64 pa_atof(char *s)
{
        f64 a = 0.0;
        s32 e = 0;
        s32 c;
        while ((c = *s++) && pa_isdigit(c)) {
                a = a * 10.0 + (c - '0');
        }
        if(c == '.') {
                while((c = *s++) && pa_isdigit(c)) {
                        a = a * 10.0 + (c - '0');
                        e = e - 1;
                }
        }
        if(c == 'e' || c == 'E') {
                s32 sign = 1;
                s32 i = 0;
                c = *s++;
                if(c == '+')
                        c = *s++;
                else if(c == '-') {
                        c = *s++;
                        sign = -1;
                }
                while(pa_isdigit(c)) {
                        i = i * 10 + (c - '0');
                        c = *s++;
                }
                e += i * sign;
        }
        while(e > 0) {
                a *= 10.0;
                e--;
        }
        while(e < 0) {
                a *= 0.1;
                e++;
        }
        return a;
}




#include <stdlib.h>
#include <string.h>


PA_LIB s8 pa_mem_init_default(struct pa_memory *mem)
{
        mem->mode = PA_DYNAMIC;
        mem->space = NULL;

        mem->allocator.alloc = &malloc;
        mem->allocator.realloc = &realloc;
        mem->allocator.free = &free;
        
        return 0;
}

PA_LIB void *pa_mem_alloc(struct pa_memory *mem, void *p, s32 size)
{
        if(p) {
                return mem->allocator.realloc(p, size);
        }

        return mem->allocator.alloc(size);
}

PA_LIB void pa_mem_free(struct pa_memory *mem, void *p)
{
        mem->allocator.free(p);
}

PA_LIB void pa_mem_set(void *p, u8 v, s32 size)
{
        memset(p, v, size);
}

PA_LIB void pa_mem_zero(void *p, s32 size)
{
        memset(p, 0, size);
}


PA_LIB void pa_mem_copy(void *dst, void *src, s32 size)
{
        memcpy(dst, src, size);
}

PA_LIB void pa_mem_move(void *dst, void *src, s32 size)
{
        memmove(dst, src, size);
}

PA_LIB s8 pa_mem_compare(void *ptr1, void *ptr2, s32 size)
{
        if(memcmp(ptr1, ptr2, size) == 0)
                return 1;

        return 0;
}




#include <stdio.h>
#include <stdlib.h>
#include <string.h>




#endif /* PA_IMPLEMENTATION */

/*
*/

