/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 *
 * Project : Mtk
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : GLString
 * License : No use w/o permission (C) 2002 Mongoose
 * Comments: Open GL rendering font/string class
 *
 *
 *           This file was generated using Mongoose's C++
 *           template generator script.  <stu7440@westga.edu>
 *
 *-- History -------------------------------------------------
 *
 * 2002.01.01:
 * Mongoose - Created
 =================================================================*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "Texture.h"

#include "GLString.h"

#ifdef DEBUG_MEMEORY
#   include "memeory_test.h"
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


void GLString::SetString(unsigned int string, char *s, ...)
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


////////////////////////////////////////////////////////////
// Test code
////////////////////////////////////////////////////////////


#ifdef __TEST__

#include <GL/glu.h>

#ifdef HAVE_MTK
#   include "Texture.h"
#   include "mtk_tga.h"

Texture gTexture;
#else
#   error "Requires MTK: Texture and mtk_tga"
#endif


GLString *TEXT;

void swap_buffers();


void event_resize(int width, int height)
{
	GLfloat aspect;

	glMatrixMode(GL_PROJECTION);
	aspect = (GLfloat)width/(GLfloat)height;

	// Mongoose 2002.01.01, Setup view volume, with a nice FOV
	gluPerspective(40.0, aspect, 1, 2000);

	glMatrixMode(GL_MODELVIEW);
}


void event_display(int width, int height)
{
	static float x = 0.0, y = 0.0, z = -150.0, r = 0.0;


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glTranslatef(0.0, 0.0, -20.0);
	glRotatef((float)cos(r)*180.0, 0.0, 0.0, 1.0);
	r += 0.01;

	// Mongoose 2002.01.01, Render color quad
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(x + 50, y + 50, z);
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(x - 50, y + 50, z);
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(x + 50, y - 50, z);
	glColor3f(0.5, 0.5, 0.5);
	glVertex3f(x - 50, y - 50, z);
	glEnd();

	// Mongoose 2002.01.01, Render text
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glColor3f(0.1, 0.2, 1.0);
	TEXT->Render(width, height);

	glFlush();
	swap_buffers();
}


#ifdef HAVE_SDL
#include <SDL/SDL.h>


SDL_Surface *SDL_WINDOW = NULL;


void swap_buffers()
{
	SDL_GL_SwapBuffers();
}


void shutdown_gl()
{
	SDL_Quit();
}


void init_gl(unsigned int width, unsigned int height,
				 int argc, char *argv[])
{
	int i, j;
	int id[4];
	float s = 1.0;


	// Setup GL
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	event_resize(width, height);

	// Mongoose 2002.01.01, Texture setup
	gTexture.reset();
	gTexture.setFlag(Texture::fUseMipmaps);
	gTexture.setMaxTextureCount(32);

	if (argc > 1)
	{
		for (i = 1, j = 0; i < argc; ++i, ++j)
		{
			if (j < 4)
			{
				id[j++] = gTexture.loadTGA(argv[i]);
			}
		}
	}
	else
	{
		id[0] = gTexture.loadTGA("data/font-0.tga");
		id[1] = gTexture.loadTGA("data/font-1.tga");
		id[2] = gTexture.loadTGA("data/font-2.tga");
		id[3] = gTexture.loadTGA("data/font-3.tga");
	}

	printf("%i %i %i %i\n", id[0], id[1], id[2], id[3]);

	TEXT->Init(4, 4, id);
	i = TEXT->glPrintf((width/2)-12*5, height/2, 0,
							 "[font %i] GLString Test", id[0]);
	if (i)
	{
		printf("TEXT->glPrintf> ERROR code %i ( 0 means no error )\n", i);
	}

	i = TEXT->glPrintf((width/2)-10*7, height/2+32, 1,
							 "[font %i] GLString Test", id[1]);

	if (i)
	{
		printf("TEXT->glPrintf> ERROR code %i ( 0 means no error )\n", i);
	}

	s = 1.1;
	TEXT->Scale(s);

	i = TEXT->glPrintf((width/2)-10*7, height/2+64, 1,
							 "[font %i] Scaled by %.3f", id[1], s);

	if (i)
	{
		printf("TEXT->glPrintf> ERROR code %i ( 0 means no error )\n", i);
	}
	i = TEXT->glPrintf((width/2)-10*7, height/2-32, 0,
							 "[font %i] Scaled by %.3f", id[0], s);

	if (i)
	{
		printf("TEXT->glPrintf> ERROR code %i ( 0 means no error )\n", i);
	}
}


int main_gl(int argc, char *argv[])
{
  SDL_Event event;
  unsigned int mkeys, mod, key;
  int flags;
  unsigned int width = 640;
  unsigned int height = 460;
  bool fullscreen = false;
  char *driver = NULL;


  // Setup clean up on exit
  atexit(shutdown_gl);

  // Get user settings
  //event_init(&width, &height, &fullscreen, &driver, argc, argv);

  // Create GL context
  SDL_Init(SDL_INIT_VIDEO);

  if (!driver || !driver[0] || SDL_GL_LoadLibrary(driver) < 0)
  {
	  SDL_ClearError();

	  // Fallback 1
	  if (SDL_GL_LoadLibrary("libGL.so") < 0)
	  {
		  SDL_ClearError();

		  // Fallback 2
		  if (SDL_GL_LoadLibrary("libGL.so.1") < 0)
		  {
			  fprintf(stderr, "main_gl> SDL_GL_LoadLibrary failed!\n");
			  fprintf(stderr, "main_gl> Error is [%s].\n", SDL_GetError());
			  exit(1);
		  }
	  }
  }

  flags = SDL_OPENGL | SDL_GL_DOUBLEBUFFER;

  if (fullscreen)
  {
	  flags |= SDL_FULLSCREEN;
	  SDL_ShowCursor(SDL_DISABLE);
  }

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_WINDOW = SDL_SetVideoMode(width, height, 16, flags);
  SDL_WM_SetCaption("GLString Test", "GLString Test");
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

  // Init rendering
  init_gl(width, height, argc, argv);

  for (;;)
  {
	  // Pause for 10-20 ms
	  SDL_Delay(10);

	  event_display(width, height);

	  while (SDL_PollEvent(&event))
	  {
		  switch (event.type)
		  {
		  case SDL_QUIT:
			  exit(0);
			  break;
		  case SDL_MOUSEMOTION:
			  break;
		  case SDL_MOUSEBUTTONDOWN:
		  case SDL_MOUSEBUTTONUP:
			  break;
		  case SDL_KEYDOWN:
			  mkeys = (unsigned int)SDL_GetModState();
			  mod = 0;

			  if (mkeys & KMOD_LSHIFT)
				  mod |= KMOD_LSHIFT;

			  if (mkeys & KMOD_RSHIFT)
				  mod |= KMOD_RSHIFT;

			  if (mkeys & KMOD_LCTRL)
				  mod |= KMOD_LCTRL;

			  if (mkeys & KMOD_RCTRL)
				  mod |= KMOD_RCTRL;

			  if (mkeys & KMOD_LALT)
				  mod |= KMOD_LALT;

			  if (mkeys & KMOD_RALT)
				  mod |= KMOD_RALT;

			  key = event.key.keysym.sym;

			  switch (key)
			  {
			  case 0x1B: // 27d, ESC
				  exit(0);
				  break;
			  }
			  break;
		  case SDL_KEYUP:
			  break;
		  case SDL_VIDEORESIZE:
			  event_resize(event.resize.w, event.resize.h);

			  width = event.resize.w;
			  height = event.resize.h;
			  event_display(width, height);
			  break;
		  }
	  }
  }

  return 0;
}
#else
#error "Requires SDL to create GL Context"
#endif


int GLString::_RegressionTest(int argc, char *argv[])
{
	TEXT = this;
	main_gl(argc, argv);
	return 0;
}


int main(int argc, char *argv[])
{
  GLString test;


  printf("[GLString class test]\n");

  return test._RegressionTest(argc, argv);
}
#endif
