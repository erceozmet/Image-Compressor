/*************************************************************************
 *
 *  dct.c
 *  Assignment: arith
 * 
 *  Authors: Erce Ozmetin, Dogacan Colak 
 *  Date: 3/07/2020
 *  
 *  This program traverses through the 2-D array of XYZ structs
 *  and creates an 2-D array of half height and width with DCT structs
 *  does the opposite on decompression step 
 *  usage: this function is called in compress40
 * 
 **************************************************************************/

#include <assert.h>
#include <math.h>
#include <pnm.h>
#include "dct.h"
#include "convert.h"
#include "arith40.h"
#include "a2blocked.h"

/*
 * This is the struct to be populated using XYZ structs
 */
struct Dct
{
    float a, b, c, d;
    float pb_avg,
          pr_avg;
};


/*
 * This is our input struct which to be replaced with Dct struct
 */
struct XYZ
{
    float y, pb, pr;
};


struct Closure_dct
{
    XYZ *pixel_group;
    A2Methods_UArray2 dct_array;
    A2Methods_T methods;
    int array_pos; /* We iterate 4 cells a time in
                       group_pixels_to_dct */
    int dct_col;
    int dct_row;
};


/*
 *Function: pixel_to_dct
 *Purpose: Calculates the values for struct Dct from XYZ and populates
           the empty 2-D array element by element
 *Inputs: void cl that has our XYZ values passed in as well as the position of
          the new array to be populated
 *Output: Populates a struct Dct 
 *Error:  pixel_group cannot be void
 */
void pixel_to_dct(void *cl) {

    struct Closure_dct *info = (struct Closure_dct*) cl;
    XYZ *pixel_group = info->pixel_group;

    float y1  = pixel_group[0]->y,
          y3  = pixel_group[1]->y,
          y2  = pixel_group[2]->y,
          y4  = pixel_group[3]->y,

          pb1 = pixel_group[0]->pb,
          pb3 = pixel_group[1]->pb,
          pb2 = pixel_group[2]->pb,
          pb4 = pixel_group[3]->pb,

          pr1 = pixel_group[0]->pr,
          pr3 = pixel_group[1]->pr,
          pr2 = pixel_group[2]->pr,
          pr4 = pixel_group[3]->pr;

    Dct dct = (Dct) info->methods->at(info->dct_array,
                                      info->dct_col,
                                       info->dct_row);
    dct->a = (y4 + y3 + y2 + y1) / 4,
    dct->b = (y4 + y3 - y2 - y1) / 4,
    dct->c = (y4 - y3 + y2 - y1) / 4,
    dct->d = (y4 - y3 - y2 + y1) / 4;

    dct->pb_avg = (pb1 + pb2 + pb3 + pb4) / 4;
    dct->pr_avg = (pr1 + pr2 + pr3 + pr4) / 4;

    info->dct_row++;
    if (info->dct_row == info->methods->height(info->dct_array)) {
        info->dct_row = 0;
        info->dct_col++;
    }
}

/*
 *Function: group_pixels_to_dct
 *Purpose: Traverses through the 2-D pixels and populates the corresponding
            dct array
 *Inputs: column i, row, j, 2-D pixels that has XYZ structs, current element, 
          closure
 *Output: None
 *Error:  This function can only be called within xyz_to_dct
 */
void group_pixels_to_dct(int i, int j , A2Methods_UArray2 pixels, void *curr,
                                                                  void *cl)
{
    (void) curr;
    struct Closure_dct *info = (struct Closure_dct*) cl;
    XYZ xyz = (XYZ) info->methods->at(pixels, i, j); /* the xyz we want to
                                                      * convert */

    XYZ *pixel_group = info->pixel_group;

    pixel_group[info->array_pos] = xyz;
    info->array_pos++;

    if (info->array_pos == 4) { /* if all 4 pixels have been collected */

        pixel_to_dct(info);
        info->array_pos = 0; /* reset array_pos of array */
    }
}

/*
 *Function: xyz_to_dct
 *Purpose: Populates a 2-D Dct array using block major mapping 
 *Inputs: Uarray2 with XYZ structs
 *Output: Returns a populated 2-D Dct array of height / 2 and width / 2
 *Error:  Input Uarray2 must have XYZ structs
 */
A2Methods_UArray2 xyz_to_dct(A2Methods_UArray2 pixels){
    assert(pixels);
    A2Methods_T methods = uarray2_methods_blocked;
    int height = methods->height(pixels)/2; /* We group 4 pixels together */
    int width  = methods->width(pixels)/2; /* therefore width and height 
                                            is halved */

    A2Methods_UArray2 dct_array = methods->new(width, height, 
                                                      sizeof(struct Dct));

    XYZ cl_array[4]; /* track dct_array pixel groups of 4 while block-major 
                        mapping */

    struct Closure_dct closure;
    closure.pixel_group = cl_array;
    closure.dct_array = dct_array;
    closure.methods = methods;
    closure.array_pos = 0;
    closure.dct_col   = 0;
    closure.dct_row   = 0;

    A2Methods_mapfun *map = methods->map_block_major;

    map(pixels,group_pixels_to_dct, &closure);

    methods->free(&pixels); /* free xyz pixels */

    return dct_array;
}

/*
 *Function: dct_to_pixel
 *Purpose: Creates a 2-D array of XYZ structs using the DCT struct.
           Populates the array by mapping through the DCT array.
 *Inputs: void cl that has our DCT values passed in as well as the position of
          the new array to be populated
 *Output: Populates a struct XYZ of double the height and width of the original
          2-D array
 *Error:  pixel_group cannot be void
 */
void dct_to_pixel(void *cl) {
  
    struct Closure_dct *info = (struct Closure_dct*) cl;
    
    XYZ *pixel_group = info->pixel_group;

    Dct dct = (Dct) info->methods->at(info->dct_array,
                                                     info->dct_col,
                                                     info->dct_row);


    float a  = dct->a,
          b  = dct->b,
          c  = dct->c,
          d  = dct->d,
          pb = dct->pb_avg,
          pr = dct->pr_avg;

    for (int i = 0; i < 4; i++) {
        XYZ pixel = malloc(sizeof(struct XYZ));
        if (i == 0)
            pixel->y = a - b - c + d; /* y1 conversion */
        else if (i == 2)
            pixel->y = a - b + c - d; /* y2 conversion */
        else if (i == 1)
            pixel->y = a + b - c - d; /* y3 conversion */
        else if (i == 3)
            pixel->y = a + b + c + d; /* y4 conversion */

    //printf("y:%f, pb%f, pr:%f\n", pixel->y, pb, pr);

        pixel->pb = pb;
        pixel->pr = pr;


        pixel_group[i] = pixel;
    }

    info->dct_row++;
    if (info->dct_row == info->methods->height(info->dct_array)) {
        info->dct_row = 0;
        info->dct_col++;
    }
}
  

/*
 *Function: dct_to_xyz
 *Purpose: Populates a 2-D XYZ array using block major mapping through the
           empty 2-D array pixels with struct XYZ
 *Inputs: Uarray2 with XYZ structs
 *Output: Returns a populated 2-D XYZ array
 *Error:  None
 */
void ungroup_dct_to_xyz(int i, int j , A2Methods_UArray2 pixels, void *curr,
                                                                  void *cl)
{
    
    (void) curr;
    struct Closure_dct *info = (struct Closure_dct*) cl;

    XYZ *pixel_group = info->pixel_group;
    XYZ xyz = (XYZ) info->methods->at(pixels, i, j); /* the xyz we want to
                                                      * convert */

    if (info->array_pos == 0) {
        dct_to_pixel(cl);
    }

    xyz->y  = pixel_group[info->array_pos]->y;
    xyz->pb = pixel_group[info->array_pos]->pb;
    xyz->pr = pixel_group[info->array_pos]->pr;
    info->array_pos++;



    if (info->array_pos == 4) {       /* if all 4 pixels have been inserted */

        for (int i = 0; i < 4; i ++) {
            free(pixel_group[i]);
        }
        info->array_pos = 0; /* reset array_pos of array */
    }
}


/*
 *Function: dct_to_xyz
 *Purpose: Creates a 2-D array of struct XYZ's and populates it with a 
           map function
 *Inputs: 2-D Uarray2 of struct Dct's 
 *Output: Populates a struct XYZ
 *Error:  dct_array cannot be void
 */
A2Methods_UArray2 dct_to_xyz(A2Methods_UArray2 dct_array){
    assert(dct_array);
    A2Methods_T methods = uarray2_methods_blocked;
    int height = methods->height(dct_array) * 2; /* We grouped 4 pixels */
    int width = methods->width(dct_array) * 2;   /* therefore width and height
                                                  * were halved */

    A2Methods_UArray2 pixels = methods->new_with_blocksize(width, 
                                                           height, 
                                                           sizeof(struct XYZ),
                                                           2);

    XYZ cl_array[4]; /* store pixel groups of 4 while block-major mapping */

    struct Closure_dct closure;
    closure.pixel_group = cl_array;
    closure.dct_array = dct_array;
    closure.methods = methods;
    closure.array_pos = 0;
    closure.dct_col   = 0;
    closure.dct_row   = 0;

    A2Methods_mapfun *map = methods->map_block_major;

    map(pixels, ungroup_dct_to_xyz, &closure);

    methods->free(&dct_array); /* free xyz pixels */

    return pixels;
}
