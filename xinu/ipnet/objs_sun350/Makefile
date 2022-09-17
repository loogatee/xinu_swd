#
#	$Id: Makefile,v 1.4 1993/04/15 23:25:42 johnr Exp $
#
TARGET		=	libipnet_sun350.a

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
CMN1		=	../sun350
CMN2		=	../common/snmp
DIR			=	$(STDCMN)

			

OBJS_X		=	eth_cntl.o eth_demux.o eth_init.o eth_int.o \
				eth_inter.o eth_strt.o eth_wstrt.o \
				arp_in.o arp_add.o arp_alloc.o arp_dq.o arp_find.o arp_init.o \
				arp_print.o arp_qsend.o arp_send.o arp_timer.o dg_alloc.o \
				dg_close.o dg_cntl.o dg_dump.o dg_init.o dg_mcntl.o \
				dg_mopen.o dg_parse.o dg_read.o dg_write.o eth_write.o \
				icmp_.o icmp_errok.o icmp_in.o icmp_redir.o icmp_setbuf.o \
				icmp_setdata.o icmp_setsrc.o ip_dbc.o ip_doopts.o \
				ip_dstopts.o ip_fadd.o ip_fcons.o ip_fhcopy.o ip_finit.o \
				ip_fjoin.o ip_fsend.o ip_ftimer.o ip_getp.o ip_h2net.o \
				ip_in.o ip_isbrc.o ip_net2h.o ip_proc.o ip_putp.o ip_reass.o \
				ip_redir.o ip_send.o \
				net_dnparse.o net_dot2ip.o net_dump.o \
				net_getaddr.o net_getname.o net_getnet.o \
				net_inithost.o net_ip2dot.o net_ip2name.o net_locout.o \
				net_mask.o net_match.o net_name2ip.o net_niin.o net_num.o \
				net_rarpin.o net_rarpsend.o net_setmask.o net_slowtim.o \
				net_start.o net_write.o rip_add.o rip_check.o rip_ifset.o \
				rip_in.o rip_metric.o rip_ok.o rip_out.o rip_recv.o \
				rip_repl.o rip_send.o rip_start.o rt_add.o rt_del.o rt_dump.o \
				rt_free.o rt_get.o rt_hash.o rt_init.o rt_new.o rt_timer.o \
				tcp_abort.o tcp_acked.o tcp_ackit.o tcp_cksum.o tcp_closed.o \
				tcp_closing.o tcp_clwait.o tcp_data.o tcp_demux.o tcp_dodat.o \
				tcp_estab.o tcp_fin1.o tcp_fin2.o tcp_h2net.o tcp_howmuch.o \
				tcp_idle.o tcp_in.o tcp_inp.o tcp_iss.o tcp_kick.o \
				tcp_killtim.o tcp_lastack.o tcp_listen.o tcp_net2h.o tcp_ok.o \
				tcp_opts.o tcp_ostate.o tcp_out.o tcp_persist.o tcp_rcvurg.o \
				tcp_reset.o tcp_rexmt.o tcp_rmss.o tcp_rtt.o tcp_rwindow.o \
				tcp_send.o tcp_shskip.o tcp_smss.o tcp_sndlen.o tcp_sndurg.o \
				tcp_swindow.o tcp_switch.o tcp_sync.o tcp_synrcvd.o \
				tcp_synsent.o tcp_tfcoal.o tcp_tfinsert.o tcp_timer.o \
				tcp_timewait.o tcp_tmclear.o tcp_tmleft.o tcp_tmset.o \
				tcp_tqdump.o tcp_uqalloc.o tcp_uqfree.o tcp_uqinit.o \
				tcp_uqprint.o tcp_wait.o tcp_wakeup.o tcp_winit.o tcp_xmit.o \
				tcpd_addhole.o tcpd_alloc.o tcpd_bind.o tcpd_close.o \
				tcpd_cntl.o tcpd_con.o tcpd_dalloc.o tcpd_getc.o \
				tcpd_getdata.o tcpd_gspace.o tcpd_init.o tcpd_lq.o \
				tcpd_mcntl.o tcpd_mopen.o tcpd_nxtp.o tcpd_putc.o tcpd_read.o \
				tcpd_rhskip.o tcpd_rurg.o tcpd_server.o tcpd_stat.o \
				tcpd_uopt.o tcpd_wr.o tcpd_write.o tcpd_wurg.o udp_alloc.o \
				udp_cksum.o udp_h2net.o udp_in.o udp_net2h.o udp_nxtp.o \
				udp_send.o snmpd.o \
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

$(ODIR)eth_cntl.o:=		DIR=$(CMN1)
$(ODIR)eth_demux.o:=	DIR=$(CMN1)
$(ODIR)eth_init.o:=		DIR=$(CMN1)
$(ODIR)eth_int.o:=		DIR=$(CMN1)
$(ODIR)eth_inter.o:=	DIR=$(CMN1)
$(ODIR)eth_strt.o:=		DIR=$(CMN1)
$(ODIR)eth_wstrt.o:=	DIR=$(CMN1)
$(ODIR)snmpd.o:=		DIR=$(CMN2)
$(ODIR)a1rwint.o:=		DIR=$(CMN2)
$(ODIR)a1rwlen.o:=		DIR=$(CMN2)
$(ODIR)a1rwoid.o:=		DIR=$(CMN2)
$(ODIR)a1rwval.o:=		DIR=$(CMN2)
$(ODIR)mksnmp.o:=		DIR=$(CMN2)
$(ODIR)sae_findn.o:=	DIR=$(CMN2)
$(ODIR)sae_get.o:=		DIR=$(CMN2)
$(ODIR)sae_getf.o:=		DIR=$(CMN2)
$(ODIR)sae_getn.o:=		DIR=$(CMN2)
$(ODIR)sae_match.o:=	DIR=$(CMN2)
$(ODIR)sae_set.o:=		DIR=$(CMN2)
$(ODIR)sat_findn.o:=	DIR=$(CMN2)
$(ODIR)sat_get.o:=		DIR=$(CMN2)
$(ODIR)sat_getf.o:=		DIR=$(CMN2)
$(ODIR)sat_getn.o:=		DIR=$(CMN2)
$(ODIR)sat_match.o:=	DIR=$(CMN2)
$(ODIR)sat_set.o:=		DIR=$(CMN2)
$(ODIR)sif_get.o:=		DIR=$(CMN2)
$(ODIR)sif_getf.o:=		DIR=$(CMN2)
$(ODIR)sif_getn.o:=		DIR=$(CMN2)
$(ODIR)sif_match.o:=	DIR=$(CMN2)
$(ODIR)sif_set.o:=		DIR=$(CMN2)
$(ODIR)sna2b.o:=		DIR=$(CMN2)
$(ODIR)snb2a.o:=		DIR=$(CMN2)
$(ODIR)snerr.o:=		DIR=$(CMN2)
$(ODIR)snfreebl.o:=		DIR=$(CMN2)
$(ODIR)snhash.o:=		DIR=$(CMN2)
$(ODIR)sninit.o:=		DIR=$(CMN2)
$(ODIR)snleaf.o:=		DIR=$(CMN2)
$(ODIR)snmib.o:=		DIR=$(CMN2)
$(ODIR)snoip.o:=		DIR=$(CMN2)
$(ODIR)snparse.o:=		DIR=$(CMN2)
$(ODIR)snprint.o:=		DIR=$(CMN2)
$(ODIR)snrslv.o:=		DIR=$(CMN2)
$(ODIR)sntable.o:=		DIR=$(CMN2)
$(ODIR)srt_findn.o:=	DIR=$(CMN2)
$(ODIR)srt_get.o:=		DIR=$(CMN2)
$(ODIR)srt_getf.o:=		DIR=$(CMN2)
$(ODIR)srt_getn.o:=		DIR=$(CMN2)
$(ODIR)srt_match.o:=	DIR=$(CMN2)
$(ODIR)srt_set.o:=		DIR=$(CMN2)
$(ODIR)stc_findn.o:=	DIR=$(CMN2)
$(ODIR)stc_get.o:=		DIR=$(CMN2)
$(ODIR)stc_getf.o:=		DIR=$(CMN2)
$(ODIR)stc_getn.o:=		DIR=$(CMN2)
$(ODIR)stc_match.o:=	DIR=$(CMN2)
$(ODIR)stc_set.o:=		DIR=$(CMN2)


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
# Revision 1.4  1993/04/15  23:25:42  johnr
# JR:  Added SNMP stuff to the Makefile.
#
# Revision 1.3  1993/01/14  23:37:14  johnr
# JR:  Got rid of net_getutim.o in the OBJS macro.  This file has been moved
#      to the sys hierarchy.
#
# Revision 1.2  1992/11/26  00:10:59  johnr
# JR:  Took out all of the nap_ files.
#
# Revision 1.1  1992/11/13  23:36:15  johnr
# JR: 1st entry for this file.
#
#
