/* ================================================================
 *  NAME        : Akshay Mittha
 *  REG NO.     : 25048_081
 *  DATE        : 22 / 05 / 2026
 *  FILE        : decode.h
 *  PROJECT     : LSB Steganography in C
 *  DESCRIPTION : Header file containing structure definitions,
 *                macros, and function prototypes required for 
 *                the decoding process.
 * ================================================================ 
 */
#ifndef DECODE_H
#define DECODE_H

typedef struct _DecodeInfo
{
    /* Stego Image Info */
    char *stego_img_fname;  // => Store the stego_img_fname
    FILE *stego_img_fptr;   // => File pointer for stego_img

    /* Secret File Info */
    char secret_fname[50];     // => Store the secret_fname
    FILE *secret_fptr;      // => File pointer for secret_file
    char secret_extn[7];    // => Store the secret file extension
    int secret_extn_size;   // => Store the secret file extn size
    unsigned long int secret_file_size;  // Store the file size

}DecodeInfo;

/* Read and Validate the Decode args from argv*/
Status validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(char *argv[],DecodeInfo *decInfo);

/* Open the files */
Status open_decode_file(DecodeInfo *decInfo);

/* Decode the magic string */
Status decode_magic_string(const char *magic_string,DecodeInfo *decInfo);

/* Decode Secret File Extension Size and Extension */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode Secret File Data */
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode 1 byte from LSB of image data */
char decode_1byte_from_lsb(char *buffer_8);

/* Decode 4 bytes from LSB of image data */
unsigned int decode_4byte_from_lsb(char *buffer_32);

/* Close Opened Files */
Status close_opened_decode_files(DecodeInfo *decInfo);

#endif