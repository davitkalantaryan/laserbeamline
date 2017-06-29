//! \file     eq_server.cc
//!
//! \brief    The file eq_server.cc contains the main() function for a server.
//!
//! \author   Kay Rehlich, Olaf Hensler, Gerhard Grygiel, Arthur Agababyan, Harald Keller -MVP-

/*
   Kay Rehlich, DESY  -PVAK-

   last update:
	22. Apr. 1993
	15. Dec. 1993	function rpc_kill_handler() added
	18. Aug. 1995	new print_rpc_statistic() and signal() -> sigaction()
	16. Dec. 1998	disable SIGALRM in rpc calls
	27. Jan. 2004   workaround for increasing number of opened files added
 */

/** 
 *  @mainpage
 *  
 *  @section intro Introduction
 * 
 *  This is the DOOCS server library, being the central part of every
 *  DOOCS server. This library is available as static or shared library
 *  and called libEqServer.a or libEqServer.so.\n
 *  The two most important classes are EqFct as location class and D_fct for all the property types\n
 *  Every DOOCS server needs to be linked at least against this library
 *  and the DOOCS client library libDOOCSapi.
 *  The file eq_server.cc contains the main() function for a server.
 * 
 *  @author   Kay Rehlich, Gerhard Grygiel, Olaf Hensler, Arthur Agababyan, Harald Keller
 *
 *  @date     29.03.2006
**/

//
//
// $Source: /doocs/doocssvr1/cvsroot/source/serverlib/eq_server.cc,v $
// Revision 1.58  2002/05/07 08:30:54  arthura
// fixed the error reports.
//
// Revision 1.57  2002/05/06 14:12:40  arthura
// rpc functions are registered in a separate thread. Done for LINUX.
//
// Revision 1.56  2002/03/22 09:21:21  arthura
// instead of printing "Server" main () reports "Server-<version>"
//
// Revision 1.55  2002/02/18 09:56:02  arthura
// fixed the bug in setup_interrupt_usr1 ()
//
// Revision 1.53  2002/02/13 15:44:39  arthura
// added the initialization of buf in main ()
//
// Revision 1.52  2002/02/12 10:27:19  arthura
// new serverlib 4.0.2
//
// Revision 1.45  2001/11/20 09:37:58  arthura
// changed eq_count to chan_count
//
// Revision 1.44  2001/11/19 08:33:09  arthura
// restored some old stuff
//
// Revision 1.43  2001/11/07 07:39:39  grygiel
// add GLIBC defines
//
// Revision 1.42  2001/11/02 16:02:41  arthura
// *** empty log message ***
//
// Revision 1.41  2001/11/02 14:25:09  arthura
//
// added permissions () function to Eq_Fct class
//
// Revision 1.40  2001/10/31 09:43:28  arthura
// *** empty log message ***
//
// Revision 1.39  2001/10/30 15:19:11  grygiel
// disable warnings
//
// Revision 1.38  2001/10/30 14:38:20  arthura
// *** empty log message ***
//
// Revision 1.37  2001/10/30 14:22:31  arthura
// new serverlib 4.0.0
//
// Revision 1.36  2001/10/30 14:02:05  arthura
// *** empty log message ***
//
// Revision 1.35  2001/10/29 16:02:27  arthura
//
// removed the type conversion SIG_PF in assignement sa_handler
//
// Revision 1.34  2001/10/27 14:51:25  arthura
// *** empty log message ***
//
// Revision 1.33  2001/10/26 16:36:30  arthura
// *** empty log message ***
//
// Revision 1.32  2001/10/26 15:45:04  arthura
// *** empty log message ***
//
// Revision 1.31  2001/10/26 15:25:31  arthura
// *** empty log message ***
//
// Revision 1.30  2001/10/26 15:19:36  arthura
// new serverlib 4.0.0
//
// Revision 1.29  2001/10/26 15:15:03  arthura
// *** empty log message ***
//
// Revision 1.28  2001/10/26 14:33:38  arthura
// *** empty log message ***
//
// Revision 1.27  2001/10/26 14:09:40  arthura
// *** empty log message ***
//
// Revision 1.26  2001/10/26 14:06:23  arthura
// *** empty log message ***
//
// Revision 1.25  2001/10/26 13:57:55  arthura
//
// added the prototypes for RPC server functions
//
// Revision 1.24  2001/10/26 13:55:20  arthura
// *** empty log message ***
//
// Revision 1.23  2001/10/26 12:02:23  arthura
// New MT server library. Vers. 4.0.0
//
// Revision 1.21  2001/08/13 09:30:07  ohensler
// new lib 3.4.18
//
// Revision 1.20  2001/03/09 12:31:05  ohensler
// SVR_VER added, bug fix in D_name_restr::set_value
//
// Revision 1.19  2000/07/27 10:59:00  ohensler
// changed date format for log file
//
// Revision 1.18  2000/07/19 14:14:50  ohensler
// SIG_PF symbol for Solaris 8
//
// Revision 1.17  2000/05/08 09:37:59  ohensler
// lots of bug fixes.
// rearrange the TDS archiver part
//
// Revision 1.16  1999/10/08 12:31:01  arthura
// Support for Solaris 7
//
// Revision 1.15  1999/09/16 07:36:21  ohensler
// changes for the HERA Gateway,
// switches for signal for 2.4/2.6
//
// Revision 1.14  1999/06/14 11:07:27  grygiel
// changes for libc6 - GNU C Library
//
// Revision 1.13  1999/01/05 11:54:37  rehlich
// bug fixes is error handling
//
// Revision 1.12  1998/10/30 18:43:21  rehlich
// new: interrupt handler,fct_name in config; bug fix: property name with no blank
//
// Revision 1.11  1998/01/08 18:11:05  rehlich
// check if user belongs to a group, disable/enable SIGALRM
//
// Revision 1.10  1998/01/05 16:56:21  rehlich
// cleaning versions
//
// Revision 1.9  1997/12/16 09:22:10  ohensler
// bug fix D_history with fast ring-buffer
// SVR.ERROR_COUNT added to server instance
// printtostderr() moved to gen_functions
//
// Revision 1.8  1997/06/25 07:40:24  ohensler
// printout to stderr and logfile with new format
//
// Revision 1.7  1997/03/19 09:13:30  ohensler
// bug fixes
//
// Revision 1.6  1997/03/13 17:08:34  ohensler
// interrupt added, bug fixes
//
// Revision 1.5  1997/02/07 17:04:03  grygiel
// remove local declaration of var clnt_name in function
// gen_lib_prog_1
//
// Revision 1.4  1996/11/13 16:31:21  rehlich
// svr statistics new, bug fixes
//
// Revision 1.2  1995/11/28 12:53:15  cobraadm
// changes for Linux 1.2.13 ELF with gcc 2.7.0
//
// Revision 1.1.1.1  1995/11/03 12:24:10  grygiel
// DOOCS sources
//
// Revision 1.2  1995/10/25 09:56:27  xgrygiel
// add typedef __sighandler_t SIG_PF for GNU
//
// Revision 1.1  1995/10/24  13:35:43  grygiel
// Initial revision
//
//
//


#define PORTMAP

#include <mtp.h>
#include <eq_rpc.h>
#include <netinet/in.h>
#include <sys/socket.h>
#ifndef __GNUC__
#include <rpc/svc_soc.h>
#endif
#include <rpc/pmap_clnt.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <memory.h>
#include <time.h>
#include <errno.h>
//#include <stropts.h>
#include <syslog.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include "gen_functions.h"
#include "config.h"
#include "eq_data.h"
#include "eq_adr.h"
#include "eq_errors.h"
#include "eq_fct_errors.h"
#include "eq_fct.h"
#include "eq_client.h"
#include "eq_svr.h"
#include "DOOCShash.h"
#include "DOOCSsleep.h"
//#include "rpctypes.h"

#ifdef __GNUC__
//#include "localtime_r.h"
#endif

#include <time.h>
#include <unistd.h>
#include <sys/types.h>

#include <string>
#include <gen_functions.h>

#ifndef eq_rpc_h
#define eq_rpc_h
#ifndef pvak_types_h
#define pvak_types_h
#endif
#endif

#ifdef __GNUC__
#define cond_wait pthread_cond_wait
#endif



typedef struct action {

       int    valid;
       void   (*proc) (void *);
       void   *param;

} action_t;


// DOOCS server user specific functions provided externaly

extern void            eq_call_back          (int);
extern void            interrupt_usr1_prolog (int);
extern void            interrupt_usr1_epilog (int);
extern void            interrupt_usr2_prolog (void);
extern void            interrupt_usr2_epilog (void);
extern void            eq_init_prolog        (void);
extern void            eq_init_epilog        (void);
extern void            post_init_prolog      (void);
extern void            post_init_epilog      (void);
extern void            refresh_prolog        (void);
extern void            refresh_epilog        (void);
extern void            eq_cancel             (void);


// Main thread of the server

int                    eq_server             (int, char **);

// MT DOOCS server internal funtions driven by threads and signals

extern "C" {
static void            *timer                (void *);
static void            *rpc                  (void *);
static void            *writer               (void *);
static void            *interrupt_usr1       (void *);
static void            *interrupt_usr2       (void *);
static void            *tine                 (void *);
static void            eqlibprog_1           (struct svc_req *, SVCXPRT *);
static void            exit_thread           (int);
static void            sig_handler           (int);
}

extern "C" void        svc_exit (void);

//------------------------------------------------

extern void            init_d_fct_globals    (void);
extern int             tine_init             (char *, char *, char *, int, int, u_char);
extern void            tine_cycle            (void);

extern int             DOOCS_debug;

void                   set_buf_size          (int, int, int);
void                   get_buf_size          (int *, int *);

int                    setup_interrupt_usr1  (int);
int                    setup_interrupt_usr2  (char *, int);
int                    setup_action          (enum act, void (*) (void *), void *);
pid_t                  get_sigpid            (void);
void                   signal_writer         (void);
void                   wait_mt               (void);

void                   set_rate_flag         (void);
int                    get_rate_flag         (void);

int                    svr_argc;
char                   **svr_argv;

MUTEX_T                mx_svr;
int	               chan_count;
DOOCSHashtable         *eq_hashtable;
std::vector<EqFct *>   *eq_list;
EqFctSvr               *server_eq;
Config                 *config;

int                    ring_buffer;
int                    memory_buffer;
int                    fct_code;
std::string            gbl_fct_name;
std::string            def_fct_name;

fio_disp               table;
action_t               act_table;

char   	               banner      [16];

MUTEX_T                mx_error; //! mutex for error thread to protect alarm queue
MUTEX_T                mx_alarminfo; //! mutex for error thread to protect alarm queue

MUTEX_T                mx_clnt;
struct permission      access_perm;
struct con_entry       *con_tab;     // connection table for statistics
int                    con_tab_size; // connection table size

static int             con_pos;      // next free entry in connection table
static char            *con_name;

static COND_T          cv_mode;
static int             svr_mode; // 1 - MT mode, 0 - ST mode
static int             build_phase; // 0 - init () phase, 1 - post_init () phase

static long            eq_prog; // RPC program number
static long            eq_vers; // RPC program version

static MUTEX_T         mx_sig;
static int             flag_sig;

static MUTEX_T         mx_wr;   // mutex to protect access to flag_wr and block
static COND_T          cv_wr;   // cond. variable
static int             flag_wr; // flag to trigger thread to update config. file
static int             block;

static MUTEX_T         mx_pid; // mutex to protect access to sigpid
static COND_T          cv_pid; // cond. variable
static pid_t           sigpid; // process ID of the signalled thread

static MUTEX_T         mx_usr1;   // mutex to protect access to flag_usr1 and sg_proc_usr1
static COND_T          cv_usr1;   // cond. variable
static int             flag_usr1; // flag to enable signal processing
static int             sg_proc_usr1 [3]; // enabled signals map

static MUTEX_T         mx_usr2;   // mutex to protect access to flag_usr2
static COND_T          cv_usr2;   // cond. variable
static int             flag_usr2; // at present not used in server library

static MUTEX_T         mx_ext;
static COND_T          cv_ext;
static int             sig_ext;
static int             ext_threads;
static int             ext_counter;
static int             ext_flag;

static THREAD_T        r_id;
static THREAD_T        t_id;
static THREAD_T        w_id;
static THREAD_T        i1_id;
static THREAD_T        i2_id;
static THREAD_T        tine_id;
static THREAD_T        err_id;
static THREAD_T        err_rec_id;
static THREAD_T        err_alive_id;

static int             sbuf_size;
static int             rbuf_size;


#ifdef SYSV

#define                SLOTS_MAX             150

// workaround to increase the number of opened files

static void            reserve_slots         (int *, int);
static void            free_slots            (int *, int);

#endif

static int             get_working_dir       (int, char **, char *, int, int *);
static void            get_eq_libvers        (char *, std::string &, std::string &);
static void            create_all_threads    (int);
static void            exit_all_threads      (void);
static void            switch_mt             (void);
static int             update_con_table      (int, permissions *);
int                    create_con_table      (int);
int                    read_con_table        (EqData *);

//static int             get_bsize             (int, int);
//static int             set_bsize             (int, int, int);



int
main (int argc, char *argv [])
{
      return eq_server (argc, argv);
}


//#include <sys/stat.h>
#include <pwd.h>

int
eq_server (int argc, char *argv [])
{
        int                 size;
        int                 flag;
        int                 sig;
	int                 count;
	int                 param;
        int                 dbg;
        sigset_t            mask;
        struct sigaction    action;
	EqFct               *p;
        char                *path;
        char                *cn;
        char                *cmd;
        char                *snp;
        char                ext [] = ".conf";
	char   	            buf [300];
	
	std::string	    svr_ver_buf;
	std::string	    svr_date;

        #ifdef SYSV
        // workaround to increase the number of opened files

        int                 slots [SLOTS_MAX];
        #endif

        svr_argc         = argc;
        svr_argv         = argv;

        dbg              = -1;
        sbuf_size        = 0;
        rbuf_size        = 0;
        svr_mode         = 0;
        build_phase      = 0; // init () phase
        block            = 0;
        sigpid           = -1;
        flag_sig         = 0;
        flag_wr          = 0;
        flag_usr1        = 0;
        flag_usr2        = 0;
        sg_proc_usr1 [0] = 0;
        sg_proc_usr1 [1] = 0;
        sg_proc_usr1 [2] = 0;
        sig_ext          = SIGQUIT;
        ext_threads      = 0;
        ext_counter      = 0;
        ext_flag         = 0; // for user activated rpc routines which are
                              // still active when the server goes down

        con_name         = "unknown";
        con_tab          = (con_entry *) 0;
        con_tab_size     = 0;
        con_pos          = 0;

        act_table.valid  = 0;
        act_table.param  = (void *) 0;
        act_table.proc   = (void (*) (void *)) 0;

        memset  (buf, '\0', sizeof (buf));

        get_eq_libvers (argv [0], svr_ver_buf, svr_date);

        memset  (banner, '\0', sizeof (banner));
	strcpy  (banner, "Server-");
	strncat (banner, svr_ver_buf.c_str(), 9);

        count = PATH_MAX + 1; // max length of pathname

        path  = new char [count];
        memset (path, '\0', count);

        flag = get_working_dir (argc, argv, path, count, &dbg);
        if (flag) {

            // change the working directory to the
            // one where the server executable resides

            if (chdir (path)) {
                sprintf (buf, "can not switch to <%s>", path);
                printtostderr (banner, buf);
            }
        }
        getwd (path);

        sprintf (buf, "working dir. <%s>", path);
        printtostderr (banner, buf);

        // build the server configuration file name:
        // extract the reference to the current working
        // directory and add suffix '.conf'
        //
        // server_path/server ===> server.conf
        //

        snp = strrchr (argv [0], '/'); // is a path specified ?
        if (!snp) cmd = argv [0];
        else      cmd = snp + 1;

        memset (path, '\0', count);
        strcpy (path, cmd);
        strcat (path, ext);

	#ifdef __GNUC__
        pthread_mutexattr_t   mxa;
        pthread_condattr_t    cva;

        pthread_mutexattr_init (&mxa);
        pthread_condattr_init  (&cva);

	#if ( __GLIBC_MINOR__ >= 2)
        pthread_mutexattr_setpshared (&mxa, PTHREAD_PROCESS_PRIVATE);
        pthread_condattr_setpshared  (&cva, PTHREAD_PROCESS_PRIVATE);
	#endif

	#endif

        sigemptyset (&mask);
        sigaddset (&mask, SIGALRM);
        sigaddset (&mask, SIGPIPE);
        sigaddset (&mask, SIGTERM);
        sigaddset (&mask, SIGBUS);
        sigaddset (&mask, SIGINT);
        sigaddset (&mask, SIGUSR1);
        sigaddset (&mask, SIGUSR2);
        sigaddset (&mask, SIGURG);

        THREAD_SIGSETMASK (SIG_BLOCK, &mask, 0);

	#ifdef __GNUC__
        pthread_mutex_init (&mx_svr, &mxa);
        pthread_mutex_init (&mx_clnt, &mxa);
        pthread_mutex_init (&mx_pid, &mxa);

        pthread_cond_init  (&cv_pid, &cva);

	#else
        mutex_init (&mx_svr, USYNC_THREAD, 0);
        mutex_init (&mx_clnt, USYNC_THREAD, 0);
        mutex_init (&mx_pid, USYNC_THREAD, 0);

        cond_init  (&cv_pid, USYNC_THREAD, 0);

	#endif

        MUTEX_LOCK (&mx_clnt);
        //??
        //access_perm.prog_uid  = ::geteuid ();
        //access_perm.prog_gid  = ::getegid ();
        access_perm.oper_uid  = ::geteuid ();
        access_perm.oper_gid  = ::getegid ();
        access_perm.xpert_uid = 0;
        access_perm.xpert_gid = 0;
        MUTEX_UNLOCK (&mx_clnt);

        // initialize the lists for D_fct objects

        init_d_fct_globals ();

	#ifdef __GNUC__

        // initialize mutex for Linux version of localtime_r ()

        //init_localtime_r_mx ();
        #endif

        #ifdef SYSV
        reserve_slots (slots, SLOTS_MAX);
	#endif
	
        MUTEX_LOCK (&mx_svr);

        // build all locations of the server
        // by reading config file and calling
        // constructors of the EqFct objects

        count = init_eq_server (path);

        if (count) {
            DOOCS_debug = (dbg < 0) ? 0 : dbg;

            eq_prog = server_eq->rpc_number_.value ();
            eq_vers = EQLIBVERS;

            server_eq->server_libinfo_.set_value ((char *) svr_ver_buf.c_str ());
            server_eq->server_libdate_.set_value ((char *) svr_date.c_str ());
            server_eq->proc_name_.set_value ((char *) cmd );
        }

        // get connection table size from property

        size = server_eq->resize_.value ();
        MUTEX_UNLOCK (&mx_svr);

        // create a new connection table

        size = create_con_table (size);

        MUTEX_LOCK (&mx_svr);

        // set the actual connection table size in property

        server_eq->resize_.set_value (size);

        MUTEX_UNLOCK (&mx_svr);

        #ifdef SYSV
        free_slots (slots, SLOTS_MAX);
	#endif

        if (!count || !eq_prog) {

            if (!count) {
                printtostderr (banner, "no devices ==> STOP");
            } else {
	        printtostderr (banner, "no SVR.RPC_NUMBER ==> EXIT, !!!!! see below ");
                printtostderr (banner, "Please specify in the config file ");
                printtostderr (banner, "for the server location a property ");
                printtostderr (banner, "with the name SVR.RPC_NUMBER to store ");
                printtostderr (banner, "the RPC number of the server ");
            }

            #ifdef __GNUC__
            pthread_mutexattr_destroy (&mxa);
            pthread_condattr_destroy (&cva);

            #endif

            MUTEX_DESTROY (&mx_clnt);
            MUTEX_DESTROY (&mx_svr);

            #ifdef __GNUC__

            destroy_localtime_r_mx ();
            #endif

            delete [] path;
	    return 1;
        }

        if (!access_perm.oper_uid && !access_perm.oper_gid)
            printtostderr (banner, "Operator uid/gid are missing");

        if (!access_perm.xpert_uid && !access_perm.xpert_gid)
            printtostderr (banner, "Expert uid/gid are missing");

	//
	// set the .conf file name into the server instance to insure the right name
	// for saving the .conf file
	//
        server_eq->file_.set_value (path);
	

        #ifdef __GNUC__
        pthread_mutex_init (&mx_sig,       &mxa);
        pthread_mutex_init (&mx_wr,        &mxa);
        pthread_mutex_init (&mx_usr1,      &mxa);
        pthread_mutex_init (&mx_usr2,      &mxa);
        pthread_mutex_init (&mx_error,     &mxa);
        pthread_mutex_init (&mx_alarminfo, &mxa);
        pthread_mutex_init (&mx_ext,       &mxa);

        pthread_cond_init  (&cv_mode, &cva);
        pthread_cond_init  (&cv_wr,   &cva);
        pthread_cond_init  (&cv_usr1, &cva);
        pthread_cond_init  (&cv_usr2, &cva);
        pthread_cond_init  (&cv_ext,  &cva);

        #else
        mutex_init (&mx_sig,       USYNC_THREAD, 0);
        mutex_init (&mx_wr,        USYNC_THREAD, 0);
        mutex_init (&mx_usr1,      USYNC_THREAD, 0);
        mutex_init (&mx_usr2,      USYNC_THREAD, 0);
        mutex_init (&mx_error,     USYNC_THREAD, 0);
        mutex_init (&mx_alarminfo, USYNC_THREAD, 0);
        mutex_init (&mx_ext,       USYNC_THREAD, 0);

        cond_init  (&cv_mode, USYNC_THREAD, 0);
        cond_init  (&cv_wr,   USYNC_THREAD, 0);
        cond_init  (&cv_usr1, USYNC_THREAD, 0);
        cond_init  (&cv_usr2, USYNC_THREAD, 0);
        cond_init  (&cv_ext,  USYNC_THREAD, 0);

        #endif

        action.sa_handler = exit_thread; // set thread exit handler
        action.sa_flags   = 0;
        sigaction (sig_ext, &action, 0);

	// initialize the TINE protocol

	param = true;
	if (server_eq->tine_.value ()) {
	    param = tine_init (server_eq->host_name_.value (),
			       server_eq->facility_.value  (),
                               server_eq->device_.value    (),
			       server_eq->group_.value     (),
                               count, (eq_prog - EQLIBPROG) % 255);

	    if (param) printtostderr (banner, "no TINE service");
	    else {
  	        sprintf (buf, "TINE started as %s/%s",
                         server_eq->facility_.value (),
			 server_eq->device_.value ());

	        printtostderr (banner, buf);
	    }
	}

	sprintf (buf, "started with %d devices on %s", count,
                      server_eq->host_name_.value ());

	printtostderr (banner, buf);

        create_all_threads (param); // create all threads of the server

        // create a directory for alarmserver recover files

	int code = mkdir ("./xml-recover", 0755);

	if (code && errno != EEXIST) {
	    printtostderr (banner, "Cannot creat dir: ./xml-recover");
	}
	if (server_eq->errorinfo_.value ()) {

            // send only by enabled alarm server

	    error2alarmsvr ();
	    devinfo2alarmsvr ();
	}

        build_phase = 1; // post_init () phase

        // do post init processing

        post_init_prolog ();

        MUTEX_LOCK (&mx_svr);
	std::vector<EqFct *>::iterator eq_iter; // list of all locations

	for (eq_iter = eq_list->begin (); eq_iter != eq_list->end (); ++eq_iter) {
             p = *eq_iter;

	     if (p) {
                 p->lock ();
                 p->post_init ();
                 p->unlock ();
             }
	}
        MUTEX_UNLOCK (&mx_svr);

	post_init_epilog ();

	server_eq->svr_startup_ = false;

        // set starttime of the server process

	server_eq->starttime_.set_value ((int)::time (0));

        switch_mt (); // switch to MULTITHREADED mode

        // process signals

        sigemptyset (&mask);
        sigaddset (&mask, SIGPIPE);
        sigaddset (&mask, SIGTERM);
        sigaddset (&mask, SIGINT);

        for ( ; ; ) {
             #ifdef __GNUC__
             sigwait (&mask, &sig);

             #else
             sig = sigwait (&mask);

             #endif

             switch (sig) {

             case SIGPIPE:
                  MUTEX_LOCK (&mx_clnt);
                  cn = con_name;
                  if (!cn) cn = "unknown";
                  sprintf (buf, "\nServer: link to client '%s' lost\n", cn);

                  printtostderr (banner, buf);
                  MUTEX_UNLOCK (&mx_clnt);
                  break;

             case SIGINT:
             case SIGTERM:
                  MUTEX_LOCK (&mx_wr);
                  flag = block;
                  MUTEX_UNLOCK (&mx_wr);

                  if (flag) break;

                  if (sig == SIGINT)
                       printtostderr (banner, "signal INT  received");
                  else printtostderr (banner, "signal TERM received");

                  printtostderr (banner, "terminating threads and closing files...");
                  eq_cancel ();        // terminate all user-specific threads
                  exit_all_threads (); // terminate all server-specific threads
                  table.close_all ();  // close all files

                  printtostderr (banner, "done. Safe exit");

                  MUTEX_LOCK (&mx_svr);  // block all threads

                  for (eq_iter = eq_list->begin (); eq_iter != eq_list->end (); ++eq_iter) {
                       p = *eq_iter;
                       if (p) delete p;
                  }

                  MUTEX_DESTROY (&mx_sig);
                  MUTEX_DESTROY (&mx_wr);
                  MUTEX_DESTROY (&mx_usr1);
                  MUTEX_DESTROY (&mx_usr2);
                  MUTEX_DESTROY (&mx_error);
                  MUTEX_DESTROY (&mx_ext);

                  COND_DESTROY (&cv_mode);
                  COND_DESTROY (&cv_wr);
                  COND_DESTROY (&cv_usr1);
                  COND_DESTROY (&cv_usr2);
                  COND_DESTROY (&cv_ext);

                  MUTEX_UNLOCK (&mx_svr);
                  pmap_unset (eq_prog, eq_vers);

	          #ifdef __GNUC__

	          destroy_localtime_r_mx ();
	          #endif

                  delete [] path;
                  return 0;
             }
        }
}



static void *
rpc (void *ip)
{
        SVCXPRT             *transp;
	int                 sock;
        char                buf  [100];

        #ifndef __GNUC__
	long                mode;
        #endif

        /*

        the following signals are masked:

                   SIGALRM,
                   SIGPIPE,
                   SIGTERM,
                   SIGBUS,
                   SIGINT,
                   SIGUSR1,
                   SIGUSR2,
                   SIGURG
        */

	sock = RPC_ANYSOCK;
        memset (buf, '\0', sizeof (buf));
 	pmap_unset (eq_prog, eq_vers);

        #ifndef __GNUC__

        mode = RPC_SVC_MT_AUTO;
	if (!rpc_control (RPC_SVC_MTMODE_SET, &mode)) {
	    printtostderr (banner, "unable to set RPC in automatic MT mode.");
	}

        #endif

	transp = svctcp_create (sock, 0, 0);
	if (transp == (SVCXPRT *) 0) {
	    printtostderr (banner, "can not create rpc service.");

            svc_destroy (transp);
            pmap_unset (eq_prog, eq_vers);
	    exit (1);
	}

        wait_mt (); // wait to switch to MT mode

	if (!svc_register (transp, eq_prog, eq_vers, eqlibprog_1, IPPROTO_TCP)) {
            sprintf (buf, "unable to register (%lx, %lx, tcp)", eq_prog, eq_vers);
            printtostderr (banner, buf);

            svc_destroy (transp);
	    pmap_unset (eq_prog, eq_vers);
            exit (1);
	}

	svc_run ();

        return ip;
}




static void *
tine (void *ip)
{
        /*

        the following signals are masked:

                   SIGALRM,
                   SIGPIPE,
                   SIGTERM,
                   SIGBUS,
                   SIGINT,
                   SIGUSR1,
                   SIGUSR2,
                   SIGURG
        */

        wait_mt (); // wait to switch to MT mode

	tine_cycle ();
        printtostderr (banner, "tine_cycle () returned");

        return ip;
}





static void *
timer (void *ip)
{
	int		   i;
	int		   k;
	int		   crt;
	int		   on_cnt;
	int		   off_cnt;
	int		   code;
	float		   f;
	double		   dt;
        sigset_t           msk;
        sigset_t           mask;
        struct sigaction   action;
        struct timespec    rqtp;
        struct timespec    rmtp;
	struct timeval     tp1;
	struct timeval     tp2;
	EqFct              *p;
        EqFctSvr           *sp;

        /*

        the following signals are masked:

                   SIGALRM,
                   SIGPIPE,
                   SIGTERM,
                   SIGBUS,
                   SIGINT,
                   SIGUSR1,
                   SIGUSR2,
                   SIGURG
        */

        sigemptyset (&msk);
        sigaddset (&msk, sig_ext);

        MUTEX_LOCK (&mx_svr);
	std::vector<EqFct *>::iterator eq_iter; // list of all locations
        sp = server_eq;
        MUTEX_UNLOCK (&mx_svr);

        sigemptyset (&action.sa_mask);
        sigaddset (&action.sa_mask, SIGALRM);
        action.sa_flags   = SA_RESTART;
        action.sa_flags   = 0;
        action.sa_handler = sig_handler;
        sigaction (SIGALRM, &action, 0);

        sigemptyset (&mask);
        sigaddset (&mask, SIGALRM);

        THREAD_SIGSETMASK (SIG_UNBLOCK, &mask, 0);

        wait_mt (); // wait to switch to MT mode

        sp->lock ();
	rqtp.tv_sec  = sp->rate_.value ()->i1_data;
	rqtp.tv_nsec = sp->rate_.value ()->i2_data * 1000;
        sp->unlock ();

        for ( ; ; ) {

             if (get_rate_flag ()) {
                 sp->lock ();
                 rqtp.tv_sec  = sp->rate_.value ()->i1_data;
                 rqtp.tv_nsec = sp->rate_.value ()->i2_data * 1000;
                 sp->unlock ();
             }
             code = nanosleep (&rqtp, &rmtp);
             if (code < 0) {

                 if (errno == EINTR && !get_rate_flag ()) {
                     rqtp.tv_sec  = rmtp.tv_sec;
                     rqtp.tv_nsec = rmtp.tv_nsec;
                 } else {
                     sp->lock ();
                     rqtp.tv_sec  = sp->rate_.value ()->i1_data;
                     rqtp.tv_nsec = sp->rate_.value ()->i2_data * 1000;
                     sp->unlock ();
                 }
                 continue;
             }
	     gettimeofday (&tp1, 0);

             THREAD_SIGSETMASK (SIG_BLOCK, &mask, 0);

             sp->lock ();
	     sp->svr_update_prolog ();
             sp->unlock ();

	     k       = 0;
             crt     = 0;
             on_cnt  = 0;
             off_cnt = 0;

	     refresh_prolog ();

             MUTEX_LOCK (&mx_svr);
	     for (eq_iter = eq_list->begin (); eq_iter != eq_list->end (); ++eq_iter) {
		  p = *eq_iter;
                  MUTEX_UNLOCK (&mx_svr);

                  p->lock ();

		  p->device_counter_ = ++k;

		  p->update ();

		  i = p->update_thr_.value ();
		  p->update_thr_.set_value (i + 1);
		  if (p->g_sts_.error ()) 	 ++crt;
		  if (p != sp) {	// skip SVR instance
		      if (p->g_sts_.online ())   ++on_cnt;
		      else			 ++off_cnt;
                  }
		  if (p->g_sts_.online ())
		      p->last_update_.set_timevalue (tp1.tv_sec, tp1.tv_usec / 1000);

                  p->unlock ();

                  MUTEX_LOCK (&mx_svr);
	     }
             MUTEX_UNLOCK (&mx_svr);

	     refresh_epilog ();

             sp->lock ();

	     sp->svr_update_epilog ();
	     sp->error_count_.set_value (crt);	    // set counter
	     sp->dev_online_->set_value (on_cnt);   // set online counter
	     sp->dev_offline_->set_value(off_cnt);  // set offline counter

	     gettimeofday (&tp2, 0);

	     dt = (tp2.tv_sec - tp1.tv_sec) * 1.0e6 + tp2.tv_usec - tp1.tv_usec;
	     i  = int(dt / UPDATE_TIME_MAX_MS + 0.5);

	     if (i < 0) i = 0;
	     if (i > 1999) i = 1999;

             f = sp->update_time_.read_spectrum (i);
             sp->update_time_.fill_spectrum (i, f + 1.0);

             rqtp.tv_sec  = sp->rate_.value ()->i1_data;
             rqtp.tv_nsec = sp->rate_.value ()->i2_data * 1000;

             sp->unlock ();

             THREAD_SIGSETMASK (SIG_UNBLOCK, &mask, 0);
        }
        return ip;
}







static void *
writer (void *ip)
{
	int		   rb;
	int		   mb;
	int		   ouid;
	int		   ogid;
	int		   xuid;
	int		   xgid;
	int                sts;
        int                err;
        long               ct;
        char               *errstr;
	struct tm          t;
	EqFct              *p;
        EqFctSvr           *sp;
	char	           tmp     [100];
        char               bakfile [200];
        char		   buffer  [200];
        sigset_t           msk;
        struct stat        buf;

	static bool	   print = false;

        /*

        the following signals are masked:

                   SIGALRM,
                   SIGPIPE,
                   SIGTERM,
                   SIGBUS,
                   SIGINT,
                   SIGUSR1,
                   SIGUSR2,
                   SIGURG
        */

        sigemptyset (&msk);
        sigaddset (&msk, sig_ext);

        MUTEX_LOCK (&mx_clnt);
        ouid = access_perm.oper_uid;
        ogid = access_perm.oper_gid;
        xuid = access_perm.xpert_uid;
        xgid = access_perm.xpert_gid;
        MUTEX_UNLOCK (&mx_clnt);

        MUTEX_LOCK (&mx_svr);
	std::vector<EqFct *>::iterator eq_iter; // list of all locations
        sp = server_eq;
        rb = ring_buffer;
        mb = memory_buffer;
        MUTEX_UNLOCK (&mx_svr);

        wait_mt (); // wait to switch to MT mode

        for ( ; ; ) {

             MUTEX_LOCK (&mx_wr);
             block   = 0;
             while (!flag_wr) cond_wait (&cv_wr, &mx_wr);
             flag_wr = 0;
             block   = 1;
             MUTEX_UNLOCK (&mx_wr);

             THREAD_SIGSETMASK (SIG_BLOCK, &msk, 0);

             sp->lock ();
	     
	     err = no_error;

             // generate config backup file name

	     strcpy (bakfile, sp->file_.value ());
	     strcat (bakfile, ".BAK" );

             if (stat (sp->file_.value (), &buf) < 0) {
	         strcpy (tmp, "config file error ");
	         sp->set_error (10000 + errno, (char *) tmp);
		 err = device_error;

		 if (errno == EEXIST) {

                     // try to remove, when file seems to exist

		     if (unlink ((char *) sp->file_.value ()) < 0) {

			 if (!print) {
			     char     err_str [200];

			     snprintf (err_str, 200, "Cannot unlink config file! ERRNO : %d", errno );
			     printtostderr ("writer thread", err_str);
			     print = true;
			 }
		     }
		 }

             } else {

		 // config file should be longer then 100 bytes, if copied

		 if (buf.st_size > 100) {

 		     // rename the config file - to make a backup

		     sts = rename ((char *) sp->file_.value (), (char *) bakfile);

		     // if error in rename

		     if (sts) {
	        	 strcpy (tmp, "Rename error in ");
	        	 strncat (tmp, sp->file_.value (), 62);

	        	 errstr = strerror (errno);
	        	 if (errstr) {
			     strcat (tmp, ": ");
			     strncat (tmp, errstr, sizeof (tmp) - strlen (tmp));
	        	 }

	        	 sp->set_error (10000 + errno, (char *) tmp);
		 	 err = device_error;
		     }

		 } else {

                     // config file length is 0

		     strcpy (tmp, "config file smaller 100 bytes");
		     sp->set_error (device_error, (char *) tmp);
		     err = device_error;

                     remove ((char *) sp->file_.value ());
        	 }
 		 print = true;
            }

	     // create new config file
	     std::fstream ConfigFile(sp->file_.value (), ios_base::out | ios_base::trunc);

             if (!ConfigFile.is_open ()) {
	         strcpy  (tmp, "Write error in ");
	         strncat (tmp, server_eq->file_.value (), 62);

	         errstr = strerror (errno);
	         if (errstr) {
	             strcat  (tmp, ": ");
	             strncat (tmp, errstr, sizeof (tmp) - strlen (tmp));
	         }

	         sp->set_error (10000 + errno, (char *) tmp);
		 err = device_error;
                 sp->unlock ();

                 THREAD_SIGSETMASK (SIG_UNBLOCK, &msk, 0);
                 continue;
             }

	     ConfigFile.clear();
	     
             // write config file header
	     
	     ct = ::time (0);
	     localtime_r (&ct, &t);
	     strftime (tmp, sizeof (tmp), "%H:%M.%S %e. %h. %Y", &t);

             sprintf (buffer, "# Conf file created at %s\n", tmp); 
             ConfigFile.write (buffer, strlen (buffer));

             sprintf (buffer, "# eq_fct_type's are defined in eq_fct_code.h\n\n"); 
             ConfigFile.write (buffer, strlen (buffer));

             sprintf (buffer, "eq_conf: \n\n"); 
             ConfigFile.write (buffer, strlen (buffer));

             sprintf (buffer, "oper_uid: \t%d\n", ouid); 
             ConfigFile.write (buffer, strlen (buffer));

             sprintf (buffer, "oper_gid: \t%d\n", ogid); 
             ConfigFile.write (buffer, strlen (buffer));

             sprintf (buffer, "xpert_uid: \t%d\n", xuid); 
             ConfigFile.write (buffer, strlen (buffer));

             sprintf (buffer, "xpert_gid: \t%d\n", xgid); 
             ConfigFile.write (buffer, strlen (buffer));

             sprintf (buffer, "ring_buffer: \t%d\n", rb); 
             ConfigFile.write (buffer, strlen (buffer));

             sprintf (buffer, "memory_buffer: \t%d\n", mb); 
             ConfigFile.write (buffer, strlen (buffer));

             sprintf (buffer, "\n"); 
             ConfigFile.write (buffer, strlen (buffer));

             // write config file entry for _______

	     sp->write (ConfigFile); // <=============== EqFctSvr
             sp->unlock ();

             MUTEX_LOCK (&mx_svr);
	     for (eq_iter = eq_list->begin (); eq_iter != eq_list->end (); ++eq_iter) {
                  p = *eq_iter;

                  if (p == sp) continue;

                  MUTEX_UNLOCK (&mx_svr);

                  // write config file entry for _______

                  p->lock ();
                  p->write (ConfigFile); // <=============== EqFct
                  p->unlock ();

                  MUTEX_LOCK (&mx_svr);
             }
             MUTEX_UNLOCK (&mx_svr);

             ConfigFile.flush ();
             ConfigFile.close ();
	     
	     if (!err) sp->set_error (no_error);

             THREAD_SIGSETMASK (SIG_UNBLOCK, &msk, 0);
	     
        } // end of endless loop
        return ip;
}




static void *
interrupt_usr1 (void *ip)
{
	int		   i;
	int		   k;
	int		   crt;
	int		   on_cnt;
	int		   off_cnt;
	int		   sig;
	float		   f;
	double		   dt;
        sigset_t           msk;
        sigset_t           mask;
	struct timeval     tp1;
	struct timeval     tp2;
	EqFct              *p;
        EqFctSvr           *sp;

        /*

        the following signals are masked:

                   SIGALRM,
                   SIGPIPE,
                   SIGTERM,
                   SIGBUS,
                   SIGINT,
                   SIGUSR1,
                   SIGUSR2,
                   SIGURG
        */

        MUTEX_LOCK (&mx_pid);
        sigpid = getpid ();
        COND_SIGNAL (&cv_pid);
        MUTEX_UNLOCK (&mx_pid);

        sigemptyset (&msk);
        sigaddset (&msk, sig_ext);

        wait_mt (); // wait to switch to MT mode

        MUTEX_LOCK (&mx_usr1);
        while (!flag_usr1) COND_WAIT (&cv_usr1, &mx_usr1);
        MUTEX_UNLOCK (&mx_usr1);

        MUTEX_LOCK (&mx_svr);
	std::vector<EqFct *>::iterator eq_iter; // list of all locations
        sp = server_eq;
        MUTEX_UNLOCK (&mx_svr);

        if (act_table.valid) (*act_table.proc) (act_table.param);

        for ( ; ; ) {

             sigemptyset (&mask);

             MUTEX_LOCK (&mx_usr1);
             if (sg_proc_usr1 [0]) sigaddset (&mask, SIGUSR1);
             if (sg_proc_usr1 [1]) sigaddset (&mask, SIGUSR2);
             if (sg_proc_usr1 [2]) sigaddset (&mask, SIGURG);
             MUTEX_UNLOCK (&mx_usr1);

             #ifdef __GNUC__

             sigwait (&mask, &sig);
             #else

             sig = sigwait (&mask);
             #endif

             switch (sig) {

             case SIGUSR1:
             case SIGUSR2:
             case SIGURG:
                  if (sig == SIGUSR1) sig = 1;
                  if (sig == SIGUSR2) sig = 2;
                  if (sig == SIGURG)  sig = 3;

                  gettimeofday (&tp1, 0);

                  THREAD_SIGSETMASK (SIG_BLOCK, &msk, 0);

                  interrupt_usr1_prolog (sig);
 
                  sp->lock ();
                  sp->error_count_.set_value (0);
                  sp->unlock ();

                  k       = 0;
                  crt     = 0;
                  on_cnt  = 0;
                  off_cnt = 0;

                  MUTEX_LOCK (&mx_svr);
	   	  for (eq_iter = eq_list->begin (); eq_iter != eq_list->end (); ++eq_iter) {
                       p = *eq_iter;
                       MUTEX_UNLOCK (&mx_svr);

                       p->lock ();
                       p->device_counter_ = k++;

                       p->interrupt_usr1 (sig);

                       if (p != sp) {	// skip SVR instance
                           if (p->g_sts_.newerror ()) crt++;
                           if (p->g_sts_.online ())   on_cnt++;
                           if (!p->g_sts_.online ())  off_cnt++;
                       }
                       if (p->g_sts_.online ())
			   p->last_update_.set_timevalue (tp1.tv_sec, tp1.tv_usec / 1000);

                       p->unlock ();

                       MUTEX_LOCK (&mx_svr);
                  }
                  MUTEX_UNLOCK (&mx_svr);

                  interrupt_usr1_epilog (sig);

                  sp->lock ();

                  //sp->device_max_ = k;
                  sp->error_count_.set_value (crt);  	 // set counter
                  sp->dev_online_->set_value (on_cnt);	 // set online counter
                  sp->dev_offline_->set_value (off_cnt); // set offline counter

                  sp->update ();

                  gettimeofday (&tp2, 0);
                  dt = (tp2.tv_sec - tp1.tv_sec) * 1.0e6 + tp2.tv_usec - tp1.tv_usec;
                  i  = int (dt / UPDATE_TIME_MAX_MS + 0.5);

                  if (i < 0)	i = 0;
                  if (i > 1999)	i = 1999;

                  f = sp->update_time_.read_spectrum (i);
                  sp->update_time_.fill_spectrum (i, f + 1.0);

                  sp->unlock ();

                  THREAD_SIGSETMASK (SIG_UNBLOCK, &msk, 0);
                  break;

             default:
                  break;
             }
        }
        return ip;
}



static void *
interrupt_usr2 (void *ip)
{
	int		   sig;
        sigset_t           mask;
        EqFctSvr           *sp;

        /*

        the following signals are masked:

                   SIGALRM,
                   SIGPIPE,
                   SIGTERM,
                   SIGBUS,
                   SIGINT,
                   SIGUSR1,
                   SIGUSR2,
                   SIGURG
        */

        sigemptyset (&mask);

        wait_mt (); // wait to switch to MT mode

        MUTEX_LOCK (&mx_usr2);
        while (!flag_usr2) cond_wait (&cv_usr2, &mx_usr2);
        flag_usr2 = 0;
        MUTEX_UNLOCK (&mx_usr2);

        MUTEX_LOCK (&mx_svr);
	//std::vector<EqFct *>::iterator eq_iter; // list of all locations
        sp = server_eq;
        MUTEX_UNLOCK (&mx_svr);

        for ( ; ; ) {
             #ifdef __GNUC__

             sigwait (&mask, &sig);
             #else

             sig = sigwait (&mask);
             #endif
        }
        return ip;
}



int
create_con_table (int size)
{
	int       i;
        int       min;
        int       max;

        max = USTR_LENGTH / 2;
        min = max / 10;

        if (size < min) size = min;
        else
        if (size > max) size = max;

	MUTEX_LOCK (&mx_clnt);

        if (con_tab) delete [] con_tab;

        con_tab = (con_entry *) new con_entry [size];

        for (i = 0; i < size; i++) {
             memset (con_tab [i].name, 0, sizeof (con_tab [i].name));
             con_tab [i].count = 0;
             con_tab [i].uid   = 0;
             con_tab [i].gid   = 0;
             con_tab [i].get   = 0;
             con_tab [i].set   = 0;
             con_tab [i].names = 0;
             con_tab [i].time  = 0;
        }
        con_tab_size = size;
        con_pos      = 0;

	MUTEX_UNLOCK (&mx_clnt);
        return size;
}



int
read_con_table (EqData *ed)
{
	int       i;
	int	  n;
	char      *cval;
	int	  ival;
	float	  fval1;
	float	  fval2;
	time_t	  tval;

	MUTEX_LOCK (&mx_clnt);
	for (n = i = 0; con_tab && i < con_tab_size; i++) {
             cval  = (char *) con_tab [i].name;
             ival  = (int)    con_tab [i].count;
             fval1 = (float)  con_tab [i].uid;
             fval2 = (float)  con_tab [i].gid;
             tval  = (time_t) con_tab [i].time;

             ed->set (ival, fval1, fval2, tval, cval, n++);

             ival  = (int)    con_tab [i].names;
             fval1 = (float)  con_tab [i].get;
             fval2 = (float)  con_tab [i].set;
             tval  = (time_t) con_tab [i].time;

             ed->set (ival, fval1, fval2, tval, cval, n++);
	}
	MUTEX_UNLOCK (&mx_clnt);
        return n;
}




static int
update_con_table (int proc, permissions *pp)
{
	int       i;
	int       permit;

	permit = 0;

	MUTEX_LOCK (&mx_clnt);
	for (i = 0; i < con_pos; i++) {
             if (!strcmp (pp->hostname, con_tab [i].name)) break;
	}
	if (i == con_pos) {
             strncpy (con_tab [i].name, pp->hostname, sizeof (con_tab [0].name));
             con_tab [i].name [sizeof (con_tab [0].name) - 1] = '\0';
             con_tab [i].count  = 0;
             con_tab [i].get    = 0;
             con_tab [i].set    = 0;
             con_tab [i].names  = 0;

             // increment index to the next free
             // position of the connection table

             if (con_pos < con_tab_size - 1) con_pos++;
	}
	con_tab [i].uid  = pp->call_uid;
	con_tab [i].gid  = pp->call_gid;
	con_tab [i].time = pp->call_time;

	con_tab [i].count++;
	switch (proc) {

	case RPC_EQ_GET:
                 con_tab [i].get++;
 	         permit = 1;
                 break;

	case RPC_EQ_SET:
                 con_tab [i].set++;
 	         permit = 1;
                 break;

	case RPC_EQ_NAMES:
                 con_tab [i].names++;
 	         permit = 1;
                 break;
	}
	MUTEX_UNLOCK (&mx_clnt);
	return permit;
}



static void 
eqlibprog_1 (struct svc_req *rqstp, SVCXPRT *transp)
{
	union iparam {

	      get_param       get;
	      set_param       set;
	      name_param      names;
	      create_param    create;
	};

	union oparam {

	      get_results     get;
	      set_results     set;
	      name_results    names;
	      create_results  create;
	};

	bool_t                (*local) (char *, void *, svc_req *, permission *);

        u_int                 i;
        int                   permit;
        double                f;
        double                dt;
        permissions           pp;
        union iparam          argument;
        union oparam          result;
	xdrproc_t             _xdr_argument;
	xdrproc_t             _xdr_result;
	struct timeval        tp1;
	struct timeval        tp2;
        struct authunix_parms *ux_cred;
        EqFctSvr              *sp;

        gettimeofday (&tp1, 0);

	permit = 0;
	if (rqstp->rq_cred.oa_flavor != AUTH_UNIX) {

	    MUTEX_LOCK (&mx_clnt);
	    con_name = "unknown";
	    MUTEX_UNLOCK (&mx_clnt);

        } else {

	    ux_cred      = (struct authunix_parms *) rqstp->rq_clntcred;
	    con_name     = ux_cred->aup_machname;

            pp.call_time = tp1.tv_sec;
	    pp.call_mask = 0;
	    pp.call_uid  = ux_cred->aup_uid;
	    pp.call_gid  = ux_cred->aup_gid;
	    pp.hostname  = ux_cred->aup_machname;
	    pp.aup_gids  = ux_cred->aup_gids;
	    pp.aup_len   = ux_cred->aup_len;
	    pp.prog_uid  = access_perm.prog_uid;
	    pp.prog_gid  = access_perm.prog_gid;
	    pp.oper_uid  = access_perm.oper_uid;
	    pp.oper_gid  = access_perm.oper_gid;
	    pp.xpert_uid = access_perm.xpert_uid;
	    pp.xpert_gid = access_perm.xpert_gid;

            // update connection table

            permit = update_con_table (rqstp->rq_proc, &pp);
        }

	switch (rqstp->rq_proc) {

	case NULLPROC:
	     svc_sendreply (transp, (xdrproc_t) xdr_void, (char *) 0);
	     return;

	case RPC_EQ_GET:
	     local = (bool_t (*) (char *, void *, svc_req *, permissions *)) rpc_svr_get;
	     _xdr_argument = (xdrproc_t) xdr_get_param;
	     _xdr_result   = (xdrproc_t) xdr_get_results;
	     break;

	case RPC_EQ_SET:
	     local = (bool_t (*) (char *, void *, svc_req *, permissions *)) rpc_svr_set;
	     _xdr_argument = (xdrproc_t) xdr_set_param;
	     _xdr_result   = (xdrproc_t) xdr_set_results;
	     break;

	case RPC_EQ_NAMES:
	     local = (bool_t (*) (char *, void *, svc_req *, permissions *)) rpc_svr_names;
	     _xdr_argument = (xdrproc_t) xdr_name_param;
	     _xdr_result   = (xdrproc_t) xdr_name_results;
	     break;

	case RPC_EQ_CREATE:
	     local = (bool_t (*) (char *, void *, svc_req *, permissions *)) rpc_svr_create;
	     _xdr_argument = (xdrproc_t) xdr_create_param;
	     _xdr_result   = (xdrproc_t) xdr_create_results;
	     break;

	default:
	     svcerr_noproc (transp);
	     return;
	}
	memset ((void *) &argument, 0, sizeof (argument));
	memset ((void *) &result, 0, sizeof (result));

	if (!svc_getargs (transp, _xdr_argument, (caddr_t) &argument)) {
	    svcerr_decode (transp);
	    return;
	}
	if (!permit) svc_sendreply (transp, (xdrproc_t) xdr_void, (char *) 0);
        else {

	    if ((*local) ((char *) &argument, (void *) &result, rqstp, &pp) > 0) {
                int    val;

   	        MUTEX_LOCK (&mx_ext);
                val = ext_flag; // is the server going down ?
   	        MUTEX_UNLOCK (&mx_ext);

                // if no, send the results back to the client,
                // otherwise exit without sending

                if (!val && !svc_sendreply (transp, _xdr_result, (char *) &result)) {
   	            svcerr_systemerr (transp);
                }
            }
	    if (!eqlibprog_1_freeresult (transp, _xdr_result, (char *) &result)) {
		printtostderr ("eqlibprog_1", "unable to free the result");
	    }
	} 
	if (!svc_freeargs (transp, _xdr_argument, (char *) &argument)) {
	    printtostderr ( "eqlibprog_1", "unable to free the argument");
	}

	gettimeofday (&tp2, 0);

	dt = (tp2.tv_sec - tp1.tv_sec) * 1.0e6 + tp2.tv_usec - tp1.tv_usec;
	i  = (u_int) (dt / 50.0 + 0.5);

        sp = server_eq;

        sp->lock ();
	f = sp->rpc_call_time_.read_spectrum (i);
	sp->rpc_call_time_.fill_spectrum (i, f + 1.0);
        sp->unlock ();
}



static void
switch_mt (void)
{
        MUTEX_LOCK (&mx_svr);

        svr_mode = 1;

        COND_SIGNAL_BROADCAST (&cv_mode);
        MUTEX_UNLOCK (&mx_svr);
}


void
wait_mt (void)
{
        MUTEX_LOCK (&mx_svr);

        // wait to switch to MT mode

        while (!svr_mode) COND_WAIT (&cv_mode, &mx_svr);
        MUTEX_UNLOCK (&mx_svr);
}


pid_t
get_sigpid (void)
{
        // MUST be called from:
        //
        //     - post_init_prolog () or
        //     - xxx::post_init ()   or
        //     - post_init_epilog ()
        //
        // otherwise the server hangs !!!
        //

        pid_t      pid;

        if (!build_phase) return -1;

        MUTEX_LOCK (&mx_pid);
        while (sigpid == -1) COND_WAIT (&cv_pid, &mx_pid);
        pid = sigpid;
        MUTEX_UNLOCK (&mx_pid);
        return pid;
}



int 
setup_action (enum act cmd, void (*hdlp) (void *), void *param)
{
        // cmd defines an action
        //
        // at present we have only one
        // action - PREPARE,
        // see enum act in eq_fct.h

        int      rc;

        if (cmd != PREPARE) return -1;

        rc = MUTEX_TRYLOCK (&mx_svr);

        if (hdlp == (void (*) (void *)) 0) 
             act_table.valid = 0;
        else act_table.valid = 1;

        act_table.proc  = hdlp;
        act_table.param = param;

        if (!rc) MUTEX_UNLOCK (&mx_svr);
        return 0;
}




int 
setup_interrupt_usr1 (int snum)
{
        // snum defines:
        //
        //  0 - SIGUSR1
        //  1 - SIGUSR2
        //  2 - SIGURG

        if (snum < 1 || snum > 3) return -1;

        MUTEX_LOCK (&mx_usr1);
        if (!flag_usr1) {
            flag_usr1 = 1;
            COND_SIGNAL (&cv_usr1);
        }
        sg_proc_usr1 [snum - 1] = 1;
        MUTEX_UNLOCK (&mx_usr1);
        return 0;
}




int
setup_interrupt_usr2 (char *, int)
{
        MUTEX_LOCK (&mx_usr2);
        flag_usr2 = 1;
        COND_SIGNAL  (&cv_usr2);
        MUTEX_UNLOCK (&mx_usr2);
        return -1;
}


void
signal_writer (void)
{
        MUTEX_LOCK (&mx_wr);
        flag_wr = 1;
        COND_SIGNAL  (&cv_wr);
        MUTEX_UNLOCK (&mx_wr);
}



static void
sig_handler (int)
{
}


void
set_rate_flag (void)
{
        MUTEX_LOCK (&mx_sig);
        flag_sig = 1;
        MUTEX_UNLOCK (&mx_sig);
}



int
get_rate_flag (void)
{
        int    flag;

        MUTEX_LOCK (&mx_sig);
        flag = flag_sig;
        flag_sig = 0;
        MUTEX_UNLOCK (&mx_sig);

        return flag;
}





extern "C" void
exit_thread (int sig)
{
        THREAD_T    id;

        if (sig != sig_ext) return;

	#ifdef __GNUC__

        id = pthread_self ();
	#else

        id = thr_self ();

	#endif

        MUTEX_LOCK (&mx_ext);
        if (!ext_threads) {
            MUTEX_UNLOCK (&mx_ext);
            return;
        }

        if (++ext_counter == ext_threads) COND_SIGNAL (&cv_ext);
        MUTEX_UNLOCK (&mx_ext);

        if (id == r_id) svc_exit ();

        THREAD_EXIT (0);
}



static void
create_all_threads (int param)
{
        // when adding a new thread to the server
        // the function exit_all_threads ()
        // must be MODIFIED too !!!!

        r_id          = (u_int) -1;
        t_id          = (u_int) -1;
        w_id          = (u_int) -1;
        i1_id         = (u_int) -1;
        i2_id         = (u_int) -1;
        tine_id       = (u_int) -1;
        err_id        = (u_int) -1;
        err_rec_id    = (u_int) -1;
        err_alive_id  = (u_int) -1;

        #ifdef __GNUC__

        pthread_attr_t      thra;
        pthread_attr_t      thrab;


        pthread_attr_init (&thra);
        pthread_attr_setscope (&thra, PTHREAD_SCOPE_PROCESS);

        pthread_attr_init (&thrab);
        pthread_attr_setscope (&thrab, PTHREAD_SCOPE_SYSTEM);

        pthread_create (&t_id, &thrab, timer, (void *) 0);
        pthread_create (&r_id, &thrab, rpc, (void *) 0);
        pthread_create (&w_id, &thra, writer, (void *) 0);
        pthread_create (&i1_id, &thrab, interrupt_usr1, (void *) 0);
        pthread_create (&i2_id, &thra, interrupt_usr2, (void *) 0);
	pthread_create (&err_id, &thra, error, (void *) 0);
	pthread_create (&err_rec_id, &thra, error_recover, (void *) 0);
	pthread_create (&err_alive_id, &thra, error_alive, (void *) 0);
	if (!param) pthread_create (&tine_id, &thra, tine, (void *) 0);

        pthread_attr_destroy (&thra);
        pthread_attr_destroy (&thrab);
        #else

        thr_create (0, 0, timer, (void *) 0, THR_BOUND, &t_id);
        thr_create (0, 0, rpc, (void *) 0, THR_BOUND, &r_id);
        thr_create (0, 0, writer, (void *) 0, 0, &w_id);
        thr_create (0, 0, interrupt_usr1, (void *) 0, THR_BOUND, &i1_id);
        thr_create (0, 0, interrupt_usr2, (void *) 0, 0, &i2_id);
	thr_create (0, 0, error, (void *) 0, 0, &err_id);
	thr_create (0, 0, error_recover, (void *) 0, 0, &err_rec_id);
	thr_create (0, 0, error_alive, (void *) 0, 0, &err_alive_id);
	if (!param) thr_create (0, 0, tine, (void *) 0, 0, &tine_id);

        #endif
}



static void
exit_all_threads (void)
{
        int             err;
        int             errtm;
        int             count;

        #ifdef __GNUC__

        timespec        tmo;

        errtm = ETIMEDOUT;
        #else

        timestruc_t     tmo;

        errtm = ETIME;
        #endif

        count = 0;

	MUTEX_LOCK (&mx_ext);

	ext_threads = 0;
	ext_counter = 0;
        ext_flag    = 1;

	if (t_id != (u_int) -1) {
            err = THREAD_KILL (t_id, 0);
            if (!err) count++;
        }
	if (r_id != (u_int) -1) {
            err = THREAD_KILL (r_id, 0);
            if (!err) count++;
        }
	if (w_id != (u_int) -1) {
            err = THREAD_KILL (w_id, 0);
            if (!err) count++;
        }
	if (i1_id != (u_int) -1) {
            err = THREAD_KILL (i1_id, 0);
            if (!err) count++;
        }
	if (i2_id != (u_int) -1) {
            err = THREAD_KILL (i2_id, 0);
            if (!err) count++;
        }
	if (tine_id != (u_int) -1) {
            err = THREAD_KILL (tine_id, 0);
            if (!err) count++;
        }
	if (err_id != (u_int) -1) {
            err = THREAD_KILL (err_id, 0);
            if (!err) count++;
        }
	if (err_rec_id != (u_int) -1) {
            err = THREAD_KILL (err_rec_id, 0);
            if (!err) count++;
        }
	if (err_alive_id != (u_int) -1) {
            err = THREAD_KILL (err_alive_id, 0);
            if (!err) count++;
        }
        if (!count) {
	    MUTEX_UNLOCK (&mx_ext);
            return;
        }
	ext_threads = count;

	if (r_id != (u_int) -1)         THREAD_KILL (r_id, sig_ext);
	if (t_id != (u_int) -1)         THREAD_KILL (t_id, sig_ext);
	if (w_id != (u_int) -1)         THREAD_KILL (w_id, sig_ext);
	if (i1_id != (u_int) -1)        THREAD_KILL (i1_id, sig_ext);
	if (i2_id != (u_int) -1)        THREAD_KILL (i2_id, sig_ext);
	if (tine_id != (u_int) -1)      THREAD_KILL (tine_id, sig_ext);
	if (err_id != (u_int) -1)       THREAD_KILL (err_id, sig_ext);
	if (err_rec_id != (u_int) -1)   THREAD_KILL (err_rec_id, sig_ext);
	if (err_alive_id != (u_int) -1) THREAD_KILL (err_alive_id, sig_ext);

        tmo.tv_sec  = time (0) + 5;
        tmo.tv_nsec = 0;

	err = COND_TIMEDWAIT (&cv_ext, &mx_ext, &tmo);
        if (err == errtm) printtostderr (banner, "forced termination !!!");
	MUTEX_UNLOCK (&mx_ext);
}


#ifdef SYSV

static void
reserve_slots (int *slot, int slot_num)
{
        int         fd;

        memset (slot, -1, slot_num * sizeof (int));

	for (int i = 0; i < slot_num; i++) {
             fd = dup (0);
             if (fd > 0) slot [i] = fd;
	}
}



static void
free_slots (int *slot, int slot_num)
{
	for (int i = 0; i < slot_num; i++) {
             if (slot [i] != -1) close (slot [i]);
	}
}


#endif




static void
get_eq_libvers (char *name, std::string &ver, std::string &date)
{
        FILE        *fp;
        char        *csp;
        char        cmd     [] = "ldd ";
        char        cmex    [] = " | grep Server";
        char        pattern [] = ".so.";
        char        tmp     [256];

	std::string		search;
	std::string		lscmd ("ls -l ");
	std::string::size_type 	idx;
	std::string::size_type 	lidx;

        csp = new char [strlen (cmd) + strlen (name) + strlen (cmex) + 1];
        strcpy (csp, cmd);
        strcat (csp, name);
        strcat (csp, cmex);

	putenv ("LC_TIME=C");	// to avoid German characters

        fp = popen (csp, "r");
        fgets (tmp, sizeof (tmp), fp);
        pclose (fp);

        delete [] csp;

	csp = strstr (tmp, " (");
        if (csp) *csp = '\0';

	search = tmp;
	idx    = search.find (pattern);
	if (idx != std::string::npos) {

	    search.erase (0, idx + 4);
	    idx = search.find (" ");

	    if (idx != std::string::npos) {
	   	ver = search.substr (0, idx);
		
		// now search lib name for ls command

	   	idx = search.rfind (" ");

	   	if (idx != std::string::npos) {
	 	    lscmd += search.substr (idx, search.length () - 1);

         	    fp = popen (lscmd.c_str (), "r");
        	    fgets (tmp, sizeof (tmp), fp);
                    pclose (fp);

		    search = tmp;
	   	    idx    = search.rfind (" ");
	   	    lidx   = idx;

		    search.erase (idx, search.length ());

	   	    idx = search.rfind (" ");

		    date.clear ();
		    date = search.substr (idx, search.length ());

		    search.erase (idx, search.length ());
		    date.insert ((string::size_type) 0, search.substr (idx - 6, idx));
		}  
	   }	
	}
}




static int 
get_working_dir (int argc, char **argv, char *pathp, int len, int *dbgp)
{
        // The environment variable $DOOCSSWD stores
        // the path to doocs servers like: /export/doocs/server
        //
        // The command line argument '-c'  also defines a way
        // in which the server can set its working directory:
        // 
        // * absent  - as the environment variable DOOCSSWD;
        // * present - from the command line;
        //
        // The setting of the command line argument has a
        // higher priority as the environment variable

        const char  evn  [] = "DOOCSSWD";

        int         c;
        int         ch;
        char        *evp;
        char        *cn;

        // parse the command line arguments

        for (c = 0; argc > 1; ) {

             ch = getopt (argc, argv, "cd:");

             if (ch == 'c') c = 1;
             if (ch == 'd') sscanf (optarg, "%d", dbgp);
             if (ch == EOF) break;
        }
        if (c) {
            cn = strrchr (argv [0], '/');
            if (!cn) return 0; // use the current directory

            // extract the path from the command line

            strncpy (pathp, argv [0], cn - argv [0]);
            return 1;
        }

        // get the environment variable

        evp = getenv (evn);
        if (evp) {

            // build the path from the environment variable
            //        "DOOCSSWD + server_name"

            // extract the name of the server from the command line

            cn = strrchr (argv [0], '/');
            if (cn) cn++;
            else    cn = argv [0]; // path is not part of the name

            strncpy (pathp, evp, len);
            strcat  (pathp, "/");
            strncat (pathp, cn,  len - strlen (evp));

            return 1; // change the working directory
        }
        return 0; // use the current directory
}



void
get_buf_size (int *sbsp, int *rbsp)
{
        *sbsp = sbuf_size;
        *rbsp = rbuf_size;
}




void
set_buf_size (int bmask, int sbs, int rbs)
{
        const int    buf_max = 8 * 1024 * 1024; // 8 Mb
        const int    buf_min = 8 * 1024;        // 8 Kb

        if ((bmask & 2) && (sbs >= buf_min && sbs <= buf_max)) {
            sbuf_size = sbs;
        }
        if ((bmask & 1) && (rbs >= buf_min && rbs <= buf_max)) {
            rbuf_size = rbs;
        }
}




/* 
 
static int
set_bsize (int fd, int opt, int value)
{
        int       rc;
        int       nm;

        rc = 0;
        nm = opt ? SO_RCVBUF : SO_SNDBUF;

        #ifdef __GNUC__

        rc = setsockopt (fd, SOL_SOCKET, nm, (char *) &value, sizeof (value));
        rc = rc ? 0 : value;

        #else
        struct t_optmgmt   *req;
        struct t_optmgmt   *ret;
        struct opthdr      *hdr;

        req = (struct t_optmgmt *) t_alloc (fd, T_OPTMGMT, T_ALL);
        ret = (struct t_optmgmt *) t_alloc (fd, T_OPTMGMT, T_ALL);
        hdr = (struct opthdr *) req->opt.buf;

        *((int *) OPTVAL (hdr)) = value; // save the value of the option

        hdr->level   = SOL_SOCKET;
        hdr->name    = nm;
        hdr->len     = OPTLEN (sizeof (int));

        req->flags   = T_NEGOTIATE;
        req->opt.len = sizeof (struct opthdr) + OPTLEN (sizeof (int));

        ret->flags   = 0;

        rc = t_optmgmt (fd, req, ret);

        if (rc || ret->flags != T_SUCCESS) rc = 0;
        else {
            hdr = (struct opthdr *) ret->opt.buf;
            rc  = *((int *) OPTVAL (hdr)); // get the value of the option
        }

        t_free ((char *) req, T_OPTMGMT);
        t_free ((char *) ret, T_OPTMGMT);

        #endif

        return rc;
}





static int
get_bsize (int fd, int opt)
{
        int       rc;
        int       nm;

        rc = 0;
        nm = opt ? SO_RCVBUF : SO_SNDBUF;

        #ifdef __GNUC__
        int       len;
        int       value;

        len = sizeof (value);

        rc = getsockopt (fd, SOL_SOCKET, nm, (char *) &value, (socklen_t *) &len);
        rc = rc ? 0 : value;

        #else
        struct t_optmgmt   *req;
        struct t_optmgmt   *ret;
        struct opthdr      *hdr;

        req = (struct t_optmgmt *) t_alloc (fd, T_OPTMGMT, T_ALL);
        ret = (struct t_optmgmt *) t_alloc (fd, T_OPTMGMT, T_ALL);
        hdr = (struct opthdr *) req->opt.buf;

        hdr->level   = SOL_SOCKET;
        hdr->name    = nm;
        hdr->len     = 0;

        req->flags   = T_CURRENT;
        req->opt.len = sizeof (struct opthdr);

        rc = t_optmgmt (fd, req, ret);
        if (rc || ret->flags != T_SUCCESS) rc = 0;
        else {
            hdr = (struct opthdr *) ret->opt.buf;
            rc = *((int *) OPTVAL (hdr)); // get the value of the option
        }

        t_free ((char *) req, T_OPTMGMT);
        t_free ((char *) ret, T_OPTMGMT);

        #endif

        return rc;
}

*/
