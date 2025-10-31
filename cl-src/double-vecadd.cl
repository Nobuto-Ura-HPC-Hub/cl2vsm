#define COPY_N 1
__kernel void main_kernel0( __global double* _arg_array1, __global double* _arg_array2, __global double* _arg_array3) 
{
    __private double array1[COPY_N];
    __private double array2[COPY_N];
    __private double array3[COPY_N];

    bm1 double buf1_bm1[COPY_N * 64];
    bm2 double buf1_bm2[COPY_N * 512];
    distribute(buf1_bm1, buf1_bm2, _arg_array1, COPY_N);
    distribute_pe(array1, buf1_bm1, COPY_N);

    bm1 double buf2_bm1[COPY_N * 64];
    bm2 double buf2_bm2[COPY_N * 512];
    distribute(buf2_bm1, buf2_bm2, _arg_array2, COPY_N);
    distribute_pe(array2, buf2_bm1, COPY_N);

    {
        array3[0] = array1[0] + array2[0];
    }

    bm1 double buf3_bm1[COPY_N * 64];
    bm2 double buf3_bm2[COPY_N * 512];

    collect_pe(buf3_bm1, array3, COPY_N);
    collect(_arg_array3, buf3_bm2, buf3_bm1, COPY_N);
}

