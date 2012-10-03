#ifndef __glcvplane_h__
#define __glcvplane_h__

#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <fcntl.h>
#include <malloc.h> 
#include <math.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

//#define EGL_USE_GLES2

#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <GLES2/gl2ext.h>
#include <EGL/eglext.h>

#include "vpu_io.h"
#include "vpu_lib.h"

//#include <EGL/eglvivante.h> 

#ifndef TRUE
	#define TRUE 1
#endif

#ifndef FALSE
	#define FALSE 0
#endif

#define PLANE_VERTEX_ARRAY 		0
#define PLANE_VERTEX_COLOR_ARRAY 	1
#define PLANE_TEXCOORD_ARRAY 		2


#define PLANE_X_AXIS 0
#define PLANE_Y_AXIS 1
#define PLANE_Z_AXIS 2

#ifndef PI_OVER_360
#define PI_OVER_360		0.00872664f
#endif

#ifndef TEXTURE_W
#define TEXTURE_W 320
#endif

#ifndef TEXTURE_H
#define TEXTURE_H 240
#endif

extern GLvoid *pTexel;

class GLPlane
{
	public:

		void 	PlaneCreate 		(float w, float h);
		void	PlaneCreateTex		(int texture_w, int texture_h);
		void 	PlaneTranslate		(float x, float z, float y);
		void 	PlaneMove		(int axis, float value);
		void 	PlaneRotate		(int axis, float angle);
		void 	PlaneScale		(int axis, float value);
		void 	PlaneReset		(void);
		void 	PlaneSetTexBuf		(char *texture_data, int w, int h);
		float	 PlaneGetAxisAngle 	(int axis);
		float	PlaneGetAxisPos		(int axis);
		void 	PlaneShowCoords		(void);
		void 	PlaneShowUVCoords	(void);
		void	PlaneDraw		(void);
		void 	PlaneShowModelMatrix	(void);
		void 	PlaneSetProjMatrix 	(float *projection_matrix);
		void 	PlaneLoadIdentity	(void);
		void 	PlaneDestroy		(void);
		void 	PlaneGPUBufferAccess 	(void);

	private:


		float _width;
		float _height;
		float _depth;

		float _xpos;
		float _ypos;
		float _zpos;

		float _xmov;
		float _ymov;
		float _zmov;

		float _xangle;
		float _yangle;
		float _zangle;

		float _xscale;
		float _yscale;
		float _zscale;

		float vertex[18];
		float	texcoords[12];

		// textures
		GLuint _texture;
		GLeglImageOES g_imgHandle;
		
		EGLDisplay _egldisplay;

		//modelview matrix
		float _modelview_matrix[16];

		// texture_w and h
		GLuint _texture_w;
		GLuint _texture_h;

		// shader program	
		GLuint _shader_program;

		// The vertex and fragment shadpenGL handles
		GLuint _vertex_shader;
		GLuint _frag_shader;

		// model and projection matrix view
		GLuint _modelview_location;
		GLuint _projview_location;

		const char *_frag_shader_src;
		const char *_vertex_shader_src;

		// projection view matrix
		float *_projection_matrix;

		void 	MultMatrix4x4 		(float *matC, float *matA, float *matB);
		void 	LoadIdentityMatrix4x4 	(float *m);
		GLuint	GenTextures 		(void);
		bool	CreateShaderProgram 	(void);
};

#endif
