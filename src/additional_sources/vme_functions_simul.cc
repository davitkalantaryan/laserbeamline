
#if 0
class VME {
   private:
    std::string	fname;		// config file name
    char		dev_name[20];	// "/dev/sbusx"
    int		sbus_slot;	// 1 .. 4		?? nicht noetig
    u_long		tmp_start_addr;	// temp start_addr
    caddr_t		vpage_start;	// mapped virtual page start
    off_t		page_start;	// SBus addr of page start
    u_long		page_offset;	//
    u_short		page_size;	// system page size in bytes
    std::string	cc;		// name of SBus controller "SBUSINTERFACE"
    caddr_t		cc_vpage_start;	// controller mapped virtual page start
    Config* 	conf;
    int		error_cond;
    int		filedes;	// device file descriptor
    VME_table*	find_name (const char *); // module name lookup
    char		host[20];
        int             mswap;
   public:
    int		temp[8];  	    // temp parameter
        int             temp_found;         // temp found parameter
    std::string     str_tmp;	    // temp name
    VME_table*      top_of_table;

        u_char  read_vme_char	(caddr_t base_offset, int &err);
    u_char  write_vme_char	(caddr_t base_offset, u_char vme_data, int &err);
    u_short read_vme_short	(caddr_t base_offset, int &err);
    u_short write_vme_short	(caddr_t base_offset, u_short vme_data, int &err);
    u_int   read_vme_int	(caddr_t base_offset, int &err);
    u_int   write_vme_int	(caddr_t base_offset, u_int vme_data, int &err);
    int read_buf_short   (caddr_t base_offset, u_short* shbuf, int smpl, int &err, int buf_stp , int vme_stp );
    int write_buf_short  (caddr_t base_offset, u_short* shbuf, int smpl, int &err, int buf_stp , int vme_stp );
    int read_buf_int     (caddr_t base_offset, u_int* intbuf, int smpl, int &err, int buf_stp , int vme_stp );
    int write_buf_int    (caddr_t base_offset, u_int* intbuf, int smpl, int &err, int buf_stp , int vme_stp );
    void set_sigbus ();
    void rem_sigbus ();
    int  get_start_addr	(const char *);
    int  get_space		(const char *);

   public:
    VME ();		// read config file ..
    ~VME() {};

    caddr_t map_device (const char *);// maps the nemed device eturns start addr of the VME module or -1 on error
    void unmap_device (const char *); // unmaps the named device

};
#endif

#include "VME.h"

VME::VME(){}
caddr_t VME::map_device (const char *){return (caddr_t)0;}
