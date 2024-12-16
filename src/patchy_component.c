#include "patchy.h"
#include "patchy_internal.h"


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

PA_INTERN void str_ensure_fit(struct pa_string *str, s32 size)
{
        s32 new_alloc;
        void *p;

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

PA_API s8 paInitStringFixed(struct pa_string *str, void *buf, s32 size)
{
        str->memory = NULL;
        str->mode = PA_FIXED;

        str->buffer = buf;
        *str->buffer = 0;

        str->length = 0;
        str->size = 0;
        str->alloc = size;
        return 0;
}

PA_API s16 paWriteString(struct pa_string *str, char *src, s16 off, s16 num)
{
        s32 free_size;
        s32 read_size;
        s32 write_offset;
        s32 mov_offset;
        s32 mov_size;
        s16 written;

        /* First we figure out how many bytes should be read from the source */
        read_size = num == PA_ALL ? pa_strlen(src) : str_offset(src, num);

        /* If the string is dynamic, scale the buffer to fit new characters */
        str_ensure_fit(str, read_size);

        /* 
         * Then we determine how much free memory is actually left in the
         * string(also subtract 1 for the null-terminator).
         */
        free_size = str->alloc - str->size - 1;

        /*
         * Then we figure out how many bytes will be written to the string
         * buffer, considering only whole characters may be added. We do that by
         * checking how many whole characters fit into the available
         * memory-space, and then convert that back into an actual byte-size.
         * Cumbersome? Yes. Efficient? No! Does it work? ???
         */
        read_size = read_size > free_size ? free_size : read_size;
        written = str_charnum(src, read_size);
        read_size = str_offset(src, written);

        /* Get the byte-offset in the string buffer */
        write_offset = paGetStringOffset(str, off);

        /* Determine the size of the trailing block which has to be moved */
        mov_size = str->size - write_offset;

        /* Move back the trailing block to make space */
        mov_offset = write_offset + read_size;
        pa_mem_move(str->buffer + mov_offset, str->buffer + write_offset,
                        mov_size);

        /* Copy from source to the string buffer */
        pa_mem_copy(str->buffer + write_offset, src, read_size);

        /* Update string byte-size and set null-terminator */
        str->size += read_size;
        str->length += num;
        str->buffer[str->size] = 0;
               
        /* Return number of written bytes */
        return written;
}

PA_API s16 paCopyString(struct pa_string *str, char *dst, s16 off, s16 num)
{
        s32 copy_size;
        s32 copy_offset;

        /* Resolve input parameters */
        num = num == PA_ALL ? str->length - off : num;

        /* Calculate the size and offset for the characters in the string */
        copy_offset = paGetStringOffset(str, off);
        copy_size = paGetStringOffset(str, off + num) - copy_offset;

        /* Copy over the characters and set null-terminator in output-buffer */
        pa_mem_copy(dst, str->buffer + copy_offset, copy_size);
        dst[copy_size] = 0;

        /* Return the number of copied characters */
        return num;
}

PA_API s16 paReadString(struct pa_string *str, char *dst, s16 off, s16 num)
{
        s32 read_offset;
        s32 read_size;
        s32 mov_offset;
        s32 mov_size;

        /* Resolve input parameters */
        num = num == PA_ALL ? str->length = off : num;

        /* Calculate the size and offset for the characters in the string */
        read_offset = paGetStringOffset(str, off);
        read_size = paGetStringOffset(str, off + num) - read_offset;

        /* Copy over the characters and set null-terminator in output-buffer */
        pa_mem_copy(dst, str->buffer + read_offset, read_size);
        dst[read_size] = 0;

        /* Calculate the size and new offset for the trailing block */
        mov_size = str->size - read_offset;
        mov_offset = read_offset + read_size;

        /* Move the trailing block forward to fill the gap */
        pa_mem_move(str->buffer + read_offset, str->buffer + mov_offset,
                        mov_size);

        /* Update string byte-size and set null-terminator */
        str->size -= read_size;
        str->length -= num;
        str->buffer[str->size] = 0;

        /* Return the number of read bytes */
        return 0;
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

        if(new_num > lst->alloc && lst->mode == PA_DYNAMIC) {
                new_alloc = new_num * 1.5; 
                new_size = new_alloc * lst->entry_size;
                if(!(p = pa_mem_alloc(lst->memory, lst->data, new_size)))
                        return;

                lst->data = p;
                lst->alloc = new_alloc;
        }
}

PA_LIB s8 paInitList(struct pa_list *lst, struct pa_memory *mem, 
                s16 size, s16 alloc)
{
        s32 tmp;

        lst->memory = mem;
        lst->mode = PA_DYNAMIC;
        lst->entry_size = size;
        lst->count = 0;
        lst->alloc = alloc;
                
        tmp = lst->alloc * lst->entry_size;
        if(!(lst->data = pa_mem_alloc(lst->memory, NULL, tmp))) {
                return -1;
        }

        paClearList(lst);
        return 0;
}

PA_LIB s8 paInitListFixed(struct pa_list *lst, s16 size, s16 alloc, void *buf)
{
        lst->memory = NULL;
        lst->mode = PA_FIXED;
        lst->entry_size = size;
        lst->count = 0;
        lst->alloc = alloc;
        lst->data = buf;

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
        s32 mov_size;

        /* If configured as dynamic, scale to fit new entries */
        lst_ensure_fit(lst, num);

        /* Figure out how many entries can be written to the list */
        open_slots = lst->alloc - lst->count;
        entry_number = num > open_slots ? open_slots : num;
        size = entry_number * lst->entry_size;

        /* Move all entries back to make space at the beginning */
        mov_size = lst->count * lst->entry_size;
        pa_mem_move(lst->data + size, lst->data, mov_size);

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
        s32 mov_offset;
        s32 mov_size;

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
        mov_size = (lst->count - start) * lst->entry_size;
        mov_offset = (lst->count * lst->entry_size) - mov_size;
        pa_mem_move(lst->data + mov_offset, lst->data + offset, mov_size);

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
        s32 mov_offset;
        s32 mov_size;

        /* Figure out how many entries can actually be returned */
        entry_left = lst->count - start;
        entry_number = num > entry_left ? entry_left : num;

        /* Copy over the entries */
        offset = start * lst->entry_size;
        size = entry_number * lst->entry_size;
        pa_mem_copy(dst, lst->data + offset, size);

        /* Move back the entries to fill the gap */
        mov_offset = offset + size;
        mov_size = (lst->count - (start + entry_number)) * lst->entry_size;
        pa_mem_move(lst->data + offset, lst->data + mov_offset, mov_size);

        /* Update the number of entries in the list and return */
        lst->count -= entry_number;
        return entry_number;
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
