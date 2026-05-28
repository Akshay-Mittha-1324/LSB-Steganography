/* ================================================================
 *  NAME        : Akshay Mittha
 *  REG NO.     : 25048_081
 *  DATE        : 22 / 05 / 2026
 *  FILE        : encode.c
 *  PROJECT     : LSB Steganography in C
 *  DESCRIPTION : Implements the entire encoding pipeline,
 *                including validation, header copying, and 
 *                LSB bitwise embedding of the secret file.
 *
 *  FUNCTION INDEX:
 *    get_image_size_for_bmp()  — extracts width and height
 *    get_file_size()           — returns the byte size of a file
 *    validate_encode_args()    — verifies input file extensions
 *    check_capacity()          — ensures image can hold secret data
 *    open_files()              — opens file pointers safely
 *    copy_bmp_header()         — copies the 54-byte BMP header
 *    encode_magic_string()     — embeds the #* identifier
 *    encode_secret_file_extn() — embeds extension size and string
 *    encode_secret_file_size() — embeds the secret file byte size
 *    encode_secret_file_data() — embeds the actual secret contents
 *    copy_remaining_img_data() — copies leftover unaltered pixels
 *    encode_1byte_to_lsb()     — embeds 1 byte into 8 image bytes
 *    encode_4byte_to_lsb()     — embeds 4 bytes into 32 image bytes
 *    do_encoding()             — coordinates all encode operations
 * ================================================================ 
 */
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "encode.h"

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
unsigned int get_image_size_for_bmp(FILE *fptr_image)
{
    unsigned int width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: success or failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    encInfo->src_image_fptr = fopen(encInfo->src_image_fname, "r");

    if (encInfo->src_image_fptr == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);
    	return failure;
    }

    printf("Source File Opened Successfully\n");

    encInfo->secret_fptr = fopen(encInfo->secret_fname, "r");

    if (encInfo->secret_fptr == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return failure;
    }

    printf("Secret File Opened Successfully\n");

    encInfo->output_image_fptr = fopen(encInfo->output_image_fname, "w");

    if (encInfo->output_image_fptr == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->output_image_fname);

    	return failure;
    }

    printf("Output File Opened Successfully\n");

    return success;
}

/* 
 * Check Operation Type
 * Input: Command line arguments
 * Output: encode, decode, or unsupported
 * Description: Checks the second argument (-e or -d) to determine the operation.
 */
Opr_type check_operation(char *argv[])
{
    // Checking for the proper tags like -e for encode and -d for decode
    if(strcmp(argv[1],"-e") == 0)
    {
        printf("Selected Operation : ENCODE\n");
        return encode;
    }
    else if(strcmp(argv[1],"-d") == 0)
    {
        printf("Selected Operation : DECODE\n");
        return decode;
    }
    else
    {
       return unsupported;
    }
}

/* 
 * Validate Encode Arguments
 * Input: Command line arguments, EncodeInfo pointer
 * Output: success or failure
 * Description: Validates if the correct number of arguments with proper 
 *              extensions (.bmp and .txt) are provided by the user.
 */
Status validate_encode_args(char *argv[], EncodeInfo *encodeInfo)
{
    
    char *pos = strchr(argv[2],'.');
    // Checking for the proper extensions .bmp and .txt using string functions
    if(pos != NULL && strcmp(pos,".bmp") == 0)
    {
        encodeInfo->src_image_fname = argv[2];
    }
    else 
    {
        printf("ERROR : The Source File Is Not a .bmp File\n");
        return failure;
    }

    pos = strchr(argv[3],'.');
    int count;
    if(pos != NULL)
    {
        encodeInfo->secret_fname = argv[3];
        strcpy(encodeInfo->secret_extn,pos);
        encodeInfo->secret_extn_size = strlen(encodeInfo->secret_extn);
    }
    else
    {
        printf("ERROR : The secret file is must be .txt file\n");
        return failure;
    }

    if(argv[4] == NULL)
    {
        encodeInfo->output_image_fname = "stego.bmp";
        printf("Output file not provided by the user, default name 'stego.bmp' is used\n");
    }
    else
    {
        pos = strchr(argv[4],'.');
        if(pos != NULL && strcmp(pos,".bmp") == 0)
        {
            encodeInfo->output_image_fname = argv[4];
        }
        else
        {
            perror("ERROR : ");
            return failure;
        }
    }
    return success;
}

/* 
 * Get File Size
 * Input: File pointer
 * Output: Size of the file in bytes
 * Description: Seeks to the end of the file to determine its byte size.
 */
unsigned int get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);
    int size = ftell(fptr);
    rewind(fptr);
    return size;
}

/* 
 * Check Capacity
 * Input: EncodeInfo pointer
 * Output: success or failure
 * Description: Compares the capacity of the source BMP image with the 
 *              required size to hide the secret file and metadata.
 */
Status check_capacity(EncodeInfo *encInfo)
{
    // Getting the file size of the .bmp and the secret file
    int src_file_size = get_image_size_for_bmp(encInfo->src_image_fptr);
    encInfo->secret_file_size = get_file_size(encInfo->secret_fptr);

    printf("The size of Source File %s is : %d Bytes\n",encInfo->src_image_fname,src_file_size);
    printf("The size of Secret File %s is : %ld Bytes\n",encInfo->secret_fname, encInfo->secret_file_size);
    
    // Caluclating the required size to encode the secret file into the bmp image
    // Size of Magic String (2 * 8) + Size of File Extension (sizeof(int) * 8) + Size of Entension (4 * 8) + Size of File (sizeof(int) * 8) + File Contents (8 * secret_file_size) 
    int required_size = (strlen(MAGIC_STRING) * 8) + (sizeof(encInfo->secret_extn_size) * 8) + (strlen(encInfo->secret_extn) * 8) + (sizeof(encInfo->secret_file_size) * 8) + (encInfo->secret_file_size * 8);
    
    printf("Required Size to store the secret file in the bmp image is : %d Bytes\n",required_size);

    if(src_file_size < required_size)
    {
        printf("ERROR : The Source File Is Not Capable Of Storing The Secret File\n");
        return failure;
    }

    return success;   
}

/* 
 * Do Encoding
 * Input: Command line arguments, EncodeInfo pointer
 * Output: success or failure
 * Description: Orchestrates the entire encoding pipeline step-by-step.
 */
Status do_encoding(char *argv[], EncodeInfo *encInfo)
{
    int status = validate_encode_args(argv, encInfo);

    printf("Validating Arguments...\n");

    if(status == failure)
    {
        printf("Error : Argument Validation Failed\nPlease enter in the format ./a.out -e img_file.bmp secretfile\n");
        return failure;
    }

    printf("Success : Arguments Validated\n");

    printf("Encoding Started...\nOpenening Files...\n");

    status = open_files(encInfo);
    
    if(status == failure)
    {
        printf("Error : File Opening Failed\nPlease enter in the format ./a.out -e img_file.bmp secretfile\n");
        return failure;
    }

    printf("Success : Files Opened\n");

    printf("Checking Capacity...\n");

    status = check_capacity(encInfo);
    
    if(status == failure)
    {
        printf("Error : Capacity Check Failed\nPlease enter in the format ./a.out -e img_file.bmp secretfile\n");
        return failure;
    }
    
    printf("Success : Sufficient Capacity in Source File to Store the Secret File\nEncoding Steps:\n");
    
    printf("1. Copy BMP Image Header\n");

    status = copy_bmp_header(encInfo);
    
    if(status == failure)
    {
        printf("Error : Header Copy Failed\nPlease enter in the format ./a.out -e img_file.bmp secretfile\n");
        return failure;
    }

    printf("Success : Header Copied\n");
    
    status = encode_magic_string(MAGIC_STRING, encInfo);

    printf("2. Encode Magic String\n");
    
    if(status == failure)
    {
        printf("Error : Magic String Encoding Failed\nPlease enter in the format ./a.out -e img_file.bmp secretfile\n");
        return failure;
    }

    printf("Success : Magic String Encoded\n");
        
    status = encode_secret_file_extn(encInfo);

    printf("3. Encode Secret File Extension Size and Extension\n");

    if(status == failure)
    {
        printf("Error : File Extension Encoding Failed\nPlease enter in the format ./a.out -e img_file.bmp secretfile\n");
        return failure;
    }

    printf("Success : File Extension Encoded\n");

    status = encode_secret_file_data(encInfo);

    printf("4. Encode Secret File Size and Data\n");

    if(status == failure)
    {
        printf("Error : File Data Encoding Failed\nPlease enter in the format ./a.out -e img_file.bmp secretfile\n");
        return failure;
    }

    printf("Success : File Data Encoded\n");   

    status = copy_remaining_img_data(encInfo);

    printf("5. Copy Remaining Image Data\n");

    if(status == failure)
    {
        printf("Error : Remaining Image Data Copy Failed\nPlease enter in the format ./a.out -e img_file.bmp secretfile\n");
        return failure;
    }

    printf("Success : Remaining Image Data Copied\n");

    close_opened_encode_files(encInfo);

    printf("6. Close Files\n");

    printf("Success : Files Closed\n");

    return success;
}

/* 
 * Copy BMP Header
 * Input: EncodeInfo pointer
 * Output: success or failure
 * Description: Copies the exact first 54 bytes (header) from the source 
 *              BMP image to the output Stego BMP image.
 */
Status copy_bmp_header(EncodeInfo *encInfo)
{
    rewind(encInfo->src_image_fptr);
    int header_size = 54;
    char header[header_size]; 
    
    printf("Copying Header (54 Bytes)...\n");

    if(fread(header, 1, header_size, encInfo->src_image_fptr) != header_size)
    {
        printf("ERROR : Failed To Read Header\n");
        return failure;
    }
    
    if(fwrite(header, 1, header_size, encInfo->output_image_fptr) != header_size)
    {
        printf("ERROR : Failed To Write Header\n");
        return failure;
    }

    return success;
}

/* 
 * Encode Magic String
 * Input: Magic string ("#*"), EncodeInfo pointer
 * Output: success or failure
 * Description: Encodes the 2-character magic string into the LSBs of the image.
 */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    // Character buffer to store 8 bits of data
    char buff[8];

    // Magic String Size varialbe
    int mg_size = strlen(magic_string);
    
    // Creating a variable to check the status of encoding msg
    int status;

    printf("Encoding Magic String \"%s\" (%d Bytes)...\n",magic_string,mg_size);

    // Iterating throught the magic string and encoding the data into the LSB of the image data
    for(int i = 0; i < mg_size; i++)
    {
        if(fread(buff, 1, 8, encInfo->src_image_fptr) != 8)
        {
            printf("ERROR : Failed to Fetch %d Bytes from %s\n",8,encInfo->src_image_fname);
            return failure;
        }

        status = encode_1byte_to_lsb(magic_string[i], buff);
        if(status == failure)
        {
            printf("ERROR : Failed To Encode Magic String\n");
            return failure;
        }

        if(fwrite(buff, 1, 8, encInfo->output_image_fptr) != 8)
        {
            printf("ERROR : Failed To Write Magic String\n");
            return failure;
        }
    }

    return success;
}

/* 
 * Encode Secret File Extension
 * Input: EncodeInfo pointer
 * Output: success or failure
 * Description: Encodes the length of the file extension (4 bytes) followed 
 *              by the actual file extension characters (e.g., ".txt").
 */
Status encode_secret_file_extn(EncodeInfo *encInfo)
{
    int buf_size = 32;
    char buf_32[buf_size];

    printf("Encoding Secret File Extension Size...\n");

    if(fread(buf_32, 1, buf_size, encInfo->src_image_fptr) != buf_size)
    {
        printf("ERROR : Failed to Fetch %d Bytes from %s\n",buf_size,encInfo->src_image_fname);
        return failure;
    }
    
    int status = encode_4byte_to_lsb(encInfo->secret_extn_size, buf_32);
    
    if(status == failure)
    {
        printf("ERROR : Failed to Encode Secret File Extension\n");
        return failure;
    }
    
    if(fwrite(buf_32, 1, buf_size, encInfo->output_image_fptr) != buf_size)
    {
        printf("ERROR : Failed to Write %d Bytes into %s\n",buf_size,encInfo->output_image_fname);
        return failure;
    }
    
    // Encoding the Secret File Extension into the LSB of the image data
    int str_size = encInfo->secret_extn_size;
    int exten_buff_size = 8;
    char buf_8[exten_buff_size];
    
    printf("Encoding Secret File Extension...\n");

    for(int i = 0 ; i < str_size; i++)
    {
        if(fread(buf_8, 1, exten_buff_size, encInfo->src_image_fptr) != exten_buff_size)
        {
            printf("ERROR : Failed to Read %d Bytes from %s\n",exten_buff_size,encInfo->src_image_fname);
            return failure;
        }
        
        int status = encode_1byte_to_lsb(encInfo->secret_extn[i], buf_8);
        
        if(status == failure)
        {
            printf("ERROR : Failed to Encode Secret File Extension\n");
            return failure;
        }
        
        if(fwrite(buf_8, 1, exten_buff_size, encInfo->output_image_fptr) != exten_buff_size)
        {
            printf("ERROR : Failed to Write %d Bytes into %s\n",exten_buff_size,encInfo->output_image_fname);
            return failure;
        }
    }
    return success;
}

/* 
 * Encode Secret File Size and Data
 * Input: EncodeInfo pointer
 * Output: success or failure
 * Description: Encodes the secret file byte size (4 bytes) followed by 
 *              all the actual secret file contents byte-by-byte.
 */
Status encode_secret_file_data(EncodeInfo *encInfo)
{

    int buf_32_size = 32;
    char buffer_32[buf_32_size];

    printf("Encoding Secret File Size...\n");

    if(fread(buffer_32, 1, buf_32_size, encInfo->src_image_fptr) != buf_32_size)
    {
        printf("ERROR : Failed to Read %d Bytes from %s\n",buf_32_size, encInfo->src_image_fname);
        return failure;
    }

    int status = encode_4byte_to_lsb(encInfo->secret_file_size, buffer_32);
    
    if(status == failure)
    {
        printf("ERROR : Failed to Encode Secret File Size\n");
        return failure;
    }
    
    if(fwrite(buffer_32, 1, buf_32_size, encInfo->output_image_fptr) != buf_32_size)
    {
        printf("ERROR : Failed to Write %d Bytes into %s\n",buf_32_size,encInfo->output_image_fname);
        return failure;
    }

    char ch;
    int buf_size = 8;
    char buf_8[buf_size];

    printf("Encoding Secret File Data...\n");

    while(fread(&ch, 1, 1, encInfo->secret_fptr) != 0)
    {
        if(fread(buf_8, 1, buf_size, encInfo->src_image_fptr) != buf_size)
        {
            printf("ERROR : Failed to Read %d Bytes from %s\n",buf_size,encInfo->src_image_fname);
            return failure;
        }
        status = encode_1byte_to_lsb(ch, buf_8);
        if(status == failure)
        {
            printf("ERROR : Failed to Encode Secret File Data\n");
            return failure;
        }
        if(fwrite(buf_8, 1, buf_size, encInfo->output_image_fptr) != buf_size)
        {
            printf("ERROR : Failed to Write %d Bytes into %s\n",buf_size,encInfo->output_image_fname);
            return failure;
        }
    }

    return success;
}

/* 
 * Copy Remaining Image Data
 * Input: EncodeInfo pointer
 * Output: success or failure
 * Description: Copies all remaining unaltered pixels from the source 
 *              image to the output image to preserve the picture.
 */
Status copy_remaining_img_data(EncodeInfo *encInfo)
{
    int bytes = 1024;
    char ch[bytes];
    int read_bytes;

    printf("Copying Remaining %s Image Data...\n",encInfo->src_image_fname);

    while(read_bytes = fread(ch, 1, bytes, encInfo->src_image_fptr))
    {
        if(fwrite(ch, 1, read_bytes, encInfo->output_image_fptr) != read_bytes)
        {
            printf("ERROR : Failed to Write %d Bytes into %s\n",read_bytes,encInfo->output_image_fname);
            return failure;
        }
    }
    return success;
}

/* 
 * Encode 1 Byte to LSB
 * Input: 1 character of data, 8-byte array from the image
 * Output: success or failure
 * Description: Embeds 8 bits of a character into the LSBs of 8 image bytes.
 */
Status encode_1byte_to_lsb(char data, char *buffer_8)
{
    // Extract one bit at a time from the data and encode it into the LSB of the image data
    int bit;
    for(int i = 0; i < 8; i++)
    {
        bit = (data >> (7 - i)) & 1;
        buffer_8[i] = (buffer_8[i] & ~1);
        buffer_8[i] = (buffer_8[i] | bit);
    }
    return success;
}

/* 
 * Encode 4 Bytes to LSB
 * Input: 4-byte integer data, 32-byte array from the image
 * Output: success or failure
 * Description: Embeds 32 bits of an integer into the LSBs of 32 image bytes.
 */
Status encode_4byte_to_lsb(int data, char *buffer_32)
{
   // Encoding 4 bytes (32 bits) of data into the LSB of the image data
   int bit;
   for(int i = 0; i < 32; i++)
   {
        bit = ((data >> (31 - i)) & 1);
        buffer_32[i] = (buffer_32[i] & ~1);
        buffer_32[i] = (buffer_32[i] | bit);
   }
   return success;
}

/* 
 * Close Opened Files
 * Input: EncodeInfo pointer
 * Output: success or failure
 * Description: Closes all the file pointers safely to prevent memory leaks.
 */
Status close_opened_encode_files(EncodeInfo *encInfo)
{
    printf("Closing All Opened Files...\n");
    fclose(encInfo -> src_image_fptr);
    fclose(encInfo -> secret_fptr);
    fclose(encInfo -> output_image_fptr);

    return success;
}