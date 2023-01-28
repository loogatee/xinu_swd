#
#	$Id: Makefile,v 1.2 1992/11/26 00:22:30 johnr Exp $
#
TARGET		=	all

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
STDINC		=	../../include
PSPINC		=	$(STDINC)/68000
SSPINC		=	$(STDINC)/mio
CONINC		=	.

FORCE		=	MKFORCE
ARCH		=
OPTIM		=	-O
DFLAGS		=
MFLAGS		=
IFLAGS		=	-I$(STDINC) -I$(PSPINC) -I$(SSPINC) -I$(CONINC)
CFLAGS		=	$(ARCH) $(OPTIM) $(DFLAGS) $(MFLAGS) $(IFLAGS) -c
SFLAGS		=	$(ARCH)


OBJS_X		=	conf.o

ODIR		=	./
OBJS		=	$(OBJS_X:%=$(ODIR)%)


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

config:		$(FORCE)
			../config/config.o

clean:
			rm -f $(TARGET) $(OBJS) core 

MKFORCE:;

#
#	MODIFICATION HISTORY:
#
# $Log: Makefile,v $
# Revision 1.2  1992/11/26  00:22:30  johnr
# JR:  Changed ../config/config to ../config/config.o.
#
# Revision 1.1  1992/11/17  17:18:31  johnr
# JR: 1st entry for these files.  All relate to the Xinu configuration for
#     the mio board.
#
#
#
