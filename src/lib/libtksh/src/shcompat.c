#pragma prototyped
#include "tkshlib.h"

int nv_getlevel()
{
	int index=0;
	Shscope_t *sp = sh_getscope(0, 1);
	while ((sp = sp->par_scope))
		index++;
	return index;
}

#ifdef nv_scan
#undef nv_scan
int _nv_scan(Hashtab_t *root, void (*fn)(Namval_t *,void*), void *data, int mask, int flags)
{
	return nv_scan(root, (void (*)(Namval_t *)) fn, mask, flags);
}
#endif

Hashtab_t *nv_globalscope()
{
	Hashtab_t *hp;
	return ((hp=hashscope(sh.var_tree)) ? hp : sh.var_tree);
}

int sh_openmax()
{
	return sysconf(_SC_OPEN_MAX);
}
