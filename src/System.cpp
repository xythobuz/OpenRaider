/*!
 * \file System.cpp
 * \brief Mostly defines the interface of System implementations.
 *
 * Currently only SDL is used, but there was a GLUT implementation.
 *
 * \author Mongoose
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdarg.h>
#include <cmath>

#ifdef USING_OPENGL
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#endif

#ifdef HAVE_LIBFERIT
#include <ferit/Url.h>
#include <ferit/TcpProtocol.h>
#include <ferit/Http.h>
#include <ferit/Ftp.h>
#endif

#if defined(linux) || defined(__APPLE__)
#include <time.h>
#include <sys/time.h>
#endif

#ifdef MEMEORY_TEST
#include "memeory_test.h"
#endif

#ifdef PS2_LINUX
#include "ps2linux.h"
#endif

#include "System.h"

////////////////////////////////////////////////////////////
// Constructors
////////////////////////////////////////////////////////////

System::System()
{
	m_width = 800;
	m_height = 600;
	m_fastCard = true;

	m_driver = 0x0;

	m_clipFar  = 4000.0f;
	m_clipNear = 4.0f;
	m_fovY     = 45.0f;

	mConsoleMode = false;

	printf("[System.Core]\n");

	// Hack for bad Map class, as well as reserved commands
	addCommandMode("[System.Console]");
	mConsoleKey = '`';
	bindKeyCommand("+console", mConsoleKey, 0);

#ifdef WIN32
	setDriverGL("libGL32.dll");
#else
	setDriverGL("/usr/lib/libGL.so.1");
#endif
}


System::~System()
{
}


////////////////////////////////////////////////////////////
// Public Accessors
////////////////////////////////////////////////////////////

char *System::bufferString(const char *string, ...)
{
	int sz = 60;
	int n;
	char *text;
	va_list args;


	// Mongoose 2002.01.01, Only allow valid strings
	//   we must assume it's NULL terminated also if it passes...
	if (!string || !string[0])
	{
		return NULL;
	}

	text = new char[sz];

	va_start(args, string);

	// Mongoose 2002.01.01, Get exact size needed if the first try fails
	n = vsnprintf(text, sz, string, args);

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
			delete [] text;
			text = new char[sz];
			n = vsnprintf(text, sz, string, args);
			break;
		}
		else // glibc 2.0
		{
			sz *= 2;
			delete [] text;
			text = new char[sz];
			n = vsnprintf(text, sz, string, args);
		}
	}

	va_end(args);

	return text;
}


char *System::fullPath(const char *path, char end)
{
	unsigned int i, lenPath, lenEnv, len;
	char *env, *dir;


	if (!path || !path[0])
		return 0;

	if (path[0] == '~')
	{
#if defined(unix) || defined(__APPLE__)
		env = getenv("HOME");

		if (!env || !env[0])
		{
			return 0;
		}

		lenEnv = strlen(env);
		lenPath = strlen(path);
		len = lenEnv + lenPath;

		dir = new char[len+1];

		// Mongoose 2002.08.17, Copy ENV,  strip '~', append rest of path
		for (i = 0; i < len; ++i)
		{
			if (i < lenEnv)
			{
				dir[i] = env[i];
			}
			else
			{
				dir[i] = path[1+(i-lenEnv)];
			}
		}
#else
#error Platform not supported!
#endif
	}
	else
	{
		lenPath = strlen(path);
		dir = new char[lenPath+1];
		strncpy(dir, path, lenPath);

		i = lenPath;
	}

	// Make sure ends in "end" char
	if (end && dir[i-1] != end)
	{
		dir[i++] = end;
	}

	dir[i] = 0;

	return dir;
}


char *System::getFileFromFullPath(char *filename)
{
	int i, j, len;
	char *str;


	len = strlen(filename);

	for (i = len, j = 0; i > 0; --i, ++j)
	{
		if (filename[i] == '/' || filename[i] == '\\')
			break;
	}

	j--;

	str = new char[len - j + 1];

	for (i = 0; i < len - j; ++i)
	{
		str[i] = filename[i + len - j];
	}

	str[i] = 0;

	return str;
}


unsigned int System::getTicks()
{
	return system_timer(1);
}


int System::createDir(char *path)
{
#ifdef WIN32
	return _mkdir(path);
#else
	return mkdir(path, S_IRWXU | S_IRWXG);
#endif
}


int System::downloadToBuffer(char *urlString,
									  unsigned char **buffer, unsigned int *size)
{
#ifdef HAVE_LIBFERIT
	printf("ERROR: This build is libferit enabled, but func not implemented\n");
	return -1;
#else
	printf("ERROR: This build not libferit enabled, unable to download\n");
	return -1000;
#endif
}


int System::downloadToFile(char *urlString, char *filename)
{
#ifdef HAVE_LIBFERIT
	int err = 0;
	unsigned int timeout = 10;
	Url *url = 0x0;
	TcpProtocol *client = 0x0;


	if (!urlString || !urlString[0])
		return -1;

	url = new Url("");
	url->UrlToSession(urlString);
	url->Options(OPT_RESUME | OPT_QUIET);
	url->Outfile(url->Filename());

	switch (url->Protocol())
	{
	case FTP_SESSION:
		client = new Ftp(timeout);
		break;
	case HTTP_SESSION:
		client = new Http(timeout);
		break;
	default:
		printf("Sorry the protocol used in the URL isn't unsupported.\n");

		if (client)
		{
			delete client;
		}

		return -2;
	}

	if (client)
	{
		err = client->Download(url, NULL);
		delete client;
	}

	if (url)
		delete url;

	return err;
#else
	printf("ERROR: This build not libferit enabled, unable to download\n");
	return -1000;
#endif
}


////////////////////////////////////////////////////////////
// Public Mutators
////////////////////////////////////////////////////////////

unsigned int System::addCommandMode(const char *command)
{
	if (command && command[0] == '[')
	{
		mCmdModes.pushBack(command);
		return (mCmdModes.size() - 1);
	}
	else
	{
		return 0;
	}
}

//! \fixme Modifer support later
void System::bindKeyCommand(const char *cmd, unsigned int key, int event)
{
	printf("Bound command '%s' -> event %i (0x%x key)\n", cmd, event, key);
	mKeyEvents.Add(key, event);
}


void System::command(const char *cmd)
{
	bool modeFound = false;
    char *cmdbuf;


	if (!cmd || !cmd[0]) // Null command string
		return;

	if (cmd[0] == '[') // Set a mode, eg "[Engine.OpenGL.Driver]"
	{
		for (mCmdModes.start(); mCmdModes.forward(); mCmdModes.next())
		{
			if (strcmp(cmd, mCmdModes.current()) == 0)
			{
				mCommandMode = mCmdModes.getCurrentIndex();
				modeFound = true;
			}
		}

		if (!modeFound)
		{
			//	mCommandMode = 0;
			printf("Command> Unknown mode '%s'\n", cmd);
		}
	}
	else // Execute a command in current mode, eg "stat fps"
	{
        cmdbuf = new char[strlen(cmd) + 1];
        strncpy(cmdbuf, cmd, strlen(cmd) + 1);
		handleCommand(cmdbuf, mCommandMode);
	}
}


int System::loadResourceFile(const char *filename)
{
	char buffer[256];
	bool line_comment = false;
	FILE *f;
	char c;
	int i, j;


	f = fopen(filename, "r");

	if (!f)
	{
		perror(filename);
		return -1;
	}

	printf("Resource system \n");

	i = 0;
	buffer[0] = 0;

	// Strip out whitespace and comments
	while (fscanf(f, "%c", &c) != EOF)
	{
		if (line_comment && c != '\n')
			continue;

		if (i > 254)
		{
			printf("loadResourceFile> Overflow handled\n");
			i = 254;
		}

		switch (c)
		{
		case '\v':
		case '\t':
			break;
		case '#':
			buffer[i++] = 0;
			line_comment = true;
			break;
		case '\n':
			if (line_comment)
			{
				line_comment = false;
			}

			if (buffer[0] == 0)
			{
				i = 0;
				continue;
			}

			buffer[i] = 0;
			//printf("'%s'\n", buffer);

			// 'Preprocessor' commands
			if (buffer[0] == '@')
			{
				if (strncmp(buffer, "@include ", 9) == 0)
				{
					for (j = 9; j < i; ++j)
					{
						buffer[j-9] = buffer[j];
						buffer[j-8] = 0;
					}

					printf("Importing '%s'\n", buffer);

					loadResourceFile(fullPath(buffer, '/'));
				}
			}
			else
			{
				command(buffer);
			}

			i = 0;
			buffer[0] = 0;
			break;
		default:
			buffer[i++] = c;
		}
	}

	fclose(f);

	return 0;
}


void System::setDriverGL(const char *driver)
{
	unsigned int len;


	if (m_driver)
	{
		delete [] m_driver;
	}

	if (driver && driver[0])
	{
		len = strlen(driver);
		m_driver = new char[len+1];
		strncpy(m_driver, driver, len);
		m_driver[len] = 0;
	}
}


void System::setFastCardPerformance(bool is_fast)
{
	m_fastCard = is_fast;
}


void System::resetTicks()
{
	system_timer(0);
}


void System::initGL()
{
	char *s;

	// Print driver support information
	printf("\n\n\t## GL Driver Info 1 ##\n");
	printf("\tVendor     : %s\n", glGetString(GL_VENDOR));
	printf("\tRenderer   : %s\n", glGetString(GL_RENDERER));
	printf("\tVersion    : %s\n", glGetString(GL_VERSION));
	printf("\tExtensions : %s\n\n\n", (char*)glGetString(GL_EXTENSIONS));

	// Testing for goodies
	// Mongoose 2001.12.31, Fixed string use to check for bad strings
	s = (char*)glGetString(GL_EXTENSIONS);

	if (s && s[0])
	{
		printf("\tGL_ARB_multitexture       \t\t");

		if (strstr(s, "GL_ARB_multitexture"))
		{
			printf("YES\n");
		}
		else
		{
			printf("NO\n");
		}

		//glActiveTextureARB
		//glMultiTexCoord2fARB
		//glFogCoordfEXT

		printf("\tGL_EXT_texture_env_combine\t\t");

		if (strstr(s, "GL_EXT_texture_env_combine"))
		{
			printf("YES\n");
		}
		else
		{
			printf("NO\n");
		}
	}

	// Set up Z buffer
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Set up culling
	glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CW);
	glFrontFace(GL_CCW);
	//glCullFace(GL_FRONT);

	// Set background to black
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Disable lighting
	glDisable(GL_LIGHTING);

	// Set up alpha blending
	if (m_fastCard)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

		//glEnable(GL_ALPHA_TEST); // Disable per pixel alpha blending
		glAlphaFunc(GL_GREATER, 0);
	}
	else
	{
		glDisable(GL_BLEND);
		glDisable(GL_ALPHA_TEST);
	}

	glPointSize(5.0);

	// Setup shading
	glShadeModel(GL_SMOOTH);

	if (m_fastCard)
	{
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glHint(GL_FOG_HINT, GL_NICEST);
		glDisable(GL_COLOR_MATERIAL);
		glEnable(GL_DITHER);

		// AA polygon edges
		//glEnable(GL_POLYGON_SMOOTH);
		//glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

		glEnable(GL_POINT_SMOOTH);
	}
	else
	{
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
		glHint(GL_FOG_HINT, GL_FASTEST);
		glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_DITHER);
		glDisable(GL_POLYGON_SMOOTH);
		glDisable(GL_POINT_SMOOTH);
		glDisable(GL_FOG);
	}

	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_AUTO_NORMAL);
	glDisable(GL_LOGIC_OP);
	glDisable(GL_TEXTURE_1D);
	glDisable(GL_STENCIL_TEST);


	glDisable(GL_NORMALIZE);

	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_EDGE_FLAG_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	glPolygonMode(GL_FRONT, GL_FILL);

	glMatrixMode(GL_MODELVIEW);
}


void System::resizeGL(unsigned int w, unsigned int h)
{
	if (!w || !h)
	{
		printf("resizeGL> ERROR assertions 'w > 0', 'h > 0' failed\n");
		return;
	}

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Adjust clipping
    // gluPerspective is deprecated!
	// gluPerspective(m_fovY, ((GLdouble)w)/((GLdouble)h), m_clipNear, m_clipFar);
    // Fix: http://stackoverflow.com/a/2417756
    GLfloat fH = tan(float(m_fovY / 360.0f * 3.14159f)) * m_clipNear;
    GLfloat fW = fH * ((GLdouble)w)/((GLdouble)h);
    glFrustum(-fW, fW, -fH, fH, m_clipNear, m_clipFar);

	glMatrixMode(GL_MODELVIEW);
}

////////////////////////////////////////////////////////////
// Private Accessors
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// Private Mutators
////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////
// Gobal helper functions
////////////////////////////////////////////////////////////

// Mongoose 2002.03.23, Checks command to see if it's same
//   as symbol, then returns the arg list in command buffer
bool rc_command(const char *symbol, char *command)
{
	int i, j, lens, lenc;


	if (!symbol || !symbol[0] || !command || !command[0])
	{
		return false;
	}

	lens = strlen(symbol);

	if (strncmp(command, symbol, lens) == 0)
	{
		lenc = strlen(command);

		// lens+1 skips '=' or ' '
		for (i = 0, j = lens+1; j < lenc; ++i, ++j)
		{
			command[i] = command[j];
			command[i+1] = 0;
		}

		return true;
	}

	return false;
}


int rc_get_bool(char *buffer, bool *val)
{
	if (!buffer || !buffer[0])
	{
		return -1;
	}

	if (strncmp(buffer, "true", 4) == 0)
		*val = true;
	else if (strncmp(buffer, "false", 5) == 0)
		*val = false;
	else
		return -2;

	return 0;
}


unsigned int system_timer(int state)
{
	static struct timeval start;
	static struct timeval stop;
	static struct timeval total;
	static struct timezone tz;


	switch (state)
	{
	case 0:
		gettimeofday(&start, &tz);
		total.tv_sec = 0;
		total.tv_usec = 0;
		break;
	case 1:
		gettimeofday(&stop, &tz);

		if (start.tv_usec > stop.tv_usec)
		{
#ifdef OBSOLETE
			stop.tv_usec = (1000000 + stop.tv_usec);
#else
			stop.tv_usec = (1000 + stop.tv_usec);
#endif
			stop.tv_sec--;
		}

		stop.tv_usec -= start.tv_usec;
		stop.tv_sec -= start.tv_sec;

#ifdef OBSOLETE
		total.tv_sec += stop.tv_sec;
		total.tv_usec += stop.tv_usec;

		while (total.tv_usec > 1000000)
		{
			total.tv_usec -= 1000000;
			total.tv_sec++;
		}

		return total.tv_sec * 1000000 + total.tv_usec;
#else
		return (stop.tv_sec-start.tv_sec)*1000+(stop.tv_usec-start.tv_usec)/1000;
#endif
		break;
	}

	return 0;
}

