#include	"dthdr.h"

/*	Restore dictionary from given tree or list of elements.
**	There are two cases. If called from within, list is nil.
**	From without, list is not nil and data->size must be 0.
**
**	Written by Kiem-Phong Vo (7/15/95)
*/

#if __STD_C
int dtrestore(reg Dt_t* dt, reg Dtlink_t* list)
#else
int dtrestore(dt, list)
reg Dt_t*	dt;
reg Dtlink_t*	list;
#endif
{
	reg Dtlink_t	*t, **slot, **eslot;
	reg int		type;
	reg Dtsearch_f	searchf = dt->meth->searchf;

	type = dt->data->type&DT_FLATTEN;
	if(!list) /* restoring a flattened dictionary */
	{	if(!type)
			return -1;
		list = dt->data->here;
	}
	else	/* restoring an extracted list of elements */
	{	if(dt->data->size != 0)
			return -1;
		type = 0;
	}
	dt->data->type &= ~DT_FLATTEN;

	if(dt->data->type&DT_HASH)
	{	dt->data->here = NIL(Dtlink_t*);
		if(type) /* restoring a flattened dictionary */
		{	eslot = (slot = dt->data->htab) + dt->data->ntab;
			for(; slot < eslot; ++slot)
			{	if(!(t = *slot) )
					continue;
				*slot = list;
				list = t->right;
				t->right = NIL(Dtlink_t*);
			}
		}
		else	/* restoring an extracted list of elements */
		{	dt->data->size = 0;
			while(list)
			{	t = list->right;
				(*searchf)(dt,(Void_t*)list,DT_RENEW);
				list = t;
			}
		}
	}
	else
	{	if(dt->data->type&DT_TREE)
			dt->data->here = list;
		else /*if(dt->data->type&(DT_LIST|DT_STACK|DT_QUEUE))*/
		{	dt->data->here = NIL(Dtlink_t*);
			dt->data->head = list;
		}
		if(!type)
			dt->data->size = -1;
	}

	return 0;
}
