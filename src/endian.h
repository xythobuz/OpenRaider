/*==========================================================================
 * 
 * Project : MDDC 
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440
 * Email   : stu7440@westga.edu
 * Object  : 
 * Comments: This is the endian utils.
 *
 *           See file COPYING for license details.
 *
 *           Based on type converting code by: 
 *           Michael Vance <mkv102@psu.edu> 
 * 
 *-- History ---------------------------------------------------------- 
 *
 * See the source file's header.
 ==========================================================================*/

#ifndef __MONGOOSE_ENDIAN_H
#define __MONGOOSE_ENDIAN_H

#include <stdio.h>


// Disable name mangling
extern "C"
{
  int fread_float_small(float *ptr, FILE *stream);
  int fread_int_small(int *ptr, FILE *stream);
  int fread_u_int_small(unsigned int *ptr, FILE *stream);
}

#endif
