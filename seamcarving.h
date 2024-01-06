#if !defined(seamcarving)
#define seamcarving

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "c_img.h"
#include "c_img.c"


void calc_energy(struct rgb_img *im, struct rgb_img **grad);
void dynamic_seam(struct rgb_img *grad, double **best_arr);
void recover_path(double *best, int height, int width, int **path);
void remove_seam(struct rgb_img *src, struct rgb_img **dest, int *path);

#endif 