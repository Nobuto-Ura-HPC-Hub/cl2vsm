#ifndef __MNCL_UTILS_H__
#define __MNCL_UTILS_H__

static
inline
void
swap(unsigned int *a, unsigned int *b)
{
    unsigned int c = *a;
    *a = *b;
    *b = c;
}

static
inline
void
swap_next(void *_a)
{
    unsigned int *a = (unsigned int *)_a;
    swap(a, a+1);
}

#define CHECK_VSM_WRITE(__argc__, __argv__) \
do { \
    if (((__argc__) >= 2 ) && (strcmp((__argv__)[1], "--vsm-write") == 0)) { \
        gpfn_setvsmwrite(); \
        (__argc__)--; \
        (__argv__)++; \
    } \
} while (0);

#endif /* __MNCL_UTILS_H__ */
