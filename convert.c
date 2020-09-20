/*************************************************************************
 *
 *  convert.c
 *  Assignment: arith
 * 
 *  Authors: Erce Ozmetin, Dogacan Colak 
 *  Date: 3/07/2020
 *  
 *  This program traverses through the 2-D array of Rgb structs
 *  and returns a newly populated 2-D array XYZ structs. Does the
 *  reverse in decompression step
 * 
 **************************************************************************/

#include <assert.h>
#include <math.h>
#include <pnm.h>
#include "convert.h"
#include "a2blocked.h"


const int BLOCKSIZE = 2;
const float DENOMINATOR = 255;

/*
 * Our struct to be populated
 */
struct XYZ 
{
    float y, pb, pr;
};

/*
 * Struct to be passed into cl in mapping
 */
struct Closure {
    A2Methods_UArray2 pixels;
    A2Methods_T methods;
};

/* 
 * Helper function that returns the smaller float value
 */
float min(float a, float b) {
    if (a < b)
        return a;
    else
        return b;
}

/* 
 * Helper function that returns the bigger float value
 */
float max(float a, float b) {
    if (a > b)
        return a;
    else
        return b;
}

/*
 *Function: rgb_to_xyz
 *Purpose: Traverses through the 2-D pixels of struct Rgb's and populates
           a new 2-D array of struct XYZ's
 *Inputs: column i, row, j, 2-D pixels that has rgb structs, current element, 
          closure
 *Output: None
 *Error:  This function can only be called within convert_to_xyz_space
 */
void rgb_to_xyz(int i , int j, A2Methods_UArray2 rgb_pixels, void *curr, 
                                                             void *cl){
    (void) rgb_pixels;
    Pnm_rgb rgb         = (Pnm_rgb) curr; /* the rgb we want to convert */
    struct Closure *img = (struct Closure*) cl; /* pixels and methods info */

    A2Methods_UArray2 xyz_pixels = img->pixels;
    A2Methods_T methods          = img->methods;

    /* if curr is in the trimmed section of an odd-sized array2 */
    if (methods->width(xyz_pixels) <= i || methods->height(xyz_pixels) <= j)
        return;

    float r = (rgb->red)   / (float) 255;
    float g = (rgb->green) / (float) 255;
    float b = (rgb->blue)  / (float) 255;


    float y, pb, pr;

    y  = (0.299     * r + 0.587    * g + 0.114    * b); /* conversions */
    pb = (-0.168736 * r - 0.331264 * g + 0.5      * b);
    pr = (0.5       * r - 0.418688 * g - 0.081312 * b);


    struct XYZ *xyz_pixel = (struct XYZ*) methods->at(xyz_pixels, i, j);
    xyz_pixel->y  = y;
    xyz_pixel->pb = pb;
    xyz_pixel->pr = pr;
}

/*
 *Function: convert_to_xyz_space
 *Purpose: Creates an empty Uarray2 and populates it with XYZ structs 
           by mapping through Rgb struct Uarray2
 *Inputs: column i, row, j, rgb_pixels, current element, 
          closure
 *Output: None
 *Error:  Its an error to pass in an empty rgb_pixels Uarray2
 */
A2Methods_UArray2 convert_to_xyz_space(A2Methods_UArray2 rgb_pixels) {
    assert(rgb_pixels);
    A2Methods_T methods = uarray2_methods_blocked;
    int width  = methods->width(rgb_pixels);
    int height = methods->height(rgb_pixels);

    if (height % 2 != 0) /* trim if height or width is odd */
        height--;
    if (width %2 != 0)
        width--;

    A2Methods_UArray2 xyz_pixels;

    /*
     * create xyz in blocked of blocksize 2, since we pack four pixels
     * together
     */
    xyz_pixels = methods->new_with_blocksize(width, height,
                                                    sizeof(struct XYZ),
                                                    BLOCKSIZE);

    A2Methods_mapfun *map = methods->map_default;

    struct Closure cl;
    cl.pixels  = xyz_pixels;
    cl.methods = methods;

    map(rgb_pixels, rgb_to_xyz, &cl);

    return xyz_pixels;
}

/*
 *Function: xyz_to_rgb
 *Purpose: Traverses through the 2-D pixels of struct XYZ's and populates
           a new 2-D array of struct Rgb's
 *Inputs: column i, row, j, 2-D pixels that has xyz structs, current element, 
          closure
 *Output: None
 *Error:  This function can only be called within convert_to_rgb_space
 */
void xyz_to_rgb(int i , int j, A2Methods_UArray2 xyz_pixels, void *curr,
                                                             void *cl) {
    (void) xyz_pixels;

    struct XYZ *xyz     = (struct XYZ*) curr; /* the xyz we want to convert */
    struct Closure *img = (struct Closure*) cl; /* pixels and methods info */

    A2Methods_UArray2 rgb_pixels = img->pixels;
    A2Methods_T methods          = img->methods;

    float y  = (xyz->y)  * (float) DENOMINATOR;
    float pb = (xyz->pb) * (float) DENOMINATOR;
    float pr = (xyz->pr) * (float) DENOMINATOR;

    float r, g, b;

    r = round(1.0 * y + 0.0      * pb + 1.402    * pr); /* conversions */
    g = round(1.0 * y - 0.344136 * pb - 0.714136 * pr);
    b = round(1.0 * y + 1.772    * pb + 0.0      * pr);

    r = min(255, r); /* fit all values into range of [0, 255] */
    r = max(0, r);
    g = min(255, g);
    g = max(0, g);
    b = min(255, b);
    b = max(0, b);

    Pnm_rgb rgb_pixel = (Pnm_rgb) methods->at(rgb_pixels, i, j);
    rgb_pixel->red   = r;
    rgb_pixel->green = g;
    rgb_pixel->blue  = b;
}


/*
 *Function: convert_to_rgb_space
 *Purpose: Creates an empty Uarray2 and populates it with Rgb structs 
           by mapping through XYZ struct Uarray2
 *Inputs: column i, row, j, xyz_pixels, current element, 
          closure
 *Output: Populates an Uarray2 of struct Rgb's
 *Error:  Its an error to pass in an empty xyz_pixels Uarray2
 */
A2Methods_UArray2 convert_to_rgb_space(A2Methods_UArray2 xyz_pixels) {
    assert(xyz_pixels);
    A2Methods_T methods = uarray2_methods_blocked;
    int width  = methods->width(xyz_pixels);
    int height = methods->height(xyz_pixels);

    A2Methods_UArray2 rgb_pixels;
    rgb_pixels = methods->new(width, height, sizeof(struct Pnm_rgb));

    A2Methods_mapfun *map = methods->map_default;

    struct Closure cl;
    cl.pixels  = rgb_pixels;
    cl.methods = methods;

    map(xyz_pixels, xyz_to_rgb, &cl);

    methods->free(&xyz_pixels); /* free rgb pixels */

    return rgb_pixels;
}
