#
#	$Id: Makefile,v 1.4 1992/12/22 18:53:43 johnr Exp $
#
TARGET		=	libc-68000.a

MKFILE		=	Makefile
SHELL		=	/bin/sh

CC			=	gcc68
LD			=	gld
AS			=	gas68
CPP			=	cpp
AR			=	gar
RANLIB		=	granlib

#
#	Standard include, and processor-specific include
#
STDINC		=	../../include
PSPINC		=	$(STDCMN)/68000

ARCH		=
OPTIM		=	-O
DFLAGS		=
MFLAGS		=	-m68000 -msoft-float
IFLAGS		=	-I$(STDINC) -I$(PSPINC)
CFLAGS		=	$(ARCH) $(OPTIM) $(DFLAGS) $(MFLAGS) $(IFLAGS) -c
SFLAGS		=	$(ARCH)

STDCMN		=	../common
CMN1		=	../68000
DIR			=	$(STDCMN)

OBJS_X		=	__main.o abs.o atoi.o atol.o bcopy.o bzero.o cksum.o \
				ctype_.o doprnt.o ecvt.o fgets.o fprintf.o fputs.o gets.o \
				index.o mcount.o memchr.o printf.o puts.o qsort.o \
				rand.o rindex.o setjmp.o sprintf.o strcat.o strcmp.o \
				strcpy.o strlen.o strncat.o strncmp.o strncpy.o swab.o \
				blkcmp.o blkcopy.o blkequ.o \
				udivsi3.o umodsi3.o mulsi3.o

$(ODIR)udivsi3.o:=	DIR=$(CMN1)
$(ODIR)umodsi3.o:=	DIR=$(CMN1)
$(ODIR)mulsi3.o:=	DIR=$(CMN1)
$(ODIR)mcount.o:=	DIR=$(CMN1)
$(ODIR)setjmp.o:=	DIR=$(CMN1)


##  LIBOBJS	=	$(OBJS:%=$(TARGET_LIB)(%))


ODIR		=	./
OBJS		=	$(OBJS_X:%=$(ODIR)%)


.KEEP_STATE:
.PRECIOUS:		$(TARGET)

$(ODIR)%.o:	%.c
			$(CC) $(CFLAGS) -o $@ $<

$(ODIR)%.o:	$$(DIR)/%.c
			$(CC) $(CFLAGS) -o $@ $<

$(ODIR)%.o:	%.s
			$(AS) $(SFLAGS) -o $@ $<

$(ODIR)%.o:	$$(DIR)/%.s
			$(AS) $(SFLAGS) -o $@ $<

$(TARGET):	$(OBJS)
			$(AR) $(ARFLAGS) $@ $?
			$(RANLIB) $@

clean:
			rm -f $(TARGET) $(OBJS) *.cof *.sym core 

#
#	MODIFICATION HISTORY:
#
#	$Log: Makefile,v $
# Revision 1.4  1992/12/22  18:53:43  johnr
# JR:  Made the necessary changes to support the addition of mulsi3.o
#      into the library.
#
# Revision 1.3  1992/11/11  17:09:14  johnr
# JR: Added blkcmp, blkequ, and blkcopy to OBJS.
#
# Revision 1.2  1992/11/10  18:43:18  johnr
# JR: Added PSPINC, and valid field for IFLAGS.  Got rid of imath.o, and
#     removed ctype_.o from macro-modifier list.  Added DIR macro-modifier
#     for mcount and setjmp.
#
# Revision 1.1  1992/11/10  16:29:58  johnr
# JR: 1st entry for this file.
#
#
