/*
 * dct.h
 *
 * Dogacan Colak, Erce Ozmetin, 3/7/2020
 * HW4
 *
 * The interface for the xyz to dct conversion module. Has two functions, one of
 * which converts y-pb-pr values into dct floats, and the other vice versa.
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <a2methods.h>

typedef struct Dct *Dct;
typedef struct XYZ *XYZ;

/*
 * xyz_to_dct
 * 
 * Returns a new UArray2 with 1/4 the size as the input 'pixels', each 
 * 4 pixel group of xyz pixels is replaced by one struct of dct float values.
 *
 * Assumes pixels is not NULL
 */
A2Methods_UArray2 xyz_to_dct(A2Methods_UArray2 pixels);

/*
 * dct_to_xyz
 * 
 * Returns a new UArray2 with 1/4 the size as the input 'dct_array', 
 * each struct of dct float values is replaced by 4 structs of xyz 
 * values.
 *
 * Assumes dct_array is not NULL
 */
A2Methods_UArray2 dct_to_xyz(A2Methods_UArray2 dct_array);