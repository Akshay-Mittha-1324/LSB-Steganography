/* ================================================================
 *  NAME        : Akshay Mittha
 *  REG NO.     : 25048_081
 *  DATE        : 22 / 05 / 2026
 *  FILE        : main.c
 *  PROJECT     : LSB Steganography in C
 *  DESCRIPTION : Main entry point for the application. Handles
 *                command-line argument parsing and coordinates
 *                the encoding or decoding operations.
 *
 *  FUNCTION INDEX:
 *    check_operation()    — checks user args for -e or -d flags
 *    main()               — drives the program execution
 * ================================================================ 
 */

#include <stdio.h>
#include "common.h"
#include "encode.h"
#include "decode.h"

int main(int argc, char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;

    if(argc <= 1)
    {
        printf("Error : The Number Of Arguments Are Less\nPlease enter for encoding in the format : ./a.out -e img_file.bmp secretfile\nPlease enter for decoding in the format : ./a.out -d img_file.bmp secretfile\n");
        return failure;
    }

    int opr = check_operation(argv);

    if( opr == encode )
    {
        if( argc < 4 || argc > 5 )
	    {
            printf("Error : The Number Of Arguments Are Less\nPlease enter in the format ./a.out -e img_file.bmp secretfile\n");
	        return failure;
	    }

        int status = do_encoding(argv, &encInfo);
        if(status == failure)
        {
            printf("Error : Encoding Failed\nPlease enter in the format ./a.out -e img_file.bmp secretfile\n");
            return failure;
        }
        printf("Success : Encoding Completed\n");
        return success;
    }

    else if( opr == decode )
    {
	    if( argc < 3 || argc > 4 )	
	    {
            printf("Error : The Number Of Arguments Are Less\nPlease enter in the format ./a.out -d img_file.bmp secretfile\n");
            return failure;
	    }

        int status = do_decoding(argv, &decInfo);

        if(status == failure)
        {
            printf("Error : Decoding Failed\nPlease enter in the format ./a.out -d img_file.bmp secretfile\n");
            return failure;
        }

        printf("Success : Decoding Completed\n");
        return success;
    }

    else
    {
        printf("The File is Unsupported\n");
        return failure;
    }
    return success;
}