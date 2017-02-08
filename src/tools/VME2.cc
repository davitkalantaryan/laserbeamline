//file: VME.cc
//
// to test the Config class
//
// Kay Rehlich -PVAK-
// last update:
// 2. Mar. 1993
//	   1995		new version for SBus device driver & dev 
//			nodes like /dev/vme24d16
// 21. Aug. 1995	signal -> sigaction + restore siganl env.

//
//
// $Date$
// $Source$
// $Revision$
// $State$
//
// $Log$
// Revision 1.11  2014/05/06 10:44:13  ohensler
// removed fprintf calls
//
// Revision 1.10  2013/06/06 09:09:09  ohensler
// moved VME_table constructor to .cc
//
// Revision 1.9  2013/05/13 10:33:32  ohensler
// removed several new warnings
//
// Revision 1.8  2013/03/18 14:13:55  ohensler
// some changes for const char*
//
// Revision 1.7  2013/02/07 07:40:49  ohensler
// removed some compiler warnings
//
// Revision 1.6  2013-01-31 09:12:20  ohensler
// removed several .h files
//
// Revision 1.5  2012/10/25 13:26:41  wilksen
// Corrected Darwin target
//
// Revision 1.4  2012/07/12 07:11:09  ohensler
// fixed several sprintf warnings
//
// Revision 1.3  2011/07/29 12:04:01  ohensler
// first version for Adam
//
// Revision 1.2  2011/07/29 07:31:52  ohensler
// new IOBUSlib 1.0.6
//
// Revision 1.1  2011/06/23 12:12:15  ohensler
// initial version
//
// Revision 1.15  2011/06/22 15:28:58  petros
// added SPI interfaces
//
// Revision 1.14  2011-06-22 13:20:02  petros
// added SPI interfaces
//
// Revision 1.13  2009-11-09 13:46:44  ohensler
// new VMElib 4.2.0
//
// Revision 1.12  2007-12-17 09:08:15  ohensler
// new VMElib 4.1.1
//
// Revision 1.11  2006/10/30 13:55:25  ohensler
// new VMElib 4.1.0
//
// Revision 1.10  2005/11/04 11:02:18  petros
// added cher read/write
//
// Revision 1.9  2005/10/14 14:33:27  ohensler
// new VMElib 4.0.6
//
// Revision 1.8  2005/06/15 08:53:54  petros
// added d8 access
//
// Revision 1.7  2004/04/22 07:33:25  petros
// remove printf
//
// Revision 1.6  2004/01/07 15:30:15  petros
// new VMElib 4.0.0
//
// Revision 1.5  2004/01/07 14:26:27  petros
// added signal handling functions
//
// Revision 1.4  2003/07/23 11:36:25  ohensler
// new VMElib 3.7.0
//
// Revision 1.3  2001/10/11 12:28:58  ohensler
// new VMElib 3.6.0
//
// Revision 1.2  2001/04/25 16:52:27  bagrat
// Remove the MkvString device_name member of class VME
//
// Revision 1.1.1.1  1995/11/03 12:24:23  grygiel
// DOOCS sources
//
// Revision 1.3  1995/10/30 09:39:29  ohensler
// SunOS version added
//
// Revision 1.2  1995/10/27 13:45:44  grygiel
// olaf has change some things
//
// Revision 1.1  1995/10/24 14:29:00  xgrygiel
// Initial revision
//
//
//

#ifdef VMEBUS
#include <sys/vme_types.h>
#include <sys/vme.h>
#include <sys/vui.h>
#include <themis/vmedmaio.h>
#endif
#include "VME.h"
#include "printtostderr.h"

#include <sys/ioctl.h>
#include <errno.h>
#include <sys/mman.h>
#ifdef SYSV
#include <sys/systeminfo.h>
#endif
#include <signal.h>
#include <setjmp.h>
#include <fcntl.h>
#include <unistd.h>
#include <mtp.h>

#if defined (__GNUC__) && !defined(__APPLE__)
	typedef __sighandler_t SIG_PF;
#else
	#ifndef SA_RESTART
		#define SA_RESTART 0x00000004
		#undef SIG_FUNC_TYP
		#undef SIG_PF
		typedef void    SIG_FUNC_TYP (int,DOTDOTDOT);
		typedef SIG_FUNC_TYP*   SIG_PF;
	#else
		#undef SIG_PF

	#if     OS == 4
		typedef void    	 SIG_FUNC_TYP2 (void);

	#elif   OS == 6
		typedef void    	 SIG_FUNC_TYP2 (int);

	#elif   OS == 7
        	typedef void             SIG_FUNC_TYP2 (int);

	#else
		typedef void    	 SIG_FUNC_TYP2 (int);
	#endif
		typedef SIG_FUNC_TYP2*   SIG_PF;
	#endif
#endif

#ifndef __GNUC__
extern	"C" int gethostname (char *, int);
#endif

const int MAP_OFFSETS = 0x400000;

static	struct sigaction	VME_bus_error_act;	// signal parameter
static	struct sigaction	VME_bus_error_set_act;	// signal parameter
static	struct sigaction	VME_old_act;
static	sigjmp_buf		VME_jmp_env;
sigset_t msk;
sigset_t msk_old;

volatile int	vme_err = 0;

void set_vmer (int &vmerr)
{
	vmerr = -1;
}

extern "C" void	VME_on_bus_error(int)
{
	siglongjmp (VME_jmp_env, 1);
}

extern "C" void	VME_set_bus_error(int)
{
	vme_err = -1;
}

VME*	vme_;

void	insert_module(int)
{
	VME_table*	tmp_tab;
	tmp_tab = new VME_table(vme_->str_tmp, vme_->temp[MAP], 
				vme_->temp[START_ADR],
				vme_->temp[ADR_SPACE],
				vme_->temp[A31], vme_->temp[AM]);
	vme_->top_of_table = tmp_tab->set_next(vme_->top_of_table);
};

VME_table::VME_table (std::string& mn, int map, int sta,  int spac, int a31, int a23)
{
    module_name = mn;
    std::transform (module_name.begin (),
                    module_name.end (),
                    module_name.begin (),
                    toupper );

    map_no = map; start_addr = sta; space = spac;
    d16_d32_am = a31; a16_a32_am = a23; virt_page_start = 0; virt_start = 0;
}


VME::VME ()
{
    unsigned int  ptrn  = 0x01020304;
    unsigned char *byte = (unsigned char*)&ptrn;

    const char*	list[] = {
                    "$VME_CONF",	// search for environment variable
                    "../config",	// go one up and then into config
                    ".",		// inside current directory
                    "../../../../../source/server/config",
                    NULL		// terminator
                    };

    top_of_table = 0;
    /*check for swapping*/
     if((*byte ) == 0x04 ) {
      mswap = 1;
      printtostderr("VME::VME", "byte swapping needed");
    }
    else{
      mswap = 0;
      printtostderr("VME::VME", "byte swapping not needed");
    }
    cc = "SBUSINTERFACE";
    #ifdef SYSV
            sysinfo( SI_HOSTNAME, host, 20);
    #else
            gethostname (host, 20);
    #endif
    fname = "VME.";
    fname = (fname + (char*)host);
    fname = (fname + ".conf");
    conf = new Config( fname.c_str(), list );

	conf->insert (
		new ConfigStart  (
			new std::string	("VME_CONFIG:")
		) );
	conf->insert (
		new ConfigInt    (
			new std::string	("SBUS_SLOT:"),	
			&sbus_slot, 
			1
		));
	conf->insert (
		new ConfigIntArray(
			new std::string	("ADR_MAP:"),	
                        &temp_found,
			temp, 
			(int)MAX_ARRAY
		));
	conf->insert (
		new ConfigString (
			new std::string	("MODULE_NAME:"),
			&str_tmp, 
			new std::string("null")
		));
	vme_	= this;
	PF_callback	pfp = (PF_callback)&insert_module;
	conf->insert (
		new ConfigCallback (
			new std::string	("}"),
			pfp, 
			0
		));
	conf->insert (
		new ConfigCallback (
			new std::string	("{"),
			(PF_callback) 0, 	// dummy
			0
		));
	

	conf->read();				// read file and set values

	ConfigSts i = conf->status();		// return status
	if ( i == CONF_ERR) {
	   char	tmp[200];
		error_cond = 1;
		snprintf (tmp, sizeof(tmp) ,"Error in reading config file = %s", (const char*)fname.c_str());
		printtostderr( "VME::VME", tmp );
	} else	error_cond = 0;


	errno = 0;
	#ifdef __GNUC__
		int test = open("/dev/vme_a24a16_user", O_RDWR );
	#else
		int test = open("/dev/vme24d16", O_RDWR );
	#endif
	if( errno != 2 ) {	// errno == 2 => "No such file or directory"
		//
		// the controller chip on FORCE board
		// needs no open of SBUS
		//
		cc_vpage_start = (caddr_t)1;
	}
	if( errno == 0 ) close( test );

	#ifdef SYSV
		page_size = sysconf( _SC_PAGESIZE );
	#else
		page_size	= getpagesize();
	#endif
#ifdef PCIEBUS
//
		// the controller chip on INTEL_LINUX board
		// needs no open of SBUS
		//
		cc_vpage_start = (caddr_t)1;
#endif
	vpage_start= 0;
}


caddr_t	VME::map_device (const char* name)
{

    VME_table*	act_tab;
    u_long		reg_addr;
    size_t		reg_length;
    char		tmp[200];
    static std::vector<struct vme_addr>  addr_list;
    struct vme_addr addr_;

    if (error_cond)	return (caddr_t) -1;

    if (!cc_vpage_start) {
        // map crate controller "SBUSINTERFACE" :
        cc_vpage_start = (caddr_t)(-1);
        cc_vpage_start = VME::map_device( (const char*)cc.c_str() );
        if (cc_vpage_start == (caddr_t)(-1)) {
            snprintf(tmp, sizeof(tmp), "VME: can't find = %s",(const char*)cc.c_str() );
            printtostderr( "VME::map_device", tmp );
            error_cond = 1;
            return (caddr_t) -1;
        }
    }

    act_tab = VME::find_name(name);		// find module "name" :
    if (!act_tab) {
        snprintf (tmp, sizeof(tmp),"VME: can't find = %s", name);
        printtostderr( "VME::map_device", tmp );
        error_cond = 1;
        return (caddr_t) -1;
    }

    if (act_tab->virt_start)
        return act_tab->virt_start;	// is it already mapped  :

    reg_addr	= act_tab->start_addr;
    reg_length 	= (size_t)act_tab->space;

    page_start	= (reg_addr / page_size) * page_size;
    page_offset	= reg_addr % page_size;

    /* map in the SBus space	*/
    errno = 0;
    int test = open("/dev/vme24d16", O_RDWR );
    if( errno != 2 ) {	// errno == 2 => "No such file or directory"
        if( errno == 0 ) close( test );

        //
        // open device driver on Solaris2
        //

        int i_am = act_tab->a16_a32_am;
        int i_dm = act_tab->d16_d32_am;
        i_am 	 = (i_am & 0x30) / 16;
        switch (i_am) {
            case 0 :
                switch (i_dm){
                    case 8 :
                            snprintf (dev_name, sizeof(dev_name), "/dev/vme32d8" );
                            filedes = open("/dev/vme32d8", O_RDWR );
                            break;
                    case 16 :
                            snprintf (dev_name, sizeof(dev_name), "/dev/vme32d16" );
                            filedes = open("/dev/vme32d16", O_RDWR );
                            break;
                    case 24 :
                            snprintf (dev_name, sizeof(dev_name), "/dev/vme32d24" );
                            filedes = open("/dev/vme32d24", O_RDWR );
                            break;
                    case 32 :
                            snprintf (dev_name, sizeof(dev_name), "/dev/vme32d32" );
                            filedes = open("/dev/vme32d32", O_RDWR );
                            break;
                    default :
                            snprintf (dev_name, sizeof(dev_name), "/dev/vme32d32" );
                            filedes = open("/dev/vme32d32", O_RDWR );
                                break;
                }
                break;
            case 2 :
                switch (i_dm){
                    case 8 :
                            snprintf (dev_name, sizeof(dev_name), "/dev/vme16d8" );
                            filedes = open("/dev/vme32d16", O_RDWR );
                            break;
                    case 16 :
                            snprintf (dev_name, sizeof(dev_name), "/dev/vme16d16" );
                            filedes = open("/dev/vme16d16", O_RDWR );
                            break;
                    case 24 :
                            snprintf (dev_name, sizeof(dev_name), "/dev/vme16d24" );
                            filedes = open("/dev/vme16d24", O_RDWR );
                            break;
                    case 32 :
                            snprintf (dev_name, sizeof(dev_name), "/dev/vme16d32" );
                            filedes = open("/dev/vme16d32", O_RDWR );
                            break;
                    default :
                            snprintf (dev_name, sizeof(dev_name), "/dev/vme16d6" );
                            filedes = open("/dev/vme16d16", O_RDWR );
                            break;
                }
                break;
            case 3 :
                switch (i_dm){
                    case 8 :
                            snprintf (dev_name, sizeof(dev_name), "/dev/vme24d8" );
                            filedes = open("/dev/vme24d8", O_RDWR );
                            break;
                    case 16 :
                            snprintf (dev_name, sizeof(dev_name), "/dev/vme24d16" );
                            filedes = open("/dev/vme24d16", O_RDWR );
                            break;
                    case 24 :
                            snprintf (dev_name, sizeof(dev_name), "/dev/vme24d24" );
                            filedes = open("/dev/vme24d24", O_RDWR );
                            break;
                    case 32 :
                            snprintf (dev_name, sizeof(dev_name), "/dev/vme24d32" );
                            filedes = open("/dev/vme24d32", O_RDWR );
                            break;
                    default :
                            snprintf (dev_name, sizeof(dev_name), "/dev/vme24d16" );
                            filedes = open("/dev/vme24d16", O_RDWR );
                            break;
                }
                break;
            default :
                snprintf (dev_name, sizeof(dev_name), "/dev/vme24d16" );
                filedes = open("/dev/vme24d16", O_RDWR );
                break;
        }
    }
    else {
        //
        // open the SBus device /dev/sbusx: without device driver
        //
        if (sbus_slot < 0 || sbus_slot > 3) sbus_slot = 0;
        snprintf (dev_name, sizeof(dev_name), "/dev/sbus%d", sbus_slot);
        filedes = open((const char*)dev_name, O_RDWR);
    }

    if (filedes == -1) {
        snprintf (tmp, sizeof(tmp), "VME: Error opening device file = %s", (const char*)dev_name);
        printtostderr( "VME::map_device", tmp );
        error_cond = 1;
        return (caddr_t) -1;
    }

    act_tab->virt_page_start = mmap ((caddr_t)0L, reg_length,  PROT_READ | PROT_WRITE, MAP_SHARED, filedes,  (off_t)page_start);
    if (act_tab->virt_page_start == (caddr_t)(-1)) {

       int	is_force = 0;
       std::string	hw_info;
            #ifdef VMEBUS
                char	info[100];
                sysinfo( SI_PLATFORM, info, 100 );
                hw_info = info;
                if( hw_info.substr(0,5) == "FORCE" ) is_force = 1;
            #endif
            if( is_force  ) {
                std::vector<struct vme_addr>::iterator	 addr_iter;
                for (addr_iter = addr_list.begin (); addr_iter != addr_list.end (); ++addr_iter) {

                    if( addr_iter->page_start <= page_start &&
                        (addr_iter->page_start + addr_iter->map_size) >= (int)(page_start + reg_length) ) {
                            return( (char*)(addr_iter->virt_addr) + page_offset );
                    }
                }
            }

            snprintf (tmp, sizeof(dev_name),"VME: unable to map = %s", name);
            printtostderr( "VME::map_device", tmp );
            snprintf (tmp, sizeof(dev_name),"reg_length - %d, filedes - %d,  page_start -%X", (int)reg_length, (int)filedes, (int)page_start);
            printtostderr( "VME::map_device", tmp );
            act_tab->virt_page_start = 0;
            error_cond = 1;
            return (caddr_t) -1;
    }
	
    addr_.virt_addr = act_tab->virt_page_start;
    addr_.page_start = page_start;
    addr_.map_size = reg_length;
    addr_list.push_back( addr_ );
    act_tab->virt_start = (char*)((char*)act_tab->virt_page_start + page_offset);
    return act_tab->virt_start;
}

void	VME::unmap_device (const char* name)
{
    VME_table*	act_tab;
    if (error_cond)	return ;
    // find module "name" :
    act_tab = VME::find_name(name);
    if (!act_tab) {
            return ;
    }
    // is it mapped ?:
    if (act_tab->virt_page_start) {
            munmap ((char*)act_tab->virt_page_start, act_tab->space);
            close( filedes );
    }
    return ;
}

VME_table*	VME::find_name(const char* name)
{
    std::string	stmp((char*)name);
    std::transform (stmp.begin (),
                    stmp.end (),
                    stmp.begin (),
                    toupper);

    for (VME_table* p = top_of_table; p; p=p->next() )
            if (p->module_name == stmp)	return p;

    return (VME_table*) 0;
}


u_char	VME::read_vme_char	(caddr_t base_offset, int &err)
{
	u_char data;
	vme_err = 0;

	VME_bus_error_act.sa_handler = VME_on_bus_error;
	VME_bus_error_act.sa_flags   = SA_RESTART;	        // allow restart sys IO
	sigemptyset (&VME_bus_error_act.sa_mask);
	sigaddset   (&VME_bus_error_act.sa_mask, SIGALRM);	// block timer events
	sigaction   (SIGBUS, &VME_bus_error_act, &VME_old_act);


        sigset_t new_msk;

        sigemptyset (&new_msk);
        sigaddset (&new_msk, SIGBUS);
        sigprocmask (SIG_UNBLOCK, &new_msk, 0);

	if (sigsetjmp(VME_jmp_env, 1)) {

		/* there was an interrupt */
		sigaction (SIGBUS, &VME_old_act, 0 );	// restore env.
		err = -1;
		vme_err = -1;
		return  0;
	}

	data = *(u_char *) (base_offset) ;

        sigemptyset (&new_msk);
        sigaddset (&new_msk, SIGBUS);
        sigprocmask (SIG_BLOCK, &new_msk, 0);
	sigaction (SIGBUS, &VME_old_act, 0 );	// restore env.
	err = 0;
	return data;
}



u_short	VME::read_vme_short	(caddr_t base_offset, int &err)
{
	u_short data;
        u_short data_sw;
	vme_err = 0;

	VME_bus_error_act.sa_handler = VME_on_bus_error;
	VME_bus_error_act.sa_flags   = SA_RESTART;	        // allow restart sys IO
	sigemptyset (&VME_bus_error_act.sa_mask);
	sigaddset   (&VME_bus_error_act.sa_mask, SIGALRM);	// block timer events
	sigaction   (SIGBUS, &VME_bus_error_act, &VME_old_act);


        sigset_t new_msk;

        sigemptyset (&new_msk);
        sigaddset (&new_msk, SIGBUS);
        sigprocmask (SIG_UNBLOCK, &new_msk, 0);

	if (sigsetjmp(VME_jmp_env, 1)) {

		/* there was an interrupt */
		sigaction (SIGBUS, &VME_old_act, 0 );	// restore env.
		err = -1;
		vme_err = -1;
		return  0;
	}

	data_sw = *(u_short *) (base_offset) ;

        sigemptyset (&new_msk);
        sigaddset (&new_msk, SIGBUS);
        sigprocmask (SIG_BLOCK, &new_msk, 0);
	sigaction (SIGBUS, &VME_old_act, 0 );	// restore env.
	err = 0;
        if(mswap){
            SwapShort(&data_sw, &data, 1);
        }else{
            data = data_sw;
        }
	return data;
}

u_int	VME::read_vme_int	(caddr_t base_offset, int &err)
{
	u_int data;
        u_int data_sw;
	vme_err = 0;

	VME_bus_error_act.sa_handler = VME_on_bus_error;
	VME_bus_error_act.sa_flags   = SA_RESTART;	        // allow restart sys IO
	sigemptyset (&VME_bus_error_act.sa_mask);
	sigaddset   (&VME_bus_error_act.sa_mask, SIGALRM);	// block timer events
	sigaction   (SIGBUS, &VME_bus_error_act, &VME_old_act);


        sigset_t new_msk;

        sigemptyset (&new_msk);
        sigaddset (&new_msk, SIGBUS);
        sigprocmask (SIG_UNBLOCK, &new_msk, 0);

	if (sigsetjmp(VME_jmp_env, 1)) {

		/* there was an interrupt */
		sigaction (SIGBUS, &VME_old_act, 0 );	// restore env.
		err = -1;
		vme_err = -1;
		return  0;
	}

	data_sw = *(u_int *) (base_offset) ;

        sigemptyset 	(&new_msk);
        sigaddset 	(&new_msk, SIGBUS);
        sigprocmask 	(SIG_BLOCK, &new_msk, 0);
	sigaction 	(SIGBUS, &VME_old_act, 0 );	// restore env.
	err = 0;
        if(mswap){
            SwapInt(&data_sw, &data, 1);
        }else{
            data = data_sw;
        }
	return data;
}


u_char	VME::write_vme_char	(caddr_t base_offset, u_char vme_data, int &err)
{
	u_char data = (0xFF & vme_data);
	vme_err = 0;

	VME_bus_error_act.sa_handler = VME_on_bus_error;
	VME_bus_error_act.sa_flags   = SA_RESTART;	        // allow restart sys IO
	sigemptyset (&VME_bus_error_act.sa_mask);
	sigaddset   (&VME_bus_error_act.sa_mask, SIGALRM);	// block timer events
	sigaction   (SIGBUS, &VME_bus_error_act, &VME_old_act);


        sigset_t new_msk;

        sigemptyset (&new_msk);
        sigaddset (&new_msk, SIGBUS);
        sigprocmask (SIG_UNBLOCK, &new_msk, 0);

	if (sigsetjmp(VME_jmp_env, 1)) {

		/* there was an interrupt */
		sigaction (SIGBUS, &VME_old_act, 0 );	// restore env.
		err = -1;
		vme_err = -1;
		return  0;
	}

	*(u_char *) (base_offset) = data ;

        sigemptyset 	(&new_msk);
        sigaddset 	(&new_msk, SIGBUS);
        sigprocmask 	(SIG_BLOCK, &new_msk, 0);
	sigaction (	SIGBUS, &VME_old_act, 0 );	// restore env.
	err = 0;
	return data;
}

u_short	VME::write_vme_short	(caddr_t base_offset, u_short vme_data, int &err)
{
	u_short data = 0;
        u_short data_sw = (0xFFFF & vme_data);
	vme_err = 0;


	VME_bus_error_act.sa_handler = VME_on_bus_error;
	VME_bus_error_act.sa_flags   = SA_RESTART;	        // allow restart sys IO
	sigemptyset (&VME_bus_error_act.sa_mask);
	sigaddset   (&VME_bus_error_act.sa_mask, SIGALRM);	// block timer events
	sigaction   (SIGBUS, &VME_bus_error_act, &VME_old_act);


        sigset_t new_msk;

        sigemptyset (&new_msk);
        sigaddset (&new_msk, SIGBUS);
        sigprocmask (SIG_UNBLOCK, &new_msk, 0);

	if (sigsetjmp(VME_jmp_env, 1)) {

		/* there was an interrupt */
		sigaction (SIGBUS, &VME_old_act, 0 );	// restore env.
		err = -1;
		vme_err = -1;
		return  0;
	}

        if(mswap){
            SwapShort(&data_sw, &data, 1);
        }else{
            data = data_sw;
        }
	*(u_short *) (base_offset) = data ;

        sigemptyset 	(&new_msk);
        sigaddset 	(&new_msk, SIGBUS);
        sigprocmask 	(SIG_BLOCK, &new_msk, 0);
	sigaction (	SIGBUS, &VME_old_act, 0 );	// restore env.
	err = 0;
	vme_err = 0;
	return data;
}

u_int	VME::write_vme_int	(caddr_t base_offset, u_int vme_data, int &err)
{
	u_int data = 0;
        u_int data_sw = (0xFFFFFFFF & vme_data);
	vme_err = 0;


	VME_bus_error_act.sa_handler = VME_on_bus_error;
	VME_bus_error_act.sa_flags   = SA_RESTART;	        // allow restart sys IO
	sigemptyset (&VME_bus_error_act.sa_mask);
	sigaddset   (&VME_bus_error_act.sa_mask, SIGALRM);	// block timer events
	sigaction   (SIGBUS, &VME_bus_error_act, &VME_old_act);


        sigset_t new_msk;

        sigemptyset 	(&new_msk);
        sigaddset 	(&new_msk, SIGBUS);
        sigprocmask 	(SIG_UNBLOCK, &new_msk, 0);

	if (sigsetjmp(VME_jmp_env, 1)) {

		/* there was an interrupt */
		sigaction (SIGBUS, &VME_old_act, 0 );	// restore env.
		err = -1;
		vme_err = -1;
		return  0;
	}

        if(mswap){
            SwapInt(&data_sw, &data, 1);
        }else{
            data = data_sw;
        }
	*(u_int *) (base_offset) = data ;

        sigemptyset 	(&new_msk);
        sigaddset 	(&new_msk, SIGBUS);
        sigprocmask 	(SIG_BLOCK, &new_msk, 0);
	sigaction 	(SIGBUS, &VME_old_act, 0 );	// restore env.
	err = 0;
	return data;
}

int	VME::read_buf_short	(caddr_t base_offset, u_short* shbuf, int smpl, int &err,
					 int buf_stp , int vme_stp )
{
	int stp1;
	int i = 0;
	stp1 = buf_stp;
	vme_err = 0;
        u_short data;
        u_short data_sw;

	volatile u_short *pvme = (u_short *)base_offset;

	VME_bus_error_act.sa_handler = VME_on_bus_error;
	VME_bus_error_act.sa_flags   = SA_RESTART;	        // allow restart sys IO
	sigemptyset (&VME_bus_error_act.sa_mask);
	sigaddset   (&VME_bus_error_act.sa_mask, SIGALRM);	// block timer events
	sigaction   (SIGBUS, &VME_bus_error_act, &VME_old_act);

        sigset_t new_msk;

        sigemptyset 	(&new_msk);
        sigaddset 	(&new_msk, SIGBUS);
        sigprocmask 	(SIG_UNBLOCK, &new_msk, 0);

	if (sigsetjmp(VME_jmp_env, 1)) {

		/* there was an interrupt */
		sigaction (SIGBUS, &VME_old_act, 0 );	// restore env.
		err = -1;
		vme_err = -1;
		return  0;
	}

	if(vme_stp)
            for (i = 0; i < smpl; i++){
                //shbuf [i*stp1] = *(pvme + i*vme_stp) ;
                data_sw = *(pvme + i*vme_stp) ;
                if(mswap){
                    SwapShort(&data_sw, &data, 1);
                }else{
                    data = data_sw;
                }
                shbuf [i*stp1] = data;
            }
	else
            for (i = 0; i < smpl; i++){
                //shbuf [i*stp1] = *(pvme) ;
                data_sw = *(pvme + i) ;
                if(mswap){
                    SwapShort(&data_sw, &data, 1);
                }else{
                    data = data_sw;
                }
                shbuf [i] = data;
            }

        sigemptyset 	(&new_msk);
        sigaddset 	(&new_msk, SIGBUS);
        sigprocmask 	(SIG_BLOCK, &new_msk, 0);
	sigaction 	(SIGBUS, &VME_old_act, 0 );	// restore env.
	err = 0;
	return i;
}

int	VME::read_buf_int	(caddr_t base_offset, u_int* intbuf, int smpl, int &err,
					int buf_stp , int vme_stp )
{
	int stp1;
	int i = 0;
	stp1 = buf_stp;
	vme_err = 0;
        u_int data;
        u_int data_sw;

	volatile u_int *pvme = (u_int *)base_offset;

	VME_bus_error_act.sa_handler = VME_on_bus_error;
	VME_bus_error_act.sa_flags   = SA_RESTART;	        // allow restart sys IO
	sigemptyset (&VME_bus_error_act.sa_mask);
	sigaddset   (&VME_bus_error_act.sa_mask, SIGALRM);	// block timer events
	sigaction   (SIGBUS, &VME_bus_error_act, &VME_old_act);

        sigset_t new_msk;

        sigemptyset 	(&new_msk);
        sigaddset 	(&new_msk, SIGBUS);
        sigprocmask 	(SIG_UNBLOCK, &new_msk, 0);

	if (sigsetjmp(VME_jmp_env, 1)) {

		/* there was an interrupt */
		sigaction (SIGBUS, &VME_old_act, 0 );	// restore env.
		err = -1;
		vme_err = -1;
		return  0;
	}
	
	if (vme_stp) {
            for (i = 0; i < smpl; i++){
                data_sw = *(pvme + i*vme_stp);
                if(mswap){
                    SwapInt(&data_sw, &data, 1);
                }else{
                    data = data_sw;
                }
                intbuf[i*stp1] = data;
            }
        }
	else {
            for (i = 0; i < smpl; i++){
                data_sw = *(pvme + i);
                if(mswap){
                    SwapInt(&data_sw, &data, 1);
                }else{
                    data = data_sw;
                }
                intbuf[i] = data;
            }
        }
        sigemptyset 	(&new_msk);
        sigaddset 	(&new_msk, SIGBUS);
        sigprocmask 	(SIG_BLOCK, &new_msk, 0);
	sigaction 	(SIGBUS, &VME_old_act, 0 );	// restore env.
	err = 0;
	return i;
}

int	VME::write_buf_short	(caddr_t base_offset, u_short* shbuf, int smpl, int &err,
					 int buf_stp , int vme_stp )
{
	int stp1;
	int i = 0;
	stp1 = buf_stp;
	vme_err = 0;
        u_short data;
        u_short data_sw;

	volatile u_short *pvme = (u_short *)base_offset;

	VME_bus_error_act.sa_handler = VME_on_bus_error;
	VME_bus_error_act.sa_flags   = SA_RESTART;	        // allow restart sys IO
	sigemptyset (&VME_bus_error_act.sa_mask);
	sigaddset   (&VME_bus_error_act.sa_mask, SIGALRM);	// block timer events
	sigaction   (SIGBUS, &VME_bus_error_act, &VME_old_act);

        sigset_t new_msk;

        sigemptyset 	(&new_msk);
        sigaddset 	(&new_msk, SIGBUS);
        sigprocmask 	(SIG_UNBLOCK, &new_msk, 0);

	if (sigsetjmp(VME_jmp_env, 1)) {

		/* there was an interrupt */
		sigaction (SIGBUS, &VME_old_act, 0 );	// restore env.
		err = -1;
		vme_err = -1;
		return  0;
	}
	for (i = 0; i < smpl; i++){
		//*(pvme + i*vme_stp) = shbuf[i*stp1];
                data_sw = (shbuf[i*stp1] & 0xFFFF);
                if(mswap){
                    SwapShort(&data_sw, &data, 1);
                }else{
                    data = data_sw;
                }
                *(pvme + i*vme_stp) = data;
	}

        sigemptyset 	(&new_msk);
        sigaddset 	(&new_msk, SIGBUS);
        sigprocmask 	(SIG_BLOCK, &new_msk, 0);
	sigaction 	(SIGBUS, &VME_old_act, 0 );	// restore env.
	err = 0;
	return i;
}

int	VME::write_buf_int	(caddr_t base_offset, u_int* intbuf, int smpl, int &err,
					int buf_stp , int vme_stp )
{
	int stp1;
	int i = 0;
	stp1 = buf_stp;
	vme_err = 0;
        u_int data;
        u_int data_sw;

	volatile u_int *pvme = (u_int *)base_offset;

	VME_bus_error_act.sa_handler = VME_on_bus_error;
	VME_bus_error_act.sa_flags   = SA_RESTART;	        // allow restart sys IO
	sigemptyset (&VME_bus_error_act.sa_mask);
	sigaddset   (&VME_bus_error_act.sa_mask, SIGALRM);	// block timer events
	sigaction   (SIGBUS, &VME_bus_error_act, &VME_old_act);

        sigset_t new_msk;

        sigemptyset 	(&new_msk);
        sigaddset 	(&new_msk, SIGBUS);
        sigprocmask 	(SIG_UNBLOCK, &new_msk, 0);

	if (sigsetjmp(VME_jmp_env, 1)) {

		/* there was an interrupt */
		sigaction (SIGBUS, &VME_old_act, 0 );	// restore env.
		err = -1;
		vme_err = -1;
		return  0;
	}
	for (i = 0; i < smpl; i++){
		*(pvme + i*vme_stp) = intbuf[i*stp1];
                data_sw = intbuf[i*stp1] & 0xFFFFFFFF;
                if(mswap){
                    SwapInt(&data_sw, &data, 1);
                }else{
                    data = data_sw;
                }
                *(pvme + i*vme_stp) = data;
	}

        sigemptyset 	(&new_msk);
        sigaddset 	(&new_msk, SIGBUS);
        sigprocmask 	(SIG_BLOCK, &new_msk, 0);
	sigaction 	(SIGBUS, &VME_old_act, 0 );	// restore env.
	err = 0;
	return i;
}

void	VME::set_sigbus ()
{
	vme_err = 0;
	VME_bus_error_set_act.sa_handler = VME_set_bus_error;
	VME_bus_error_set_act.sa_flags   = SA_RESTART;	        // allow restart sys IO
	sigemptyset (&VME_bus_error_set_act.sa_mask);
	sigaddset   (&VME_bus_error_set_act.sa_mask, SIGALRM);	// block timer events
	sigaction   (SIGBUS, &VME_bus_error_set_act, &VME_old_act);

        sigset_t new_msk;
        sigemptyset (&new_msk);
        sigaddset (&new_msk, SIGBUS);
        sigprocmask (SIG_UNBLOCK, &new_msk, 0);
}

void	VME::rem_sigbus ()
{
        sigset_t new_msk;
        sigemptyset (&new_msk);
        sigaddset (&msk, SIGBUS);
        sigprocmask (SIG_BLOCK, &new_msk, 0);
	sigaction (SIGBUS, &VME_old_act, 0 );
}

int	VME::get_start_addr (const char* name)
{
	VME_table*	act_tab;

	if (error_cond)	return  -1;

	// find module "name" :
	act_tab = VME::find_name(name);
	if (!act_tab) {
		return -1;
	}
	return( act_tab->start_addr );
}

int	VME::get_space (const char* name)
{
	VME_table*	act_tab;

	if (error_cond)	return  -1;

	// find module "name" :
	act_tab = VME::find_name(name);
	if (!act_tab) {
		return -1;
	}
	return( act_tab->space );
}

VME_DMA::VME_DMA (int m_am, int m_dm, int m_blk)
{
    char          device[28], msg[128];
    unsigned int  ptrn  = 0x01020304;
    unsigned char *byte = (unsigned char*)&ptrn;

    vmedma  = 0;
    machine = UNKNOWN;
    io_am  = m_am;
    io_dm  = m_dm;
    io_blk = m_blk;

    /*check for swapping*/
    if((*byte ) == 0x04 ) {
      mswap = 1;
      printtostderr("VME::VME", "byte swapping needed");
    }
    else{
      mswap = 0;
      printtostderr("VME::VME", "byte swapping not needed");
    }

    #ifdef VMEBUS
        snprintf(device, sizeof(device), "/dev/vmedma0");
        snprintf(msg, sizeof(msg),"Trying to open THEMIS DMA device: %s... ", device);
        printtostderr( "setup_themis", msg );
        vmedma = open(device, O_RDWR);
        if (vmedma < 0){
            printtostderr( "setup_themis", "failed to open VME device");
            vmedma = 0;
        }else{
            printtostderr("setup_themis", "OK");
            snprintf(msg, sizeof(msg),"Open THEMIS DMA device: %s... ", device);
            printtostderr( "setup_themis", msg );
            machine = THEMIS;
            return;
        }

        snprintf(device, sizeof(device),"/dev/vmedma16d16blt");
        snprintf(msg, sizeof(msg),"Trying to open FORCE DMA device: %s... ", device);
        printtostderr( "setup_force", msg );
        vmedma = open(device, O_RDWR);
        if (vmedma < 0){
            printtostderr( "setup_force", "failed to open VME device");
            vmedma = 0;
        }else{
            printtostderr("setup_force", "OK");
            close(vmedma);
            vmedma = 0;
            switch(io_blk){
                case 0:
                    snprintf(device, sizeof(device),"/dev/vmedma%dd%d", io_am, io_dm);
                    break;
                case 1:
                    snprintf(device, sizeof(device),"/dev/vmedma%dd%dblt", io_am, io_dm);
                    break;
                default:
                    snprintf(device, sizeof(device),"/dev/vmedma%dd%d", io_am, io_dm);
                    break;
            }
            snprintf(msg, sizeof(msg),"Trying to open FORCE DMA device: %s... ", device);
            printtostderr( "setup_force", msg );
            vmedma = open(device, O_RDWR);
            if (vmedma < 0){
                printtostderr( "setup_force", "failed to open VME device");
                vmedma = 0;
            }else{
                printtostderr("setup_force", "OK");
            }
            machine = FORCE;
            return;
        }
    #else
        snprintf(device, sizeof(device),"/dev/vmedma16d16");
        snprintf(msg, sizeof(msg),"Trying to open TCD64 DMA device: %s... ", device);
        printtostderr( "setup_tcd64", msg );
        vmedma = open(device, O_RDWR);
        if (vmedma < 0){
            printtostderr( "setup_tcd64", "failed to open VME device");
            vmedma = 0;
        }
	else{
            printtostderr("setup_tcd64", "OK");
            close(vmedma);
            vmedma = 0;
            snprintf(device, sizeof(device),"/dev/vmedma%dd%d", io_am, io_dm);
            snprintf(msg, sizeof(msg),"Trying to open TCD64 DMA device: %s... ", device);
            printtostderr( "setup_tcd64", msg );
            vmedma = open(device, O_RDWR);
            if (vmedma < 0){
                printtostderr( "setup_tcd64", "failed to open VME device");
                vmedma = 0;
            }else{
                printtostderr("setup_tcd64", "OK");
            }
            machine = TCD64;
            return;
        }
    #endif

    printtostderr( "setup_dma", "NO ANY VME_DMA device");
    vmedma  = 0;
    machine = UNKNOWN;
    return;
}

int VME_DMA::read_vme_dma(int addr, unsigned int size, unsigned int* buff)
{
    int result, smpl;

    if (!vmedma) // not initalized yet
        return -1;
    if (size <= 0) // zero size
        return -1;

    smpl = size/sizeof(int);
    #ifdef VMEBUS
        switch (machine){
            case FORCE:
                if (lseek(vmedma, (off_t)addr, SEEK_SET) == -1)
                {
                    perror("Force DMA lseek");
                    return -1;
                }
                if ((result = ::read(vmedma, (char *)buff, size)) < 0)
                {
                    perror("Force DMA read");
                    return -1;
                }
                break;
            case THEMIS:
                struct vme_dmacopy   dma;
                struct vme_dmaresult dma_res;

                dma.dma_source	= (void *) addr;
                dma.dma_dest	= (void *) buff;
                dma.dma_sourcehi	= 0;
                dma.dma_desthi	= 0;
                dma.dma_count	= size;
                dma.dma_result	= &dma_res;
                dma.dma_flags	= VF_SVME | VF_BLT;

                if (ioctl(vmedma, VIOCDMACOPY, &dma) < 0)
                {
                    return -1;
                }
                result = size;
                break;
        }
    #else
       u_int data;
       u_int data_sw;
       volatile int *dma_buff = new int[size/sizeof(int)];
        /*
         * read data from VME
         */
        lseek ( vmedma, addr, SEEK_SET);
        result = read ( vmedma, (char *)dma_buff, size);
	if ( result != (int)size)
	{
            perror("In reading VME");
            delete dma_buff;
            return -1;
	}
        for ( int i = 0; i < smpl; i++){
            data_sw = dma_buff[i];
            if(mswap){
                SwapInt(&data_sw, &data, 1);
            }else{
                data = data_sw;
            }
            buff[i] = data;
        }
        delete dma_buff;
    #endif
    return result;
}

int VME_DMA::write_vme_dma  (int addr,    unsigned int size, unsigned int* buff)
{
    int result, smpl;
    volatile int *dma_buff;

    if (!vmedma) // not initalized yet
        return -1;
    if (size <= 0) // zero size
        return -1;

    smpl = size/sizeof(int);
    dma_buff = new int[size/sizeof(int)];
    
    #ifdef VMEBUS
         switch (machine){
            case FORCE:
                if (lseek(vmedma, (off_t)addr, SEEK_SET) == -1){
                    perror("Force DMA lseek");
                    return -1;
                }
                if((result = write(vmedma, (char *)dma_buff, size)) < 0){
                    perror("Force DMA write");
                }
                break;
            case THEMIS:
                struct vme_dmacopy   dma;
                struct vme_dmaresult dma_res;

                dma.dma_source	= (void *)addr;
                dma.dma_dest	= (void *) buff;
                dma.dma_sourcehi	= 0;
                dma.dma_desthi	= 0;
                dma.dma_count	= size;
                dma.dma_result	= &dma_res;
                dma.dma_flags	= VF_SVME | VF_BLT;

                if (ioctl(vmedma, VIOCDMACOPY, &dma) < 0)
                {
                    return -1;
                }
                result = size;
                break;
         }
    #else
       u_int data;
       u_int data_sw;

        for ( int i = 0; i < smpl; i++){
            data_sw = buff[i];
            if(mswap){
                SwapInt(&data_sw, &data, 1);
            }else{
                data = data_sw;
            }
            dma_buff[i] = data;
        }
        lseek ( vmedma, addr, SEEK_SET);
        result = write(vmedma, (char *)dma_buff, size);
	if ( result != (int)size)
	{
            perror("In reading VME");
            delete dma_buff;
            return -1;
	}
        delete dma_buff;
    #endif
    return result;
}

