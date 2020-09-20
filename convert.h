/*
 * convert.h
 *
 * Dogacan Colak, Erce Ozmetin, 3/7/2020
 * HW4
 *
 * The interface for the XYZ-RGB conversion module. Has two functions, one of
 * which converts from Video space to RGB space, and the other vice versa.
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <a2methods.h>

/*
 * convert_to_xyz_space
 * 
 * Returns a new UArray2 with the same size as the input 'rgb_pixels', except 
 * each rgb pixel is replaced by an xyz pixel
 *
 * Assumes rgb_pixels is not NULL
 */
A2Methods_UArray2 convert_to_xyz_space(A2Methods_UArray2 rgb_pixels);

/*
 * convert_to_rgb_space
 * 
 * Returns a new UArray2 with the same size as the input 'xyz_pixels', except 
 * each xyz pixel is replaced by an rgb pixel
 *
 * Assumes xyz_pixels is not NULL
 */
A2Methods_UArray2 convert_to_rgb_space(A2Methods_UArray2 xyz_pixels);