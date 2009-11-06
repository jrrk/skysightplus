 /*
 				manobjlist.c

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	SExtractor
*
*	Author:		E.BERTIN, IAP/ESO
*
*	Contents:	functions for the management of object lists.
*
*	Last modify:	30/03/95
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

#include    <stdio.h>
#include	<stdlib.h>
#include	<assert.h>

#include	"define.h"
#include	"globals.h"

/********************************* belong ************************************/
/*
say if an object is "included" in another.
*/
int	belong(int corenb, objliststruct *coreobjlist,
	       int shellnb, objliststruct *shellobjlist)

	{
	objstruct	* cobj = &(coreobjlist->obj[corenb]),
			* sobj = &(shellobjlist->obj[shellnb]);
	pliststruct	* cpl = coreobjlist->plist,
			* spl = shellobjlist->plist;

	int		i, k=0,
			xc=cpl[cobj->firstpix].x,
			yc=cpl[cobj->firstpix].y;

	for(i = sobj->firstpix; i != -1; i = spl[i].nextpix)
		if ((spl[i].x == xc) && (spl[i].y == yc))
			return	1;

	return	0;
	}
	
/********************************* addobj ************************************/
/*
Add an object to an objlist.
*/
int	addobj(int objnb, objliststruct *objl1, objliststruct *objl2)

	{
        objstruct	* objl2obj;
	pliststruct	* plist1 = objl1->plist,
			* plist2 = objl2->plist;

	int	fp, i, j, npx, objnb2;

	j = fp = objl2->npix;
	objnb2 = objl2->nobj;

/*------ Update the object list */
	if (objl2->nobj)
		{
		if (!(objl2obj = (objstruct *)myrealloc(objl2->obj,
		    (++objl2->nobj) * sizeof(objstruct))))
                      goto exit_addobj;
		}
	else
		if (!(objl2obj = (objstruct *)myalloc((++objl2->nobj)
		    * sizeof(objstruct))))
                      goto exit_addobj;

/*------ Update the pixel list */
	npx = 0;
	for(i=objl1->obj[objnb].firstpix; i!=-1; i=plist1[i].nextpix)
		npx++;
	if (fp)
		{
		if (!(plist2 = (pliststruct *)myrealloc(plist2,
		    (objl2->npix+=npx) * sizeof(pliststruct))))
                      goto exit_addobj;
		}
	else
		if (!(plist2 = (pliststruct *)myalloc((objl2->npix=npx)
			* sizeof(pliststruct))))
                      goto exit_addobj;

        objl2->obj = objl2obj;
        objl2->plist = plist2;

	for(i=objl1->obj[objnb].firstpix; i!=-1; i=plist1[i].nextpix)
		{
		plist2[j].x = plist1[i].x;
		plist2[j].y = plist1[i].y;
		plist2[j].value = plist1[i].value;
		plist2[j].cvalue = plist1[i].cvalue;
		plist2[j].nextpix = j+1;
		j++;
		}
	plist2[j-1].nextpix = -1;

	objl2->obj[objnb2] = objl1->obj[objnb];
	objl2->obj[objnb2].firstpix = fp;
	objl2->obj[objnb2].lastpix = j-1;
	return	objnb2;

exit_addobj:

        objl2->nobj--;
        objl2->npix = fp;
        return RETURN_FATAL_ERROR;
	}
