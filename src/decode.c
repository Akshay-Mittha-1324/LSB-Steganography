/* ================================================================
 *  NAME        : Akshay Mittha
 *  REG NO.     : 25048_081
 *  DATE        : 22 / 05 / 2026
 *  FILE        : decode.c
 *  PROJECT     : LSB Steganography in C
 *  DESCRIPTION : Implements the entire decoding pipeline,
 *                including Stego image validation, and LSB
 *                bitwise extraction to reconstruct the secret file.
 *
 *  FUNCTION INDEX:
 *    validate_decode_args()      — parses Stego and output file args
 *    open_decode_file()          — safely opens Stego image
 *    decode_magic_string()       — verifies #* identifier exists
 *    decode_secret_file_extn()   — extracts extension and opens file
 *    decode_secret_file_data()   — extracts the secret file contents
 *    decode_1byte_from_lsb()     — extracts 1 byte from 8 image bytes
 *    decode_4bytes_from_lsb()    — extracts 4 bytes from 32 image bytes
 *    close_opened_decode_files() — closes active file pointers
 *    do_decoding()               — coordinates all decode operations
 * ================================================================ 
 */
#include <stdio.h>
#include "common.h"
#include "decode.h"
#include <string.h>


/* 
 * Do Decoding
 * Input: Command line arguments, DecodeInfo pointer
 * Output: success or failure
 * Description: Orchestrates the entire decoding pipeline step-by-step.
 */
Status do_decoding(char *argv[], DecodeInfo *decInfo)
{
    int status = validate_decode_args(argv, decInfo);

    printf("Validating Arguments...\n");
    
    if(status == failure)
    {
        printf("ERROR : Argument Validation Failed\n");
        return failure;
    }

    printf("Success : Argument Validation Completed\n");

    printf("Decoding Started...\nOpening File...\n");

    status = open_decode_file(decInfo);

    if(status == failure)
    {
        printf("ERROR : Opening Files Failed\n");
        return failure;
    }

    printf("Success : Files Opened\n");

    printf("Decoding Steps:\n");

    printf("1. Decode Magic String\n");

    status = decode_magic_string(MAGIC_STRING, decInfo);
    
    if(status == failure)
    {
        printf("ERROR : Magic String Decoding Failed\n");
        return failure;
    }

    printf("Success : Magic String Decoded\n");

    printf("2. Decode Secret File Extension Size and Extension\n");

    status = decode_secret_file_extn(decInfo);
    
    if(status == failure)
    {
        printf("ERROR : Secret File Extension Decoding Failed\n");
        return failure;
    }

    printf("Success : Secret File Extension Decoded\n");

    printf("3. Decode Secret File Size and Data\n");

    status = decode_secret_file_data(decInfo);
    
    if(status == failure)
    {
        printf("ERROR : Secret File Data Decoding Failed\n");
        return failure;
    }

    printf("Success : Secret File Data Decoded\n");

    printf("4. Close Files\n");

    close_opened_decode_files(decInfo);

    printf("Success : Files Closed\n");

    return success;
}

/* 
 * Validate Decode Arguments
 * Input: Command line arguments, DecodeInfo pointer
 * Output: success or failure
 * Description: Validates if the correct Stego BMP is provided and 
 *              manages the output file name string extraction.
 */
Status validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    char *dot_position = strchr(argv[2],'.');
    
    if(dot_position != NULL && strcmp(dot_position,".bmp") == 0)
    {
        decInfo->stego_img_fname = argv[2];
    }
    else
    {
        printf("ERROR : The Image File Is Not A .bmp File\n");
        return failure;
    }
    
    char *dot_pos;
    int char_before_dot;
    if(argv[3] != NULL)
    {
        dot_pos = strchr(argv[3], '.');
        if(dot_pos != NULL)
        {
            char_before_dot = dot_pos - argv[3];
            strncpy(decInfo->secret_fname, argv[3], char_before_dot);
            decInfo->secret_fname[char_before_dot] = '\0';
        }
        else
        {
            strcpy(decInfo->secret_fname, argv[3]);
        }
    }
    else
    {
        strcpy(decInfo->secret_fname, "decode_output");
    }
    return success;
}

/* 
 * Open Decode File
 * Input: DecodeInfo pointer
 * Output: success or failure
 * Description: Safely opens the Stego BMP image in read binary ("rb") mode.
 */
Status open_decode_file(DecodeInfo *decInfo)
{
    decInfo->stego_img_fptr = fopen(decInfo->stego_img_fname, "rb");

    printf("Openeing Stego Encoded File %s\n",decInfo->stego_img_fname);

    if(decInfo->stego_img_fptr == NULL)
    {
        printf("ERROR : Opening Stego Image File Failed\n");
        return failure;
    }

    printf("Stego File Opened Successfully\n");

    return success;
}

/* 
 * Decode Magic String
 * Input: Magic string ("#*"), DecodeInfo pointer
 * Output: success or failure
 * Description: Skips the 54-byte BMP header, extracts the next 2 bytes 
 *              from the LSBs, and verifies they match the magic string.
 */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    int header_size = 54;

    printf("Decoding Magic String ...\n");

    fseek(decInfo->stego_img_fptr, header_size, SEEK_SET);

    int size = strlen(magic_string);

    int read_size = 8;
    char buffer_8[read_size];
    char extracted_char;

    for(int i = 0; i < size; i++)
    {
        fread(buffer_8, 1, read_size, decInfo->stego_img_fptr);
        extracted_char = decode_1byte_from_lsb(buffer_8);
        if(extracted_char != magic_string[i])
        {
            printf("ERROR : Magic String not Matched\n");
            return failure;
        }
    }

    return success;
}

/* 
 * Decode Secret File Extension
 * Input: DecodeInfo pointer
 * Output: success or failure
 * Description: Extracts the extension size and string, appends it to the 
 *              output file name, and opens the output file in write mode.
 */
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    int buf_size = 32;
    char buf_32[buf_size];

    fread(buf_32, 1, buf_size, decInfo->stego_img_fptr);
    
    printf("Decoding Secret File Extension Size...\n");

    int size = decode_4byte_from_lsb(buf_32);

    decInfo->secret_extn_size = size;

    printf("Secret File Extension Size Decoded Successfully\n");
    
    int read_size = 8;
    char buf_8[read_size];

    printf("Decoding Secret File Extension...\n");

    for(int i = 0; i < size; i++)
    {
        fread(buf_8, 1, read_size, decInfo->stego_img_fptr);
        decInfo->secret_extn[i] = decode_1byte_from_lsb(buf_8);
    }
    
    decInfo->secret_extn[size] = '\0';

    printf("Secret File Extension Decoded Successfully\n");

    strcat(decInfo->secret_fname, decInfo->secret_extn);
    
    decInfo->secret_fptr = fopen(decInfo->secret_fname, "wb");
    
    printf("Opening Secret File %s\n",decInfo->secret_fname);

    if(decInfo->secret_fptr == NULL)
    {
        printf("ERROR : Opening Secret File Failed\n");
        return failure;
    }

    printf("Opened Secret File Successfully\n");

    return success;
}

/* 
 * Decode Secret File Data
 * Input: DecodeInfo pointer
 * Output: success or failure
 * Description: Extracts the exact size of the secret file, loops to extract 
 *              all characters, and writes them securely to the output file.
 */
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    int buf_size = 32;
    char buf_32[buf_size];

    printf("Decoding Secret File Size...\n");

    fread(buf_32, 1, buf_size, decInfo->stego_img_fptr);

    int size = decode_4byte_from_lsb(buf_32);

    decInfo->secret_file_size = size;

    printf("Secret File Size Decoded Successfully\n");

    int read_size = 8;
    char buf_8[read_size];
    char decoded_char;

    printf("Decoding Secret File Data...\n");

    for(int i = 0; i < size; i++)
    {
        fread(buf_8, 1, read_size, decInfo->stego_img_fptr);
        decoded_char = decode_1byte_from_lsb(buf_8);
        fwrite(&decoded_char, 1, 1, decInfo->secret_fptr);
    }

    printf("Secret File Data Decoded Successfully\n");
    
    return success;
}

/* 
 * Decode 1 Byte from LSB
 * Input: 8-byte array from the image
 * Output: Extracted character
 * Description: Combines the LSBs from 8 image bytes to reconstruct 1 byte of data.
 */
char decode_1byte_from_lsb(char *buffer_8)
{
    char data = 0;
    char bit;

    for(int i = 0; i < 8; i++)
    {
        bit = buffer_8[i] & 1;
        data = (data | (bit << (7 - i)));
    }
    return data;
}

/* 
 * Decode 4 Bytes from LSB
 * Input: 32-byte array from the image
 * Output: Extracted integer
 * Description: Combines the LSBs from 32 image bytes to reconstruct a 4-byte integer.
 */
unsigned int decode_4byte_from_lsb(char *buffer_32)
{
    unsigned int data = 0;
    unsigned int bit;

   for(int i = 0; i < 32; i++)
   {
        bit = buffer_32[i] & 1;
        data = data | (bit << (31 - i));
   }

   return data;
}

/* 
 * Close Opened Files
 * Input: DecodeInfo pointer
 * Output: success or failure
 * Description: Closes the Stego image and the decoded secret file safely.
 */
Status close_opened_decode_files(DecodeInfo *decInfo)
{
    printf("Closing All Opened Files...\n");
    fclose(decInfo->stego_img_fptr);
    fclose(decInfo->secret_fptr);
    return success;
}