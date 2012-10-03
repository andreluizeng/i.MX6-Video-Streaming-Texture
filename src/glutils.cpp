/***************************************************************************
 *   Copyright (C) 2010 by Andre L. V. da Silva   									*
 *   andreluizeng@yahoo.com.br   														*
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "glutils.h"

//--------------------------------------------------------------------------------------
// Name: GLInit ()
// Desc: Initializate the EGL functions
//--------------------------------------------------------------------------------------
bool GLUtils::GLInit (void)
{
	static const EGLint s_configAttribs[] =
	{
		EGL_RED_SIZE,		5,
		EGL_GREEN_SIZE, 	6,
		EGL_BLUE_SIZE,		5,
		EGL_ALPHA_SIZE, 	EGL_DONT_CARE,
		EGL_SAMPLES,		0,
		EGL_DEPTH_SIZE, 	24,
		EGL_NONE
	};

	EGLint numconfigs;

	EGLint ContextAttribList[] =	{EGL_CONTEXT_CLIENT_VERSION, 2,	EGL_NONE};

	// Initialize Display - Get the default display.
	egldisplay = eglGetDisplay (EGL_DEFAULT_DISPLAY);
	eglInitialize (egldisplay, NULL, NULL);
	assert (eglGetError () == EGL_SUCCESS);
	
	//Initialize EGL.
	eglInitialize(egldisplay, NULL, NULL);
	assert (eglGetError () == EGL_SUCCESS);

	// EGL Surface Config
	eglChooseConfig (egldisplay, s_configAttribs, &eglconfig, 1, &numconfigs);
	assert (eglGetError () == EGL_SUCCESS);
	assert (numconfigs == 1);

	//You must pass in the file system handle to the linux framebuffer when creating a window
	EGLNativeDisplayType native_display = fbGetDisplay();
	EGLNativeWindowType  native_window  = fbCreateWindow(native_display, 0, 1, 1024, 768);

	eglsurface = eglCreateWindowSurface (egldisplay, eglconfig, native_window, NULL);
	assert (eglGetError () == EGL_SUCCESS);

	eglBindAPI (EGL_OPENGL_ES_API);

	// Create the EGL graphic Context
	EGLContext eglcontext = eglCreateContext (egldisplay, eglconfig, EGL_NO_CONTEXT, ContextAttribList);
	assert (eglGetError() == EGL_SUCCESS);

	eglMakeCurrent (egldisplay, eglsurface, eglsurface, eglcontext);
	assert (eglGetError() == EGL_SUCCESS);

	// clear buffer to the black color
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	assert (eglGetError() == EGL_SUCCESS);

	// Swap Buffers.
	// Brings to the native display the current render surface.
	eglSwapBuffers (egldisplay, eglsurface);
	assert (eglGetError () == EGL_SUCCESS);

	return true;
}

void GLUtils::GLEnd (void)
{
	eglMakeCurrent(egldisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglTerminate(egldisplay);
	eglReleaseThread ();
	
	return;
}

//--------------------------------------------------------------------------------------
// Name: CVEnd()
// Desc: Free OpenCV´s variable
//--------------------------------------------------------------------------------------
int GLUtils::Kbhit (void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();
    
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}


//--------------------------------------------------------------------------------------
// Name: LoadIdentityMatrix4x4
// Desc: Load a 4x4 indentity matrix
//--------------------------------------------------------------------------------------
void GLUtils::LoadIdentityMatrix4x4 (float *m)
{
	m[0] = 1;
	m[1] = 0;
	m[2] = 0;
	m[3] = 0;
	
	m[4] = 0;
	m[5] = 1;
	m[6] = 0;
	m[7] = 0;
	
	m[8] = 0;
	m[9] = 0;
	m[10] = 1;
	m[11] = 0;
	
	m[12] = 0;
	m[13] = 0;
	m[14] = 0;
	m[15] = 1;

	return;
}


//--------------------------------------------------------------------------------------
// Name: GenOrthoMatrix
// Desc: Generate a orthogonal matrix
//--------------------------------------------------------------------------------------
void GLUtils::GenOrthoMatrix(float left,float right,float bottom,float top,float zNear,float zFar, float *m)
{	
	float tx=-(right+left)/(right-left);
	float ty=-(top+bottom)/(top-bottom);
	float tz=-(zFar+zNear)/(zFar-zNear);
	LoadIdentityMatrix4x4 (m);
	m[0]=2/(right-left);
	m[5]=2/(top-bottom);
	m[10]=-2/(zFar-zNear);	
	m[12]=tx;
	m[13]=ty;
	m[14]=tz;

}

//--------------------------------------------------------------------------------------
// Name: GenPerspectiveMatrix
// Desc: Generate a perspective matrix
//--------------------------------------------------------------------------------------
void GLUtils::GenPerspectiveMatrix (float FOV, float n, float f, float aspect, float *m)
{

// n = near plane
// f = far plane

	float e = 1 / tan (FOV / 2);			
	float a = aspect;		// viewport

	// Build a perspective projection matrix
	m[ 0] = e;
	m[ 5] = e / a;
	m[10] = - (n + f) / (f - n);
	m[11] = - (2 * f * n) / (f - n);
	m[14] = -1.0;

	return;
}



