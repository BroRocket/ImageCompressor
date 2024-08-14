# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include "c_img.h"
# include "receipt_splitter.h"


void create_sum_arr(struct sum_arr **arr, size_t len) {
    *arr = (struct sum_arr *)malloc(sizeof(struct sum_arr));
    if (*arr == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    (*arr)->length = len;
    (*arr)->columns = (double *)malloc(sizeof(double) * len);
    if ((*arr)->columns == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
}

double mean_rgb(struct rgb_img *im, int y, int x){
    return (get_pixel(im, y, x, 0) + get_pixel(im, y, x, 1) + get_pixel(im, y, x, 2)) / 3.0;
}

void get_row_sums(struct rgb_img *im, struct sum_arr **col_sums){
    double sum;
    create_sum_arr(col_sums, im->height);
    for(int y = 0; y < im->height; y++){
        sum = 0;
        for(int x = 0; x < im->width; x++){
            sum += mean_rgb(im, y, x);
        }
        (*col_sums)->columns[y] = sum / im->width;
    }
}

void show_col_sums(struct sum_arr *arr){
    if (arr == NULL){
    printf("Error: arr is NULL\n");
    exit(1);
    }
    printf("---------------------\n");
    for(int i = 0; i < arr->length; i++){
        printf("value = %f | row= %d \n", arr->columns[i], i);
    }
    printf("---------------------\n");
}

void free_all(struct rgb_img *im, struct sum_arr *arr){
    free(im->raster);
    free(im);
    free(arr->columns);
    free(arr);
}

//void remove_chunk(){}
// run min max algorithm on the data and take chunks at max values preset must be over 220 or smt
// maybe try to split receipt in half for prices and ids

int main(){
    struct rgb_img *im;
    struct sum_arr *col_sum;

    read_in_img(&im, "test.bin");
    //print_grad(im);
    //return 0;
    get_row_sums(im, &col_sum);
    show_col_sums(col_sum);
    printf("%d", im->height);
    free_all(im, col_sum);
    return 0;
}