#include <stdio.h>
#include <math.h>
#include <unistd.h>    // sleep()
#include <string.h>    // strcpy()
#include "gpfnutil.h"
#include <stdarg.h>
#include <assert.h>

#include "mncl_utils.h"

static unsigned int *W0buf;
static unsigned int *W1buf;
static unsigned int *Rbuf;

#define W0_OFFSET   (   0*1024/4)  // ZERO
#define W1_OFFSET   ( 512*1024/4)  // 0.5MB in 32bit word
#define RECV_OFFSET (1024*1024/4)  // 1.0MB in 32bit word

int
main(int argc, char *argv[])
{
    int rv;
    unsigned int n;
    unsigned int view_n;
    double *w0dp, *w1dp;
    double *rdp;
    int w0_tag_id;
    int w1_tag_id;
    int rd_tag_id;
    char dlim_c;
    int w0_pdm_adr;
    int w1_pdm_adr;
    int rd_pdm_adr;
    int err_n = 0;
    int w0_lc_addr;
    int w1_lc_addr;
    int rd_lc_addr;

    int wait_l2b_tag_id;

    int w0_lb_addr;
    int w1_lb_addr;
    int r0_lb_addr;
    int r1_lb_addr;

    double m_e = M_E;
    int m_e_a[3];
    *(double *)m_e_a = m_e;

    CHECK_VSM_WRITE(argc, argv);

    rv = gpfn_open();
    if ( rv != 0 ) {
        error_monitor(1);
        return 1;
    }
    W0buf = gpfn_get_wbufp();
    W1buf = gpfn_get_wbufp() + W1_OFFSET;
    Rbuf = gpfn_get_wbufp() + RECV_OFFSET ;

    n = 8192;
    view_n = 16;

    for(unsigned int i=0;i<4*n;i++) Rbuf[i] = 0xFFFFFFFF;
    for(unsigned int i=0;i<4*n;i++) W0buf[i] = 0;
    for(unsigned int i=0;i<4*n;i++) W1buf[i] = 0;

#ifdef DEBUG
    for(unsigned int i=0;i<2*n;i++) W0buf[i] = i;
    W0buf[0] = 0xdeadbeaf + 0;
    W0buf[4] = 0xcafebabe + 0;
#endif

    w0dp = (double *)&W0buf[0];
    for( unsigned int i = 0, si = 0  ; i < n ; i++, si += 2, w0dp++ ) {
        *w0dp = (double)i;
        swap(&W0buf[si ], &W0buf[si + 1]);
    }

    w1dp = (double *)&W1buf[0];
    for( unsigned int i = 0, si = 0  ; i < n ; i++, si += 2, w1dp++ ) {
        *w1dp = (double)(n - i);
        swap(&W1buf[si ], &W1buf[si + 1]);
    }

    //----------------------------------------------------------------
#ifdef DEBUG
    {
        double *tmp_dp = (double *)&W0buf[2048];
        *tmp_dp = 8.123456;
        swap(&W0buf[2048], &W0buf[2049]);
    }
#endif

    //----------------------------------------------------------------
    w0_tag_id = 1;
    w1_tag_id = 2;
    rd_tag_id = 0x20;
    
    w0_pdm_adr = 0;
    w1_pdm_adr = n;
    rd_pdm_adr = RECV_OFFSET/2;

    w0_lc_addr = 0;
    w1_lc_addr = 1024;

    rd_lc_addr = 2048;

    w0_lb_addr = 0;
    w1_lb_addr = 128;
    r0_lb_addr = 256;
    r1_lb_addr = 384;

    wait_l2b_tag_id = 0x23;

    //----------------------------------------------------------------
    gpfn_send(w0_tag_id, n, 0, w0_pdm_adr); 
    gpfn_send(w1_tag_id, n, W1_OFFSET, w1_pdm_adr); 

#if 0
    if ( w1_tag_id || w0_tag_id ) {
        printf("Now, it doesn't work well\n");
    }
    if ( rd_tag_id ) {
        printf("Now, it doesn't work well\n");
    }
#endif

    //----------------------------------------------------------------
    //----------------------------------------------------------------
    gpfn_setvsmfile_ro("_build/double-vecadd.vsm");

    gpfn_sendvsm();
    if ( is_vsmwrite()) {
        fprintf(stderr, "is_vsmwrite OK\n");
        return(0);
    }

    //----------------------------------------------------------------
    gpfn_recv(rd_tag_id, n*2, RECV_OFFSET, rd_pdm_adr);

    //----------------------------------------------------------------
    printf("\n\n****************\nResult\n");

    w0dp = (double *)&W0buf[0];
    w1dp = (double *)&W1buf[0];
    rdp = (double *)&Rbuf[0];

    dlim_c = ' ';
    for( unsigned int i = 0 ; i < n ; i++, rdp += 1 ) {
        unsigned int *ip = (unsigned int *)rdp;
        unsigned err_flag = 0;

        double data0, data1;
        data0 = *w0dp;
        swap_next((unsigned int *)&data0);
        data1 = *w1dp;
        swap_next((unsigned int *)&data1);
        swap(ip, ip+1);

        if ( *rdp != (data0 + data1)) {
            err_flag = 1;
        }
        w0dp += 1;
        w1dp += 1;

        if ((i < view_n) && err_flag ) {
            printf("\n%f + %f-> %f\n", data0, data1, *rdp);
        }
        
        if ( i < view_n ) {
            dlim_c = ((i % 8) == 7)?'\n':' ';
            printf("%s%f%c", err_flag?"!!!!":"", *rdp, dlim_c);
        }
        if ( i == view_n ) {
           printf("...\n");
        }
        if ( err_flag ) {
            err_n += 1;
        }
    }

    printf("\n");
    printf("err_n:%d\n", err_n);

    rv = gpfn_close();
    if ( rv != 0 ) {
        fprintf(stderr, "gpfn_close error_flag:%x\n", rv);
        error_monitor(1);
    }
    return rv;
}
