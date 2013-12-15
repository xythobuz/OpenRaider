/*================================================================
 * 
 * Project : Freyja
 * Author  : Mongoose
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : MtkImage
 * License : GPL See file COPYING, also (C) 2000 Mongoose
 * Comments: This is the new mtkImage class for mtk
 *
 *
 *           This file was generated using Mongoose's C++ 
 *           template generator script.  <stu7440@westga.edu>
 * 
 *-- History ------------------------------------------------- 
 *
 * 2000-10-05:
 * Mongoose - Created
 =================================================================*/

#include "MtkImage.h"

#ifdef DEBUG_MEMEORY
#   include "memeory_test.h"
#endif


#include <dlfcn.h>
static const char *mtk_image_module_names[] = {
  "pcx",
  "ppm",
  "tga",
  "jpeg",
  "png"
};

#define MTK_IMAGE_MODULE_NAME_COUNT (sizeof(mtk_image_module_names) / sizeof(mtk_image_module_names[0]))


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int mtkExtCheck(char *filename, char *ext)
{
  unsigned int len_f, len_e;
 
 
  len_f = strlen(filename) - 1;
  len_e = strlen(ext) - 1;

  while (len_f+1 && len_e+1)
  {
    //printf("extcheck: %c %c\n", filename[len_f], ext[len_e]);

    if (filename[len_f] == ext[len_e])
    {
      if (ext[len_e] == '.')
      {
	//printf("extcheck: match\n");
	return 0;
      }
    }
    else if (filename[len_f] < ext[len_e])
      return -1;
    else if (filename[len_f] > ext[len_e])
      return 1;

    len_f--;
    len_e--;
  }

  return 1;
}

//////////////////////////////////////////////

MtkImage::MtkImage()
{
  _image = NULL;
  _palette = NULL;
  _width = 0;             
  _height = 0;            
  _original_width = 0;    
  _original_height = 0;   
  _color_mode = COLORMODE_RGB;
  _image_bpp = 0;
  _palette_bpp = 0;
}


MtkImage::~MtkImage()
{
  Clear();
}


void MtkImage::Clear()
{
   if (_image)
      delete [] _image;

   if (_palette)
      delete [] _palette;

   _width = 0;
   _height = 0;  
   _original_width = 0;
   _original_height = 0;
   _palette_bpp = 0;
}


int MtkImage::Width()
{
  return _width;
}


int MtkImage::Height()
{
  return _height;
}


int MtkImage::OriginalWidth()
{
  return _original_width;
}


int MtkImage::OriginalHeight()
{
  return _original_height;
}


void MtkImage::Mode(mtk_colormode mode)
{
  fprintf(stderr, "MtkImage::Mode> ERROR not implemented, %s:%i\n",
	  __FILE__, __LINE__);
}


//FIXME: Add support for 32bit!
int MtkImage::PaletteLoad(unsigned char *buffer, int bpp)
{
  if (buffer && buffer != _palette && 
      (bpp == 8 || bpp == 16 || bpp == 24))
  {
    if (_palette)
      delete [] _palette;

    _palette_bpp = bpp;

    _palette = new unsigned char[768];
    memcpy(_palette, buffer, 768);
      
    return 0;
  }

  printf("MtkImage::LoadPaletteBuffer()> Assertion Error bpp not {8,16,24}\n");
  return -1;
}


int MtkImage::LoadIndexed(unsigned char *image, int width, int height)
{
  int i, size, index;


  if (!image || !_palette || !width || !height || image == _image)
  {
    printf("MtkImage::LoadIndexedBuffer> Assertion failed\n");
    return -1;
  }

  if (_image)
    delete [] _image;

  size = width * height;

  _width = _original_width = width;
  _height = _original_height = height;
  _image = new unsigned char[size * 3];

  _color_mode = COLORMODE_RGB;

  for (i = 0; i < size; i++)
  {
    index = image[i];

    if (index * 3 + 2 > 768)
      printf("Index outside of palette!\n");
    else
    {
      _image[i*3] = _palette[index*3];
      _image[i*3+1] = _palette[index*3+1];
      _image[i*3+2] = _palette[index*3+2];
    }
  }

  return 0;
}

void MtkImage::PaletteBrighten(float p)
{
  int i;


  if (p <= 0.0 || p > 0.999)
  {
    printf("MtkImage::BrightenPalette> %f isn't a postive percentage\n", p);
    return;
  }

  if (_palette)
  {
    for (i = 0; i < 255; i++)
    {
      _palette[i*3] += (unsigned char)(_palette[i*3] * p);
      _palette[i*3+1] += (unsigned char)(_palette[i*3+1] * p);
      _palette[i*3+2] += (unsigned char)(_palette[i*3+2] * p);
    }    
  }
}


void MtkImage::Image(unsigned char **buffer)
{
  *buffer = NULL;

  if (!_image)
  {
    printf("Image::ImageBuffer> No image allocated\n");
    return;
  }

  switch (_color_mode)
  {
  case COLORMODE_RGB:
    *buffer = new unsigned char[_width*_height*3];
    memcpy(*buffer, _image, _width*_height*3);
    break;
  case COLORMODE_RGBA:
    *buffer = new unsigned char[_width*_height*4];
    memcpy(*buffer, _image, _width*_height*4);
    break;
  case COLORMODE_INDEXED:
    break;
  }
}


void MtkImage::Palette(unsigned char **buffer)
{
  *buffer = NULL;

  if (!_palette)
  {
    printf("Image::PaletteBuffer> No palette allocated\n");
    return;
  }

  *buffer = new unsigned char[768];
  memcpy(*buffer, _palette, 768);
}


void MtkImage::IndexedImage(unsigned char **buffer)
{
  int size = _width * _height;
  float rgb[3];
  int i;


  *buffer = NULL;

  if (!_image || !_palette)
  {
    printf("MtkImage::IndexedImage> No image or palette allocated\n");
    return;
  }

  *buffer = new unsigned char[size];

  for (i = 0; i < size; i++)
  {
    rgb[0] = _image[i*3]/255.0;
    rgb[1] = _image[i*3+1]/255.0;
    rgb[2] = _image[i*3+2]/255.0;

    (*buffer)[i] = PaletteMatchColor(rgb);
  }
}

void MtkImage::FlipVertical()
{
  int i, j;
  unsigned char *swap_row = NULL;


  if (!_image)
    return;

  switch(_color_mode)
  {
  case COLORMODE_INDEXED:
    // We don't store images as indexed, they're computed
    break;
  case COLORMODE_RGB:
    swap_row = new unsigned char[_width*3];

    for (i = 0, j = _height-1; i < _height/2; i++, j--)
    {
      memcpy(swap_row, &_image[i*_width*3], _width*3);
      memcpy(&_image[i*_width*3], &_image[j*_width*3], _width*3);
      memcpy(&_image[j*_width*3], swap_row, _width*3);
    }

    delete [] swap_row;
    break;
  case COLORMODE_RGBA:
    swap_row = new unsigned char[_width*4];

    for (i = 0, j = _height-1; i < _height/2; i++, j--)
    {
      memcpy(swap_row, &_image[i*_width*4], _width*4);
      memcpy(&_image[i*_width*4], &_image[j*_width*4], _width*4);
      memcpy(&_image[j*_width*4], swap_row, _width*4);
    }

    delete [] swap_row;
    break;
  }
}

// FIXME: static only for now
int MtkImage::Load(char *filename)
{
  int (*load)(char *filename, unsigned char **image, 
	      unsigned int *width, unsigned int *height, char *type);
  int (*check)(char *filename);
  void *handle;
  char *error;
  FILE *f;
  unsigned int i;
  char module_filename[128];
  char module_check[128];
  char module_load[128];
  char *name;
  unsigned char *image;
  unsigned int w, h;
  bool loaded = false;
  char type;


  f = fopen(filename, "rb");

  if (!f)
  {
    perror("MtkImage::Load> File open failed.\n");
    return -1;
  }

  printf("[MtkImage module loader invoked]\n");

  for (i = 0; i < MTK_IMAGE_MODULE_NAME_COUNT && !loaded; i++)
  {
    name = (char *)mtk_image_module_names[i];

    snprintf(module_filename, 128, "libmtk_loader-%s.so", name);
    snprintf(module_load, 128, "mtk_image__%s_load", name);
    snprintf(module_check, 128, "mtk_image__%s_check", name);

    if (!(handle = dlopen (module_filename, RTLD_NOW))) //RTLD_LAZY)))
    {
      fprintf(stderr, "\tERROR: Couldn't find module '%s'.\n", module_filename);
      return -1;
    }
    else
    {
      printf("\tModule '%s' opened.\n", module_filename);
    
      check = (int (*)(char *filename))dlsym(handle, module_check);
      
      load = (int (*)(char * filename, unsigned char **image, 
		      unsigned int *w, unsigned int *h, 
		      char *type))dlsym(handle, module_load);

      if ((error = dlerror()) != NULL)  
      {
	fprintf (stderr, "\tERROR: %s\n", error);
	return -5;
      }

      loaded = (loaded || (!(*check)(filename) && 
			   !(*load)(filename, &image, &w, &h, &type)));

      if ((error = dlerror()) != NULL) 
	return -10;
     
      dlclose(handle);
    }
  }


  if (loaded)
  {
    Clear();

    _image = image;
    _original_width = _width = w;
    _original_height = _height = h;

    switch (type)
    {
    case 4:
    case 2:
      _color_mode = COLORMODE_RGBA;
      break;
    default:
      _color_mode = COLORMODE_RGB;
    }
    return 0;
  }
  else
    return -2;
}

int MtkImage::Save(char *filename, char *format)
{
  extern int mtk_image__tga_save(FILE *f, unsigned char *image,
				 unsigned int width, unsigned int height, 
				 char bpp);
  FILE *f;
  int ret;
  

  f = fopen(filename, "wb");


  ret = mtk_image__tga_save(f, _image, _width, _height, _color_mode+1);

  if (f)
    fclose(f);

  return ret;
}


int MtkImage::Load(unsigned char *image, unsigned int w, unsigned int h,
	 mtk_colormode mode)
{
  switch (mode)
  {
  case COLORMODE_INDEXED:
    return LoadIndexed(image, w, h);
    break;
  case COLORMODE_RGB:
    if (!image || !w || !h || image == _image)
      return -1;  

    _width = _original_width = w;
    _height = _original_height = h;

    if (_image)
      delete [] _image;

    _color_mode = COLORMODE_RGB;

    _image = new unsigned char[w * h * 3];
    memcpy(_image, image, w * h * 3);

    return 0;
    break;
  case COLORMODE_RGBA:
    if (!image || !w || !h || image == _image)
      return -1;  

    _width = _original_width = w;
    _height = _original_height = h;

    if (_image)
      delete [] _image;

    _color_mode = COLORMODE_RGBA;

    _image = new unsigned char[w * h * 4];
    memcpy(_image, image, w * h * 4);

    return 0;
    break;
  }

  return -1;
}


// Palette ////////////////////////////////////////////

// Search palette for matching ( or closest ) rgb color
// Colors *must be 0.0 to 1.0 form!
//
// Note if you have a small palette this may 'wash' to black
// quickly after running through a few times ( 0.0 is default )
int MtkImage::PaletteMatchColor(float rgb[3])
{
  float color[3];
  float best_weight, current_weight;
  int i, best;


  if (!_palette)
  {
    printf("MtkImage::MatchColor> ERROR no palette loaded\n");
    return -1;
  }

  best_weight = current_weight = 0.0;
  
  for (best = 0, i = 0; i < 256; i++)
  {
    // Convert from 0-255 to 0.0-1.0 color scale
    PaletteColor(i, color);
    //color[0] = _palette[i*3]/255.0;
    //color[1] = _palette[i*3+1]/255.0;
    //color[2] = _palette[i*3+2]/255.0;

    // FIXME: Should be 0.0 (far off) to 1.0 (exact match)
    current_weight = 1.0 - ((fabs(rgb[0] - color[0]) + 
			     fabs(rgb[1] - color[1]) + 
			     fabs(rgb[2] - color[2])) / 3.0);

    if (current_weight > best_weight)
    {
      best_weight = current_weight;
      best = i;

      if (best_weight == 1.0)
	i = 1024; // break;
    }
  }

  return best;
}

void MtkImage::PaletteColor(int i, float rgb[3])
{
  if (i > 0 && i < 256 && rgb && _palette)
  {
    rgb[0] = (float)_palette[i*3]/255.0;
    rgb[1] = (float)_palette[i*3+1]/255.0;
    rgb[2] = (float)_palette[i*3+2]/255.0;
  }
}	


int MtkImage::PaletteLoadLMP(char *filename)
{
  FILE *f;
	

  f = fopen(filename, "rb");

  if (f)
  {
    if (!_palette)
      _palette = new unsigned char[768];

    fread(_palette, 1, 768, f);
    fclose(f);
    return 0;
  }

  return -1;
}

int MtkImage::PaletteLoad(char *filename)
{
  FILE *f;
  int i;
  unsigned int rgb[3];


  if (!filename)
    return -1;

  f = fopen(filename, "r");

  if (f)
  {
    if (_palette)
      delete [] _palette;

    _palette = new unsigned char[768];

    for (i = 0; i < 768; i+=3)
    {
      fscanf(f, "%u %u %u\n", 
	     &rgb[0],
	     &rgb[1],
	     &rgb[2]);

      _palette[i]   = rgb[0];
      _palette[i+1] = rgb[1];
      _palette[i+2] = rgb[2];
    }
    
    fclose(f);
    return 0;
  }

  return 1;
}

int MtkImage::PaletteSave(char *filename)
{
  FILE *f;
  int i;


  if (!_palette || !filename)
    return -1;

  f = fopen(filename, "w");

  if (f)
  {
    for (i = 0; i < 768; i+=3)
    {
      fprintf(f, "%i %i %i\n", 
	      (int)_palette[i], 
	      (int)_palette[i+1], 
	      (int)_palette[i+2]);
    }
    
    fclose(f);
    return 0;
  }

  return 1;
}

mtk_colormode MtkImage::Mode()
{
  return _color_mode;
}


/* This code based off on gluScaleImage()  */
void MtkImage::Scale()
{
   int i, j, k;
   float* tempin;
   float* tempout;
   float sx, sy;
   int components = 3;
   unsigned char *image;


   if (!_image || !_width || !_height)
     return;

   _height = NextPower(_height);
   _width = NextPower(_width);

   if (_height > 256) 
     _height = 256;
    
   if (_width > 256) 
     _width = 256;
    

   // Check to see if scaling is needed
   if (_height == _original_height && _width == _original_width) 
     return;

   image = new unsigned char[_height * _width * 3];

   tempin = new float[_original_width * _original_height * 
		      components * sizeof(float)];
   tempout = new float[_width * _height * components * sizeof(float)];

   if (!tempout || !tempin)
   {
     if (tempout)
       delete [] tempout;

     if (tempin)
       delete [] tempin;

     printf("Oh shit out of memory!\n");
     return;
   }

   // Copy user data to float format.
   for (i = 0; i < _original_height * _original_width * components; i++) 
   {
     tempin[i] = (float)_image[i];
   }

   // Determine which filter to use by checking ratios.
   if (_width > 1) 
   {
     sx = (float)(_original_width - 1) / (float)(_width - 1);
   } 
   else 
   {
     sx = (float)(_original_width - 1);
   }

   if (_height > 1)
   {
     sy = (float)(_original_height - 1) / (float) (_height - 1);
   } 
   else 
   {
     sy = (float)(_original_height - 1);
   }

   if (sx < 1.0 && sy < 1.0) 
   {
     /* Magnify both width and height:  use weighted sample of 4 pixels */
     int i0, i1, j0, j1;
     float alpha, beta;
     float* src00;
     float* src01;
     float* src10;
     float* src11;
     float s1, s2;
     float* dst;

     for(i = 0; i < _height; i++) 
     {
       i0 = (int)(i * sy);
       i1 = i0 + 1;

       if (i1 >= _original_height) 
       {
	 i1 = _original_height - 1;
       }

       alpha = i * sy - i0;

       for (j = 0; j < _width; j++) 
       {
	 j0 = (int) (j * sx);
	 j1 = j0 + 1;

	 if (j1 >= _original_width) 
	 {
	   j1 = _original_width - 1;
	 }

	 beta = j * sx - j0;

	 /* Compute weighted average of pixels in rect (i0,j0)-(i1,j1) */
	 src00 = tempin + (i0 * _original_width + j0) * components;
	 src01 = tempin + (i0 * _original_width + j1) * components;
	 src10 = tempin + (i1 * _original_width + j0) * components;
	 src11 = tempin + (i1 * _original_width + j1) * components;

	 dst = tempout + (i * _width + j) * components;

	 for (k = 0; k < components; k++) 
	 {
	   s1 = *src00++ * (1.0 - beta) + *src01++ * beta;
	   s2 = *src10++ * (1.0 - beta) + *src11++ * beta;
	   *dst++ = s1 * (1.0 - alpha) + s2 * alpha;
	 } 
       }
     }     
   }
   else 
   {
     /* Shrink width and/or height:  use an unweighted box filter */
     int i0, i1;
     int j0, j1;
     int ii, jj;
     float sum;
     float* dst;

     for (i = 0; i < _height; i++) 
     {
       i0 = (int) (i * sy);
       i1 = i0 + 1;

       if (i1 >= _original_height) 
       {
	 i1 = _original_height - 1;
       }

       for (j = 0; j < _width; j++) 
       {
	 j0 = (int) (j * sx);
	 j1 = j0 + 1;

	 if (j1 >= _original_width) 
	 {
	   j1 = _original_width - 1;
	 }

	 dst = tempout + (i * _width + j) * components;

	 /* Compute average of pixels in the rectangle (i0,j0)-(i1,j1) */
	 for (k = 0; k < components; k++) 
	 {
	   sum = 0.0;

	   for (ii = i0; ii <= i1; ii++) 
	   {
	     for (jj = j0; jj <= j1; jj++) 
	     {
	       sum += *(tempin + (ii * _original_width + jj) 
			* components + k);
	     }
	   }

	   sum /= ( j1 - j0 + 1 ) * ( i1 - i0 + 1 );
	   *dst++ = sum;
	 }
       } 
     }
   }

   // Copy to our results.
   for( i = 0; i < _height * _width * components; i++ ) 
   {
     image[i] = (unsigned char) tempout[i];
   }

   // Delete our temp buffers.
   delete[] tempin;
   delete[] tempout;
   delete[] _image;

   _image = image;
}

int MtkImage::NextPower(int seed)
{
  int i;

  for(i = 1; i < seed; i *= 2) 
    ;

  return i;
}



