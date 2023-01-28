#
#	$Id: Makefile,v 1.1 1992/12/22 19:02:48 johnr Exp $
#
TARGET		=	libapps_miombc.a

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
DFLAGS		=	-DBSDURG
MFLAGS		=	-m68000 -msoft-float
IFLAGS		=	-I$(STDINC) -I$(PSPINC) -I$(SSPINC) -I$(CONINC)
CFLAGS		=	$(ARCH) $(OPTIM) $(DFLAGS) $(MFLAGS) $(IFLAGS) -c
SFLAGS		=	$(ARCH)

STDCMN		=	../common
DIR			=	$(STDCMN)

			

OBJS_X		=	spooler.o despool.o testpage.o

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
			$(AR) $(ARFLAGS) $@ $?
			$(RANLIB) $@

clean:
			rm -f $(TARGET) $(OBJS) core 

#
#	MODIFICATION HISTORY:
#
# $Log: Makefile,v $
# Revision 1.1  1992/12/22  19:02:48  johnr
# JR:  1st entry for this Makefile.
#
# Revision 1.1  1992/11/26  00:09:19  johnr
# JR:  Initial version of a working Makefile for building the apps library
#      for the mio board.
#
#
