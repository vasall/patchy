#include "patchy.h"
#include "patchy_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


PA_INTERN void lst_ensure_fit(pa_List *lst, s32 num)
{
        s32 new_num = lst->number + num;
        s32 new_alloc;

        if(new_num > alloc && lst->mode == PA_DYNAMIC) {
                new_alloc = lst->alloc * 1.5; 
                lst->data = pa_mem_alloc(lst->data, alloc * lst->entry_size);
                lst->alloc = new_alloc;
        }
}


PA_LIB s8 paInitList(pa_List *lst, PAenum mode, s16 size, s16 alloc, void *mem)
{
        s32 tmp;

        lst->mode = mode;
        lst->entry_size = size;
        lst->count = 0;
        lst->alloc = alloc;

        if(mode == PA_DYNAMIC) {
                tmp = lst->alloc * lst->entry_size;
                if(!(lst->data = pa_mem_alloc(NULL, tmp))) {
                        return -1;
                }
        }
        else {
                lst->data = mem;
        }

        paClearList(lst);
        return 0;
}

PA_LIB void paDestroyList(pa_List *lst)
{
        if(lst->mode == PA_DYNAMIC) {
                pa_mem_free(lst->data);
        }
}

PA_LIB void paClearList(pa_List *lst)
{
        pa_mem_set(lst->data, 0, lst->alloc * lst->entry_size);
}

PA_LIB s16 paPushList(pa_List *lst, void *src, s16 num)
{
        s32 open_slots;
        s32 fit_number;
        s32 offset;
        s32 size;

        /* If configured as dynamic, scale to fit new entries */
        lst_ensure_fit(lst, num);

        /* Figure out how many entries can be written to the list */
        open_slot = lst->alloc - lst->number;
        fit_number = num > open_slot ? open_slot : num;

        /* Copy entries to list */
        offset = lst->number * lst->entry_size;
        size = fit_number * lst->entry_size;
        pa_mem_copy(lst->data + off, src, size);

        /* Update number of entries in list and return number of written */
        lst->number += fit_number;
        return fit_num;
}

PA_LIB s16 paPopList(pa_List *lst, void *dst, s16 num)
{
        s32 entry_number;
        s32 entry_left;
        s32 size;
        s32 offset;

        /* Figure out how many entries can be returned */
        entry_number = num > lst->number ? lst->number : num;

        /* Copy entries from list */
        size = entry_number * lst->entry_size;
        offset = (lst->number * lst->entry_size) - size;
        pa_mem_copy(dst, lst->data + offset, size);

        /* Update the number of entries in the list and return */
        lst->number -= entry_number;
        return entry_number;
}

PA_LIB s16 paUnshiftList(pa_List *lst, void *src, s16 num)
{
        s16 open_slots;
        s16 entry_number;
        s16 entry_left;
        s16 size;

        /* If configured as dynamic, scale to fit new entries */
        lst_ensure_fit(lst, num);

        /* Figure out how many entries can be written to the list */
        open_slots = lst->alloc - lst->number;
        entry_number = num > open_slots ? open_slots : num;
        size = entry_number * lst->entry_size;

        /* Move the entries at the beginning to make place */
        pa_mem_move(lst->data + size, lst->data, size);

        /* Now copy over the data from the source */
        pa_mem_copy(lst->data, src, size);

        /* Update the number of entries in the list and return */
        lst->number += entry_number;
        return entry_number;
}

PA_LIB s16 paShiftList(pa_List *lst, void *dst, s16 num)
{
        s32 entry_number;
        s32 entry_left;
        s32 size;
        s32 offset;

        /* Figure out how many entries can be returned */
        entry_number = num > lst->number ? lst->number : num;

        /* Copy entries from list */
        size = entry_number * lst->entry_size;
        pa_mem_copy(dst, lst->data, size);

        /* Move the rest of the entries forward to close the gap */
        entry_left = lst->number - entry_number;
        if(entry_left > 0) {
                offset = size;
                size = entry_left * lst->entry_size;
                pa_mem_move(lst->data, lst->data + offset, size);
        }

        /* Update the number of entries in the list and return */
        lst->number -= entry_number;
        return entry_number;
}

PA_LIB s16 paInsertList(paList *lst, void *src, s16 start, s16 num)
{
        s32 open_slots;
        s32 entry_number;
        s32 offset;
        s32 size;
        s32 mov_offset;
        s32 mov_size;

        if(start == PA_END) {
                start = lst->number;
        }

        /* If configured as dynamic, scale to fit new entries */
        lst_ensure_fit(lst, num);

        /* Figure out how many entries can be inserted into the list */
        open_slots = lst->alloc - lst->number;
        entry_number = num > open_slots ? open_slots : num;
        size = entry_number * lst->entry_size;
        offset = start * lst->entry_size;
        
        /* Move entries back to make space */
        mov_size = (lst->number - start) * lst->entry_size;
        mov_offset = (lst->number * lst->entry_size) - mov_size;
        pa_mem_move(lst->data + mov_offset, lst->data + offset, mov_size);

        /* Copy over the entries from the source */
        pa_mem_copy(lst->data + offset, src, size);

        /* Update the number of entries in the list and return */
        lst->number += entry_number;
        return entry_number;
}

PA_LIB s16 paPeekList(pa_List *lst, void *dst, s16 start, s16 num)
{
        s32 entry_number;
        s32 entry_left;
        s32 offset;
        s32 size;

        /* Figure out how many entries can actually be returned */
        entry_left = lst->number - start;
        entry_number = num > entry_left ? entry_left : num;

        /* Copy over the entries */
        offset = start * lst->entry_size;
        size = entry_number * lst->entry_size;
        pa_mem_copy(dst, lst->data + offset, size);

        /* Return the number of returned entries */
        return entry_number;
}

PA_LIB s16 paGetList(pa_List *lst, void *dst, s16 start, s16 num)
{
        s32 entry_left;
        s32 entry_number;
        s32 offset;
        s32 size;
        s32 mov_offset;
        s32 mov_size;

        /* Figure out how many entries can actually be returned */
        entry_left = lst->number - start;
        entry_number = num > entry_left ? entry_left : num;

        /* Copy over the entries */
        offset = start * lst->entry_size;
        size = entry_number * lst->entry_size;
        pa_mem_copy(dst, lst->data + offset, size);

        /* Move back the entries to fill the gap */
        mov_offset = offset + size;
        mov_size = (lst->number - (start + entry_number)) * lst->entry_size;
        pa_mem_move(lst->data + mov_offset, lst->data + offset, mov_size);

        /* Update the number of entries in the list and return */
        lst->number -= entry_number;
        return entry_number;
}
