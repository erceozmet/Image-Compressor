#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include <math.h>
#include <pnm.h>
#include "a2plain.h"
#include "a2blocked.h"
#include "convert.h"
#include "dct.h"
#include "quant.h"
#include "wordpack.h"
#include "bitpack.h"

struct Dct
{
    float a, b, c, d;
    float pb_avg,
          pr_avg;
};

void compress40(FILE *input){
    assert(input);
    A2Methods_T methods = uarray2_methods_blocked;

    /*
     * Creates 2-D blocked array and populates it with rgb values
     * from input FILE
     */
    Pnm_ppm img = Pnm_ppmread(input, methods);
    int width_trimmed = 0,
        height_trimmed = 0;
    if (img->height % 2 != 0){
        img->height--;
        height_trimmed++;
    }
    if (img->width  % 2 != 0){
        img->width--;
        width_trimmed++;
    }

    A2Methods_UArray2 xyz_pixels = convert_to_xyz_space(img->pixels);
    A2Methods_UArray2 dct_array  = xyz_to_dct(xyz_pixels);
    A2Methods_UArray2 codewords  = float_to_quant(dct_array);
    A2Methods_UArray2 compressed = pack_codewords(codewords);


    printf("COMP40 Compressed image format 2\n%u %u", img->width,
                                                      img->height);
    printf("\n");

    for (int j = 0; j < img->methods->height(compressed); j++) {
        for (int i = 0; i < img->methods->width(compressed); i++) {
            char *word = (char*) img->methods->at(compressed, i, j);
            int index = 3;
            while (index >= 0){
                putchar(*(word + index));
                index--;
            }
        }
    }
    if (width_trimmed  == 1)
        img->width++;
    if (height_trimmed == 1)
        img->height++;

    methods->free(&compressed);
    Pnm_ppmfree(&img);
}


void decompress40(FILE *input){
    assert(input);
    A2Methods_T methods = uarray2_methods_blocked;
    FILE * output = stdout;

    unsigned height, width;

    int read = fscanf(input, "COMP40 Compressed image format 2\n%u %u",
                                &width, &height);
    assert(read == 2);
    int c = getc(input);
    assert(c == '\n');

    A2Methods_UArray2 compressed = methods->new(width / 2, height / 2,
                                                       sizeof(uint32_t));
    for (int j = 0; j < methods->height(compressed); j++) {
        for (int i = 0; i < methods->width(compressed); i++) {
            uint32_t *word_p = (uint32_t *) methods->at(compressed, i, j);
            *word_p = 0;
            int byte_pos = 3; /* insert to lowest order */
            while (byte_pos >= 0) {
                int32_t byte = (int32_t) getc(input);
                assert(byte != EOF);
                byte = Bitpack_getu(byte, 8, 0);
                *word_p = Bitpack_newu(*word_p, 8, 8 * byte_pos, byte);
                byte_pos--;
            }
        }
    }

    A2Methods_UArray2 unpacked = unpack_codewords(compressed);
    A2Methods_UArray2 dct_array  = quant_to_float(unpacked);
    A2Methods_UArray2 xyz_pixels  = dct_to_xyz(dct_array);
    A2Methods_UArray2 rgb_pixels = convert_to_rgb_space(xyz_pixels);

    int denominator = 255;
    assert(denominator < 65535 && denominator > 200);

    struct Pnm_ppm pixmap = { .width = width, .height = height
                                , .denominator = denominator, .pixels = rgb_pixels
                                , .methods = methods
                            };

    Pnm_ppmwrite(output, &pixmap);
    methods->free(&rgb_pixels);
}
