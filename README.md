# Arith
A lossy image compression algorithm that utilizes bit packing.

### Installation
* Unfortunately, it is not possible to run the project on your machine, since the required libraries are hosted in Tufts servers.

### Explanation of Files
* __40image.c__   is the main() in which the program is ran.

* __compress40__  has the functions for compressing and decompressing, which are
            called by 40image

* __convert__     has functions to convert from rgb space to video space and vice
            versa.

* __dct__         has functions that transfrom y, pb, pr to dct float values and vice
versa.

* __quant__       has functions that quantize dct float values into indices and vice
            versa.

* __wordpack__    has functions that pack quantized dct values into 32-bit codewords
            and vice versa.

* __bitpack__     has functions that are used by wordpack in order to directly
            manipulate the bits in a word.
            
* __ppmdiff__ is used for checking the percent difference between the original image and 
            an image that went through compression & decompression.
            
### Algorithm
It is a very lengthy algorithm, we recommend to check the [project spec](https://www.cs.tufts.edu/comp/40-2011f/homework/arith.html).

Our implementation is able to preserve the image 94%.

![Before](/demo/animals.jpg)
![After](/demo/animals_2.jpg)
<br/>
Before and after compression & decompression.
