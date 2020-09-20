/*************************************************************************
 *
 *  wordpack.c
 *  Assignment: arith
 * 
 *  Authors: Erce Ozmetin, Dogacan Colak 
 *  Date: 3/07/2020
 *  
 *  This file implements functions that pack/unpack quantized dct values 
 *  into/from a 32-bit word.
 * 
 **************************************************************************/

#include <string.h>
#include <stdlib.h>
#include "wordpack.h"
#include "bitpack.h"
#include "a2blocked.h"


struct Codeword /* holds quantized dct values */
{
    unsigned a;
    int b, c, d;
    unsigned index_pb, index_pr;
};

struct Closure /* to pass into apply functions */
{
    A2Methods_UArray2 array2;
    A2Methods_T methods;
};

/*
 *Function: pack_32
 *Purpose: Inserts each value in struct Codeword into a 32-bit codeword in a
 *         UArray2 that is passed around in cl
 *Inputs: Default inputs of an apply function
 *Output: None
 *Error: none
 */
void pack_32(int i, int j , A2Methods_UArray2 codewords, void *curr, void *cl){
    (void) codewords;
    struct Codeword *codeword = (struct Codeword *) curr;
    struct Closure *cl_p      = (struct Closure *) cl;
    A2Methods_UArray2 packed  = (A2Methods_UArray2) cl_p->array2;
    A2Methods_T methods       = (A2Methods_T) cl_p->methods;

    uint32_t *word_32 = (uint32_t*) methods->at(packed, i, j); /* pack into */

    *word_32 = Bitpack_newu(0, 6, 26, codeword->a);
    *word_32 = Bitpack_news(*word_32, 6, 20, codeword->b);
    *word_32 = Bitpack_news(*word_32, 6, 14, codeword->c);
    *word_32 = Bitpack_news(*word_32, 6, 8, codeword->d);
    *word_32 = Bitpack_newu(*word_32, 4, 4, codeword->index_pb);
    *word_32 = Bitpack_newu(*word_32, 4, 0, codeword->index_pr);
}

/*
 *Function: pack_codewords
 *Purpose: Replaces each struct of separate dct values in a UArray2 with 32-bit
           packed codewords.
 *Inputs: UArray2 codewords that has separate dct values in it.
 *Output: UArray2 that holds packed 32-bit codewords.
 *Error: none
 */
A2Methods_UArray2 pack_codewords(A2Methods_UArray2 codewords) {
    A2Methods_T methods =  uarray2_methods_blocked;
    int width  = methods->width(codewords);
    int height = methods->height(codewords);

    A2Methods_UArray2 packed = methods->new(width, height, sizeof(uint32_t));
    
    struct Closure cl;
    cl.array2  = packed;
    cl.methods = methods;

    A2Methods_mapfun *map = methods->map_block_major;

    map(codewords, pack_32, &cl);

    methods->free(&codewords);

    return packed;
}

/*
 *Function: unpack_32
 *Purpose: Extracts each value from struct Codeword in a UArray2 that is passed 
           around in cl, inserts them into another UArray2
 *Inputs: Default inputs of an apply function
 *Output: None
 *Error: none
 */
void unpack_32(int i, int j , A2Methods_UArray2 packed, void *curr, void *cl){
    (void) packed;

    struct Closure *cl_p       = (struct Closure *) cl;
    A2Methods_UArray2 unpacked = (A2Methods_UArray2) cl_p->array2;
    A2Methods_T methods        = (A2Methods_T) cl_p->methods;

    struct Codeword *codeword = (struct Codeword*) methods->at(unpacked, i, j);

    uint32_t word_32 = *((uint32_t*) curr);

    codeword->a = Bitpack_getu(word_32, 6, 26);
    codeword->b = Bitpack_gets(word_32, 6, 20);
    codeword->c = Bitpack_gets(word_32, 6, 14);
    codeword->d = Bitpack_gets(word_32, 6, 8);
    codeword->index_pb = Bitpack_getu(word_32, 4, 4);
    codeword->index_pr = Bitpack_getu(word_32, 4, 0);
}

/*
 *Function: unpack_codewords
 *Purpose: Replaces each packed codeword in a UArray2 with structs of unpacked 
           dct values.
 *Inputs: UArray2 packed that has 32-bit codewords in it
 *Output: UArray2 that holds separate dct values.
 *Error: none
 */
A2Methods_UArray2 unpack_codewords(A2Methods_UArray2 packed) {
    A2Methods_T methods = uarray2_methods_blocked;
    int width  = methods->width(packed);
    int height = methods->height(packed);

    A2Methods_UArray2 unpacked = methods->new(width, height, 
                                              sizeof(struct Codeword));
    
    struct Closure cl;
    cl.array2  = unpacked;
    cl.methods = methods;

    A2Methods_mapfun *map = methods->map_block_major;

    map(packed, unpack_32, &cl);

    methods->free(&packed);

    return unpacked; 
}
