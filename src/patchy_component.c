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


PA_LIB s8 paInitList(pa_List *lst, PAenum mode, s16 size, s16 alloc)
{
        s32 tmp;

        lst->mode = mode;
        lst->entry_size = size;
        lst->count = 0;
        lst->alloc = alloc;

        tmp = lst->alloc * lst->entry_size;
        if(!(lst->data = pa_mem_alloc(NULL, tmp))) {
                return -1;
        }
 
        paClearList(lst);
        return 0;
}

PA_LIB void paDestroyList(pa_List *lst)
{
        pa_mem_free(lst->data);
}

PA_LIB void paClearList(pa_List *lst)
{
        pa_mem_set(lst->data, 0, lst->alloc * lst->entry_size);
}

PA_LIB s16 paPushList(pa_List *lst, void *ptr, s16 num)
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
        pa_mem_copy(lst->data + off, ptr, size);

        /* Update number of entries in list and return number of written */
        lst->number += fit_number;
        return fit_num;
}

PA_LIB s16 paPopList(pa_List *lst, void *out, s16 num)
{
        s32 entry_number;
        s32 offset;
        s32 size;

       /* Figure out how many entries can be returned */
       entry_number = num > lst->number ? lst->number : num;

       /* Copy entries from list */
}
