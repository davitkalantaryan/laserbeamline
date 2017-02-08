# Makefile to generate a DOOCS server
#
# Author:       Michael Winde <Michael.Winde@desy.de>
#               Copyright  2004
#


SVR_VER=\"1.0.0\"
SERVERNAME = laserBeamLine
KEYM=921

DOOCSROOT = /doocs

# DOOCSROOT = ../../../..
MYDOOCSROOT = ../../../..

# to define DOOCSROOT as an absolute path
#include $(DOOCSROOT)/$(DOOCSARCH)/DEFINEDOOCSROOT
include /doocs/DEFINEDOOCSROOT

OBJDIR = $(MYDOOCSROOT)/$(DOOCSARCH)/obj/server/pitz/$(SERVERNAME)_server

SRCDIR = $(MYDOOCSROOT)/source/server/pitz/$(SERVERNAME)_server


CONFM=\"$(SERVERNAME).conf\"

ALLPROGS = \
	$(OBJDIR)/$(SERVERNAME)_server 
	
all: $(ALLPROGS)

# to define the arch dependend things
include /doocs/CONFIG

#        $(OBJDIR)/eq_rpc_server.o \
#        $(OBJDIR)/eq_server.o \

SOURCEOBJ = \
        $(OBJDIR)/Lbl_XYDevice.o \
        $(OBJDIR)/Lbl_XYZ_Device.o \
        $(OBJDIR)/Lbl_Aperture.o \
        $(OBJDIR)/Lbl_Mirror.o \
        $(OBJDIR)/Lbl_WedgePlates.o \
        $(OBJDIR)/StepperM_MICOS.o \
        $(OBJDIR)/MICOS_Controller.o \
        $(OBJDIR)/$(SERVERNAME)_rpc_server.o

# $(OBJDIR)/SMTrapezProfile.o \
# $(OBJDIR)/lbl_Mirror.o \

UTILDIR = $(MYDOOCSROOT)/source/utils
include $(UTILDIR)/makeTimeClass
include $(UTILDIR)/makeNumberedStrings

# DOOCAPIDIR = $(MYDOOCSROOT)/source/clients/pitz/doocsAPI
# include $(DOOCAPIDIR)/makeDoocsAPI
        
IPLIBDIR = ../ipLib
include $(IPLIBDIR)/makeIP500
include $(IPLIBDIR)/makeIPUnidig
        
MOTORLIBDIR = ../motorLib
include $(MOTORLIBDIR)/makeStepperMotorWithProfile
        

SOURCEHFILES = \
	$(SRCDIR)/eq_$(SERVERNAME).h 

INSTBINDIRS = \
	NODENAME:/sect/package/DOOCS/1.0/$(DOOCSARCH)/$(SERVERNAME)_server

LOCALINSTDIR = /sect/package/DOOCS/1.0/$(DOOCSARCH)/$(SERVERNAME)_server


$(OBJDIR)/.depend depend:
		@if [ ! -f $(OBJDIR) ] ; then \
		  echo ---------- create dir $(OBJDIR) --------------; \
		  mkdir -p $(OBJDIR) ; \
		fi
		for i in $(SRCDIR)/*.cc ;do $(CCDEP) $$i ;done > $(OBJDIR)/.depend_temp
		for i in $(IMPORTFILES) ;do $(CCDEP) $$i ;done >> $(OBJDIR)/.depend_temp
		cat $(OBJDIR)/.depend_temp | sed -e "/:/s/^/\$$\(OBJDIR\)\//g" > $(OBJDIR)/.depend
		chmod g+w $(OBJDIR)/.depend*

include $(OBJDIR)/.depend

$(OBJDIR)/$(SERVERNAME)_server:    $(SOURCEOBJ)
		@echo ---------- linking $(OBJDIR)/$(SERVERNAME)_server  --- 
		$(LINK.cc) -g -o $(OBJDIR)/$(SERVERNAME)_server $(SOURCEOBJ) \
		           -lEqServer -lDOOCSapi -lVME\
			   $(LDLIBS) 
		@chmod g+w $(OBJDIR)/$(SERVERNAME)_server
		@echo "----------------$(SERVERNAME) done---------------"

static $(OBJDIR)/static_$(SERVERNAME)_server:    $(SOURCEOBJ)	
		@echo ---------- linking $(OBJDIR)/static_$(SERVERNAME)_server  --- 
		$(LINK.cc.static) -o $(OBJDIR)/static_$(SERVERNAME)_server $(SOURCEOBJ) \
		       -lEqServer -lDOOCSapi  \
			   $(LDLIBS) 
		@chmod g+w $(OBJDIR)/static_$(SERVERNAME)_server
		@echo "----------------$(SERVERNAME) done---------------"


install:  $(ALLPROGS)
		# install in all installdirs
		@for i in $(INSTBINDIRS); do \
		 for p in $(ALLPROGS); do \
		  echo "- INSTALL: rcp $$i/`basename $$p` $$i/`basename $$p`.OLD -" ; \
		  rcp $$i/`basename $$p` $$i/`basename $$p`.OLD ; \
		  rsh `echo $$i | cut -d ':' -f 1` chgrp ttf `echo $$i | cut -d ':' -f 2`/`basename $$p`.OLD ; \
		  echo "- INSTALL: $$p in \n  $$i/`basename $$p` " ; \
		  rdist -c $$p $$i/`basename $$p` ; \
		 done \
		done
	
localinstall: $(ALLPROGS)
		@for p in $(ALLPROGS); do \
		 echo "- INSTALL: cp $(LOCALINSTDIR)/`basename $$p` $(LOCALINSTDIR)/`basename $$p`.OLD -" ; \
		 cp $(LOCALINSTDIR)/`basename $$p` $(LOCALINSTDIR)/`basename $$p`.OLD ; \
		 echo "- INSTALL: $$p in \n  $(LOCALINSTDIR)/`basename $$p` " ; \
		 cp $$p $(LOCALINSTDIR)/`basename $$p` ; \
		done

clean:
	rm -f $(SOURCEOBJ) $(OBJDIR)/*.o $(SRCDIR)/*.ps $(OBJDIR)/$(SERVERNAME)_server $(OBJDIR)/.depend*
	rm -r $(OBJDIR)/SunWS_cache
	
update:
	cvs update

checkin:
	cvs commit
