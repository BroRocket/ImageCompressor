# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include "c_img.h"
# include "receipt_splitter.h"
# include "c_img.c"

void free_dbl_arr(struct double_arr *arr){
    free(arr->data);
    free(arr);
}

void free_im(struct rgb_img *im){
    free(im->raster);
    free(im);
}

void free_img_batch(struct image_batch *batch) {
    if (batch == NULL) {
        return;
    }
    for (size_t i = 0; i < batch->length; i++) {
        destroy_image(batch->imgs[i]); // Free individual images
    }
    free(batch->imgs); // Free the array of image pointers
    free(batch);       // Free the batch itself
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

void create_int_arr(struct int_arr **arr, size_t len) {
    *arr = (struct int_arr *)malloc(sizeof(struct int_arr));
    if (*arr == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    (*arr)->length = len;
    (*arr)->data = (int *)calloc(len, sizeof(double));
    if ((*arr)->data == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
}

void create_img_batch(struct image_batch **images, size_t len){
    *images = (struct image_batch *)malloc(sizeof(struct image_batch));
    if (*images == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    (*images)->length = len;
    (*images)->imgs = (struct rgb_img **)malloc(len * sizeof(struct rgb_img *));
    if ((*images)->imgs == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
}

double mean_rgb(struct rgb_img *im, int y, int x){
    //printf("mean: %d \n", (get_pixel(im, y, x, 0) + get_pixel(im, y, x, 1) + get_pixel(im, y, x, 2))/3);
    return (get_pixel(im, y, x, 0) + get_pixel(im, y, x, 1) + get_pixel(im, y, x, 2)) / 3;
}

void get_col_sums(struct rgb_img *im, struct double_arr **col_sums){
    create_double_arr(col_sums, im->width);
    for(int x = 0; x < im->width; x++){
        for(int y = 0; y < im->height; y++){
            if(mean_rgb(im, y, x) < 50){
                (*col_sums)->data[x] = 1;
            }
        }
        // (*col_sums)->data[x] /= im->height;
    }
}

void clean_data(struct double_arr *arr){
    int mode = 0;
    for(int i = 0; i < arr->length; i++){
        if(mode == 1){
            if(arr->data[i] != mode){
                for(int j = i; j < i + 30; j++){
                    if(arr->data[j] == 1){
                        for(int k = i; k < j; k++){
                            arr->data[k] = 1;
                        }
                        i = j;
                    }
                }
                mode = arr->data[i];
            }
        } else if (mode == 0){
            if(arr->data[i] != mode){
                mode = 1;
            }
        }
    }
}

void get_sum_gradient(struct double_arr *arr){
    struct double_arr *temp;
    create_double_arr(&temp, arr->length);
    for(int i = 0; i < 5; i++){
        temp->data[i] = 0;
    }
    for(int i = 5; i < arr->length - 5; i++){
        temp->data[i] = abs(arr->data[i-5] - arr->data[i]) + abs(arr->data[i-1] - arr->data[i]) + abs(arr->data[i] - arr->data[i + 1]) + abs(arr->data[i] - arr->data[i+5]);
    }
    for(int i = arr->length - 5; i < arr->length; i++){
        temp->data[i] = 0;
    }
    memcpy(arr->data, temp->data, sizeof(double) * temp->length);
    free_dbl_arr(temp);
} 

void transition_indexes(struct double_arr *arr, struct int_arr **locs){
    double cur_val = arr->data[0];
    create_int_arr(locs, 20);
    int index = 0;

    for(int i = 0; i < arr->length; i++){
        if(arr->data[i] != cur_val){
            (*locs)->data[index] = i;
            index += 1;
            if(index == (*locs)->length){
                int* ptr = realloc((*locs)->data, sizeof(int)*((*locs)->length + 10));
                if (ptr == NULL) {
                    fprintf(stderr, "Memory reallocation failed\n");
                    exit(1);
                }
                (*locs)->data = ptr;
                (*locs)->length += 10;
            }
            cur_val = arr->data[i];
        }
    } 
    int *tmp = realloc((*locs)->data, sizeof(int)*index);
    if (tmp == NULL) {
        fprintf(stderr, "Memory reallocation failed\n");
        exit(1);
    }
    (*locs)->data = tmp;
    (*locs)->length = index;
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

void split_img_vertically(struct rgb_img *im, struct double_arr *col_sums, struct image_batch **img_batch){
    struct int_arr **locs;
    transition_indexes(col_sums, locs);
    for(int i = 0; i < (*locs)->length; i++){
        printf("%d \n", (*locs)->data[i]);
    }

    create_img_batch(img_batch, (*locs)->length);
    for(int i = 0; i < (*locs)->length; i++){
        create_img(&((*img_batch)->imgs[i]), im->height, (*locs)->data[0]);

        for (int y = 0; y < im->height; y++) {
            for (int x = 0; x < (*locs)->data[0]; x++) {
                set_pixel((*img_batch)->imgs[i], y, x,
                        get_pixel(im, y, x, 0),
                        get_pixel(im, y, x, 1),
                        get_pixel(im, y, x, 2));
            }
        }

        struct double_arr *temp;
        create_double_arr(&temp, col_sums->length - (*locs)->data[0]);
        memcpy(temp->data, col_sums->data+(*locs)->data[0], sizeof(double) * (temp->length - (*locs)->data[0]));

        col_sums->length = temp->length;
        double* ptr = realloc(col_sums->data, sizeof(double) * (col_sums->length));
        if (ptr == NULL) {
            fprintf(stderr, "Memory reallocation failed\n");
            exit(1);
        }
        col_sums->data = ptr;
        memcpy(col_sums->data, temp->data, sizeof(double) * (col_sums->length));

        free_dbl_arr(temp);
    }

    // create_img(new_img1, im->height, (*locs)->data[0]);
    // //create_img(new_img2, im->height, im->width - (*locs)->data[0]); 
  
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

    // Column sums
    get_col_sums(im, &col_sums);
    clean_data(col_sums);
    //get_sum_gradient(col_sums);
    //show_arr_sums(col_sums);

    // Row sums (initialize before freeing)
    get_row_sums(im, &row_sums);
   // show_arr_sums(row_sums);

    // Splitting the image 
    struct image_batch *images;
    split_img_vertically(im, col_sums, &images);
    for(int i = 0; i < images->length; i++){
        write_img(images->imgs[i], "res.bin");
    }
    
    

    free_img_batch(images);
    free_dbl_arr(col_sums);
    free_dbl_arr(row_sums);

    return 0;
}
