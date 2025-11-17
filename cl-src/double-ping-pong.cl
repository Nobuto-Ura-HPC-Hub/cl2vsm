__kernel void double_ping_pong(__global double *in_v, __global double *out_v)
{
    bm1 double bufi_bm1[64];
    bm2 double bufi_bm2[512];
    bm1 double bufo_bm1[64];
    bm2 double bufo_bm2[512];
    double v;

    distribute(bufi_bm1, bufi_bm2, in_v, 1);
    distribute_pe(&v, bufi_bm1, 1);

    collect_pe(bufo_bm1, &v, 1);
    collect(out_v, bufo_bm2, bufo_bm1, 1);

    return;
}
