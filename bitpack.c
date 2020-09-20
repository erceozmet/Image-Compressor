/*************************************************************************
 *
 *  bitpack.c
 *  Assignment: arith
 * 
 *  Authors: Erce Ozmetin, Dogacan Colak 
 *  Date: 3/07/2020
 *  
 *  This file implements functions that test whether a value can be represented 
 *  in n bits, inserts/extracts a value into/from a given range within a 64-bit 
 *  word.
 * 
 **************************************************************************/
 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <except.h>
#include <assert.h>
#include <math.h>

Except_T Bitpack_Overflow = { "Overflow packing bits" };

const uint64_t WORD_SIZE = 64;

/*
 *Function: Bitpack_fitsu
 *Purpose: Tells whether a given unsigned integer is able to be represented
           in the given number of pixels
 *Inputs: 'uint64_t n' is the value to be represented in 'unsigned width' bits.
 *Output: True or false
 *Error: width must be less than or equal to WORD_SIZE
 */
bool Bitpack_fitsu(uint64_t n, unsigned width) {
    assert(width <= WORD_SIZE);

    if (width == 0)
        return false;

    if (n < pow(2, width)){
        return true;
    }
    else{
        return false;
    }
}

/*
 *Function: Bitpack_fitss
 *Purpose: Tells whether a given signed integer can be represented
           in the given number of pixels
 *Inputs: 'int64_t n' is the value to be represented in 'unsigned width' bits.
 *Output: True or false
 *Error: width must be less than or equal to WORD_SIZE
 */
bool Bitpack_fitss( int64_t n, unsigned width){
    assert(width <= WORD_SIZE);

    if (width == 0)
        return false;

    int64_t min = -pow(2, width) / 2;
    int64_t max =  pow(2, width) / 2 - 1;

    if (n <= max && n >= min){
        return true;
    }
    else{
        return false;
    }
}

/*
 *Function: Bitpack_getu
 *Purpose: Extracts a desired range of bits (treated as an unsigned) from a 
           given word.
 *Inputs: 'uint64_t word' is the word which 'unsigned width' bits to the
          left of the 'unsigned lsb'th bit will be extracted from.
 *Output: uint64_t value that was extracted
 *Error: width, as well as width + lsb, must be less than or equal to WORD_SIZE
 */
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb){
    assert(width <= WORD_SIZE);
    assert(width + lsb <= WORD_SIZE);

    if (width == 0){
        return 0;
    }
    uint64_t mask = ~0U >> (WORD_SIZE - width) << lsb;
    word &= mask;
    word >>= lsb;
    return word;
}

/*
 *Function: Bitpack_gets
 *Purpose: Extracts a desired range of bits (treated as a signed) from a 
           given word.
 *Inputs: 'uint64_t word' is the word which 'unsigned width' bits to the
          left of the 'unsigned lsb'th bit will be extracted from.
 *Output: int64_t value that was extracted
 *Error: width, as well as width + lsb, must be less than or equal to WORD_SIZE
 */
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb){
    assert(width <= WORD_SIZE);
    assert(width + lsb <= WORD_SIZE);

    int64_t field = (int64_t) Bitpack_getu(word, width, lsb);

    if (field < pow(2, width) / 2){
        return field;
    } 
    else {
        int64_t mask = ~0;    
        mask <<= width;
        field |= mask;
        return field;
    }
}

/*
 *Function: Bitpack_newu
 *Purpose: Inserts a value (treated as an unsigned) into a desired range of bits
           in a given word.
 *Inputs: 'uint64_t value' will be inserted to 'unsigned width' bits to the left
          of the 'unsigned lsb'th bit in the 'uint64_t word'.
 *Output: uint64_t new word with the inserted value
 *Error: width, as well as width + lsb, must be less than or equal to WORD_SIZE,
         value must fit into 'width' bits.
 */
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, 
                                                     uint64_t value) {
    assert(width <= WORD_SIZE);
    assert(width + lsb <= WORD_SIZE);
    if (!Bitpack_fitsu(value, width)){
        RAISE(Bitpack_Overflow);
    }
        
    uint64_t mask = ~(~0U >> (WORD_SIZE - width) << lsb);
    word &= mask;
    uint64_t new_mask = value << lsb;

    word |= new_mask;
    return word;
}

/*
 *Function: Bitpack_news
 *Purpose: Inserts a value (treated as a signed) into a desired range of bits
           in a given word.
 *Inputs: 'int64_t value' will be inserted to 'unsigned width' bits to the left
          of the 'unsigned lsb'th bit in the 'uint64_t word'.
 *Output: uint64_t new word with the inserted value
 *Error: width, as well as width + lsb, must be less than or equal to WORD_SIZE,
         'value' must fit into 'width' bits.
 */
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb, 
                                                     int64_t value) {
    assert(width <= WORD_SIZE);
    assert(width + lsb <= WORD_SIZE);

    if (!Bitpack_fitss(value, width)){
        RAISE(Bitpack_Overflow);
    }
    uint64_t mask = ~(~0U >> (WORD_SIZE - width) << lsb);
    word &= mask;

    uint64_t to_insert = (uint64_t) value;
    to_insert <<= (WORD_SIZE - width);
    to_insert >>= (WORD_SIZE - width);
    uint64_t new_mask = to_insert << lsb;

    word |= new_mask;
    return word;
}