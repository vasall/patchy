#include "patchy.h"
#include "patchy_internal.h"

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
        s32 entry_sz = PA_DICT_HEAD_SIZE + dct->value_size;

        for(i = 0; i < dct->alloc; i++) {
                if(*(s16 *)(dct->buffer + i * entry_sz) == -1)
                        return i;
        }

        return -1;
}

PA_INTERN u8 *dct_next_bucket(struct pa_dictionary *dct, s16 bucket,
                u8 *ptr)
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
