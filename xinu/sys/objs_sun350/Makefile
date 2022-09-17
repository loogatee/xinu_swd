#
#	$Id: Makefile,v 1.5 1993/01/14 23:52:55 johnr Exp $
#
TARGET		=	libsys_sun350.a

MKFILE		=	Makefile
SHELL		=	/bin/sh

CC			=	gcc68
LD			=	gld
AS			=	gas68
CPP			=	cpp
AR			=	gar
RANLIB		=	granlib

#
#	Standard includes, Processor-specific includes, and
#	system-specific includes
#
STDINC		=	../../../include
PSPINC		=	$(STDINC)/68000
SSPINC		=	$(STDINC)/sun350
CONINC		=	../../config/sun350

ARCH		=
OPTIM		=	-O
DFLAGS		=	-DBSDURG
MFLAGS		=
IFLAGS		=	-I$(STDINC) -I$(PSPINC) -I$(SSPINC) -I$(CONINC)
CFLAGS		=	$(ARCH) $(OPTIM) $(DFLAGS) $(MFLAGS) $(IFLAGS) -c
SFLAGS		=	$(ARCH)

STDCMN		=	../common
CMN1		=	../68000
CMN2		=	../68020
CMN3		=	../sun350
DIR			=	$(STDCMN)


OBJS_X		=	autostart.o chprio.o \
				clkinit.o clkint.o close.o control.o create.o ctxsw.o \
				dma.o evec.o freebuf.o freemem.o getbuf.o getc.o \
				getdmem.o getidprom.o getitem.o getmem.o getpid.o getprio.o \
				getstk.o gettime.o gpq.o init.o initialize.o insert.o \
				insertd.o intr.o ioerr.o ionull.o kill.o kprintf.o mark.o \
				mkdpool.o mkpool.o \
				newqueue.o open.o panic.o pcount.o pcreate.o pdelete.o \
				pinit.o poolinit.o preceive.o preset.o psend.o \
				ptclear.o putc.o queue.o read.o ready.o receive.o recvclr.o \
				recvtim.o resched.o resume.o ret_mon.o \
				scount.o screate.o sdelete.o seek.o send.o sendf.o set_evec.o \
				setdev.o setnok.o signal.o signaln.o sleep.o sleep10.o \
				sreset.o ssclock.o startup.o suspend.o trap.o unsleep.o \
				userret.o wait.o wakeup.o write.o xdone.o trace.o getfc3.o \
				errlog.o tty_cntl.o tty_getc.o tty_iin.o tty_oin.o tty_putc.o \
				tty_read.o tty_write.o tty_iin-ssp.o tty_init.o tty_int.o \
				tty_oin-ssp.o tty_utils.o getutim.o

ODIR		=	./
OBJS		=	$(OBJS_X:%=$(ODIR)%)

$(ODIR)autostart.o:=	DIR=$(CMN3)
$(ODIR)clkinit.o:=		DIR=$(CMN3)
$(ODIR)clkint.o:=		DIR=$(CMN3)
$(ODIR)create.o:=		DIR=$(CMN1)
$(ODIR)ctxsw.o:=		DIR=$(CMN1)
$(ODIR)dma.o:=			DIR=$(CMN3)
$(ODIR)dma.o:=			OPTIM=
$(ODIR)evec.o:=			DIR=$(CMN3)
$(ODIR)getdmem.o:=		DIR=$(CMN3)
$(ODIR)getidprom.o:=	DIR=$(CMN3)
$(ODIR)getstk.o:=		DIR=$(CMN1)
$(ODIR)initialize.o:=	DIR=$(CMN3)
$(ODIR)intr.o:=			DIR=$(CMN1)
$(ODIR)kprintf.o:=		DIR=$(CMN3)
$(ODIR)resched.o:=		DIR=$(CMN1)
$(ODIR)ret_mon.o:=		DIR=$(CMN3)
$(ODIR)set_evec.o:=		DIR=$(CMN2)
$(ODIR)startup.o:=		DIR=$(CMN3)
$(ODIR)trap.o:=			DIR=$(CMN1)
$(ODIR)getfc3.o:=		DIR=$(CMN2)
$(ODIR)tty_iin-ssp.o:=	DIR=$(CMN3)
$(ODIR)tty_init.o:=		DIR=$(CMN3)
$(ODIR)tty_int.o:=		DIR=$(CMN3)
$(ODIR)tty_oin-ssp.o:=	DIR=$(CMN3)
$(ODIR)tty_utils.o:=	DIR=$(CMN3)


.KEEP_STATE:
.PRECIOUS:		$(TARGET)

$(ODIR)%.o:	%.c
			$(CC) $(CFLAGS) -o $@ $<
			@echo

$(ODIR)%.o:	$$(DIR)/%.c
			$(CC) $(CFLAGS) -o $@ $<
			@echo

$(ODIR)%.o:	%.s
			$(AS) $(SFLAGS) -o $@ $<
			@echo

$(ODIR)%.o:	$$(DIR)/%.s
			$(AS) $(SFLAGS) -o $@ $<
			@echo

$(TARGET):	$(OBJS)
			$(AR) $(ARFLAGS) $@ $?
			$(RANLIB) $@

clean:
			rm -f $(TARGET) $(OBJS) core 

#
#	MODIFICATION HISTORY:
#
# $Log: Makefile,v $
# Revision 1.5  1993/01/14  23:52:55  johnr
# JR:  Added getutim.o to OBJS.
#
# Revision 1.4  1992/11/25  19:26:08  johnr
# JR:  Got rid of the following .o's in OBJS_X:  ckmode.o, naminit.o,
#      nammap.o, namopen.o, remove.o, and rename.o.
#
# Revision 1.3  1992/11/11  22:51:07  johnr
# JR: Changed name of TARGET, so library goes in the current working directory.
# 	Changed pathnames on includes to delete one ../ layer.  Added tty_
# 	files to OBJS, and to the := list.
#
# Revision 1.2  1992/11/10  23:31:12  johnr
# JR: Changed all instances of 3-50 to 350.
#
# Revision 1.1  1992/11/10  23:16:05  johnr
# JR: Initial entry for this file.
#
#
#
