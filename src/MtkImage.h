/*================================================================
 * 
 * Project : Freyja
 * Author  : Mongoose
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : MtkImage
 * License : GPL See file COPYING, also (C) 2000 Mongoose
 * Comments: This is the mtkImage agent/class for mtk
 *
 *
 *           This file was generated using Mongoose's C++ 
 *           template generator script.  <stu7440@westga.edu>
 * 
 *-- History ------------------------------------------------ 
 *
 * 2000-10-05:
 * Mongoose - Created
 ================================================================*/


#ifndef __MTK_MONGOOSE_MTKIMAGE_H_
#define __MTK_MONGOOSE_MTKIMAGE_H_

#include <stdio.h>

typedef enum mtk_colormode
{
  COLORMODE_INDEXED = 1, 
  COLORMODE_RGB     = 2, 
  COLORMODE_RGBA    = 3
};

int mtkExtCheck(char *filename, char *ext);

class MtkImage
{
 public:

  MtkImage();
  /*--------------------------------------------
   * Created  : 2000-10-05 by Mongoose
   * Modified : 
   *
   * Pre  : None
   * Post : MtkImage agent is constructed
   --------------------------------------------*/

  ~MtkImage();
  /*--------------------------------------------
   * Created  : 2000-10-05 by Mongoose
   * Modified : 
   *
   * Pre  : MtkImage agent is allocated
   * Post : MtkImage agent is deconstructed
   --------------------------------------------*/

   void Clear();
  /*--------------------------------------------
   * Created  : 2000-10-05 by Mongoose
   * Modified : 
   *
   * Pre  : None
   * Post : MtkImage agent's data is reset to 
   *        initial state
   --------------------------------------------*/

   int Width();
  /*--------------------------------------------
   * Created  : 2000-10-05 by Mongoose
   * Modified : 
   *
   * Pre  : None
   * Post : Returns image width
   --------------------------------------------*/

   int Height();
  /*--------------------------------------------
   * Created  : 2000-10-05 by Mongoose
   * Modified : 
   *
   * Pre  : None
   * Post : Returns image height
   --------------------------------------------*/

   int OriginalWidth();
  /*--------------------------------------------
   * Created  : 2000-10-05 by Mongoose
   * Modified : 
   *
   * Pre  : None
   * Post : Returns image width before transforms
   --------------------------------------------*/

   int OriginalHeight();
  /*--------------------------------------------
   * Created  : 2000-10-05 by Mongoose
   * Modified : 
   *
   * Pre  : None
   * Post : Returns image height before transforms
   --------------------------------------------*/

   void Mode(mtk_colormode mode);
  /*--------------------------------------------
   * Created  : 2000-10-05 by Mongoose
   * Modified : 
   *
   * Pre  : mode is an mtk_colormode
   * 
   *        NOTE: You can't set to an indexed mode
   *        since they are computed per palette
   * 
   * Post : Converts image into a new color mode
   *        if not in that mode already
   --------------------------------------------*/

   void Image(unsigned char **image);
  /*--------------------------------------------
   * Created  : 2000-10-05 by Mongoose
   * Modified : 
   *
   * Pre  : image must be an unallocated pointer
   *
   * Post : Returns a copy of the image buffer
   *        in mode, or NULL if unable
   --------------------------------------------*/

   void IndexedImage(unsigned char **image);
  /*--------------------------------------------
   * Created  : 2000-10-05 by Mongoose
   * Modified : 
   *
   * Pre  : image must be an unallocated pointer
   *        A palette must have been loaded
   *
   * Post : Returns a copy of a computed
   *        indexed image
   --------------------------------------------*/

   int Load(unsigned char *image, unsigned int width, unsigned int height, 
	    mtk_colormode mode);
  /*--------------------------------------------
   * Created  : 2000-10-05 by Mongoose
   * Modified : 
   *
   * Pre  : image must be valid
   *        width and height must be valid
   *        mode must be valid
   *
   *        1. Palettes must be loaded to make
   *           a computed INDEXED mode image
   *
   * Post : Loads image of type mode
   --------------------------------------------*/

   void Palette(unsigned char **buffer);
  /*--------------------------------------------
   * Created  : 2000-10-05 by Mongoose
   * Modified : 
   *
   * Pre  : buffer must be an unallocated pointer
   * Post : Returns a copy of the palette buffer
   --------------------------------------------*/

   int PaletteMatchColor(float rgb[3]);
  /*--------------------------------------------
   * Created  : 2000-10-05 by Mongoose
   * Modified : 
   *
   * Pre  : rgb is a color as defined as an
   *        array of Red Green and Blue with
   *        colors ranged 0.0 to 1.0
   *
   * Post : Returns closest matched color index
   *        it can find in a loaded palette
   *
   *        NOTE: tends to lean to darker shades
   --------------------------------------------*/

   //////////////////////////////////////////////
   int Load(char *filename);
   int Save(char *filename, char *format);

   int PaletteLoad(unsigned char *filename, int bpp);
   int PaletteSave(char *filename);
   int PaletteLoad(char *filename);
   int PaletteLoadLMP(char *filename);
   //////////////////////////////////////////////

   void Scale();
   void PaletteBrighten(float p);
   void PaletteGamma(float b);
   void PaletteColor(int i, float rgb[3]);
   void FlipVertical();
   mtk_colormode Mode();

 private:

   int LoadIndexed(unsigned char *image, int width, int height);
  /*--------------------------------------------
   * Created  : 2000-10-05 by Mongoose
   * Modified : 
   *
   * Pre  : image must be valid indexed image
   *        width must be valid
   *        height must be valid
   *        pallette must have been previously
   *        loaded
   *
   * Post : Loads indexed image using previously
   *        loaded pallette
   --------------------------------------------*/

   int NextPower(int seed);
  /*--------------------------------------------
   * Created  : 2000-10-05 by Mongoose
   * Modified : 
   *
   * Pre  : seed is valid ( last power )
   * Post : Returns next power of 2
   --------------------------------------------*/

   unsigned char *_image;                 /* Image buffer */

   unsigned char *_palette;               /* Image palette */

   int _width;                            /* Width of image */

   int _height;                           /* Height of image */

   int _original_width;                   /* Width before transforms */

   int _original_height;                  /* Height before transforms */

   enum mtk_colormode _color_mode;        /* Color mode of image */

   int _image_bpp;                        /* Color depth of image */ 

   int _palette_bpp;                      /* Color depth of palette */
};

#endif
