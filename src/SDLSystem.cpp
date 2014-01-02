/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 *
 * Project : OpenRaider
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : SDL
 * License : No use w/o permission (C) 2002 Mongoose
 * Comments:
 *
 *
 *           This file was generated using Mongoose's C++
 *           template generator script.  <stu7440@westga.edu>
 *
 *-- History -------------------------------------------------
 *
 * 2003.06.30,
 * Mongoose - SDL_TTF support moved to Texture class
 *
 * 2003.06.03:
 * Mongoose - SDL_TTF support based on Sam Lantinga's public domain
 *            SDL_TTF demo functions and algorithms
 *
 * 2002.06.06:
 * Mongoose - Created
 =================================================================*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cmath>

#ifdef MEMEORY_TEST
#include "memeory_test.h"
#endif

#ifdef USING_OPENGL
#include <SDL/SDL_opengl.h>
#else
#error "SDLSystem requires -DUSING_OPENGL"
#endif

#ifdef PS2_LINUX
#include "ps2.h"
#endif

#include "SDLSystem.h"


unsigned int *gWidth = 0x0;
unsigned int *gHeight = 0x0;

/* 500, 50 */
void glDrawGrid(float size, float step)
{
	float x, y;


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
}

void glDrawAxis(float length, float arrowLenght)
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


////////////////////////////////////////////////////////////
// Constructors
////////////////////////////////////////////////////////////

SDLSystem::SDLSystem() : System()
{
	mWindow = 0x0;
	gWidth = &m_width;
	gHeight = &m_height;
    mFirstMouseEvent = false;
}


SDLSystem::~SDLSystem()
{
}


////////////////////////////////////////////////////////////
// Public Accessors
////////////////////////////////////////////////////////////

unsigned int SDLSystem::getTicks()
{
	return SDL_GetTicks();
}


////////////////////////////////////////////////////////////
// Public Mutators
////////////////////////////////////////////////////////////

#ifdef FIXME
void SDLSystem::bindKeyCommand(const char *cmd, int key, int event)
{
	if (key > 255)
	{
		printf("Currently key event mapping only handles ASCII characters\n");
		return;
	}

	printf("Bound command '%s' -> event %i (0x%x key)\n", cmd, event, key);

	keyEvents[key] = event;
}
#endif



void SDLSystem::glPrintf2d(float x, float y, char *string)
{
#ifdef HAVE_SDL_TTF
#   ifdef FIXME
	// FIXME: Filler
	glBindTexture(GL_TEXTURE_2D, texture);
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(texMinX, texMinY); glVertex2i(x,   y  );
	glTexCoord2f(texMaxX, texMinY); glVertex2i(x+w, y  );
	glTexCoord2f(texMinX, texMaxY); glVertex2i(x,   y+h);
	glTexCoord2f(texMaxX, texMaxY); glVertex2i(x+w, y+h);
	glEnd();
#   endif
#endif
}


void SDLSystem::glPrintf3d(float x, float y, float z, char *string)
{
#ifdef HAVE_SDL_TTF_FIXME
	// FIXME: Filler
	// FIXME: Billboarding here requires a yaw jackass   =)
	glBindTexture(GL_TEXTURE_2D, texture);
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(texMinX, texMinY); glVertex2i(x,   y  );
	glTexCoord2f(texMaxX, texMinY); glVertex2i(x+w, y  );
	glTexCoord2f(texMinX, texMaxY); glVertex2i(x,   y+h);
	glTexCoord2f(texMaxX, texMaxY); glVertex2i(x+w, y+h);
	glEnd();
#endif
}


void SDLSystem::setGrabMouse(bool on)
{
	SDL_WM_GrabInput(on ? SDL_GRAB_ON : SDL_GRAB_OFF);
}


void SDLSystem::initVideo(unsigned int width, unsigned int height,
								  bool fullscreen)
{
	int flags; //, x, y;


	// Create GL context
	SDL_Init(SDL_INIT_VIDEO);
	printf("@ Created OpenGL Context...\n");
	atexit(SDL_Quit);

	m_width = width;
	m_height = height;

#ifndef __APPLE__
	if (!m_driver || !m_driver[0] || SDL_GL_LoadLibrary(m_driver) < 0)
	{
		SDL_ClearError();

		// Fallback 1
		if (SDL_GL_LoadLibrary("libGL.so") < 0)
		{
			SDL_ClearError();

			// Fallback 2
			if (SDL_GL_LoadLibrary("libGL.so.1") < 0)
			{
				fprintf(stderr, "initVideo> SDL_GL_LoadLibrary failed!\n");
				fprintf(stderr, "initVideo> Error is [%s].\n", SDL_GetError());
				shutdown(1);
			}
		}
	}
#endif

	flags = 0;

	flags |= SDL_OPENGL;

    mFullscreen = fullscreen;
	if (mFullscreen)
	{
		flags |= SDL_FULLSCREEN;
	}

    SDL_ShowCursor(SDL_DISABLE);
    setGrabMouse(true); // Always grab mouse!

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	mWindow = SDL_SetVideoMode(width, height, 16, flags);
	SDL_WM_SetCaption(VERSION, VERSION);
	//SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	SDL_EnableKeyRepeat(100, SDL_DEFAULT_REPEAT_INTERVAL);

#ifdef UNICODE_SUPPORT
	//@JML get Unicode value of key for international keyboard
	SDL_EnableUNICODE(1);
#endif

	// Start game renderer
	initGL();

	// Resize context
	resizeGL(width, height);
}


void SDLSystem::resize(unsigned int width, unsigned int height)
{
    int flags;

	GLfloat aspect;


	m_width = width;
	m_height = height;

	aspect = (GLfloat)width/(GLfloat)height;

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

    // gluPerspective is deprecated!
	// gluPerspective(m_fovY, aspect, m_clipNear, m_clipFar);
    // fix: http://stackoverflow.com/a/2417756
    GLfloat fH = tan(float(m_fovY / 360.0f * 3.14159f) ) * m_clipNear;
    GLfloat fW = fH * aspect;
    glFrustum(-fW, fW, -fH, fH, m_clipNear, m_clipFar);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Resize window
    flags = SDL_OPENGL;
    if (mFullscreen)
        flags |= SDL_FULLSCREEN;

	mWindow = SDL_SetVideoMode(width, height, 16, flags);

	// Resize context
	resizeGL(width, height);
}


void SDLSystem::runGame()
{
	SDL_Event event;
	unsigned int mkeys, mod, key;
	int btn;
	bool specialKey;


	for (;;)
	{
		// Pause for 10-20 ms
		SDL_Delay(10);

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				shutdown(0);
				break;
			case SDL_MOUSEMOTION:
				// Wrap motion
                if (!mFirstMouseEvent) {
                    mFirstMouseEvent = true;
                } else {
                    handleMouseMotionEvent(event.motion.xrel/2, event.motion.yrel/2);
                }
				break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				//handleMouseEvent(event.button.button, event.button.state,
				//			          event.button.x, event.button.y);

				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
					btn = SYS_MOUSE_LEFT;
					break;
				case SDL_BUTTON_RIGHT:
					btn = SYS_MOUSE_RIGHT;
					break;
				case SDL_BUTTON_MIDDLE:
					btn = SYS_MOUSE_MIDDLE;
					break;
				}

				if (event.button.state == SDL_PRESSED)
				{
					handleKeyPressEvent(btn, 0); // FIXME: mod not used
				}
				else
				{
					handleKeyReleaseEvent(btn, 0); // FIXME: mod not used
				}
				break;
			case SDL_KEYUP:
			case SDL_KEYDOWN:
				//SDL_GetMouseState(&x, &y); // Get cursor pos

				mkeys = (unsigned int)SDL_GetModState();
				mod = 0;

				if (mkeys & KMOD_LSHIFT)
					mod |= SYS_MOD_KEY_LSHIFT;

				if (mkeys & KMOD_RSHIFT)
					mod |= SYS_MOD_KEY_RSHIFT;

				if (mkeys & KMOD_LCTRL)
					mod |= SYS_MOD_KEY_LCTRL;

				if (mkeys & KMOD_RCTRL)
					mod |= SYS_MOD_KEY_RCTRL;

				if (mkeys & KMOD_LALT)
					mod |= SYS_MOD_KEY_LALT;

				if (mkeys & KMOD_RALT)
					mod |= SYS_MOD_KEY_RALT;

                if (mkeys & KMOD_LMETA)
					mod |= SYS_MOD_KEY_LMETA;

				if (mkeys & KMOD_RMETA)
					mod |= SYS_MOD_KEY_RMETA;

				key = event.key.keysym.sym;
				specialKey = false;

				switch (key)
				{
				case SDLK_F1:
					key = SYS_KEY_F1;
					specialKey = true;
					break;
				case SDLK_F2:
					key = SYS_KEY_F2;
					specialKey = true;
					break;
				case SDLK_F3:
					key = SYS_KEY_F3;
					specialKey = true;
					break;
				case SDLK_F4:
					key = SYS_KEY_F4;
					specialKey = true;
					break;
				case SDLK_F5:
					key = SYS_KEY_F5;
					specialKey = true;
					break;
				case SDLK_F6:
					key = SYS_KEY_F6;
					specialKey = true;
					break;
				case SDLK_F7:
					key = SYS_KEY_F7;
					specialKey = true;
					break;
				case SDLK_F8:
					key = SYS_KEY_F8;
					specialKey = true;
					break;
				case SDLK_F9:
					key = SYS_KEY_F9;
					specialKey = true;
					break;
				case SDLK_F10:
					key = SYS_KEY_F10;
					specialKey = true;
					break;
				case SDLK_F11:
					key = SYS_KEY_F11;
					specialKey = true;
					break;
				case SDLK_F12:
					key = SYS_KEY_F12;
					specialKey = true;
					break;
				case SDLK_UP:
					key = SYS_KEY_UP;
					specialKey = true;
					break;
				case SDLK_DOWN:
					key = SYS_KEY_DOWN;
					specialKey = true;
					break;
				case SDLK_RIGHT:
					key = SYS_KEY_RIGHT;
					specialKey = true;
					break;
				case SDLK_LEFT:
					key = SYS_KEY_LEFT;
					specialKey = true;
					break;
				case SDLK_PAGEDOWN:
					key = SYS_KEY_PAGEDOWN;
					specialKey = true;
					break;
				case SDLK_PAGEUP:
					key = SYS_KEY_PAGEUP;
					specialKey = true;
					break;
				}

#ifdef __APPLE__
                // Handle CMD+Q to quit in all circumstances
                if (key == 'q') {
                    if (mod & SYS_MOD_KEY_LMETA) {
                        shutdown(0);
                    }
                }
#endif

#ifdef UNICODE_SUPPORT
				// JML: if a std key was pressed get it ascii code
				if (!specialKey && key != 0)
				{
					if ((event.key.keysym.unicode & 0xFF80) == 0)
					{
						key= (unsigned int)(event.key.keysym.unicode & 0x7F);
					}
					else
					{
						key = 0;
					}
				}
#else
				// FIXME: Avoid passing modifers as a key, since the
				// consoles using this expect text characters, add unicode
				// support later when they're able to handle it
				if (key > 255 && key < 1000)
				{
					key = 0;
				}
#endif

				if (key == mConsoleKey)
				{
					if (event.type == SDL_KEYDOWN)
					{
						mConsoleMode = !mConsoleMode;
						// Tmp hack
						handleConsoleKeyPressEvent(mConsoleKey, 0);
					}
				}
				else if (mConsoleMode) // Console keying ( text input )
				{
					switch (event.type)
					{
					case SDL_KEYDOWN:
						handleConsoleKeyPressEvent(key, mod);
						break;
					default:
						;
					}
				}
				else if (mKeyEvents[key] != 0)// Bound key
				{
					//if (key < 255 && mKeyEvents[key] != 0)
					key = mKeyEvents[key];

					switch (event.type)
					{
					case SDL_KEYDOWN:
						handleBoundKeyPressEvent(key);
						break;
					default:
						handleBoundKeyReleaseEvent(key);
					}
				}
				else // 'Classic' key event handlers
				{
					switch (event.type)
					{
					case SDL_KEYDOWN:
						handleKeyPressEvent(key, mod);
						break;
					default:
						handleKeyReleaseEvent(key, mod);
					}
				}
				break;
			case SDL_VIDEORESIZE:
				resizeGL(event.resize.w, event.resize.h);
				gameFrame();
				break;
			}
		}

		// Game frame
		gameFrame();
	}
}


void SDLSystem::shutdown(int i)
{
	//SDL_QuitSubSystem(SDL_OPENGL);
	//SDL_Quit(); // Moved to atexit() call

	//#ifdef DEBUG_MEMEORY
	//printf("[Mongoose MEMEORY_DEBUG]\nUnfreed memory table:\n");
	//dump_memory_report();
	//#endif

	exit(0);
}


void SDLSystem::toggleFullscreen()
{
	if (mWindow)
	{
        mFullscreen = !mFullscreen;
		SDL_ShowCursor(SDL_DISABLE);

        // SDL_WM_ToggleFullScreen does not work on all platforms
        // eg. Mac OS X
		// SDL_WM_ToggleFullScreen(mWindow);

        // I added a mFullscreen flag to this class. Then I modified it's
        // resize() method to use the SDL_FULLSCREEN flag in the
        // SetVideoMode() call based on the mFullscreen flag.
        // Then, I modified this method to find out an available
        // resolution for the fullscreen mode.
        // Now you can see something when switching to Fullscreen,
        // but it's full of graphical glitches...? I don't know...
        // -- xythobuz 2013-12-31
        int width, height;
        if (mFullscreen) {
            m_old_width = m_width;
            m_old_height = m_height;
            SDL_Rect **dimensions = SDL_ListModes(NULL, SDL_OPENGL | SDL_FULLSCREEN);
            if (dimensions == NULL) {
                printf("Can't enter fullscreen!\n");
                mFullscreen = !mFullscreen;
            }
            if (dimensions != (SDL_Rect **)-1) {
                // TODO dont just use first available resolution...
                width = dimensions[0]->w;
                height = dimensions[0]->h;
            } else {
                // No restrictions, use current resolution
                width = m_width;
                height = m_height;
            }
        }
        if (!mFullscreen) {
            width = m_old_width;
            height = m_old_height;
        }
        resize(width, height);
	}
}


void SDLSystem::swapBuffersGL()
{
	SDL_GL_SwapBuffers();
}


////////////////////////////////////////////////////////////
// Unit Test code
////////////////////////////////////////////////////////////

#ifdef __UNIT_TEST__
int SDL::unitTest(int argc, char *argv[])
{
	return 0;
}


int main(int argc, char *argv[])
{
	SDL test;


	printf("[SDL class test]\n");

	return test._UnitTest(argc, argv);
}
#endif
