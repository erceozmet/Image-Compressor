/*************************************************************************
 *
 *  quant.c
 *  Assignment: arith
 * 
 *  Authors: Erce Ozmetin, Dogacan Colak 
 *  Date: 3/7/2020
 *  
 *  This program traverses through a 2-D Uarray2 with struct Dct's and
 *  does chroma quantization to populate 2-D array of struct Codeword's
 * 
 **************************************************************************/
#include "quant.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "arith40.h"
#include "a2methods.h"
#include "a2blocked.h"
#include "assert.h"

/* 
 * Includes the float variables to be quantized in our 
 * apply function quantize. 
 */
struct Dct
{
    float a, b, c, d;
    float pb_avg,
          pr_avg;
};

/*  
 * Includes the quantized integer values to be
 *  stored after quantization
 */
struct Codeword
{
    unsigned a;
    int b, c, d;
    unsigned index_pb, index_pr;
};

struct Closure
{
    A2Methods_UArray2 array2;
    A2Methods_T methods;
};

/* 
 * Our chromatization scale that quantizes values 
 * between -0.5 to 0.5 into -15 to 15 integer 
 * values.
 */
const float QUANT_SCALE[63] = {-0.30,-0.25,-0.20,-0.16,-0.12,-0.09,-0.07,-0.05, 
                              -0.03,-0.012,-0.0096,-0.009,-0.007,-0.0055,-0.005,
                              -0.0044,-0.0037,-0.0033,-0.0026,-0.0022,-0.0018,
                              -0.0016,-0.0013,-0.0011,-0.0009,-0.0008,-0.0006,
                               -0.0005,-0.0002,-0.0001,0,0.0001,0.0002,0.0005,
                               0.0006,0.0008,0.0009,0.0011,0.0013,0.0016,0.0018,
                               0.0022,0.0026,0.0033,0.0037,0.0044,0.005,0.0055,
                               0.007,0.009,0.0096,0.012,0.03,0.05,0.06,0.07,
                               0.09,0.12,0.16,0.20,0.25,0.30};

/*
 * Function: findIndex
 * Purpose: A helper for our apply function that determines where input 
            values correspond to in QUANT_SCALE array.
 * Inputs: float value: our variable to be quantized
 * Output: int index  : the integer value input was in the interval of
 * Error:  It is illegal use a float value more than 1.
 */
int findIndex(float value) {
    float interval = 1;
    int index;
    for (int k = -31; k < 32; k++){
        if (fabs(value - QUANT_SCALE[k + 31]) < interval) {
            /* Calculates the chroma interval by comparing the differences from
             * each value in the QUANT_SCALE
             */
            interval = fabs(value - QUANT_SCALE[k + 31]);
          index = k;
        }
    }
    return index;
}

/*
 *Function: quantize
 *Purpose: Changes struct Dct to struct Codeword
 *Inputs: column i , width j, dct array that has float value structs, 
          current struct , cl
 *Output: None
 *Error: None
 */
void quantize(int i, int j , A2Methods_UArray2 dct, void *curr, void *cl) {
    (void) dct;
    struct Closure *cl_p        = (struct Closure *) cl;
    A2Methods_UArray2 codewords = (A2Methods_UArray2) cl_p->array2;
    A2Methods_T methods         = (A2Methods_T) cl_p->methods;
    struct Dct *dct_float       = (struct Dct*) curr;

    struct Codeword *codeword = (struct Codeword *) methods->at(codewords, 
                                                                i , j);

    codeword->index_pb = Arith40_index_of_chroma(dct_float->pb_avg);
    codeword->index_pr = Arith40_index_of_chroma(dct_float->pr_avg);

    codeword->a = round(dct_float->a  * 63);
    codeword->b = findIndex(dct_float->b);
    codeword->c = findIndex(dct_float->c);
    codeword->d = findIndex(dct_float->c);
}

/*
 *Function: float_to_quant
 *Purpose: Creates a new 2-D Uarray2 that has the quantized values
            of struct Dct under struct Codewords.
 *Inputs: A2Methods_UArray2 dct. 2-D Uarray2 that has struct Dct's
 *Output: A populated Codewords Uarray2
 *Error: Input cannot be void
 */
A2Methods_UArray2 float_to_quant(A2Methods_UArray2 dct) {
    assert(dct);
    A2Methods_T methods = uarray2_methods_blocked;
    int width  = methods->width(dct);
    int height =  methods->height(dct);
   
    A2Methods_UArray2 codewords = methods->new(width, height, 
                                                      sizeof(struct Codeword));

    struct Closure cl;
    cl.array2  = codewords;
    cl.methods = methods;

    A2Methods_mapfun *map = methods->map_block_major;

    map(dct, quantize, &cl);

    methods->free(&dct); /* free dct array */

    return codewords;
}

/*
 *Function: dequantize
 *Purpose: Helper function that changes struct Codeword to struct Dct
 *Inputs: column i , width j, dct array that has float value structs, 
          current struct , cl
 *Output: None
 *Error: None
 */
void dequantize(int i, int j , A2Methods_UArray2 codewords, void *curr,
                                                            void *cl) {
    (void) codewords;
    struct Closure *cl_p  = (struct Closure *) cl;
    A2Methods_UArray2 dct = (A2Methods_UArray2) cl_p->array2;
    A2Methods_T methods   = (A2Methods_T) cl_p->methods;

    struct Codeword *codeword = (struct Codeword *) curr;
    struct Dct *dct_float     = (struct Dct *) methods->at(dct, i, j);

    dct_float->pb_avg = Arith40_chroma_of_index(codeword->index_pb);
    dct_float->pr_avg = Arith40_chroma_of_index(codeword->index_pr);

    dct_float->a = ((float) codeword->a) / 63;
    dct_float->b = QUANT_SCALE[codeword->b + 31];
    dct_float->c = QUANT_SCALE[codeword->c + 31];
    dct_float->d = QUANT_SCALE[codeword->d + 31];
}


/*
 *Function: quant_to_float
 *Purpose: Creates a new 2-D Uarray2 that has the struct Dct values
           estimated from struct Codewords
 *Inputs: A2Methods_UArray2 Codewords that has struct Codeword
 *Output: A populated Codewords Uarray2
 *Error: Input cannot be void
 */
A2Methods_UArray2 quant_to_float(A2Methods_UArray2 codewords)
{
    A2Methods_T methods = uarray2_methods_blocked;
    int width  = methods->width(codewords);
    int height =  methods->height(codewords);

    A2Methods_UArray2 dct = methods->new(width, height, sizeof(struct Dct));

    struct Closure cl;
    cl.array2  = dct;
    cl.methods = methods;

    A2Methods_mapfun *map = methods->map_block_major;

    map(codewords, dequantize, &cl);

    methods->free(&codewords);

    return dct;
}
