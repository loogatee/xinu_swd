#
#	$Id: Makefile,v 1.5 1993/01/14 23:52:10 johnr Exp $
#
TARGET		=	libsys_mio.a

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
SSPINC		=	$(STDINC)/mio
CONINC		=	../../config/mio

ARCH		=
OPTIM		=	-O
DFLAGS		=	-DBSDURG
MFLAGS		=	-m68000 -msoft-float
IFLAGS		=	-I$(STDINC) -I$(PSPINC) -I$(SSPINC) -I$(CONINC)
CFLAGS		=	$(ARCH) $(OPTIM) $(DFLAGS) $(MFLAGS) $(IFLAGS) -c
SFLAGS		=	$(ARCH)

STDCMN		=	../common
CMN1		=	../68000
CMN2		=	../mio
DIR			=	$(STDCMN)


OBJS_X		=	clkint.o chprio.o startup.o initialize.o evec.o \
				close.o control.o create.o ctxsw.o clkinit.o common_isr.o \
				freebuf.o freemem.o getbuf.o getc.o ret_mon.o \
				getitem.o getmem.o getpid.o getprio.o kprintf.o \
				getstk.o gettime.o gpq.o init.o insert.o \
				insertd.o intr.o ioerr.o ionull.o kill.o mark.o \
				mkdpool.o mkpool.o \
				newqueue.o open.o panic.o pcount.o pcreate.o pdelete.o \
				pinit.o poolinit.o preceive.o preset.o psend.o \
				ptclear.o putc.o queue.o read.o ready.o receive.o recvclr.o \
				recvtim.o resched.o resume.o \
				scount.o screate.o sdelete.o seek.o send.o sendf.o \
				setdev.o setnok.o signal.o signaln.o sleep.o sleep10.o \
				sreset.o ssclock.o suspend.o trap.o unsleep.o \
				userret.o wait.o wakeup.o write.o xdone.o trace.o \
				errlog.o tty_cntl.o tty_getc.o tty_iin.o tty_oin.o tty_putc.o \
				tty_read.o tty_write.o tty_init.o tty_utils.o tty_iin-ssp.o \
				tty_oin-ssp.o \
				getname.o getutim.o

ODIR		=	./
OBJS		=	$(OBJS_X:%=$(ODIR)%)

$(ODIR)clkinit.o:=		DIR=$(CMN2)
$(ODIR)clkint.o:=		DIR=$(CMN2)
$(ODIR)create.o:=		DIR=$(CMN1)
$(ODIR)ctxsw.o:=		DIR=$(CMN1)
$(ODIR)evec.o:=			DIR=$(CMN2)
$(ODIR)getstk.o:=		DIR=$(CMN1)
$(ODIR)initialize.o:=	DIR=$(CMN2)
$(ODIR)common_isr.o:=	DIR=$(CMN2)
$(ODIR)intr.o:=			DIR=$(CMN1)
$(ODIR)kprintf.o:=		DIR=$(CMN2)
$(ODIR)resched.o:=		DIR=$(CMN1)
$(ODIR)ret_mon.o:=		DIR=$(CMN2)
$(ODIR)startup.o:=		DIR=$(CMN2)
$(ODIR)trap.o:=			DIR=$(CMN1)
$(ODIR)tty_iin-ssp.o:=	DIR=$(CMN2)
$(ODIR)tty_init.o:=		DIR=$(CMN2)
$(ODIR)tty_oin-ssp.o:=	DIR=$(CMN2)
$(ODIR)tty_utils.o:=	DIR=$(CMN2)
$(ODIR)getname.o:=		DIR=$(CMN2)


.KEEP_STATE:

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
# Revision 1.5  1993/01/14  23:52:10  johnr
# JR:  Got rid of the DIR macro re-define for getutim.o.  This file is
#      now in the common directory, so there is no need for the macro
#      re-define.
#
# Revision 1.4  1992/11/25  19:27:33  johnr
# JR:  Got rid of the following .o's:  ckmode.o, naminit.o, nammap.o, namopen.o,
#      remove.o, and rename.o
#
# Revision 1.3  1992/11/24  17:49:24  johnr
# JR: Got rid of the O_NOTYET macro, and moved the O_NOTYET objects to
#     OBJS_X.  Modified some of the macro re-define rules.
#
# Revision 1.2  1992/11/11  23:44:07  johnr
# JR: Changed TARGET so that the library is made in the current working
#     directory.  Changed pathname of includes to remove one layer of ../
#     Added tty_ filename to OBJS.
#
# Revision 1.1  1992/11/11  17:19:36  johnr
# JR: Initial entry for this file.
#
#
#
