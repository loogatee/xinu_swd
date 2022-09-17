#
#	$Id: Makefile,v 1.3 1992/11/26 00:23:03 johnr Exp $
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
SSPINC		=	$(STDINC)/sun350
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
# Revision 1.3  1992/11/26  00:23:03  johnr
# JR:  Changed ../config/config to ../config/config.o.
#
# Revision 1.2  1992/11/17  17:25:55  johnr
# JR:  Added a "config" target,  make config now caused a new conf.c and
#      and new conf.h to be generated.
#
# Revision 1.1  1992/11/13  21:03:36  johnr
# JR: 1st entry for these files.
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
