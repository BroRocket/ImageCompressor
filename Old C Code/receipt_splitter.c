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

void free_int_arr(struct int_arr *arr){
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


void transition_indexes(struct double_arr *arr, struct int_arr **locs){
    create_int_arr(locs, 20);
    double cur_val = arr->data[0];
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
    struct int_arr *locs = NULL;
    transition_indexes(col_sums, &locs);
    
    // for(int i = 0; i < locs->length; i++){
    //     printf("%d \n", locs->data[i]);
    // }

    int slice_index = 0;
    create_img_batch(img_batch, locs->length/2);
    for(int i = 0; i < locs->length; i+=2){

        size_t start_col = locs->data[i];         
        size_t end_col = locs->data[i + 1];       
        size_t slice_width = end_col - start_col; 

        create_img(&((*img_batch)->imgs[slice_index]), im->height, slice_width);

        // Copy the pixels from the original image to the slice
        for (int y = 0; y < im->height; y++) {
            for (int x = 0; x < slice_width; x++) {
                set_pixel((*img_batch)->imgs[slice_index], y, x,
                          get_pixel(im, y, start_col + x, 0),
                          get_pixel(im, y, start_col + x, 1),
                          get_pixel(im, y, start_col + x, 2));
            }
        }
        slice_index++;
    }
    free_int_arr(locs);
  
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
    for (size_t i = 0; i < images->length; i++) {
        char filename[256]; // Buffer for the dynamic filename
        sprintf(filename, "split_image_%zu.bin", i); // Create a unique filename
        write_img(images->imgs[i], filename);        // Write the image to the file
    }
    

    free_img_batch(images);
    free_dbl_arr(col_sums);
    free_dbl_arr(row_sums);
    destroy_image(im);

    return 0;
}

// void get_sum_gradient(struct double_arr *arr){
//     struct double_arr *temp;
//     create_double_arr(&temp, arr->length);
//     for(int i = 0; i < 5; i++){
//         temp->data[i] = 0;
//     }
//     for(int i = 5; i < arr->length - 5; i++){
//         temp->data[i] = abs(arr->data[i-5] - arr->data[i]) + abs(arr->data[i-1] - arr->data[i]) + abs(arr->data[i] - arr->data[i + 1]) + abs(arr->data[i] - arr->data[i+5]);
//     }
//     for(int i = arr->length - 5; i < arr->length; i++){
//         temp->data[i] = 0;
//     }
//     memcpy(arr->data, temp->data, sizeof(double) * temp->length);
//     free_dbl_arr(temp);
// } 
