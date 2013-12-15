/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
#ifdef SDL_UNIT_TEST
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef USING_OPENGL

#   include <GL/gl.h>
#   include <GL/glu.h>
#   include <SDL/SDL.h>
#   include <Texture.h>

// Implement these functions in your Unit Test
void renderScene(float time, float lastTime);
void handleKey(int key);
void handleArgKey(char key);
void initScene(int argc, char *argv[]);

// Call this function to start SDL Unit Test
int runSDLUnitTest(int argc, char *argv[]);

//////////////////////////////////////////////////////////////////
Texture gTexture;

char gTitle[128];
float gYaw = 0.0f;
bool gYawOn = true;
float gPitch = -20.0f;
float gScale = 1.0f;
float gLightPos[] = { 0.0, 512.0, 1024.0, 0.0 };
unsigned int gWidth = 640, gHeight = 460;


void event_resize(unsigned int width, unsigned int height);

void handle_key(int key)
{
	static bool wireframe = false;
	static bool alphaBlend = false;
	static bool lighting = false;

	switch (key)
	{
	case SDLK_F5:
		wireframe = !wireframe;

		if (wireframe)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		printf("%sabled wireframe rendering\n", wireframe ? "En" : "Dis");
		break;
	case SDLK_F6:
		alphaBlend = !alphaBlend;

		if (alphaBlend)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glAlphaFunc(GL_GREATER, 0);
		}
		else
		{
			glDisable(GL_BLEND);
		}

		printf("%sabled alpha blending\n", alphaBlend  ? "En" : "Dis");
		break;
	case SDLK_F7:
		lighting = !lighting;

		if (lighting)
		{
			static bool firsttime = true;


			glEnable(GL_LIGHTING);

			if (firsttime)
			{
				float specular[] = { 0.6, 0.6, 0.6, 1.0 };
				float ambient[] = { 0.64, 0.64, 0.64, 1.0 };
				float shine[] = { 50.0 };


				// Caustic fx possible with mod amb?
				glMaterialfv(GL_FRONT, GL_SPECULAR,  specular);
				glMaterialfv(GL_FRONT, GL_AMBIENT,   ambient);
				//glMaterialfv(GL_FRONT, GL_DIFFUSE,   ambient);
				glMaterialfv(GL_FRONT, GL_SHININESS, shine);
				glLightfv(GL_LIGHT0,   GL_POSITION,  gLightPos);
				glEnable(GL_LIGHT0);

				glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 0);

				//glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 0);
				//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, WHITE);
				//glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, WHITE);
				//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, DIM_WHITE);
				//glLightf(GL_LIGHT0,  GL_SPOT_CUTOFF,    cutoff);
				//glLightfv(GL_LIGHT0, GL_POSITION,       pos);
				//glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dir);
				//glLightfv(GL_LIGHT0, GL_DIFFUSE,        color);

				firsttime = false;
			}
		}
		else
		{
			glDisable(GL_LIGHTING);
		}
		break;
	case SDLK_F8:
		gYawOn = !gYawOn;
		break;
	case SDLK_F9:
		gTexture.glScreenShot("screenshot", gWidth, gHeight);
		break;
	case SDLK_F10:
		event_resize(640, 460);
		break;
	case SDLK_F11:
		event_resize(800, 600);
		break;
	case SDLK_F12:
		event_resize(1024, 768);
		break;
	case SDLK_RIGHT:
		++gYaw;
		break;
	case SDLK_LEFT:
		--gYaw;
		break;
	case SDLK_UP:
		++gPitch;
		break;
	case SDLK_DOWN:
		--gPitch;
		break;
	case '-':
		if (gScale *= 0.75f > 0.000001)
			gScale *= 0.75f;
		break;
	case '=':
		gScale *= 1.25f;
		break;
	case 'i':
		gLightPos[1] += 16.0f;
		glLightfv(GL_LIGHT0,   GL_POSITION,  gLightPos);
		break;
	case 'k':
		gLightPos[1] -= 16.0f;
		glLightfv(GL_LIGHT0,   GL_POSITION,  gLightPos);
		break;
	case 'j':
		gLightPos[2] -= 16.0f;
		glLightfv(GL_LIGHT0,   GL_POSITION,  gLightPos);
		break;
	case 'l':
		gLightPos[2] += 16.0f;
		glLightfv(GL_LIGHT0,   GL_POSITION,  gLightPos);
		break;
	case 'h':
		printf("[SDLUnitTest.test]\n");
		printf("----------------------------------\n");
		printf("ESC        - Exit\n");
		printf("F5         - Toggle wireframe rendering\n");
		printf("F6         - Toggle alpha blending\n");
		printf("F8         - Toggle scene rotation\n");
		printf("F7         - Toggle lighting\n");
		printf("F9         - Take screenshot\n");
		printf("F11        - Set window to 640x460\n");
		printf("F11        - Set window to 800x600\n");
		printf("F12        - Set window to 1024x768\n");
		printf("ALT+ENTER  - Toogle fullscreen\n");
		printf("UP/DOWN    - Adjust scene pitch\n");
		printf("RIGHT/LEFT - Adjust scene yaw\n");
		printf("-/=        - Adjust model scaling\n");
		printf("----------------------------------\n");
		printf("i,k,j,l - Move light\n");
		printf("h          - Help\n");
	default:
		handleKey(key);
	}
}


///////////////////////////////////////////////////

SDL_Surface *gSDLWindow = NULL;


void updateWindowTitle(char *newTitle)
{
	static char title[64];

	memcpy(title, newTitle, 64);
	title[63] = 0;

	SDL_WM_SetCaption(title, "PSKModel.test");
}


unsigned int getTicks()
{
	return SDL_GetTicks();
}


void swap_buffers()
{
	SDL_GL_SwapBuffers();
}


void event_resize(unsigned int width, unsigned int height)
{
	GLfloat aspect;

	
	gWidth = width;
	gHeight = height;

	aspect = (GLfloat)width/(GLfloat)height;

	glViewport(0, 0, width, height); 

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, aspect, 8.0f, 4600.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Resize window
	gSDLWindow = SDL_SetVideoMode(width, height, 16, SDL_OPENGL);
}


#ifdef FIXME
void draw_axis(float length, float arrowLenght)
{
	/* Draw axis list to show bone orientation */
	glBegin(GL_LINES);

	/* X axis */
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-8.0f, 0.0f, 0.0f);
	glVertex3f(8.0f, 0.0f, 0.0f);

	/* X direction */
	glVertex3f(8.0f, 0.0f, 0.0f);
	glVertex3f(7.0f, 1.0f, 0.0f);
	glVertex3f(8.0f, 0.0f, 0.0f);
	glVertex3f(7.0f, -1.0f, 0.0f);

	/* Y axis */
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, -8.0f, 0.0f);
	glVertex3f(0.0f, 8.0f, 0.0f);

	/* Y direction */
	glVertex3f(0.0f, 8.0f, 0.0f);
	glVertex3f(0.0f, 7.0f, 1.0f);
	glVertex3f(0.0f, 8.0f, 0.0f);
	glVertex3f(0.0f, 7.0f, -1.0f);

	/* Z axis */
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, -8.0f);
	glVertex3f(0.0f, 0.0f, 8.0f);

	/* Z direction */
	glVertex3f(0.0f, 0.0f, 8.0f);
	glVertex3f(0.0f, 1.0f, 7.0f);
	glVertex3f(0.0f, 0.0f, 8.0f);
	glVertex3f(0.0f, -1.0f, 7.0f);

	glEnd();
}
#endif


void renderUnitTestScene() 
{

	static float lastTime = 0.0f;
	const float size = 500.0f, step = 50.0f;
	float x, y, time;


	gluLookAt(0.0, 0.0, -256.0, 
				 0.0, 8.0, 0.0, 
				 0.0, 1.0, 0.0);

	glDisable(GL_TEXTURE_2D);

	time = getTicks() * 0.1f;

	if (time - lastTime > 5.0f)
	{
		lastTime = time;

		if (gYawOn)
			++gYaw;
	}

#ifdef DRAW_ACTUAL_ORIGIN_AXIS_WITH_ROTATION
	glPushMatrix();
	glRotatef(yaw, 0, 1, 0);
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(10.0f, 0.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 10.0f, 0.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 10.0f);
	glEnd();
	glPopMatrix();
#endif

	// Draw light symbol
	glPushMatrix();
	glTranslatef(gLightPos[0], gLightPos[1], gLightPos[2]);
	
	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(10.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 10.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 10.0f);
	glEnd();
	glPopMatrix();

	glTranslatef(0.0f, -128.0f, 512.0f);
	glRotatef(gPitch, 1, 0, 0);
	glRotatef(gYaw,   0, 1, 0);

	// Draw transformed origin axis
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-30.0f, 0.0f, 0.0f);
	glVertex3f(30.0f, 0.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, -30.0f, 0.0f);
	glVertex3f(0.0f, 30.0f, 0.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, -30.0f);
	glVertex3f(0.0f, 0.0f, 30.0f);
	glEnd();

	// Draw grid
	glPushMatrix();
	glScalef(2.0f, 2.0f, 2.0f);
	glColor3f(0.4f, 0.4f, 0.6f);

	for (x = -size; x < size; x += step)
	{
		glBegin(GL_LINE_LOOP);
		for (y = -size; y < size; y += step)
		{
			glVertex3f(x + step, 0.0f, y);	
			glVertex3f(x, 0.0f, y);	
			glVertex3f(x, 0.0f, y + step);
			glVertex3f(x + step, 0.0f, y + step);
		}
		glEnd();
	}

	glPopMatrix();

	
	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f);
	glPushMatrix();
	glTranslatef(0.0f, 8.0f, 0.0f);
	glScalef(gScale, gScale, gScale);

	renderScene(time, lastTime);

	glPopMatrix();
}


void event_display(int width, int height)
{
	glClearColor(0.3, 0.3, 0.5, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	renderUnitTestScene();

	glFlush();
	swap_buffers();
}


void shutdown_gl()
{
	SDL_Quit();
}


void init_gl(unsigned int width, unsigned int height)
{
	// Print driver support information
	printf("\n\n\t## GL Driver Info ##\n");
	printf("\tVendor     : %s\n", glGetString(GL_VENDOR));
	printf("\tRenderer   : %s\n", glGetString(GL_RENDERER));
	printf("\tVersion    : %s\n", glGetString(GL_VERSION));
	printf("\tExtensions : %s\n\n\n", (char*)glGetString(GL_EXTENSIONS));

	// Setup GL
	glClearColor(0.3, 0.3, 0.5, 1.0);
	event_resize(width, height);

	// Texture setup
	//gTexture.reset();
	//gTexture.setMaxTextureCount(64);
}


int main_gl(int argc, char *argv[])
{
  SDL_Event event;
  unsigned int mkeys, mod, key, flags;
  unsigned int width = gWidth;
  unsigned int height = gHeight;
  bool fullscreen = false;
  char *driver = 0x0;


  // Setup clean up on exit
  atexit(shutdown_gl);

  // NOTE: Removed fullscreen/driver option parser args

  // Create GL context
  SDL_Init(SDL_INIT_VIDEO);
  printf("\n@Created OpenGL Context...\n");

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

  flags = SDL_OPENGL;

  if (fullscreen)
  {
	  flags |= SDL_FULLSCREEN;
	  SDL_ShowCursor(SDL_DISABLE);
  }

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE,     5);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,   5);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,    5);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,  16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  gSDLWindow = SDL_SetVideoMode(width, height, 16, flags);
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
  
  // Init rendering
  init_gl(width, height);
  initScene(argc, argv);

  printf("\n@Starting event loop...\n");
  updateWindowTitle(argv[0]);

  for (;;)
  {
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
			  case SDLK_ESCAPE: // 0x1B, 27d, ESC
				  exit(0);
				  break;
			  case SDLK_RETURN:
				  if (mod & KMOD_LALT)
				  {
					  SDL_ShowCursor(SDL_DISABLE);
					  SDL_WM_ToggleFullScreen(gSDLWindow);
				  }
			  }
			  
			  handle_key(key);
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

	  event_display(width, height);
  }
  
  return 0;
}
#endif


int runSDLUnitTest(int argc, char *argv[])
{
	handle_key('h');

	if (argc > 1)
	{
		for (int i = 0; argv[1][i] != 0; ++i)
		{
			switch (argv[1][i])
			{
			default:
			  handleArgKey(argv[1][i]);
			}
		}
	}
	else
	{
		printf("'%s h' for help\n", argv[0]);
	}

	
#ifdef USING_OPENGL
	main_gl(argc, argv);
#else
	printf("Build not OpenGL enabled\n");
#endif

	return 0;
}
#endif
