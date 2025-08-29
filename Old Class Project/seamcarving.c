# include <stdlib.h>
# include <stdio.h>
# include <math.h>
# include <string.h>
# include "c_img.h"
# include "seamcarving.h"

void calc_energy(struct rgb_img *im, struct rgb_img **grad){
    create_img(grad, im->height, im->width);
    for(int y = 0; y < im->height; y++){
        for(int x = 0; x < im->width; x++){
            int x_minus = x - 1, y_minus = y - 1, x_plus = x + 1, y_plus = y + 1, i; 
            if(x == 0){
                x_minus = im->width-1;
            }
            if(x == im->width-1){
                x_plus = 0;
            }
            if(y == 0){
                y_minus = im->height-1;
            }
            if(y == im->height-1){
                y_plus = 0;
            }
        
            int sum_x = 0, sum_y = 0;
            for(i = 0; i < 3; i++){
                sum_x += pow(get_pixel(im, y, x_plus, i) - get_pixel(im, y, x_minus, i), 2);
                sum_y += pow(get_pixel(im, y_plus, x, i) - get_pixel(im, y_minus, x, i), 2);
            }
            
            uint8_t intensity = (uint8_t)(sqrt(sum_x + sum_y) / 10);
            set_pixel(*grad, y, x, intensity, intensity, intensity); 
        }
    }
}

void dynamic_seam(struct rgb_img *grad, double **best_arr){
    *best_arr = (double *)malloc(grad->height * grad->width * sizeof(double));
    int width = grad->width;
    for(int y = 0; y < grad->height; y++){
        for(int x = 0; x < grad->width; x++){
            int i;
            double value;
            double min_energy = 1000000;
            if(y == 0){
                min_energy = (double)get_pixel(grad, y, x, 0);
            }else if(x == 0){
                for(i = 0; i < 2; i++){
                    value = *((*best_arr) + (y - 1)*(width) + (x + i)) + (double)get_pixel(grad, y, x, 0);
                    if (value < min_energy){
                        min_energy = value;
                    }
                }
            }else if(x == width - 1){
                for(i = 0; i > -2; i--){
                    value = *((*best_arr) + (y - 1)*(width) + (x + i)) + (double)get_pixel(grad, y, x, 0);
                    if (value < min_energy){
                        min_energy = value;
                    }
                }
            }else{
                for( i = -1; i < 2; i++){
                    value = *((*best_arr) + (y - 1)*(width) + (x + i)) + (double)get_pixel(grad, y, x, 0);
                    if (value < min_energy){
                        min_energy = value;
                    }
                }
            }
            *((*best_arr) + y*(width)+x) = min_energy;
        }
    }
}

void recover_path(double *best, int height, int width, int **path){
    *path = (int *)malloc(height * sizeof(int));
    int x, i;

    double min_energy = *(best + (height- 1)*(width)), cur_min, min_value = 100000000;
    for(x = 1; x < width - 1; x++){
        cur_min = *(best + (height-1)*width + x);
        if(cur_min < min_energy){
            min_energy = cur_min;
        }
    }

    for(x = 0; x < width - 1; x++){
        double value = 0;
        if(*(best + (height - 1)*width + x) == min_energy){
            int *temp_index_arr = (int *)malloc(height * sizeof(int));
            value = *(best + (height - 1)*width + x); 
            *(temp_index_arr + height - 1) = x;
            int min_ind, count = height - 2, ind = x;

            for(int y = 2; y <= height; y++){
                double min = 1000000;
                if(ind == 0){
                    for(i = 0; i < 2; i++){
                        cur_min = *(best + (height-y)*width + i);
                        if(cur_min < min){
                            min = cur_min;
                            min_ind = i;
                        }
                    }
                }else if(ind == width - 1){
                    for(i = ind-1; i < ind + 1; i++){
                        cur_min = *(best + (height-y)*width + i);
                        if(cur_min < min){
                            min = cur_min;
                            min_ind = i;
                        } 
                    }
                }else{
                    for(i = ind - 1; i < ind + 2; i++){
                        cur_min = *(best + (height-y)*width + i);
                        if(cur_min < min){
                            min = cur_min;
                            min_ind = i;
                        } 
                    }
                }
                value += min;
                *(temp_index_arr + count) = min_ind;
                count -= 1;
                ind = min_ind;
            }
            if(value < min_value){
                min_value = value;
                memcpy(*path, temp_index_arr, height*sizeof(int));
                free(temp_index_arr);
            }
        }
    }
}

void remove_seam(struct rgb_img *src, struct rgb_img **dest, int *path){
    create_img(dest, src->height, src->width - 1);
    uint8_t r, g, b;

    int i;
    for(int y = 0; y < src->height; y++){
        int target_index = *(path + y);
        i = 0;
        for(int x = 0; x < src->width; x++){
            if(x != target_index){
                r = get_pixel(src, y, x, 0);
                g = get_pixel(src, y, x, 1);
                b = get_pixel(src, y, x, 2);
                set_pixel(*dest, y, i, r, g, b);
                i++;
            }
        }
    }   
}

int main(){
    struct rgb_img *im;
    struct rgb_img *cur_im;
    struct rgb_img *grad;
    double *best;
    int *path;

    read_in_img(&im, "test.bin"); //replace with wanted file
    
    for(int i = 0; i < 500; i++){
        calc_energy(im,  &grad);
        dynamic_seam(grad, &best);
        recover_path(best, grad->height, grad->width, &path);
        remove_seam(im, &cur_im, path);

        destroy_image(im);
        destroy_image(grad);
        free(best);
        free(path);
        im = cur_im;
        printf("Seam %d succesfully removed \n", i + 1);
    }
    write_img(cur_im, "result.bin"); //final reuslt image
    destroy_image(im);
    return 0;
}