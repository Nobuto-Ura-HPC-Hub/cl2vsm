#ifndef __GPFN_UTILS__H__
#define __GPFN_UTILS__H__

unsigned int *gpfn_get_wbufp(void);
void gpfn_readinst(char filename[],unsigned int qinst[][32],int *iqret);
void gpfn_sendinst(unsigned int qinst0[][32], int iq, int nopflag);
void gpfn_recv(unsigned int wd, int nword, int offset, int adrpdm);
void gpfn_send(unsigned int dmaid, int nword, int offset, int adrpdm);
int gpfn_open();
int gpfn_close();
void gpfn_setvsmwrite();
void gpfn_setvsmwriteoff();

void gpfn_setvsmfile_ro(char filename[]);
void gpfn_setvsmfile(char filename[]);
void gpfn_sendvsm();
void qvsm(char* format, ...);
void gpfn_sendinst_kickonly();
void gpfn_ibufassign(char filename[]);

void gpfn_send_wd(unsigned int dmaid, int nword, int offset, int adrpdm, int wd);
void gpfn_recv_dmaid(unsigned int wd, int nword, int offset, int adrpdm, int dmaid);
int is_vsmwrite();
int error_monitor(int do_fprintf);

#endif /* __GPFN_UTILS__H__ */
