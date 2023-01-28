#
#	$Id: Makefile,v 1.5 1992/12/02 17:39:20 johnr Exp $
#
TARGET		=	a.out

MKFILE		=	Makefile
SHELL		=	/bin/sh

TAGS		=	/home/johnr/.tags_xinu
TAGD		=	/home/johnr/swd

CC			=	gcc68
LD			=	gld
AS			=	gas68
CPP			=	cpp
AR			=	gar
RANLIB		=	granlib

STDINC		=	../../include
PSPINC		=	$(STDINC)/68000
SSPINC		=	$(STDINC)/sun350
CONINC		=	../../xinu/config/sun350

FORCE		=	MKFORCE
ARCH		=
OPTIM		=	-O
DFLAGS		=
MFLAGS		=
IFLAGS		=	-I$(STDINC) -I$(PSPINC) -I$(SSPINC) -I$(CONINC)
CFLAGS		=	$(ARCH) $(OPTIM) $(DFLAGS) $(MFLAGS) $(IFLAGS) -c
SFLAGS		=	$(ARCH)

LB1			=	../../xinu/ipnet/objs_snmp
LB2         =   ../../libc/objs_68020


SUBDIRS		=	$(LB1) $(LB2)
SUBLIBS		=	-L$(LB1) -L$(LB2)


UNDEFS		=
ENTRY		=	-e _start
LFLAGS		=	-M $(ARCH) -T link.me $(SUBLIBS) $(UNDEFS) $(ENTRY)
LIBS		=	-lsnmp -lc-68020
#LIBS		=	-lsnmp

CONFDIR		=	$(CONINC)
ODIR_SYS	=	../../xinu/sys/objs_sun350
CONFO		=
FIRST		=	$(ODIR_SYS)/startup.o $(CONFO)

OBJS		=	main.o

.KEEP_STATE:

#
#	Rule for building .o's given a .c file
#
%.o:		%.c
			$(CC) $(CFLAGS) $<

#
#	Rule for building .o's given a .s file
#
%.o:		%.s
			rm -f $(<:.s=.c)
			cp $< $(<:.s=.c)
			-$(CC) $(ARCH) -E $(IFLAGS) $(DFLAGS) $(<:.s=.c) | \
				sed 1,/cuthere/d | $(AS) $(ARCH) -o $@
			rm -f $(<:.s=.c)

#
#	Default make target right here.  This target relies on all the
#	sub-libraries already being built.  If they are not already built,
#	invoke "make world" instead.
#
$(TARGET):	$(OBJS) $(FORCE)
			$(LD) -o x.out $(LFLAGS) $(FIRST) $(OBJS) $(LIBS) > x.out.lmap
			cp x.out b.out
			gstrip b.out
			fix_gldo b.out
			mv b.out.fix a.out
			rm b.out

#
#	The "world" target descends to all the sub-directories and forces
#	all of the libraries to be built.  Then it invokes "make" in order
#	to build the output binary in the current working directory.
#
world: 		$(FORCE)
			cd $(LB1); $(MAKE)
			cd $(LB2); $(MAKE)
			cd $(LB3); $(MAKE)
			cd $(LB4); $(MAKE)
			cd $(LB5); $(MAKE)
			cd $(CONFDIR); $(MAKE)
			$(MAKE)

#
#	The "clean" target just cleans up everything in this (current working)
#	directory.
#
clean:
			rm -f $(TARGET) $(OBJS) *.sort *.sym [abx].out*

#
#	The "worldclean" target descends to all of the sub-directories and
#	deletes everything there, as well as everthing in this (current working)
#	directory.
#
worldclean:
			rm -f $(TARGET) $(OBJS) *.sort *.sym [abx].out*
			@for i in $(SUBDIRS); do (cd $$i; $(MAKE) clean); done
			cd $(CONFDIR); $(MAKE) clean

#
#	The "tags" target rebuilds the vi tag database
#
tags:		$(FORCE)
			rm -f $(TAGS)
			touch $(TAGS)
			find $(TAGD) \( -name '*.c' -o -name '*.h' \) -exec \
				ctags -d -t -u -f $(TAGS) {} \;

MKFORCE:;
