#
#	$Id: Makefile,v 1.2 1992/12/02 17:37:51 johnr Exp $
#
TARGET		=	libapps_sun350.a

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
DIR			=	$(STDCMN)

			

OBJS_X		=	despool.o lpd.o spooler.o tftpd.o rlogind.o

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
# Revision 1.2  1992/12/02  17:37:51  johnr
# JR:  Added rlogind.o to OBJS_X.
#
# Revision 1.1  1992/11/26  00:10:03  johnr
# JR:  Initial version of a working Makefile for building the apps library
#      for the sun3/50.
#
#
