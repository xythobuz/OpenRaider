/*!
 * \file GLString.cpp
 * \brief Open GL rendering font/string Unit Test
 *
 * \author Mongoose
 */

#include <math.h>

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#ifdef HAVE_MTK
#include <Texture.h>
#include <mtk_tga.h>

Texture gTexture;
#else
#error "Requires MTK: Texture and mtk_tga"
#endif

#include <GLString.h>

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
				id[j] = gTexture.loadTGA(argv[i]);
			}
		}
	}
	else
	{
		// id[0] = gTexture.loadTGA("data/font-0.tga");
		// id[1] = gTexture.loadTGA("data/font-1.tga");
		// id[2] = gTexture.loadTGA("data/font-2.tga");
		// id[3] = gTexture.loadTGA("data/font-3.tga");
        id[0] = id[1] = id[2] = id[3] = gTexture.loadTGA("data/font-0.tga");
	}

	TEXT->Init(4, 4, id);
	i = TEXT->glPrintf((width/2)-12*5, height/2, 0,
							 "[font %i] GLString Test", id[0]);
	if (i)
	{
		printf("TEXT->glPrintf> ERROR code %i ( 0 means no error )\n", i);
	}

	i = TEXT->glPrintf((width/2)-12*5, height/2+32, 1,
							 "[font %i] GLString Test", id[1]);
	if (i)
	{
		printf("TEXT->glPrintf> ERROR code %i ( 0 means no error )\n", i);
	}

	s = 1.1;
	TEXT->Scale(s);

	i = TEXT->glPrintf((width/2)-10*7, height/2+64, 1,
							 "[font %i] Scaled by %.1f", id[2], s);
	if (i)
	{
		printf("TEXT->glPrintf> ERROR code %i ( 0 means no error )\n", i);
	}

	i = TEXT->glPrintf((width/2)-10*7, height/2-32, 0,
							 "[font %i] Scaled by %.1f", id[3], s);
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
  unsigned int height = 480;
  bool fullscreen = false;
#ifndef __APPLE__
  char *driver = NULL;
#endif


  // Setup clean up on exit
  atexit(shutdown_gl);

  // Get user settings
  //event_init(&width, &height, &fullscreen, &driver, argc, argv);

  // Create GL context
  SDL_Init(SDL_INIT_VIDEO);

#ifndef __APPLE__
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
#endif

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

int main(int argc, char *argv[])
{
  printf("[GLString class test]\n");

  TEXT = new GLString();
  main_gl(argc, argv);
  return 0;
}

