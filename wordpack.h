/*
 * wordpack.h
 *
 * Dogacan Colak, Erce Ozmetin, 3/7/2020
 * HW4
 *
 * The interface for packing quantized dct values into a 32-bit codeword.
 * Has two functions, one of which inserts quantized values into appropriate
 * bits of a 32-bit codeword, and other extracts them from a codeword.
 */
#include <stdio.h>
#include "a2methods.h"

/*
 * pack_codewords
 * 
 * Returns a new UArray2 with the same size as the input 'values', except 
 * each set of values in the array is replaced by 32-bit codewords, to which
 * the values have been inserted.
 *
 * Assumes values is not NULL
 */
A2Methods_UArray2 pack_codewords(A2Methods_UArray2 values);

/*
 * unpack_codewords
 * 
 * Returns a new UArray2 with the same size as the input 'codewords', except 
 * each 32-bit codeword in the array is replaced by a struct of quantized dct
 * values, which have been extracted from the former.
 *
 * Assumes codewords is not NULL
 */
A2Methods_UArray2 unpack_codewords(A2Methods_UArray2 codewords);