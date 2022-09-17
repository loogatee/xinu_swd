#
#	$Id: Makefile,v 1.3 1992/11/25 19:30:57 johnr Exp $
#
TARGET		=	libsh_sun350.a

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
DFLAGS		=
MFLAGS		=
IFLAGS		=	-I$(STDINC) -I$(PSPINC) -I$(SSPINC) -I$(CONINC)
CFLAGS		=	$(ARCH) $(OPTIM) $(DFLAGS) $(MFLAGS) $(IFLAGS) -c
SFLAGS		=	$(ARCH)

STDCMN		=	../common
CMN1		=	../68000
CMN2		=	../sun350
DIR			=	$(STDCMN)


OBJS_X		=	ascdate.o cmds.o lexan.o login.o shell.o x_arp.o x_bpool.o \
				x_close.o x_conf.o x_create.o x_date.o x_devs.o \
				x_dg.o x_dumper.o x_echo.o x_enttyb.o x_exit.o x_finger.o \
				x_hd.o x_help.o x_ifstat.o x_kill.o x_net.o \
				x_ns.o x_ping.o x_prelog.o x_ps.o x_ptrace.o x_reboot.o \
				x_route.o x_routes.o x_sleep.o x_snmp.o x_spoolq.o x_timerq.o \
				x_trcb.o x_trkp.o x_trse.o x_trsp.o \
				x_who.o addarg.o x_mem.o

ODIR		=	./
OBJS		=	$(OBJS_X:%=$(ODIR)%)

$(ODIR)addarg.o:=	DIR=$(CMN1)
$(ODIR)x_mem.o:=	DIR=$(CMN1)
$(ODIR)cmds.o:=		DIR=$(CMN2)


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
# Revision 1.3  1992/11/25  19:30:57  johnr
# JR:  Got rid of the following .o's in OBJS_X:  x_mount.o, x_unmou.o,
#      x_uptime.o.
#
# Revision 1.2  1992/11/17  17:20:04  johnr
# JR:  Added macro for CMN2, and made a rule for cmds.o to indicate that it
#      is now found in directory CMN2.
#
# Revision 1.1  1992/11/13  21:05:22  johnr
# JR: 1st entry for these files.
#
#
#
