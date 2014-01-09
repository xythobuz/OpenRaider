/*================================================================
 *
 * Project : Freyja
 * Author  : Mongoose
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  :
 * License : GPL See file COPYING, also (C) 2000 Mongoose
 * Comments: TGA plug-in
 *
 *           TODO: type should pass more info
 *                 2 bits for RGBA | RGB | GREY
 *                 val for depth
 *
 *           This file was generated using Mongoose's C++
 *           template generator script.  <stu7440@westga.edu>
 *
 *-- History ------------------------------------------------
 *
 * 2001-10-25:
 * Mongoose - support for screen origin bit
 *
 * 2000-10-15:
 * Mongoose - Created
 ================================================================*/

#include <string.h>
#include <stdarg.h>

#include <mtk_tga.h>

int main(int argc, char *argv[])
{
  FILE *f;
  unsigned char *image;
  unsigned int width;
  unsigned int height;
  char type;

  if (argc > 1)
  {
    f = fopen(argv[1], "r");

    if (!f)
    {
      perror("Failed to open file> ");
      return -1;
    }

    if (!mtk_image__tga_check(f))
    {
      if (!mtk_image__tga_load(f, &image, &width, &height, &type))
      {
        printf("Loaded %s successfully!\n", argv[1]);
        delete [] image;
      }
    }

    fclose(f);
  } else {
      printf("Usage: %s testfile.tga\n", argv[0]);
  }

  return 0;
}

