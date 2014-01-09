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
 * 2000-03-17:
 * Mongoose: Created, I combined some endian checking with fread wrapper
 ==========================================================================*/

#include <endian.h>


int fread_u_int_small(unsigned int *ptr, FILE *stream)
{
#ifdef BIG_ENDIAN
   unsigned char b1, b2, b3, b4;
   unsigned int l;
#endif

   if (fread(ptr, 4, 1, stream) != 1)
   {
      printf("FreadSmallInt> Failed fread.\n");
      return false;
   }

#ifdef BIG_ENDIAN
   l = *ptr;

   b1 = l & 255;
   b2 = ( l >> 8 ) & 255;
   b3 = ( l >> 16 ) & 255;
   b4 = ( l >> 24 ) & 255;
  *ptr = ( (int) b1 << 24 ) + ( (int) b2 << 16 ) + ( (int) b3 << 8 ) + b4;
#endif

   return true;
}


int fread_int_small(int *ptr, FILE *stream)
{
#ifdef BIG_ENDIAN
   unsigned char b1, b2, b3, b4;
   int l;
#endif

   if (fread(ptr, 4, 1, stream) != 1)
   {
      printf("FreadSmallInt> Failed fread.\n");
      return false;
   }

#ifdef BIG_ENDIAN
   l = *ptr;

   b1 = l & 255;
   b2 = ( l >> 8 ) & 255;
   b3 = ( l >> 16 ) & 255;
   b4 = ( l >> 24 ) & 255;
  *ptr = ( (int) b1 << 24 ) + ( (int) b2 << 16 ) + ( (int) b3 << 8 ) + b4;
#endif

   return true;
}

int fread_float_small(float *ptr, FILE *stream)
{
#ifdef BIG_ENDIAN
   union
   {
      unsigned char b[4];
      float f;
   } in, out;

   float l;
#endif

   if (fread(ptr, 4, 1, stream) != 1)
   {
      printf("FreadSmallFloat> Failed fread.\n");
      return false;
   }

#ifdef BIG_ENDIAN
   l = *ptr;

   in.f = l;
   out.b[0] = in.b[3];
   out.b[1] = in.b[2];
   out.b[2] = in.b[1];
   out.b[3] = in.b[0];

   *ptr = out.f;
#endif

   return true;
}


#ifdef NEW_API_PLANS

/* FIXME: Add Big Endian support later */
void fread_u8int_small(unsigned char *u, FILE *f)
{
	u_int8_t n;

	fread(&n, 1, 1, f);
	*u = n;
}

void fread_u16int_small(unsigned short *u, FILE *f)
{
	u_int16_t n;

	fread(&n, 2, 1, f);
	*u = n;
}

void fread_u32int_small(unsigned int *u, FILE *f)
{
	u_int32_t n;

	fread(&n, 4, 1, f);
	*u = n;
}
#endif
