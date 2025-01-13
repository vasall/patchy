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
