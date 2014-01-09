/*!
 * \file GLString.cpp
 * \brief Open GL rendering font/string class
 *
 * \author Mongoose
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <Texture.h>
#include <GLString.h>

#ifdef DEBUG_MEMORY
#include <memory_test.h>
#endif


GLString::GLString()
{
	_num_string_max = 0;
	_num_font_max = 0;
	_num_font = 0;
	_num_string = 0;
	_scale = 1.0;
	_font_texture = NULL;
	_font_base = NULL;
	_string = NULL;
}


GLString::~GLString()
{
	unsigned int i;


	for (i = 0; i < _num_font; ++i)
	{
		glDeleteLists(_font_base[i], 256);
	}

	if (_font_texture)
	{
		delete [] _font_texture;
	}

	if (_font_base)
	{
		delete [] _font_base;
	}

	if (_string)
	{

		for (i = 0; i < _num_string; ++i)
		{
			if (_string[i].text)
			{
				delete [] _string[i].text;
			}
		}

		delete [] _string;
	}
}


void GLString::Init(unsigned int max_strings, unsigned int max_fonts,
						  int *tex_map)
{
	unsigned int i;


	if (!max_strings || !max_fonts || !tex_map)
	{
		return;
	}

	_num_string_max = max_strings;
	_num_font_max = max_fonts;

	_font_texture = new int[max_fonts];
	_font_base = new int[max_fonts];
	_string = new gl_string_t[max_strings];

	for (i = 0; i < max_fonts; ++i)
	{
		_font_texture[i] = tex_map[i];

		if (BuildFontList(i) < 0)
		{
			printf("GLString::Init> BuildFontList failed for %i\n", i);
		}
	}
}


void GLString::SetChar(unsigned int string, unsigned int pos, char c)
{
	gl_string_t *str = GetString(string);


	if (str && pos < str->len)
	{
		str->text[pos] = c;
	}
}


unsigned int GLString::GetStringLen(unsigned int string)
{
	gl_string_t *str = GetString(string);


	if (str)
	{
		return str->len;
	}

	return 0;
}


char *GLString::GetBuffer(unsigned int string)
{
	gl_string_t *str = GetString(string);


	if (str)
	{
		return str->text;
	}

	return 0;
}


void GLString::setActive(unsigned int string, bool active)
{
	gl_string_t *str;


	str = GetString(string);

	if (str)
	{
		str->active = active;
	}
}


void GLString::SetString(unsigned int string, const char *s, ...)
{
	va_list args;
	gl_string_t *str;
	unsigned int len;


	str = GetString(string);

	if (s && s[0] && str)
	{
		str->active = true;

		len = strlen(s);

		if (len > str->len)
		{
			len = str->len - 1;
		}

		va_start(args, s);
		vsnprintf(str->text, str->len-2, s, args);
		str->text[str->len-1] = 0;
		va_end(args);
	}
}


void GLString::Scale(float scale)
{
	_scale = scale;
}


int GLString::BuildFontList(int index)
{
	int i;
	float cx;
	float cy;


	if (_num_font >= _num_font_max || index < 0 || index >= (int)_num_font_max)
	{
		return -1;
	}

	_font_base[index] = glGenLists(256);
	glBindTexture(GL_TEXTURE_2D, _font_texture[index]);

	// Mongoose 2002.01.01, Generate 256 lists per font
	//   one per symbol
	for (i = 0; i < 256; i++)
	{
		/* X Position Of Current Character */
		cx = 1 - (float)(i % 16) / 16.0f;
		/* Y Position Of Current Character */
		cy = 1 - (float)(i / 16) / 16.0f;

		/* Start Building A List */
		glNewList(_font_base[index] + (255 - i), GL_COMPILE);
		/* Use A Quad For Each Character */
		glBegin(GL_QUADS);
		/* Texture Coord (Bottom Left) */
		glTexCoord2f(cx - 0.0625, cy);
		/* Vertex Coord (Bottom Left) */
		glVertex2i(0, 0);

		/* Texture Coord (Bottom Right) */
		glTexCoord2f(cx, cy);
		/* Vertex Coord (Bottom Right) */
		glVertex2i(16, 0);

		/* Texture Coord (Top Right) */
		glTexCoord2f(cx, cy - 0.0625f);
		 /* Vertex Coord (Top Right) */
		glVertex2i(16, 16);

		/* Texture Coord (Top Left) */
		glTexCoord2f(cx - 0.0625f, cy - 0.0625f);
		/* Vertex Coord (Top Left) */
		glVertex2i(0, 16);
		glEnd();

		/* Move To The Left Of The Character */
		glTranslated(10, 0, 0);
		glEndList();
	}

	return 0;
}


int GLString::glPrintf(int x, int y, int font, const char *string, ...)
{
	int sz = 60;
	int n;
	va_list args;


	// Mongoose 2002.01.01, Only allow valid strings
	//   we must assume it's NULL terminated also if it passes...
	if (!string || !string[0])
	{
		return -1;
	}

	if (_num_string > _num_string_max)
	{
		return -2;
	}

	if (font < 0 || font > (int)_num_font_max)
	{
		return -3;
	}

	// Mongoose 2002.01.01, Assume no longer than 'sz' wide lines
	//   on first try
	_string[_num_string].text = new char[sz];

	// Mongoose 2002.01.03, Setup scale factor
	_string[_num_string].scale = _scale;

	//  Mongoose 2002.01.01, Setup position
	_string[_num_string].x = x;
	_string[_num_string].y = y;

	//  Mongoose 2002.01.01, Setup font list base index to use
	_string[_num_string].font = font;

	va_start(args, string);

	// Mongoose 2002.01.01, Get exact size needed if the first try fails
	n = vsnprintf(_string[_num_string].text, sz, string, args);

	// Mongoose 2002.01.01, Realloc correct amount if truncated
	while (1)
	{
		if (n > -1 && n < sz)
		{
			break;
		}

		// Mongoose 2002.01.01, For glibc 2.1
		if (n > -1)
		{
			sz = n + 1;
			delete [] _string[_num_string].text;
			_string[_num_string].text = new char[sz];
			n = vsnprintf(_string[_num_string].text, sz, string, args);

			break;
		}
		else // glibc 2.0
		{
			sz *= 2;
			delete [] _string[_num_string].text;
			_string[_num_string].text = new char[sz];
			n = vsnprintf(_string[_num_string].text, sz, string, args);
		}
	}

	va_end(args);


	// Mongoose 2002.01.04, Remeber string size, for future rebuffering use
	_string[_num_string].len = sz;

	// Mongoose 2002.01.01, Incement string counter, since we just
	//   allocated a string
	++_num_string;

	return 0;
}


void GLString::Render(int width, int height)
{
	unsigned int i;

#ifndef HAVE_SDL_TTF
	// Mongoose 2001.12.31, Start the evil font rendering...
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);

	// Mongoose 2001.12.31, New 'flat' projection
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, width, 0, height, -1, 1);

	// Mongoose 2001.12.31, New rendering matrix
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// Mongoose 2001.12.31, Rasterize strings' text
	for (i = 0; i < _num_string; ++i)
	{
		if (_string[i].active)
		{
			glPushMatrix();
			glBindTexture(GL_TEXTURE_2D, _font_texture[_string[i].font]);
			glTranslated(_string[i].x, _string[i].y, 0);
			glScaled(_string[i].scale, _string[i].scale, _string[i].scale);
			glListBase(_font_base[_string[i].font] - 32);
			glCallLists(strlen(_string[i].text), GL_BYTE, _string[i].text);
			glPopMatrix();
		}
	}


	// Mongoose 2001.12.31, Restore scene projection
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	// Mongoose 2001.12.31, Restore scene matrix
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);
#else
	for (i = 0; i < _num_string; ++i)
	{
		if (_string[i].active)
		{
			glPrint2d(_string[i].x, _string[i].y,
						 _string[i].scale,
						 _string[i].text);
		}
	}
#endif
}


gl_string_t *GLString::GetString(unsigned int id)
{
	if (id < _num_string)
	{
		return _string + id;
	}

	return NULL;
}

