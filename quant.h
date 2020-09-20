/*
 * quant.h
 *
 * Dogacan Colak, Erce Ozmetin, 3/7/2020
 * HW4
 *
 * The interface for the quantization module. Has two functions specifically
 * designed for lossy image compression, one of which quantizes float values,
 * and the other dequantizes indices.
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <a2methods.h>

/*
 * float_to_quant
 * 
 * Returns a new UArray2 with the same size as the input 'dct', except 
 * each dct struct of floats is quantized into a corresponding index.
 *
 * Assumes dct is not NULL
 */
A2Methods_UArray2 float_to_quant(A2Methods_UArray2 dct);

/*
 * quant_to_float
 * 
 * Returns a new UArray2 with the same size as the input 'codewords', except 
 * each struct of quantized dct values is unquantized the average value of
 * its corresponding range.
 *
 * Assumes dct is not NULL
 */
A2Methods_UArray2 quant_to_float(A2Methods_UArray2 codewords);
