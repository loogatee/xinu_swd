#
#	$Id: Makefile,v 1.2 1993/01/27 16:31:44 johnr Exp $
#
TARGET		=	libsh_miombc.a

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
CONINC		=	../../config/miombc

ARCH		=
OPTIM		=	-O
DFLAGS		=
MFLAGS		=	-m68000 -msoft-float
IFLAGS		=	-I$(STDINC) -I$(PSPINC) -I$(SSPINC) -I$(CONINC)
CFLAGS		=	$(ARCH) $(OPTIM) $(DFLAGS) $(MFLAGS) $(IFLAGS) -c
SFLAGS		=	$(ARCH)

STDCMN		=	../common
CMN1		=	../68000
CMN2		=	../mio
DIR			=	$(STDCMN)

NOTYET		=	x_dg.o x_dumper.o x_finger.o x_ifstat.o x_net.o \
				x_ns.o x_ping.o x_route.o x_routes.o x_snmp.o x_timerq.o \
				x_uptime.o


OBJS_X		=	ascdate.o cmds.o lexan.o login.o shell.o x_arp.o x_bpool.o \
				x_close.o x_conf.o x_create.o x_date.o x_devs.o \
				x_echo.o x_exit.o \
				x_hd.o x_help.o x_kill.o \
				x_prelog.o x_ps.o x_ptrace.o x_reboot.o \
				x_sleep.o x_spoolq.o \
				x_trcb.o x_trkp.o x_trse.o x_trsp.o \
				x_who.o addarg.o x_mem.o flash.o

ODIR		=	./
OBJS		=	$(OBJS_X:%=$(ODIR)%)

$(ODIR)addarg.o:=	DIR=$(CMN1)
$(ODIR)x_mem.o:=	DIR=$(CMN1)
$(ODIR)cmds.o:=		DIR=$(CMN2)
$(ODIR)flash.o:=	DIR=$(CMN2)


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
# Revision 1.2  1993/01/27  16:31:44  johnr
# JR:  Added flash.o to OBJS.
#
# Revision 1.1  1992/12/22  19:05:16  johnr
# JR:  Initial entry for this files.
#
# Revision 1.3  1992/11/25  19:32:12  johnr
# JR:  Got rid of the following .o's in OBJS_X:  x_mount.o, and x_unmou.o.
#
# Revision 1.2  1992/11/24  17:58:29  johnr
# JR:  Added a macro for CMN2, and made a re-define rule (using CMN2) for
#      finding cmds.c.
#
# Revision 1.1  1992/11/17  17:20:43  johnr
# JR:  Initial version of Makefile for the mio version of the xinu shell.
#
# Revision 1.1  1992/11/13  21:05:22  johnr
# JR: 1st entry for these files.
#
#
#
