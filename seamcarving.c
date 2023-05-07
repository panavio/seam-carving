
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "c_img.h"
#include "seamcarving.h"
#include <math.h>
/*
void print_cost(struct rgb_img* grad, double** best_arr) {
    for (int i = 0; i < (grad->height); i++) {
        for (int j = 0; j < (grad->width); j++) {
            printf("%f ", (*best_arr)[(i * (grad->width)) + j]);
        }
        printf("\n");
    }

}*/
void calc_energy(struct rgb_img* im, struct rgb_img** grad) {
    create_img(grad, im->height, im->width);
    int left;
    int down;
    int up;
    int right;
    for (int i = 0; i < (im->height); i++) {
        for (int j = 0; j < (im->width); j++) {

            if (j - 1 < 0) {
                left = (im->width) - 1;
            }
            else {
                left = j - 1;
            }

            if (j + 1 >= (im->width)) {
                right = 0;
            }
            else {
                right = j + 1;
            }

            if (i - 1 < 0) {
                up = (im->height) - 1;
            }
            else {
                up = i - 1;
            }

            if (i + 1 >= (im->height)) {
                down = 0;
            }
            else {
                down = i + 1;
            }

            int Rx = get_pixel(im, i, right, 0) - get_pixel(im, i, left, 0);
            int Gx = get_pixel(im, i, right, 1) - get_pixel(im, i, left, 1);
            int Bx = get_pixel(im, i, right, 2) - get_pixel(im, i, left, 2);
            int Ry = get_pixel(im, up, j, 0) - get_pixel(im, down, j, 0);
            int Gy = get_pixel(im, up, j, 1) - get_pixel(im, down, j, 1);
            int By = get_pixel(im, up, j, 2) - get_pixel(im, down, j, 2);
            double total = pow(Rx, 2) + pow(Gx, 2) + pow(Bx, 2) + pow(Ry, 2) + pow(Gy, 2) + pow(By, 2);
            double energy = sqrt(total);
            energy = (uint8_t)(energy / 10);
            set_pixel(*grad, i, j, energy, energy, energy);
        }
    }
}


void dynamic_seam(struct rgb_img* grad, double** best_arr) {
    *best_arr = (double*)malloc((grad->width) * (grad->height) * sizeof(double));
    for (int i = 0; i < grad->width; i++) {
        (*best_arr)[i] = get_pixel(grad, 0, i, 0);
    }

    for (int i = 1; i < (grad->height); i++) {

        for (int j = 0; j < (grad->width); j++) {
            int left;
            int right;
            if ((j - 1) < 0) {
                left = j;
            }
            else {
                left = j - 1;
            }

            if ((j + 1) >= (grad->width)) {
                right = j;
            }
            else {
                right = j + 1;
            }


            int a = (*best_arr)[(i - 1) * (grad->width) + left];
            int b = (*best_arr)[(i - 1) * (grad->width) + j];
            int c = (*best_arr)[(i - 1) * (grad->width) + right];
            int res1 = fmin(a, b);

            int res = get_pixel(grad, i, j, 0) + fmin(res1, c);
            (*best_arr)[i * grad->width + j] = res;

        }
    }
}/*
void print_arr(int** arr, int length) {

    for (int i = 0; i < length; i++) {
        printf("%d:   %d\n ", i, (*arr)[i]);
    }
}*/

void recover_path(double* best, int height, int width, int** path) {
    *path = (int*)malloc(height * sizeof(int));
    for (int i = height - 1; i >= 0; i--) {

        int min = 2147483640;
        int idx = 2147483640;/*
        if (i >= 125 && i<=130) {
            printf("cost    ");
            for (int j = 450; j < 470; j++) {
                printf("%d ", (int)(best)[i * width + j]);
            }
            printf("\n");
        }*/
        if (i == height - 1) {
            for (int j = 0; j < width; j++) {
                if ((best)[i * width + j] < min) {
                    min = (best)[i * width + j];
                    idx = j;
                }
            }
        }
        else {
            int left;
            int right;
            if (((*path)[i + 1] - 1) < 0) {
                left = (*path)[i + 1];
            }
            else {
                left = (*path)[i + 1] - 1;
            }

            if (((*path)[i + 1] + 1) >= (width)) {
                right = (*path)[i + 1];
            }
            else {
                right = (*path)[i + 1] + 1;
            }
            for (int j = left; j <= right; j++) {

                if ((best)[i * width + j] < min) {
                    min = (best)[i * width + j];
                    idx = j;
                }
            }
        }

        (*path)[i] = idx;
    }

}


void remove_seam(struct rgb_img* src, struct rgb_img** dest, int* path) {
    create_img(dest, (src->height), (src->width) - 1);

    for (int i = 0; i < (src->height); i++) {
        int k = 0;
        for (int j = 0; j < (src->width); j++) {
            if (j != path[i]) {

                int r = get_pixel(src, i, j, 0);
                int g = get_pixel(src, i, j, 1);
                int b = get_pixel(src, i, j, 2);
                set_pixel(*dest, i, k, r, g, b);
                k++;
            }


        }
    }

}
