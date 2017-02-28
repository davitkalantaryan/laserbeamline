
OS		= $(shell uname -s)
#ifeq ($(OS),Linux)
#	CPP = g++
#	LIBS= -O2 -lpthread -lrt -lresolv -lnsl -lADCShm -lADCDma -L$(DOOCS)/$(SYSTEM)/lib
#	CFLAGS= -O2 -DLINUX -D_REENTRANT -D_GNU_SOURCE -D_BSD -D__USE_GNU -D__USE_UNIX98 -I$(DOOCS)/develop/kalantar/$(SYSTEM)/lib/include -I$(DOOCS)/$(SYSTEM)/lib/include
#else
	CPP = CC
	LINK = CC
	CFLAGS= -O2 -DLINUX -D_REENTRANT -D_GNU_SOURCE -D_BSD -D__USE_GNU -D__USE_UNIX98 -I/doocs/lib/include -I../../../src/additional_sources -DSOLARIS -I../../../src/original_from_mwinde -I../../../src/server
	LFLAGS  = -O2
	#LIBS = -O2 -lpthread -lrt -lresolv -lsocket -lnsl -L/doocs/lib -lADCShm -L/doocs/lib -lADCDma
	#LIBS = -O2 -lpthread -lrt -lresolv -lsocket -lnsl
	#LIBS = -lsocket -lnsl -lpthread -lrt -L/doocs/lib -lADCShm -lADCDma -L$(MY_ROOT)/$(SYSTEM)/lib -ladcsendertcp
	#LIBS = -lsocket -lnsl -lpthread -lrt -L/doocs/lib -lADCShm -lADCDma
	#LIBS = -lsocket -lnsl -lpthread -lrt /local/lib/libADCShm.a /local/lib/libADCDma.a
	#LIBS = -L/doocs/lib -lEqServer -lDOOCSapi -lVME -lsocket -lnsl -lpthread -lrt
	LIBS = -L/doocs/lib -lEqServer -lDOOCSapi -lsocket -lnsl -lpthread -lrt
#endif



all: laserbeamline_server


##########################################################################################################
IP500_Acromag.o:
	$(CPP) -c $(CFLAGS) -o IP500_Acromag.o		../../../src/original_from_mwinde/IP500_Acromag.cc	
IP501_Acromag.o:
	$(CPP) -c $(CFLAGS) -o IP501_Acromag.o		../../../src/original_from_mwinde/IP501_Acromag.cc
IPModule.o:
	$(CPP) -c $(CFLAGS) -o IPModule.o		../../../src/original_from_mwinde/IPModule.cc
IPUnidig.o:
	$(CPP) -c $(CFLAGS) -o IPUnidig.o		../../../src/original_from_mwinde/IPUnidig.cc
laserBeamLine_rpc_server.o:
	$(CPP) -c $(CFLAGS) -o laserBeamLine_rpc_server.o	../../../src/original_from_mwinde/laserBeamLine_rpc_server.cc	
Lbl_Aperture.o:
	$(CPP) -c $(CFLAGS) -o Lbl_Aperture.o		../../../src/original_from_mwinde/Lbl_Aperture.cc	
Lbl_Mirror.o:
	$(CPP) -c $(CFLAGS) -o Lbl_Mirror.o		../../../src/original_from_mwinde/Lbl_Mirror.cc	
Lbl_WedgePlates.o:
	$(CPP) -c $(CFLAGS) -o Lbl_WedgePlates.o	../../../src/original_from_mwinde/Lbl_WedgePlates.cc
Lbl_XYDevice.o:
	$(CPP) -c $(CFLAGS) -o Lbl_XYDevice.o		../../../src/original_from_mwinde/Lbl_XYDevice.cc	
Lbl_XYZ_Device.o:
	$(CPP) -c $(CFLAGS) -o Lbl_XYZ_Device.o		../../../src/original_from_mwinde/Lbl_XYZ_Device.cc	
messageQ.o:
	$(CPP) -c $(CFLAGS) -o messageQ.o		../../../src/original_from_mwinde/messageQ.cc	
MICOS_Controller.o:
	$(CPP) -c $(CFLAGS) -o MICOS_Controller.o	../../../src/original_from_mwinde/MICOS_Controller.cc	
NumberedStrings.o:
	$(CPP) -c $(CFLAGS) -o NumberedStrings.o	../../../src/original_from_mwinde/NumberedStrings.cc	
simpleThreads.o:
	$(CPP) -c $(CFLAGS) -o simpleThreads.o		../../../src/original_from_mwinde/simpleThreads.cc	
SMProfile.o:
	$(CPP) -c $(CFLAGS) -o SMProfile.o		../../../src/original_from_mwinde/SMProfile.cc	
SMTrapezProfile.o:
	$(CPP) -c $(CFLAGS) -o SMTrapezProfile.o	../../../src/original_from_mwinde/SMTrapezProfile.cc	
StepperM_MICOS.o:
	$(CPP) -c $(CFLAGS) -o StepperM_MICOS.o		../../../src/original_from_mwinde/StepperM_MICOS.cc	
StepperMotor.o:
	$(CPP) -c $(CFLAGS) -o StepperMotor.o		../../../src/original_from_mwinde/StepperMotor.cc	
timeClass.o:
	$(CPP) -c $(CFLAGS) -o timeClass.o		../../../src/original_from_mwinde/timeClass.cc	

vme_functions_simul.o:
	$(CPP) -c $(CFLAGS) -o vme_functions_simul.o	../../../src/additional_sources/vme_functions_simul.cc
VME2.o:
	$(CPP) -c $(CFLAGS) -o VME2.o			/doocs/develop/kalantar/doocs.git/doocs/library/common/IOBUSlib/VME2.cc
pitz_eqfctmirroronedim.o:
	$(CPP) -c $(CFLAGS) -o pitz_eqfctmirroronedim.o		../../../src/server/pitz_eqfctmirroronedim.cpp
	


laserbeamline_server:	IP500_Acromag.o IP501_Acromag.o IPModule.o IPUnidig.o laserBeamLine_rpc_server.o Lbl_Aperture.o Lbl_Mirror.o Lbl_WedgePlates.o \
			Lbl_XYDevice.o Lbl_XYZ_Device.o messageQ.o MICOS_Controller.o NumberedStrings.o \
			simpleThreads.o SMProfile.o SMTrapezProfile.o StepperM_MICOS.o StepperMotor.o timeClass.o VME2.o pitz_eqfctmirroronedim.o
			
						@echo "*** Linking laserbeamline_server"
						$(LINK) *.o $(LIBS) $(LFLAGS) -o laserbeamline_server
						@mkdir -p ../../../sys/solaris/.objects
						@mkdir -p ../../../sys/solaris/bin
						@mv laserbeamline_server ../../../sys/solaris/bin/.
						@mv *.o ../../../sys/solaris/.objects/.
						

# Rule for generating object files from C++ source files
.cpp.o: 
	@echo "*** Compiling $<"
	$(CPP) -c $(CFLAGS) $<
	#$(CPP) -c $(CFLAGS) -o "$@" "$<"
	#$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

clean:	
	@rm -f ../../../sys/solaris/bin/laserbeamline_server ../../../sys/solaris/.objects/*.o

