#
#	$Id: Makefile,v 1.4 1992/11/11 17:09:46 johnr Exp $
#
TARGET		=	libc-68020.a

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
PSPINC		=	$(STDINC)/68000

ARCH		=
OPTIM		=	-O
DFLAGS		=
MFLAGS		=
IFLAGS		=	-I$(STDINC) -I$(PSPINC)
CFLAGS		=	$(ARCH) $(OPTIM) $(DFLAGS) $(MFLAGS) $(IFLAGS) -c
SFLAGS		=	$(ARCH)

STDCMN		=	../common
CMN1		=	../68020
CMN2		=	../68000
DIR			=	$(STDCMN)

OBJS_X		=	__main.o abs.o atoi.o atol.o bcopy.o bzero.o cksum.o \
				ctype_.o doprnt.o ecvt.o fgets.o fprintf.o fputs.o gets.o \
				index.o mcount.o memchr.o printf.o puts.o qsort.o \
				rand.o rindex.o setjmp.o sprintf.o strcat.o strcmp.o \
				strcpy.o strlen.o strncat.o strncmp.o strncpy.o swab.o \
				blkcmp.o blkcopy.o blkequ.o \
				aldiv.o almul.o alrem.o auldiv.o aulmul.o aulrem.o ldiv.o \
				lmul.o lrem.o uldiv.o ulmul.o ulrem.o

$(ODIR)aldiv.o:=	DIR=$(CMN1)
$(ODIR)almul.o:=	DIR=$(CMN1)
$(ODIR)alrem.o:=	DIR=$(CMN1)
$(ODIR)auldiv.o:=	DIR=$(CMN1)
$(ODIR)aulmul.o:=	DIR=$(CMN1)
$(ODIR)aulrem.o:=	DIR=$(CMN1)
$(ODIR)ldiv.o:=		DIR=$(CMN1)
$(ODIR)lmul.o:=		DIR=$(CMN1)
$(ODIR)lrem.o:=		DIR=$(CMN1)
$(ODIR)uldiv.o:=	DIR=$(CMN1)
$(ODIR)ulmul.o:=	DIR=$(CMN1)
$(ODIR)ulrem.o:=	DIR=$(CMN1)
$(ODIR)mcount.o:=	DIR=$(CMN2)
$(ODIR)setjmp.o:=	DIR=$(CMN2)


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
# Revision 1.4  1992/11/11  17:09:46  johnr
# JR: Added blkequ, blkcmp, and blkcopy to OBJS.
#
# Revision 1.3  1992/11/10  18:44:11  johnr
# JR: Added memchr.o to OBJS.
#
# Revision 1.2  1992/11/10  18:13:29  johnr
# JR: Added PSPINC, and a valid field for IFLAGS.  Added CMN2, and changed
#     location of source for mcount, and setjmp.
#
# Revision 1.1  1992/11/10  16:30:34  johnr
# JR: 1st entry for this file.
#
#
