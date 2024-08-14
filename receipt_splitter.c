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
    (*arr)->columns = (double *)calloc(sizeof(double) * len);
    if ((*arr)->columns == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
}

double mean_rgb(struct rgb_img *im, int y, int x){
    return (get_pixel(im, y, x, 0) + get_pixel(im, y, x, 1) + get_pixel(im, y, x, 2)) / 3.0;
}

void get_col_sums(struct rgb_img *im, struct sum_arr **col_sums){
   create_sum_arr(col_sums, im->height);
    for(int x = 0; x < im->width; x++){
        for(int y = 0; y < im->height; y++){
            (*col_sums)->columns[x] += mean_rgb(im, y, x);
        }
        (*col_sums)->columns[x] /= im->height;
    }
}

int arr_max(struct sum_arr *arr){
   double max_val = 0;
   int max_loc
   for(int i = 0; i < (*arr)->length; i++){
      if (max_val < (*arr)->columns[i]){
         max_val = (*arr)->columns[i];
         max_loc = i;
      }  
   }
   return max_loc
}

void split_img_vertically(struct rgb_img *im, struct sum_arr *col_sums, struct sum_arr **new_img1, struct sum_arr **new_img2){
   int split_index = arr_max(col_sums);
   create_img(new_img1, im->height, split_index - 1);
   create_img(new_img2, im->height, im->width - split_index);
   for( int y = 0; y < im->height; y++){
      for(int x = 0; x < split_index; x++){
         set_pixel(*im, y, x, get_pixel(*im, y, x, 0), get_pixel(im, y, x, 1), get_pixel(*im, y, x, 2))
      }
   }
   for( int y = 0; y < im->height; y++){
      for(int x = split_index + 1; x < im->width; x++){
         set_pixel(*im, y, x, get_pixel(*im, y, x, 0), get_pixel(im, y, x, 1), get_pixel(*im, y, x, 2))
      }
   }
}

void get_row_sums(struct rgb_img *im, struct sum_arr **row_sums){
    create_sum_arr(row_sums, im->height);
    for(int y = 0; y < im->height; y++){
        for(int x = 0; x < im->width; x++){
            (*row_sums)->columns[y] += mean_rgb(im, y, x);
        }
        (*row_sums)->columns[y] /= im->width;
    }
}

void show_arr_sums(struct sum_arr *arr){
    printf("---------------------\n");
    for(int i = 0; i < arr->length; i++){
        printf("value = %f | row= %d \n", arr->columns[i], i);
    }
    printf("---------------------\n");
}

void free_im(struct rgb_img *im){
    free(im->raster);
    free(im);
}

void free_sum_arr(struct sum_arr *arr){
   free(arr->columns);
   free(arr);
}

//void remove_chunk(){}
// run min max algorithm on the data and take chunks at max values preset must be over 220 or smt
// maybe try to split receipt in half for prices and ids

int main(){
    struct rgb_img *im;
    struct sum_arr *row_sum;

    read_in_img(&im, "test.bin");
    //print_grad(im);
    //return 0;
    get_row_sums(im, &row_sum);
    show_col_sums(row_sum);
    printf("%d", im->height);
    free_im(im);
    free_sum_arr(row_sum);
    return 0;
}
