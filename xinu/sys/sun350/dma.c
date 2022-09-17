/* dma.c - various Sun virtual memory utils */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <systypes.h>
#include <dma.h>


pg_size
getpgreg(addr)
u_long			addr;
{
	addr = ((addr & (u_long)~PAGEMASK) + (u_long)PG_OFF) & (u_long)ADDRMASK;
	return((pg_size) getfc3(sizeof(pg_size), addr));
}

pg_size
setpgreg(addr,entry)
u_long			addr;
pg_size		entry;
{
	pg_size	ret = getpgreg(addr);

	addr = ((addr & ~PAGEMASK) + PG_OFF) & ADDRMASK;
	setfc3(sizeof(pg_size), addr, entry);
	return(ret);
}

/*
 * getsmreg - get the contents of the segment map corresponding
 *            to virtual address "addr"
 */
sm_size
getsmreg(addr)
u_long			addr;
{
	addr = ((addr & ~SEGMASK) + SM_OFF) & ADDRMASK;
	return((sm_size) getfc3(sizeof(sm_size), addr));
}

/*
 * setsmreg - set the segment map entry for virtual address "addr"
 *            to point to "entry"
 */
sm_size
setsmreg(addr,entry)
u_long			addr;
sm_size		entry;
{
	sm_size	ret = getsmreg(addr);

	addr = ((addr & ~SEGMASK) + SM_OFF) & ADDRMASK;
	setfc3(sizeof(sm_size), addr, entry);
	return(ret);
}

/*
 * map(virt, size, phys, space)
 *    Make redundant virtual mapping for frames.
 *    Used to allow ethernet DMA for V7.
 */
map(virt, size, phys, space)
u_long				virt, size, phys;
enum pm_type			space;
{
	pg_t				page;
	struct pg_field	*pgp = &page.pg_field;
	int	i;

	pgp->pg_valid = 1;
	pgp->pg_permission = PMP_ALL;
	pgp->pg_space = space;

	phys = BTOP(phys);
	size = BTOP(size);

	for (i = 0; i < size; i++){		/* for each page, */
		pgp->pg_pagenum = phys++;
		setpgreg(virt + PTOB(i), page.pg_whole);
	}
}
