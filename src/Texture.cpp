/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*==========================================================================
 *
 * Project : MTK, Freyja, OpenRaider
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440
 * Email   : stu7440@westga.edu
 * Object  : Texture
 * Comments: This is the Texture class.
 *
 *           See file COPYING for license details.
 *
 *
 *-- History ----------------------------------------------------------
 *
 * 2003.06.30,
 * Mongoose - API update, SDL_TTF support moved here, misc features
 *            SDL_TTF support based on Sam Lantinga's public domain
 *            SDL_TTF demo functions and algorithms
 *
 * 2001.05.29:
 * Mongoose - Removed legacy code and done clean up
 *
 * 2001.02.19:
 * Mongoose - Moved from GooseEgg to mtk
 *
 * 2000.04.29:
 * Mongoose - Created from old PPM/PCX codebases I have done before
 ==========================================================================*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef DEBUG_MEMEORY
#   include "memeory_test.h"
#endif

#ifdef USING_MTK_TGA
#   include "mtk_tga.h"
#endif

#ifdef USING_MTK_PNG
#   include "mtk_png.h"
#endif

#ifdef HAVE_SDL_TTF
#   include <SDL/SDL_ttf.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "Texture.h"

//Texture *gTextureManager = 0x0;
gl_font_t *gFontTest = 0x0;


////////////////////////////////////////////////////////////
// Constructors
////////////////////////////////////////////////////////////

Texture::Texture()
{
	mTextureIds = NULL;

	mFlags = 0;
	mTextureId = -1;
	mTextureId2 = -1;
	mTextureCount = 0;
	mTextureLimit = 0;

	//gTextureManager = this;

	initSDL_TTF();
}


Texture::~Texture()
{
	if (gFontTest)
	{
		glDeleteLists(gFontTest->drawListBase, gFontTest->count);
		delete gFontTest;
	}

	reset();
}


////////////////////////////////////////////////////////////
// Public Accessors
////////////////////////////////////////////////////////////

unsigned char *Texture::generateColorTexture(unsigned char rgba[4],
															unsigned int width,
															unsigned int height)
{
	unsigned char *image;
	unsigned int i, size;


	image = new unsigned char[height*width*4];

	for (i = 0, size = width*height; i < size; ++i)
	{
		/* RBGA */
		image[i*4]   = rgba[0];
		image[i*4+1] = rgba[1];
		image[i*4+2] = rgba[2];
		image[i*4+3] = rgba[3];
	}

	return image;
}


void glEnterMode2d(unsigned int width, unsigned int height)
{
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);

	/* This allows alpha blending of 2D textures with the scene */
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glOrtho(0.0, (GLdouble)width, (GLdouble)height, 0.0, 0.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}


void glExitMode2d()
{
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glPopAttrib();

	glMatrixMode(GL_MODELVIEW);
}


void bufferedPrintf(char *string, unsigned int len, char *s, ...)
{
	va_list args;


	if (s && s[0])
	{
		va_start(args, s);
		vsnprintf(string, len-1, s, args);
		string[len-1] = 0;
		va_end(args);
	}
}


void glPrint2d(float x, float y, float scale, char *string)
{
	gl_font_t *font = gFontTest;

	if (!font)
	{
		static int errors = 0;

		if (errors < 10)
			printf("ERROR: glPrintf2d failed, %i\n", ++errors);
		return;
	}

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, font->textureId);
	glTranslatef(x, y, 0);
	glScalef(scale, scale, 1);

	/* FIXME:
	 * Add utf-8 dencoding of char* string
	 *
	 *	Also this string must be preprocessed to have glyph offsets
	 * instead of ASCII text in it and support counts over 256 */
	glListBase(font->drawListBase - font->utf8Offset);
	glCallLists(strlen(string), GL_BYTE, string);
	glPopMatrix();
}


void glPrint3d(float x, float y, float z,
					float pitch, float yaw, float roll,
					float scale,
					char *string)
{
	gl_font_t *font = gFontTest;

	if (!font)
	{
		static int errors = 0;

		if (errors < 10)
			printf("ERROR: glPrintf3d failed, %i\n", ++errors);
		return;
	}

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, font->textureId);
	glTranslatef(x, y, z);
	glRotatef(roll,  1, 0, 0);
	glRotatef(yaw,   0, 1, 0);
	glRotatef(pitch, 0, 0, 1);
	glScalef(scale, scale, scale);

	/* FIXME:
	 * Add utf-8 dencoding of char* string
	 *
	 *	Also this string must be preprocessed to have glyph offsets
	 * instead of ASCII text in it and support counts over 256 */
	glListBase(font->drawListBase - font->utf8Offset);
	glCallLists(strlen(string), GL_BYTE, string);
	glPopMatrix();
}


int Texture::loadFontTTF(char *filename,
								 unsigned int utf8Offset, unsigned int count)
{
	ttf_texture_t *texture;
	unsigned char rgb[3] = {0xff, 0xff, 0xff};


	if (!filename || !filename[0])
	{
		printf("fontTest> Passed bad filename\n");
		return -1;
	}

	texture = generateFontTexture(filename, 24, 256, rgb,
											//0x303f, 0x3093-0x303f, // hiragana
											//32, 126 - 32,  // english
											utf8Offset, count,
											false);

	if (texture)
	{
#ifdef DUMP_TTF_TGA
		FILE *f = fopen("/tmp/font.tga", "wb");
		if (f)
		{
			mtk_image__tga_save(f, texture->texture, 256, 256, 4);
			fclose(f);
		}
		else
		{
			perror("/tmp/ttf_font.tga");
		}
#endif

		gFontTest = generateFont(texture);

		/* FIXME: Until UTF8 decoder is working, we map from
			ASCII when rendering */
		gFontTest->utf8Offset = 32; // hack to use ASCII strings to test unicode

		delete [] texture->texture;
		delete [] texture->glyphs;
		delete texture;
	}
	else
	{
		return -2;
	}

	return 0;
}


gl_font_t *Texture::generateFont(ttf_texture_t *texture)
{
#ifdef HAVE_SDL_TTF
	const float spacing = 4.0;
	unsigned int i;
	float u, v, u2, v2;
	int h;
	gl_font_t *font;


	if (!texture)
		return NULL;

	printf("@ Generating gl font from texture...\n");

	font = new gl_font_t;
	font->utf8Offset = texture->utf8Offset;
	font->count = texture->count;
	font->textureId = loadBuffer(texture->texture,
										  texture->width, texture->width, RGBA, 32);

	glColor3f(1.0, 1.0, 1.0);
	font->drawListBase = glGenLists(texture->count);
	glBindTexture(GL_TEXTURE_2D, font->textureId);

	for (i = 0; i < texture->count; i++)
	{
		/* Generate texture coordinates for this TTF glyph */
		u = (float)texture->glyphs[i].x / (float)texture->width;
		v = (float)texture->glyphs[i].y / (float)texture->width;
		u2 = (((float)texture->glyphs[i].x + (float)texture->glyphs[i].w) /
				(float)texture->width);
		v2 = (((float)texture->glyphs[i].y + (float)texture->glyphs[i].h) /
				(float)texture->width);

#ifdef DEBUG_TTF_OFFSET
		if (i+texture->utf8Offset == 'y' || i+texture->utf8Offset == 'x')
		{
			printf("%c: %i %i %i\n",
					 i+texture->utf8Offset,
					 texture->fontDescent,
					 texture->glyphs[i].miny, texture->glyphs[i].maxy);
		}
#endif

		if (texture->glyphs[i].h < texture->fontHeight)
		{
			h = texture->fontHeight - texture->glyphs[i].h;
		}
		else
		{
			h = 0;
		}

		h += -texture->fontHeight/2-(texture->fontDescent + texture->glyphs[i].miny);

		/* Make a list for this TTF glyph, one nonuniform Quad per glyph */
		glNewList(font->drawListBase + i, GL_COMPILE);

		glBegin(GL_QUADS);
		 glTexCoord2f(u2, v);	/* Top, right */
		 glVertex3i(texture->glyphs[i].w, h, 0);

		 glTexCoord2f(u, v);	/* Top, left */
		 glVertex3i(0, h, 0);

		 glTexCoord2f(u, v2);	/* Bottom, left */
		 glVertex3i(0, h+texture->glyphs[i].h, 0);

		 glTexCoord2f(u2, v2);	/* Bottom, right */
		 glVertex3i(texture->glyphs[i].w, h+texture->glyphs[i].h, 0);
		glEnd();

		/* Move To The Left Of The Character */
		glTranslated(texture->glyphs[i].w + spacing, 0, 0);
		glEndList();
	}

	return font;
#else
	return NULL;
#endif
}


ttf_texture_t *Texture::generateFontTexture(char *filename, int pointSize,
														  unsigned int textureWidth,
														  unsigned char color[3],
														  unsigned int utf8Offset,
														  unsigned int count,
														  char verbose)
{
#ifdef HAVE_SDL_TTF
	unsigned int i;
	int xx = 0, yy = 0, hh = 0, k, h, w, offset;
	unsigned char b;
	unsigned char *image;
	SDL_Surface* glyph;
	SDL_Color sdlColor;
	TTF_Font *font;
	ttf_texture_t *texture;
	FILE *f;


	sdlColor.r = color[0];
	sdlColor.g = color[1];
	sdlColor.b = color[2];

	if (!mFlags & fUseSDL_TTF)
	{
		printf("SDL_TTF couldn't be used... exiting\n");
		return NULL;
	}

	if (pointSize < 8)
	{
		pointSize = 8;
	}

	/* Work around for TTF_OpenFont for file not found segfault */
	f = fopen(filename, "rb");
	if (!f)
	{
		printf("generateFontTexture> Couldn't load '%s'\n", filename);
		perror(filename);
		return NULL;
	}
	fclose(f);

	// Open the font file at the requested point size
	font = TTF_OpenFont(filename, pointSize);

	if (font == NULL)
	{
		fprintf(stderr, "generateFontTexture> Couldn't load %d pt font from %s: %s\n",
				  pointSize, filename, SDL_GetError());
		return NULL;
	}

	// TTF_STYLE_NORMAL, TTF_STYLE_BOLD, TTF_STYLE_ITALIC, TTF_STYLE_UNDERLINE
	int renderStyle = TTF_STYLE_NORMAL;
	TTF_SetFontStyle(font, renderStyle);

	/* Allocate a new TTF font texture */
	printf("@ Creating font texture from '%s'...\n", filename);
	texture = new ttf_texture_t;
	texture->width = textureWidth;
	texture->utf8Offset = utf8Offset;
	texture->count = count;
	texture->glyphs = new ttf_glyph_t[count];
	texture->texture = new unsigned char[textureWidth*textureWidth*4];
	memset(texture->texture, 0, textureWidth*textureWidth*4);

	texture->fontHeight = TTF_FontHeight(font);
	texture->fontAscent = TTF_FontAscent(font);
	texture->fontDescent = TTF_FontDescent(font);
	texture->fontSpacing = TTF_FontLineSkip(font);

	for (i = 0; i < count; ++i)
	{
		glyph = TTF_RenderGlyph_Blended(font, i + utf8Offset, sdlColor);

		if (glyph)
		{
	      image = (unsigned char*)glyph->pixels;

			TTF_GlyphMetrics(font, i + utf8Offset,
								  &texture->glyphs[i].minx, &texture->glyphs[i].maxx,
								  &texture->glyphs[i].miny, &texture->glyphs[i].maxy,
								  &texture->glyphs[i].advance);

	      texture->glyphs[i].w = glyph->w;
	      texture->glyphs[i].h = glyph->h;

	      if (xx + texture->glyphs[i].w > ((int)textureWidth - 1))
	      {
				yy += hh;
				hh = 2;
				xx = 2;
				texture->glyphs[i].x = 0;
				texture->glyphs[i].y = yy;
	      }
	      else
	      {
				(texture->glyphs[i].h > hh) ? hh = texture->glyphs[i].h: 0;
				texture->glyphs[i].x = xx;
				texture->glyphs[i].y = yy;
	      }

	      xx += glyph->w;

			if (verbose)
			{
				printf("0x%x : %ix%i @ %i, %i\n", i + utf8Offset,
						 texture->glyphs[i].w, texture->glyphs[i].h,
						 texture->glyphs[i].x, texture->glyphs[i].y);
			}

	      /* Blit @ xx, yy - in pixels */
	      for (k = 0; k < glyph->w*glyph->h; ++k)
	      {
				w = texture->glyphs[i].x + k%glyph->w;
				h = texture->glyphs[i].y + k/glyph->w;
				offset = (w + h*textureWidth);

				if (verbose)
				{
					printf("Offset: %i; Pixel: %i,%i; Data: 0x%08x\n",
							 offset, w, h, *((unsigned int *)&image[k*4]));
				}

				/* 32-bit ARGB to RGBA */
				b = image[k*4+3];
				texture->texture[offset*4]   = image[k*4]   = image[k*4+1];
				texture->texture[offset*4+1] = image[k*4+1] = image[k*4+2];
				texture->texture[offset*4+2] = image[k*4+2] = image[k*4+3];
				texture->texture[offset*4+3] = image[k*4+3] = b;
	      }
		}
	}

	TTF_CloseFont(font);

	return texture;
#else
	printf("SDL_TTF not enabled in this build.\n");
	return NULL;
#endif
}


////////////////////////////////////////////////////////////
// Public Mutators
////////////////////////////////////////////////////////////

int Texture::loadColorTexture(unsigned char rgba[4],
										unsigned int width, unsigned int height)
{
	unsigned char *image;
	int id = -1;

	image = generateColorTexture(rgba, width, height);
	id = loadBuffer(image, width, height, RGBA, 32);
	delete [] image;

	return id;
}


void Texture::initSDL_TTF()
{
#ifdef HAVE_SDL_TTF
	// Initialize the TTF library
	if (TTF_Init() < 0)
	{
		fprintf(stderr, "initSDL_TTF> TTF_Init() failed!\n");
		fprintf(stderr, "initSDL_TTF> Error is [%s].\n", SDL_GetError());
	}
	else
	{
		mFlags |= fUseSDL_TTF;

		printf("@ Started SDL_TTF subsystem...\n");
		atexit(TTF_Quit);
	}
#endif
}


void Texture::setFlag(TextureFlag flag)
{
	mFlags |= flag;
}


void Texture::clearFlag(TextureFlag flag)
{
	mFlags |= flag;
	mFlags ^= flag;
}


void Texture::reset()
{
	if (mTextureIds)
	{
		glDeleteTextures(mTextureLimit, mTextureIds);
		delete [] mTextureIds;
	}

	mTextureIds = 0x0;
	mTextureCount = 0;
	mTextureLimit = 0;
}


void Texture::disableMultiTexture()
{
	mFlags ^= fUseMultiTexture;
	mTextureId = -1;
	mTextureId2 = -1;

	glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE0_ARB);
}


void Texture::useMultiTexture(float aU, float aV, float bU, float bV)
{
	if (!(mFlags & fUseMultiTexture))
		return;

	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, aU, aV);
	glMultiTexCoord2fARB(GL_TEXTURE1_ARB, bU, bV);
}


void Texture::useMultiTexture(float u, float v)
{
	if (!(mFlags & fUseMultiTexture))
		return;

	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, u, v);
	glMultiTexCoord2fARB(GL_TEXTURE1_ARB, u, v);
}


void Texture::bindMultiTexture(int texture0, int texture1)
{
	if (//(int)a == mTextureId && (int)b == mTextureId2 ||
		 !mTextureIds ||
		 texture0 < 0 || texture0 > (int)mTextureCount ||
		 texture1 < 0 || texture1 > (int)mTextureCount)
   {
		return;
	}

	mFlags |= fUseMultiTexture;
	mTextureId  = texture0;
	mTextureId2 = texture1;

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mTextureIds[texture0]);

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mTextureIds[texture1]);
}


void Texture::setMaxTextureCount(unsigned int n)
{
	mTextureLimit = n;

	mTextureIds = new unsigned int[n];

	glGenTextures(n, mTextureIds);
}


int Texture::getTextureCount()
{
	return (mTextureCount-1);
}



int Texture::loadBuffer(unsigned char *image,
								unsigned int width, unsigned int height,
								ColorMode mode, unsigned int bpp)
{
	int id;


	id = loadBufferSlot(image, width, height, mode, bpp, mTextureCount++);

	if (id < 0)
	{
		return id;
	}

	return ++id;
}


void convertARGB32bppToRGBA32bpp(unsigned char *image,
											unsigned int w, unsigned int h)
{
	unsigned int i, size = w*h;
	unsigned char swap;


	for (i = 0; i < size; ++i)
	{
		/* 32-bit ARGB to RGBA */
		swap = image[i*4+3];
		image[i*4]   = image[i*4+1];
		image[i*4+1] = image[i*4+2];
		image[i*4+2] = image[i*4+3];
		image[i*4+3] = swap;
	}
}

GLint deprecated_gluBuild2DMipmaps(GLenum target, GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *data);

int Texture::loadBufferSlot(unsigned char *image,
									 unsigned int width, unsigned int height,
									 ColorMode mode, unsigned int bpp,
									 unsigned int slot)
{
	unsigned char bytes;
	unsigned int glcMode;


	if (!mTextureIds || slot >= mTextureLimit)
	{
		printf("Texture::Load> ERROR Not initalizied or out of free slots\n");
		return -1000;
	}

	if (!width || !height || !image)
	{
		printf("Texture::Load> ERROR Assertion 'image is valid' failed\n");
		return -1;
	}

	switch (mode)
	{
	case GREYSCALE:
		if (bpp != 8)
		{
			printf("Texture::Load> ERROR Unsupported GREYSCALE, %i bpp\n", bpp);
			return -2;
		}

		bytes = 1;
		glcMode = GL_LUMINANCE;
		break;
	case RGB:
		if (bpp != 24)
		{
			printf("Texture::Load> ERROR Unsupported RGBA, %i bpp\n", bpp);
			return -2;
		}

		bytes = 3;
		glcMode = GL_RGB;
		break;
	case ARGB:
		if (bpp == 32)
		{
			convertARGB32bppToRGBA32bpp(image, width, height);
		}
	case RGBA:
		if (bpp != 32)
		{
			printf("Texture::Load> ERROR Unsupported RGBA/ARGB, %i bpp\n", bpp);
			return -2;
		}

		bytes = 4;
		glcMode = GL_RGBA;
		break;
	default:
		printf("Texture::Load> ERROR Unknown color mode %i\n", mode);
		return -2;
	}

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glBindTexture(GL_TEXTURE_2D, mTextureIds[slot]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	if (mFlags & fUseMipmaps)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
							 GL_NEAREST_MIPMAP_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
							 GL_LINEAR_MIPMAP_LINEAR);

		//gluBuild2DMipmaps(GL_TEXTURE_2D, bytes, width, height, glcMode, GL_UNSIGNED_BYTE, image);
        // gluBuild2DMipmaps is deprecated. Replacement by xythobuz
        deprecated_gluBuild2DMipmaps(GL_TEXTURE_2D, bytes, width, height, glcMode, GL_UNSIGNED_BYTE, image);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, glcMode, width, height, 0,
						 glcMode, GL_UNSIGNED_BYTE, image);
	}

	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	return slot;
}


// http://mmmovania.blogspot.de/2011/01/opengl-30-and-above-deprecated-func-and.html
// http://www.g-truc.net/post-0256.html
GLint deprecated_gluBuild2DMipmaps(GLenum target, GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *data) {
    glTexParameteri(target, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexImage2D(target, 0, internalFormat, width, height, 0, format, type, data);
    return 0;
}


void Texture::bindTextureId(unsigned int n)
{
	if ((int)n == mTextureId || !mTextureIds || n > mTextureCount)
   {
		return;
	}

	mTextureId = n;

	glEnable(GL_TEXTURE_2D);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, mTextureIds[n]);
}


void Texture::glScreenShot(char *base, unsigned int width, unsigned int height)
{
#ifdef USING_MTK_PNG
  FILE *f;
  int sz = width*height;
  unsigned char *image = new unsigned char[sz*3];
  char filename[1024];
  static int count = 0;
  bool done = false;


  if (!image)
  {
    printf("glScreenShot> ERROR: Couldn't allocate image!\n");
    return;
  }

  while (!done)
  {
    snprintf(filename, 1024, "%s-%03i.png", base, count++);

    f = fopen(filename, "rb");

    if (f)
      fclose(f);
    else
      done = true;
  }

  f = fopen(filename, "wb");

  if (!f)
  {
    printf("glScreenShot> ERROR: Couldn't write screenshot.\n");
    perror("glScreenShot> ERROR: ");
    return;
  }

  // Capture frame buffer
  glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image);

  mtk_image__png_save(f, image, width, height, 3);
  fclose(f);

  delete [] image;

  printf("glScreenShot> Took screenshot '%s'.\n", filename);
#else
  FILE *f;
  int sz = width*height;
  unsigned char *image = new unsigned char[sz*3];
  unsigned char *swap_row = new unsigned char[width*3];
  char filename[1024];
  static int count = 0;
  bool done = false;
  int i, j, size;
  unsigned char comment_lenght;
  unsigned char colormap_type;
  unsigned char image_type;
  unsigned short colormap_index;
  unsigned short colormap_lenght;
  unsigned char colormap_bbp;
  unsigned short origin_x;
  unsigned short origin_y;
  unsigned short swidth;
  unsigned short sheight;
  char comment[32] = "Mongoose TGA 0.0.1\0";
  unsigned char tmp, bpp, desc_flags;


  if (!image || !width || !height)
  {
    if (image)
    {
      delete [] image;
    }

    printf("glScreenShot> ERROR: Couldn't allocate image!\n");
    return;
  }

  // Don't overwrite files
  while (!done)
  {
    snprintf(filename, 1024, "%s-%04i.tga", base, count++);

    f = fopen(filename, "rb");

    if (f)
      fclose(f);
    else
      done = true;
  }

  f = fopen(filename, "wb");

  if (!f)
  {
    printf("glScreenShot> ERROR: Couldn't write screenshot.\n");
    perror("glScreenShot> ERROR: ");
    return;
  }

  // Capture frame buffer
  glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image);

  // Flip vertical
  for (i = 0, j = (int)height-1; i < (int)height/2; ++i, --j)
  {
    memcpy(swap_row, &image[i*width*3], width*3);
    memcpy(&image[i*width*3], &image[j*width*3], width*3);
    memcpy(&image[j*width*3], swap_row, width*3);
  }

  delete [] swap_row;

  comment_lenght = strlen(comment);
  colormap_type = 0;
  image_type = 2;
  colormap_index = 0;
  colormap_lenght = 0;
  colormap_bbp = 0;
  origin_x = origin_y = 0;
  swidth = width;
  sheight = height;
  bpp = 24;
  desc_flags = 32;

  // Write TGA header
  fwrite(&comment_lenght, 1, 1, f);
  fwrite(&colormap_type, 1, 1, f);
  fwrite(&image_type, 1, 1, f);
  fwrite(&colormap_index, 2, 1, f);
  fwrite(&colormap_lenght, 2, 1, f);
  fwrite(&colormap_bbp, 1, 1, f);
  fwrite(&origin_x, 2, 1, f);
  fwrite(&origin_y, 2, 1, f);
  fwrite(&swidth, 2, 1, f);
  fwrite(&sheight, 2, 1, f);
  fwrite(&bpp, 1, 1, f);
  fwrite(&desc_flags, 1, 1, f);

  // Write comment
  fwrite(&comment, 1, comment_lenght, f);

  size = width * height * 3;

  for (i = 0; i < size; i += 3)
  {
    tmp = image[i];
    image[i] = image[i + 2];
    image[i + 2] = tmp;
  }

  // Write image data
  if (fwrite(image, size, 1, f) < 1)
  {
    perror("glScreenShot> Disk write failed.\n");
	 fclose(f);
    return;
  }

  fclose(f);

  delete [] image;

  printf("Took screenshot '%s'.\n", filename);
#endif
}


int Texture::loadPNG(char *filename)
{
#ifdef USING_MTK_PNG
	FILE *f;
	unsigned char *image = NULL;
	unsigned char *image2 = NULL;
	unsigned int w, h;
	char type;
	int id = -1;


	f = fopen(filename, "rb");

	if (!f)
	{
		perror("Couldn't load file");
	}
	else if (!mtk_image__png_check(f))
	{
		mtk_image__png_load(f, &image, &w, &h, &type);

		type += 2;

		image2 = scaleBuffer(image, w, h, (type == 4) ? 4 : 3);

		if (image2)
		{
			image = image2;
			w = h = 256;
		}

		if (image)
		{
			id = loadBuffer(image, w, h,
								 (type == 4) ? RGBA : RGB,
								 (type == 4) ? 32 : 24);

			printf("%c", (id == -1) ? 'x' : 'o');
			fflush(stdout);

			delete [] image;
		}

		if (f)
		{
			fclose(f);
		}
	}

	if (id == -1)
	{
		printf("Texture::loadPNG> ERROR: Failed to load '%s'\n", filename);
	}

	return id;
#else
	printf("ERROR: MTK PNG support not enabled in this build\n");
	return -1;
#endif
}


int Texture::loadTGA(char *filename)
{
#ifdef USING_MTK_TGA
	FILE *f;
	unsigned char *image = NULL;
	unsigned char *image2 = NULL;
	unsigned int w, h;
	char type;
	int id = -1;


	f = fopen(filename, "rb");

	if (!f)
	{
		perror("Couldn't load file");
	}
	else if (!mtk_image__tga_check(f))
	{
		mtk_image__tga_load(f, &image, &w, &h, &type);

		type += 2;

		image2 = scaleBuffer(image, w, h, (type == 4) ? 4 : 3);

		if (image2)
		{
			image = image2;
			w = h = 256;
		}

		if (image)
		{
			id = loadBuffer(image, w, h,
								 (type == 4) ? RGBA : RGB,
								 (type == 4) ? 32 : 24);

			printf("%c", (id == -1) ? 'x' : 'o');
			fflush(stdout);

			delete [] image;
		}

		fclose(f);
	}

	if (id == -1)
	{
		printf("Texture::loadTGA> ERROR: Failed to load '%s'\n", filename);
	}

	return id;
#else
	printf("ERROR: MTK TGA support not enabled in this build\n");
	return -1;
#endif
}


////////////////////////////////////////////////////////////
// Private Accessors
////////////////////////////////////////////////////////////

int Texture::nextPower(int seed)
{
  int i;

  for(i = 1; i < seed; i *= 2)
    ;

  return i;
}


/* This code based off on gluScaleImage()  */
unsigned char *Texture::scaleBuffer(unsigned char *image,
												int width,  int height,
												int components)
{
   int i, j, k;
   float* tempin;
   float* tempout;
   float sx, sy;
	//   int components = 3;
   unsigned char *timage;
	int original_height = height;
	int original_width = width;


   if (!image || !width || !height)
	{
		return NULL;
	}

   height = nextPower(height);
   width = nextPower(width);

   if (height > 256)
     height = 256;

   if (width > 256)
     width = 256;

   // Check to see if scaling is needed
   if (height == original_height && width == original_width)
		return NULL;

	//printf("%i\n", components);

   timage = new unsigned char[height * width * components];
   tempin = new float[original_width * original_height * components * sizeof(float)];
   tempout = new float[width * height * components * sizeof(float)];

   if (!tempout || !tempin)
   {
     if (tempout)
       delete [] tempout;

     if (tempin)
       delete [] tempin;

     printf("Oh shit out of memory!\n");
     return NULL;
   }

   // Copy user data to float format.
   for (i = 0; i < original_height * original_width * components; ++i)
   {
     tempin[i] = (float)image[i];
   }

   // Determine which filter to use by checking ratios.
   if (width > 1)
   {
     sx = (float)(original_width - 1) / (float)(width - 1);
   }
   else
   {
     sx = (float)(original_width - 1);
   }

   if (height > 1)
   {
     sy = (float)(original_height - 1) / (float) (height - 1);
   }
   else
   {
     sy = (float)(original_height - 1);
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

     for(i = 0; i < height; ++i)
     {
       i0 = (int)(i * sy);
       i1 = i0 + 1;

       if (i1 >= original_height)
       {
			 i1 = original_height - 1;
       }

       alpha = i * sy - i0;

       for (j = 0; j < width; ++j)
       {
			 j0 = (int) (j * sx);
			 j1 = j0 + 1;

			 if (j1 >= original_width)
			 {
				 j1 = original_width - 1;
			 }

			 beta = j * sx - j0;

			 /* Compute weighted average of pixels in rect (i0,j0)-(i1,j1) */
			 src00 = tempin + (i0 * original_width + j0) * components;
			 src01 = tempin + (i0 * original_width + j1) * components;
			 src10 = tempin + (i1 * original_width + j0) * components;
			 src11 = tempin + (i1 * original_width + j1) * components;

			 dst = tempout + (i * width + j) * components;

			 for (k = 0; k < components; ++k)
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

     for (i = 0; i < height; ++i)
     {
       i0 = (int) (i * sy);
       i1 = i0 + 1;

       if (i1 >= original_height)
       {
			 i1 = original_height - 1;
       }

       for (j = 0; j < width; ++j)
       {
			 j0 = (int) (j * sx);
			 j1 = j0 + 1;

			 if (j1 >= original_width)
			 {
				 j1 = original_width - 1;
			 }

			 dst = tempout + (i * width + j) * components;

			 /* Compute average of pixels in the rectangle (i0,j0)-(i1,j1) */
			 for (k = 0; k < components; ++k)
			 {
				 sum = 0.0;

				 for (ii = i0; ii <= i1; ++ii)
				 {
					 for (jj = j0; jj <= j1; ++jj)
					 {
						 sum += *(tempin + (ii * original_width + jj)
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
   for( i = 0; i < height * width * components; ++i)
   {
     timage[i] = (unsigned char)tempout[i];
   }

   // Delete our temp buffers.
   delete[] tempin;
   delete[] tempout;
   delete[] image;

   return timage;
}


////////////////////////////////////////////////////////////
// Private Mutators
////////////////////////////////////////////////////////////

