#
#	$Id: Makefile,v 1.4 1993/04/15 23:25:42 johnr Exp $
#
TARGET		=	libsnmp.a

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

STDCMN		=	../common/snmp
DIR			=	$(STDCMN)

			

OBJS_X		=	snmpd.o \
				a1rwint.o a1rwlen.o a1rwoid.o a1rwval.o mksnmp.o sae_findn.o \
				sae_get.o sae_getf.o sae_getn.o sae_match.o sae_set.o \
				sat_findn.o sat_get.o sat_getf.o sat_getn.o sat_match.o \
				sat_set.o sif_get.o sif_getf.o sif_getn.o sif_match.o \
				sif_set.o sna2b.o snb2a.o snerr.o snfreebl.o \
				snhash.o sninit.o snleaf.o snmib.o snoip.o snparse.o \
				snprint.o snrslv.o sntable.o srt_findn.o srt_get.o srt_getf.o \
				srt_getn.o srt_match.o srt_set.o stc_findn.o stc_get.o \
				stc_getf.o stc_getn.o stc_match.o stc_set.o


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
# $Log$
#
