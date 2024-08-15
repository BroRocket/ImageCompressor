# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include "c_img.h"
# include "receipt_splitter.h"

void free_dbl_arr(struct double_arr *arr){
   free(arr->data);
   free(arr);
}

void free_im(struct rgb_img *im){
    free(im->raster);
    free(im);
}

void create_double_arr(struct double_arr **arr, size_t len) {
    *arr = (struct double_arr *)malloc(sizeof(struct double_arr));
    if (*arr == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    (*arr)->length = len;
    (*arr)->data = (double *)calloc(len, sizeof(double));
    if ((*arr)->data == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
}

double mean_rgb(struct rgb_img *im, int y, int x){
    return (get_pixel(im, y, x, 0) + get_pixel(im, y, x, 1) + get_pixel(im, y, x, 2)) / 3.0;
}

void get_col_sums(struct rgb_img *im, struct double_arr **col_sums){
   create_double_arr(col_sums, im->height);
    for(int x = 0; x < im->width; x++){
        for(int y = 0; y < im->height; y++){
            (*col_sums)->data[x] += mean_rgb(im, y, x);
        }
        (*col_sums)->data[x] /= im->height;
    }
}

void get_sum_gradient(struct double_arr *arr){
    struct double_arr *temp;
    create_double_arr(&temp, arr->length);
    for(int i = 0; i < 5; i++){
        temp->data[i] = 256;
    }
    for(int i = 5; i < arr->length - 5; i++){
        temp->data[i] = abs(arr->data[i-5] - arr->data[i]) + abs(arr->data[i-1] - arr->data[i]) + abs(arr->data[i] - arr->data[i + 1]) + abs(arr->data[i] - arr->data[i+5]);
    }
    for(int i = arr->length - 5; i < arr->length; i++){
        temp->data[i] = 256;
    }
    memcpy(arr->data, temp->data, sizeof(double) * temp->length);
    free_dbl_arr(arr);
}

int arr_max(struct double_arr *arr){
   double max_val = 0;
   int max_loc;
   for(int i = 0; i < arr->length; i++){
      if (max_val < arr->data[i]){
         max_val = arr->data[i];
         max_loc = i;
      }  
   }
   return max_loc;
}

void get_row_sums(struct rgb_img *im, struct double_arr **row_sums){
    create_double_arr(row_sums, im->height);
    for(int y = 0; y < im->height; y++){
        for(int x = 0; x < im->width; x++){
            (*row_sums)->data[y] += mean_rgb(im, y, x);
        }
        (*row_sums)->data[y] /= im->width;
    }
}

void show_arr_sums(struct double_arr *arr){
    printf("---------------------\n");
    for(int i = 0; i < arr->length; i++){
        printf("value = %f | row= %d \n", arr->data[i], i);
    }
    printf("---------------------\n");
}

void split_img_vertically(struct rgb_img *im, struct double_arr *col_sums, struct rgb_img **new_img1, struct rgb_img **new_img2){
   int split_index = arr_max(col_sums);
   create_img(new_img1, im->height, split_index - 1);
   create_img(new_img2, im->height, im->width - split_index);
   for( int y = 0; y < im->height; y++){
      for(int x = 0; x < split_index; x++){
         set_pixel(*new_img1, y, x, get_pixel(im, y, x, 0), get_pixel(im, y, x, 1), get_pixel(im, y, x, 2));
      }
   }
   for( int y = 0; y < im->height; y++){
      for(int x = split_index + 1; x < im->width; x++){
         set_pixel(*new_img2, y, x, get_pixel(im, y, x, 0), get_pixel(im, y, x, 1), get_pixel(im, y, x, 2));
      }
   }
}

// void create_image_chunks(struct rgb_img *im){
//    struct double_arr *row_sums;
//    get_row_sums(im, &row_sums);
//    double max_val = 0;
//    for(int i = 0; i < row_sums->length; i++){
      
//    }
//    free_double_arr(row_sums);
// }


//void remove_chunk(){}
// run min max algorithm on the data and take chunks at max values preset must be over 220 or smt
// maybe try to split receipt in half for prices and ids

int main(){
    struct rgb_img *im;
    struct double_arr *col_sums;
    struct double_arr *row_sums;

    read_in_img(&im, "test.bin");
    get_col_sums(im, &col_sums);
    // struct double_arr *col_gradient;
    get_sum_gradient(col_sums);
    show_arr_sums(col_sums);
    printf("%f", arr_max(col_sums));
    free_im(im);
    free_dbl_arr(col_sums);
    free_dbl_arr(row_sums);

    // struct rgb_img *im1, *im2;
    // split_img_vertically(im, col_sums, &im1, &im2);
    // write_img(im1, "res1.bin");
    // write_img(im2, "res2.bin");
    // free_im(im);
    // free_im(im1);
    // free_im(im2);
    // free_dbl_arr(col_sums);
    return 0;
}
