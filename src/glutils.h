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

#ifndef __glutils_h__
#define __glutils_h__

#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <fcntl.h>
#include <malloc.h> 
#include <math.h> 
#include <stdlib.h>

//#define EGL_USE_GLES2

#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <GLES2/gl2ext.h>
#include <EGL/eglext.h>
//#include <EGL/eglfslext.h>

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>


#define VERTEX_ARRAY	0
#define TEXCOORD_ARRAY	1

#ifndef TRUE
	#define TRUE 1
#endif

#ifndef FALSE
	#define FALSE 0
#endif

class GLUtils
{
	public:
		EGLDisplay	egldisplay;
		EGLConfig	eglconfig;
		EGLSurface	eglsurface;
		EGLContext	glcontext;

		bool 		GLInit						(void);
		void 		GLEnd							(void);
		int 		Kbhit							(void);
		void		LoadIdentityMatrix4x4 	(float *m);
		void 		GenOrthoMatrix				(float left,float right,float bottom,float top,float zNear,float zFar, float *m);
		void 		GenPerspectiveMatrix 	(float FOV, float n, float f, float aspect, float *m);

	private:


};

#endif

