#if !defined(RECEIPTSPLITTER)
#define RECEIPTSPLITTER

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "c_img.h"

struct double_arr
{
    double *data;
    size_t length;
};

struct int_arr
{
    int *data;
    size_t length;
};

struct image_batch
{
    struct rgb_img **imgs;
    size_t length;
};

#endif