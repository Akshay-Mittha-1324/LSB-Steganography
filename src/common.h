/* ================================================================
 *  NAME        : Akshay Mittha
 *  REG NO.     : 25048_081
 *  DATE        : 22 / 05 / 2026
 *  FILE        : common.h
 *  PROJECT     : LSB Steganography in C
 *  DESCRIPTION : Shared utility file containing enums, constants,
 *                and macros used by both encoding and decoding.
 * ================================================================ 
 */
#ifndef COMMON_H
#define COMMON_H

/* Magic string to identify whether stegged or not */
#define MAGIC_STRING "#*"

/* Status will be used in fn. return type */
typedef enum
{
    failure,
    success
} Status;

typedef enum
{
    unsupported,
    encode,
    decode
} Opr_type;

#endif