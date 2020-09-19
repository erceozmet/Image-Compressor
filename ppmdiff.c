#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "compress40.h"
#include <math.h>
#include "pnm.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"

float square(float a){
    return a *a;
}

float min(float a , float b){
    if (a >= b){
        return b;
    }
    else{
        return a;
    }
}

int main(int argc, char *argv[]){
    
    A2Methods_T methods = uarray2_methods_plain; 
    assert(methods);


    FILE *file;
    if (argc != 3){
        fprintf(stderr, "Incorrect number of arguments, please enter 2 ppm files\n");
        exit(EXIT_FAILURE);
    }
    Pnm_ppm img2;
    Pnm_ppm img1;
    Pnm_ppm temp;
    int check_dash = 0;
    for (int i = 1; i < argc; i++){
        char *ppmfile;

       
        if(argv[i] == "-" && check_dash == 0){
            file = stdin;
            temp = Pnm_ppmread(file, methods);
            check_dash = 1;
        }
        else{
            ppmfile = argv[i];
            file = fopen(ppmfile, "r");
            temp = Pnm_ppmread(file, methods);
        }   

        if (i == 1 ){
            img1 = temp;
        }
        else if(i == 2){
            img2 = temp;
        }
    }
    int height1 = img1->height;
    int width1 = img1->width;
    int height2 = img2->height;
    int width2 = img2->width;

    if (abs(height1- height2) > 1 || abs(width1- width2) >1 ){
        fprintf(stderr, "height or width differs by more than 1 pixel\n");
        printf("Height difference: %d, Width difference: %d", abs(height1- height2), abs(width1- width2));
        exit(1);
    }

    float nominator = 0;

    for (int i = 0; i < min(width1, width2); i++) {
        for (int j = 0; j < min(height1, height2); j++) {
            Pnm_rgb rgb1 = img1->methods->at(img1->pixels, i, j);
            Pnm_rgb rgb2 = img2->methods->at(img2->pixels, i, j);
            int red1 = rgb1->red;
            int red2 = rgb2->red;
            int green1 = rgb1->green;
            int green2 = rgb2->green;
            int blue1 = rgb1->blue;
            int blue2 = rgb2->blue;

            nominator += square(red1 - red2) + square(green1 - green2) + square(blue1- blue2);
        }
    }

    float denominator = 3*min(width1, width2)*min(height1, height2);
    
    float root_mean_sq_diff = sqrt(nominator/denominator);

    printf("Root mean square difference: %0.4f\n", root_mean_sq_diff);
}