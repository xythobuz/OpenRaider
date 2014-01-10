/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*==========================================================================
 *
 * Project : MTK, Freyja, OpenRaider
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440
 * Email   : stu7440@westga.edu
 * Object  : MtkTexture
 * Comments: This is the Texture registery
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


#ifndef __MTK_MONGOOSE_MTKTEXTURE_H
#define __MTK_MONGOOSE_MTKTEXTURE_H

#include <stdio.h>

typedef struct
{
	int x, y, w, h;
	int minx; int maxx; int miny; int maxy; int advance;

} ttf_glyph_t;

typedef struct
{
	unsigned int utf8Offset;
	unsigned int count;

/* [utf8Offset -> utf8Offset+count],
	 matches indexing into glyphs[0 -> count] for texcoords, etc
	----------------------------------------
	41     -> 126     ASCII English w/ special chars,
	0x303f -> 0x3093  Japanese hiragana kana,
	0x301a -> 0x30f6  Japanese katakana kana */

	unsigned int width;     /* Width and height of RGBA texture */
	unsigned char *texture; /* RGBA texture data */

	ttf_glyph_t *glyphs;    /* For typesetting and rendering use */
	int fontHeight;
	int fontAscent;
	int fontDescent;
	int fontSpacing;

} ttf_texture_t;


typedef struct
{
	unsigned int utf8Offset;
	unsigned int count;
	int textureId;
	int drawListBase;

} gl_font_t;



class Texture
{
 public:

	enum ColorMode
	{
		GREYSCALE = 1,
		RGB,
		RGBA,
		ARGB
	};

	enum TextureFlag
	{
		fUseMipmaps			= 1,
		fUseMultiTexture	= 2,
		fUseSDL_TTF			= 4
	};


	////////////////////////////////////////////////////////////
	// Constructors
	////////////////////////////////////////////////////////////

   Texture();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Constructs an object of Texture
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.29:
	 * Mongoose - Big code clean up, documentation
	 *
	 * 2000.10.05:
	 * Mongoose - Created
	 ------------------------------------------------------*/

   ~Texture();
	/*------------------------------------------------------
	 * Pre  : This object exists
	 * Post : Deconstructs an object of Texture
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.29:
	 * Mongoose - Big code clean up, documentation
	 *
	 * 2000.10.05:
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Public Accessors
	////////////////////////////////////////////////////////////

	static unsigned char *generateColorTexture(unsigned char rgba[4],
															 unsigned int width,
															 unsigned int height);
	/*------------------------------------------------------
	 * Pre  : <Rgba> is 32bpp RGBA color
	 *        <Width> and <Height> are powers of two, pref
	 *        the same number
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.30:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	gl_font_t *generateFont(ttf_texture_t *texture);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.30:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	ttf_texture_t *generateFontTexture(const char *filename, int pointSize,
												  unsigned int textureWidth,
												  unsigned char color[3],
												  unsigned int utf8Offset,
												  unsigned int count,
												  char verbose);
	/*------------------------------------------------------
	 * Pre  : <Filename> of TTF font
	 *        <PointSize> to generate
	 *        <TextureWidth> is width of texture, height will match it
	 *        <Color> is RGB 24bit color
	 *        <Utf8Offset> is offset into font's encoding chart
	 *        <Count> is number of glyphs to read from offset start
	 *        <Verbose> dumps debug info to stdout
	 *
	 * Post : Generates a font texture with typeset info from TTF
	 *
	 *        DOES NOT load the texture itself, call loadFont()
	 *        on returned ttf_texture_t
	 *
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.03:
	 * Mongoose - Created
	 ------------------------------------------------------*/

   int getTextureCount();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Returns number of textures in use, or -1 for
	 *        error ( Not initalized )
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.29:
	 * Mongoose - Big code clean up, documentation
	 *
	 * 2000.10.05:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void glScreenShot(char *base, unsigned int width, unsigned int height);
	/*------------------------------------------------------
	 * Pre  : <Base> is base filename,
	 *        <Width> and <Height> are viewport dim
	 *
	 * Post : Dumps a screenshot to disk,
	 *        avoids overwriting files with same base name
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.16:
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Public Mutators
	////////////////////////////////////////////////////////////

	void bindMultiTexture(int texture0, int texture1);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Sets up multitexture rendering with passed texture ids
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.24:
	 * Mongoose - Created
	 ------------------------------------------------------*/

   void bindTextureId(unsigned int n);
	/*------------------------------------------------------
	 * Pre  : n is valid texture index
	 * Post : Binds the texture for use in GL
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.29:
	 * Mongoose - Big code clean up, documentation
	 *
	 * 2000.10.05:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void clearFlag(TextureFlag flag);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : CLears a option flag
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.01.05:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void disableMultiTexture();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.24:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void initSDL_TTF();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Loads SDL_TTF if avalible
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.03:
	 * Mongoose - Created
	 ------------------------------------------------------*/

   int loadBuffer(unsigned char *image,
						unsigned int width, unsigned int height,
						ColorMode mode, unsigned int bpp);
	/*------------------------------------------------------
	 * Pre  : image must be a valid pixmap that agrees
	 *        with mode, width, and height
	 *
	 * Post : Returns texture id or < 0 error flag
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.29:
	 * Mongoose - Big code clean up, documentation
	 *
	 * 2000.10.05:
	 * Mongoose - Created
	 ------------------------------------------------------*/

   int loadBufferSlot(unsigned char *image,
							 unsigned int width, unsigned int height,
							 ColorMode mode, unsigned int bpp,
							 unsigned int slot);
	/*------------------------------------------------------
	 * Pre  : image must be a valid pixmap that agrees
	 *        with mode, width, and height, slot ( ID )
	 *
	 * Post : Returns texture id or < 0 error flag
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.09.05:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	int loadColorTexture(unsigned char rgba[4],
								unsigned int width, unsigned int height);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Generates and loads a solid color texture,
	 *        returns texture Id or -1 if failed
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.30:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	int loadFontTTF(const char *filename,
						 unsigned int utf8Offset, unsigned int count);
	/*------------------------------------------------------
	 * Pre  : <Filename> of TTF font
	 *        <Utf8Offset> is offset into UNICODE table
	 *        <Count> is number of glyphs to load
	 *
	 * Post : Loads a TTF,
	 *        Generates: texture image, glyph list, and drawlist
	 *
	 *        Returns font id if sucessful, or < 0 if error
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.07.05:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	int loadPNG(const char *filename);
	/*------------------------------------------------------
	 * Pre  : Texture is init and filename/file is valid
	 * Post : Loads PNG as texture and returns ID or -1 error
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.16:
	 * Mongoose - Created, from Freyja
	 ------------------------------------------------------*/

	int loadTGA(const char *filename);
	/*------------------------------------------------------
	 * Pre  : Texture is init and filename/file is valid
	 * Post : Loads TGA as texture and returns ID or -1 error
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.16:
	 * Mongoose - Created
	 ------------------------------------------------------*/

   void reset();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Resets all texture data
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.29:
	 * Mongoose - Big code clean up, documentation
	 *
	 * 2000.10.05:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void setFlag(TextureFlag flag);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Sets a option flag
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.01.05:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void setMaxTextureCount(unsigned int n);
	/*------------------------------------------------------
	 * Pre  : n is max number of textures you wish to allow
	 * Post : Sets up GL texturing, and must be called
	 *        as the first setup step
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.05.29:
	 * Mongoose - Big code clean up, documentation
	 *
	 * 2000.10.05:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void useMultiTexture(float u, float v);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.24:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void useMultiTexture(float aU, float aV, float bU, float bV);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.24:
	 * Mongoose - Created
	 ------------------------------------------------------*/


 private:

	////////////////////////////////////////////////////////////
	// Private Accessors
	////////////////////////////////////////////////////////////

	int nextPower(int seed);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.16:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	unsigned char *scaleBuffer(unsigned char *image, int width, int height,
										int components);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.16:
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Private Mutators
	////////////////////////////////////////////////////////////

   unsigned int *mTextureIds;		/* GL texture list */

   unsigned int mTextureCount;	/* Texture counter */

   unsigned int mTextureLimit;	/* The texture limit */

	unsigned int mFlags;				/* Class options */

   int mTextureId;					/* Currently bound texture id */

	int mTextureId2;					/* Multitexture Texture Id */
};


/* Experimental testing */
void bufferedPrintf(char *string, unsigned int len, char *s, ...);

void glPrint3d(float x, float y, float z,
					float pitch, float yaw, float roll,
					float scale,
					char *string);

void glPrint2d(float x, float y, float scale, char *string);

	void glEnterMode2d(unsigned int width, unsigned int height);
	/*------------------------------------------------------
	 * Pre  :
	 * Post : OpenGL ortho projection
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.03:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void glExitMode2d();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : OpenGL model matrix projection
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.06.03:
	 * Mongoose - Created
	 ------------------------------------------------------*/
#endif
