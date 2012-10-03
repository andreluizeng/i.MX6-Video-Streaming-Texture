#include <glplane.h>

GLvoid *pTexel;

// Fragment and vertex shaders code
const char *planefrag_shader_src =	/*"uniform sampler2D sampler2d; 				\n"
					"varying mediump vec2	myTexCoord; 			\n"
					"void main (void)					\n"
					"							\n"
					"{							\n"
					" gl_FragColor = texture2D(sampler2d,myTexCoord);	\n" 
					"}							\n";*/
	"#ifdef GL_FRAGMENT_PRECISION_HIGH				\n"
	"   precision highp float;					\n"
	"#else							\n"
	"   precision mediump float;				\n"
	"#endif							\n"
	"								\n"
//RGB
	"uniform sampler2D s_texture;				\n" 

	"varying   vec3      g_vVSColor;				\n"
	"varying   vec2 g_vVSTexCoord;				\n"
	"								\n"
	"void main()						\n"
	"{								\n"
	"    gl_FragColor = texture2D(s_texture,g_vVSTexCoord);	\n"
	"}								\n";


const char *planevertex_shader_src =/*	"attribute highp vec4	myVertex;			\n"
					"attribute mediump vec4	myUV;				\n"
					"uniform mediump mat4	myPMVMatrix;			\n"
					"varying mediump vec2	myTexCoord;			\n"
					"uniform   mat4 g_matModelView;				\n"
					"uniform   mat4 g_matProj;				\n"
					"attribute vec4 g_vPosition;				\n"
					"							\n"
					"void main(void)					\n"
					"{							\n"
					" vec4 vPositionES = g_matModelView * g_vPosition;	\n"
					" gl_Position  = g_matProj * vPositionES;		\n"
					" myTexCoord = myUV.st;					\n"
					"}							\n";*/

    "uniform   mat4 g_matModelView;				\n"
    "uniform   mat4 g_matProj;					\n"
    "								\n"
    "attribute vec4 g_vPosition;				\n"
    "attribute vec3 g_vColor;					\n"
    "attribute vec2 g_vTexCoord;				\n"
    "								\n"
    "varying   vec3 g_vVSColor;					\n"
    "varying   vec2 g_vVSTexCoord;				\n"
    "								\n"
    "void main()						\n"
    "{								\n"
    "    vec4 vPositionES = g_matModelView * g_vPosition;	\n"
    "    gl_Position  = g_matProj * vPositionES;		\n"
    "    g_vVSColor = g_vColor;					\n"
    "    g_vVSTexCoord = g_vTexCoord;				\n"
    "}								\n";

//--------------------------------------------------------------------------------------
// Name: CreatePlane
// Desc: Create a cube with  user-defined width, height and depth at (0, 0, 0)
// which means center of screen, since coordinates goes from (-x, -y, -z)  to  (x, y, z)
//--------------------------------------------------------------------------------------
void GLPlane::PlaneCreate (float w, float h)
{
	int i = 0;
	int j = 0;
	int error;

	float uvcoords[] =
	{
			0.0f, 1.0f,		// lower left
		  	1.0f, 1.0f,		// lower right
		  	0.0f, 0.0f,		// upper left
		  	0.0f, 0.0f,		// upper left
		   1.0f, 1.0f,		// lower right
			1.0f, 0.0f		// upper right
	};

	float xmid;
	float ymid;

	struct PlaneAxis
	{
		float coord[3];
	};

	PlaneAxis llf;	// lower left 		- front
	PlaneAxis lrf;	// lower right 	- front
	PlaneAxis ulf;	// upper left  	- front
	PlaneAxis urf;	// upper right		- front

	_width 	= 0.0;
	_height 	= 0.0;
	_depth 	= 0.0;
	_xpos 	= 0.0;	// translating
	_ypos 	= 0.0;
	_zpos 	= 0.0;

	_xangle	= 0.0;
	_yangle	= 0.0;
	_zangle	= 0.0;
	_xmov 	= 0.0;	// move (screen location)
	_ymov 	= 0.0;
	_zmov 	= 0.0;

	_texture_w = TEXTURE_W;
	_texture_h = TEXTURE_H;

	for (i = 0; i < 12; i++)
	{
		vertex[i] = 0;
	}

	for (i = 0; i < 12; i++)
	{
		texcoords[i] = 0;
	}

	_width  = w;
	_height = h;

	xmid = _width / 2.0;
	ymid = _height / 2.0;

	_xscale = 1;
	_yscale = 1;

	// lower left
	llf.coord[0] = -xmid;
	llf.coord[1] = -ymid;
	llf.coord[2] = 0;

	// lower right
	lrf.coord[0] = xmid;
	lrf.coord[1] = -ymid;
	lrf.coord[2] = 0;

	// upper left
	ulf.coord[0] = -xmid;
	ulf.coord[1] = ymid;
	ulf.coord[2] = 0;
	
	// upper right
	urf.coord[0] = xmid;
	urf.coord[1] = ymid;
	urf.coord[2] = 0;
	

	// -----------------------------
	// front face
	// lower left - front
	vertex[0] 	= llf.coord[0];
 	vertex[1] 	= llf.coord[1];
	vertex[2] 	= llf.coord[2];

	// lower right - front
	vertex[3] 	= lrf.coord[0];
	vertex[4] 	= lrf.coord[1];
	vertex[5] 	= lrf.coord[2];

	// upper left - front
	vertex[6] 	= ulf.coord[0];
	vertex[7] 	= ulf.coord[1];
	vertex[8] 	= ulf.coord[2];

	// upper left - front
	vertex[9] 	= ulf.coord[0];
	vertex[10] 	= ulf.coord[1];
	vertex[11] 	= ulf.coord[2];

	// lower right - front
	vertex[12] 	= lrf.coord[0];
	vertex[13] 	= lrf.coord[1];
	vertex[14] 	= lrf.coord[2];

	// upper right - front
	vertex[15] 	= urf.coord[0];
	vertex[16] 	= urf.coord[1];
	vertex[17] 	= urf.coord[2];

	// -----------------------------
	// UV coords - Front
	for (i = 0, j = 0; j < 12; j++, i++)
	{
		texcoords[i] = uvcoords[j];
	}

	LoadIdentityMatrix4x4 (_modelview_matrix);
	
	
	if (! CreateShaderProgram ())
	{
		printf ("\nError Creating Plane Shaders Program");
	}
	

	glBindTexture(GL_TEXTURE_2D, _texture);
	
	glTexDirectVIV (GL_TEXTURE_2D, w, h, GL_VIV_YV12, &pTexel);
	error = glGetError ();
	printf("glTexDirectVIV: 0x%08x\n", error);		

	return;
}

//--------------------------------------------------------------------------------------
// Name: PlaneTranslate
// Desc: Translates the cube along the axis
//--------------------------------------------------------------------------------------
void GLPlane::PlaneTranslate (float x, float z, float y)
{
 	float trans[16] = {0};
	float aux[16] = {0};
	int i;

	for (i = 0; i < 16; i++)
		aux[i] = _modelview_matrix[i];

	LoadIdentityMatrix4x4 (_modelview_matrix);
 	LoadIdentityMatrix4x4(trans);

	_xpos = x;
	_ypos = y;
	_zpos = z;


 	trans[12] = _xpos;
 	trans[13] = _ypos;
 	trans[14] = _zpos;

 	MultMatrix4x4 (_modelview_matrix, trans, aux);

	return;
}

//--------------------------------------------------------------------------------------
// Name: PlaneMove
// Desc: move the cube along a determined axis
//--------------------------------------------------------------------------------------
void GLPlane::PlaneMove	(int axis, float value)
{

	switch (axis)
	{
		case PLANE_X_AXIS:		_xmov = value;
									break;

		case PLANE_Y_AXIS:		_ymov = value;
									break;

		case PLANE_Z_AXIS:		_zmov = value;
									break;

		default:					printf("invalid axis \n");
									break;
	}

	
	_modelview_matrix[12] = _xmov;
	_modelview_matrix[13] = _ymov;
	_modelview_matrix[14] = _zmov;

	return;
}


//--------------------------------------------------------------------------------------
// Name: PlaneRotate
// Desc: Rotates the cube over the given axis
//--------------------------------------------------------------------------------------
void GLPlane::PlaneRotate	(int axis, float angle)
{
	float radians = PI_OVER_360*2*angle;
	float rotate[16] = {0};
	float aux[16] = {0};
	int i;

	for (i = 0; i < 16; i++)
		aux[i] = _modelview_matrix[i];

	LoadIdentityMatrix4x4 (_modelview_matrix);
	LoadIdentityMatrix4x4 (rotate);

	
	switch (axis)
	{
		case PLANE_X_AXIS:		_xangle = radians;
									rotate[5] 	= 	cos (_xangle);
									rotate[6] 	= 	sin (_xangle);
									rotate[9] 	= -sin (_xangle);
									rotate[10] 	= 	cos (_xangle);
									MultMatrix4x4 (_modelview_matrix, rotate, aux);
									break;

		case PLANE_Y_AXIS:		_yangle = radians;
									rotate[0] 	= 	cos(_yangle);
									rotate[2] 	= -sin(_yangle);
									rotate[8] 	= 	sin(_yangle);
									rotate[10] 	= 	cos(_yangle);
									MultMatrix4x4 (_modelview_matrix, rotate, aux);
									break;

		case PLANE_Z_AXIS:		_zangle = radians;
									rotate[0] 	= 	cos(_zangle);
									rotate[1] 	= 	sin(_zangle);
									rotate[4] 	= -sin(_zangle);
									rotate[5] 	= 	cos(_zangle);
									MultMatrix4x4 (_modelview_matrix, rotate, aux);
									break;

		default:					printf("invalid axis \n");
									break;
	}


	return;
}

//--------------------------------------------------------------------------------------
// Name: PlaneScale
// Desc: Scales the cube along the given axis
//--------------------------------------------------------------------------------------
void GLPlane::PlaneScale (int axis, float value)
{
	float aux[16] = {0};
	float scale[16] = {0};
	int i;


	for (i = 0; i < 16; i++)
		aux[i] = _modelview_matrix[i];

	LoadIdentityMatrix4x4 (_modelview_matrix);
	LoadIdentityMatrix4x4 (scale);

	switch (axis)
	{
		case PLANE_X_AXIS:	_xscale = value;
									scale[0] = _xscale;
									MultMatrix4x4 (_modelview_matrix, scale, aux);
									break;

		case PLANE_Y_AXIS:	_yscale = value;
									scale[5] = _yscale;
									MultMatrix4x4 (_modelview_matrix, scale, aux);
									break;

		default:					printf("invalid axis \n");
									break;
	}

	return;

}

//--------------------------------------------------------------------------------------
// Name: PlaneLoadIdentity
// Desc: Loads Identity matrix
//--------------------------------------------------------------------------------------
void GLPlane::PlaneLoadIdentity (void)
{
	LoadIdentityMatrix4x4 (_modelview_matrix);	

	return;
}

//--------------------------------------------------------------------------------------
// Name: PlaneReset
// Desc: Reset cube rotation and location
//--------------------------------------------------------------------------------------
void GLPlane::PlaneReset (void)
{
	_xangle = 0;
	_yangle = 0;
	_zangle = 0;

	_xpos = 0;
	_ypos = 0;
	_zpos = 0;

	_xmov = 0;
	_ymov = 0;
	_zmov = 0;

	PlaneRotate (PLANE_X_AXIS, _xangle);
	PlaneRotate (PLANE_Y_AXIS, _yangle);
	PlaneRotate (PLANE_Z_AXIS, _zangle);

	PlaneTranslate (_xpos, _ypos, _zpos);

	LoadIdentityMatrix4x4 (_modelview_matrix);

	return;
}

//--------------------------------------------------------------------------------------
void GLPlane::PlaneSetTexBuf (char *texture_data, int w, int h)
{

	glBindTexture(GL_TEXTURE_2D, _texture);
	
	memmove (pTexel, texture_data, w * h * 3/2);
	
	glTexDirectInvalidateVIV (GL_TEXTURE_2D);
	
	
	return;
}

//--------------------------------------------------------------------------------------
// Name: PlaneGetAxisAngle
// Desc: Get the angle value of rotation over a given axis
//--------------------------------------------------------------------------------------
float GLPlane::PlaneGetAxisAngle (int axis)
{
	float angle = 0;

	switch (axis)
	{
		case PLANE_X_AXIS:	angle = _xangle;
									break;

		case PLANE_Y_AXIS:	angle = _yangle;
									break;

		case PLANE_Z_AXIS: 	angle = _zangle;
									break;

		default:					printf("invalid axis \n");
									break;
	}

	return angle;
}

//--------------------------------------------------------------------------------------
// Name: PlaneGetAxisPos
// Desc: Get the position value of translation on a given axis
//--------------------------------------------------------------------------------------
float GLPlane::PlaneGetAxisPos (int axis)
{
	float pos = 0;

	switch (axis)
	{
		case PLANE_X_AXIS:	pos = _xpos;
									break;

		case PLANE_Y_AXIS:	pos = _ypos;
									break;

		case PLANE_Z_AXIS: 	pos = _zpos;
									break;

		default:					printf("invalid axis \n");
									break;
	}

	return pos;
}

//--------------------------------------------------------------------------------------
// Name: PlaneDraw
// Desc: Draws the previously created cube
//--------------------------------------------------------------------------------------
void GLPlane::PlaneDraw (void)
{
	// Set the shader program
	glUseProgram (_shader_program);

	// First gets the location of that variable in the shader using its name
	glUniformMatrix4fv (_modelview_location, 1, 0, _modelview_matrix);
	glUniformMatrix4fv (_projview_location,  1, 0, _projection_matrix);

	// Pass the vertex data
	glVertexAttribPointer(PLANE_VERTEX_ARRAY, 3, GL_FLOAT, 0, 0, vertex);
	glEnableVertexAttribArray(PLANE_VERTEX_ARRAY);

	// Pass the texture coordinates data
	glVertexAttribPointer (PLANE_TEXCOORD_ARRAY, 2, GL_FLOAT, 0, 0, texcoords);
	glEnableVertexAttribArray (PLANE_TEXCOORD_ARRAY);
	
	// enable depth test
	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);
	glDepthMask (GL_TRUE);

	// cull backside of polygons
	//glEnable(GL_CULL_FACE);
	//glCullFace (GL_BACK);
	glDisable(GL_CULL_FACE);

	// Binds this texture handle so we can load the data into it
	//glBindTexture(GL_TEXTURE_2D, _texture);
       /* Select Our Texture */
	glActiveTexture(GL_TEXTURE0);
	//Select eglImage
	//glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, g_imgHandle);
	

	glDrawArrays (GL_TRIANGLES, 0, 6);

	//disable client state
	glDisableVertexAttribArray	(PLANE_VERTEX_ARRAY);
	glDisableVertexAttribArray	(PLANE_TEXCOORD_ARRAY);

	LoadIdentityMatrix4x4 (_modelview_matrix);
	

	return;
}

void GLPlane::PlaneShowCoords (void)
{
	int i = 0;
	int j = 0;
	int k = 0;

	printf ("\nFront Face:\n");
	for (j = 0; j < 6; j++)
	{
		for (i = 0; i < 3; i++)
		{
			printf ("%lf ", vertex[k]);
			k++;
  		}
  		printf ("\n");
	}

	return;
}

//--------------------------------------------------------------------------------------
// Name: PlaneShowUVCoords
// Desc: Show cube UV coordinates
//--------------------------------------------------------------------------------------
void GLPlane::PlaneShowUVCoords (void)
{
	int i = 0;
	int j = 0;
	int k = 0;

	printf ("\nFront Face:\n");
	for (j = 0; j < 6; j++)
	{
		for (i = 0; i < 2; i++)
		{
			printf ("%lf ", texcoords[k]);
			k++;
  		}
  		printf ("\n");
	}

	return;
}

//--------------------------------------------------------------------------------------
// Name: PlaneShowModelMatrix
// Desc: Show cube modelview matrix
//--------------------------------------------------------------------------------------
void GLPlane::PlaneShowModelMatrix (void)
{
	int i;
	int j = 0;

	printf ("\nModelview Matrix:\n");

	for (i = 0; i < 16; i++)
	{
		if (j > 3)
		{
			printf ("\n");
			j = 0;
		}

		printf ("%lf ", _modelview_matrix[i]);
		j++;
	}

	return;
}

//--------------------------------------------------------------------------------------
// Name: MultMatrix4x4
// Desc: 4x4 Matrix Multiplication
//--------------------------------------------------------------------------------------
void GLPlane::MultMatrix4x4 (float *matC, float *matA, float *matB)
{
	matC[ 0] = matA[ 0] * matB[ 0] + matA[ 1] * matB[ 4] + matA[ 2] * matB[ 8] + matA[ 3] * matB[12];
	matC[ 1] = matA[ 0] * matB[ 1] + matA[ 1] * matB[ 5] + matA[ 2] * matB[ 9] + matA[ 3] * matB[13];
	matC[ 2] = matA[ 0] * matB[ 2] + matA[ 1] * matB[ 6] + matA[ 2] * matB[10] + matA[ 3] * matB[14];
	matC[ 3] = matA[ 0] * matB[ 3] + matA[ 1] * matB[ 7] + matA[ 2] * matB[11] + matA[ 3] * matB[15];
	matC[ 4] = matA[ 4] * matB[ 0] + matA[ 5] * matB[ 4] + matA[ 6] * matB[ 8] + matA[ 7] * matB[12];
	matC[ 5] = matA[ 4] * matB[ 1] + matA[ 5] * matB[ 5] + matA[ 6] * matB[ 9] + matA[ 7] * matB[13];
	matC[ 6] = matA[ 4] * matB[ 2] + matA[ 5] * matB[ 6] + matA[ 6] * matB[10] + matA[ 7] * matB[14];
	matC[ 7] = matA[ 4] * matB[ 3] + matA[ 5] * matB[ 7] + matA[ 6] * matB[11] + matA[ 7] * matB[15];
	matC[ 8] = matA[ 8] * matB[ 0] + matA[ 9] * matB[ 4] + matA[10] * matB[ 8] + matA[11] * matB[12];
	matC[ 9] = matA[ 8] * matB[ 1] + matA[ 9] * matB[ 5] + matA[10] * matB[ 9] + matA[11] * matB[13];
	matC[10] = matA[ 8] * matB[ 2] + matA[ 9] * matB[ 6] + matA[10] * matB[10] + matA[11] * matB[14];
	matC[11] = matA[ 8] * matB[ 3] + matA[ 9] * matB[ 7] + matA[10] * matB[11] + matA[11] * matB[15];
	matC[12] = matA[12] * matB[ 0] + matA[13] * matB[ 4] + matA[14] * matB[ 8] + matA[15] * matB[12];
	matC[13] = matA[12] * matB[ 1] + matA[13] * matB[ 5] + matA[14] * matB[ 9] + matA[15] * matB[13];
	matC[14] = matA[12] * matB[ 2] + matA[13] * matB[ 6] + matA[14] * matB[10] + matA[15] * matB[14];
	matC[15] = matA[12] * matB[ 3] + matA[13] * matB[ 7] + matA[14] * matB[11] + matA[15] * matB[15];

	return;
}

//--------------------------------------------------------------------------------------
// Name: LoadIdentityMatrix4x4
// Desc: Load a 4x4 indentity matrix
//--------------------------------------------------------------------------------------
void GLPlane::LoadIdentityMatrix4x4 (float *m)
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
// Name: GenTexture()
// Desc: Generate texture
//--------------------------------------------------------------------------------------
GLuint GLPlane::GenTextures (void)
{
	GLuint texture_id = 0;

	// Use tightly packed data
	//glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

	// Allocates one texture handle
	glGenTextures (1, &texture_id);

	return texture_id;
}

//--------------------------------------------------------------------------------------
// Name: PlaneCreateTex()
// Desc: alocates cube textures
//--------------------------------------------------------------------------------------
void GLPlane::PlaneCreateTex	(int texture_w, int texture_h)
{
	// textures
	_texture_w = texture_w;
	_texture_h = texture_h;

	_texture = 0;
	_texture			= GenTextures ();
	
	glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return;
}

//--------------------------------------------------------------------------------------
// Name: PlaneSetProjMatrix ()
// Desc: Set the prjection view matrix
//--------------------------------------------------------------------------------------
void GLPlane::PlaneSetProjMatrix (float *projection_matrix)
{
	_projection_matrix = projection_matrix;

	return;
}

//--------------------------------------------------------------------------------------
// Name: CreateShaderProgram ()
// Desc: Create the shader program needed to render the scene
//--------------------------------------------------------------------------------------
bool GLPlane::CreateShaderProgram (void)
{
	GLint shader_compiled_flag;

	_frag_shader_src = planefrag_shader_src;
	_vertex_shader_src = planevertex_shader_src;

	// Create the fragment shader object, Load the source code into it and Compile the source code
	_frag_shader = glCreateShader (GL_FRAGMENT_SHADER);
	glShaderSource (_frag_shader, 1, & _frag_shader_src, NULL);
	glCompileShader (_frag_shader);

	// Check if compilation succeeded
	glGetShaderiv (_frag_shader, GL_COMPILE_STATUS, &shader_compiled_flag);
	if (! shader_compiled_flag)
	{
		// An error happened, first retrieve the length of the log message
		int i32InfoLogLength, i32CharsWritten;
		glGetShaderiv(_frag_shader, GL_INFO_LOG_LENGTH, &i32InfoLogLength);

		// Allocate enough space for the message and retrieve it
		char* pszInfoLog = new char[i32InfoLogLength];
		glGetShaderInfoLog (_frag_shader, i32InfoLogLength, &i32CharsWritten, pszInfoLog);

		printf("Failed to compile fragment shader: %s", pszInfoLog);

		delete [] pszInfoLog;
		return false;
	}

	// Loads the vertex shader in the same way
	_vertex_shader = glCreateShader (GL_VERTEX_SHADER);
	glShaderSource (_vertex_shader, 1, &_vertex_shader_src, NULL);
	glCompileShader (_vertex_shader);
	glGetShaderiv (_vertex_shader, GL_COMPILE_STATUS, &shader_compiled_flag);
	if (! shader_compiled_flag)
	{
		int i32InfoLogLength, i32CharsWritten;
		glGetShaderiv (_vertex_shader, GL_INFO_LOG_LENGTH, &i32InfoLogLength);

		char* pszInfoLog = new char[i32InfoLogLength];
		glGetShaderInfoLog (_vertex_shader, i32InfoLogLength, &i32CharsWritten, pszInfoLog);


		printf ("Failed to compile vertex shader: %s", pszInfoLog);

		delete [] pszInfoLog;
		return false;
	}

	// Create the shader program
	_shader_program = glCreateProgram();

	// Attach the fragment and vertex shaders to it
	glAttachShader (_shader_program, _frag_shader);
	glAttachShader (_shader_program, _vertex_shader);


        // Init attributes BEFORE linking
        glBindAttribLocation(_shader_program, PLANE_VERTEX_ARRAY,   "g_vPosition" );
        glBindAttribLocation(_shader_program, PLANE_TEXCOORD_ARRAY,   "g_vTexCoord" );
	
	
	// Link the program
	glLinkProgram (_shader_program);

	// Check if linking succeeded in the same way we checked for compilation success
	GLint bLinked;
	glGetProgramiv (_shader_program, GL_LINK_STATUS, &bLinked);
	if (!bLinked)
	{
		int i32InfoLogLength, i32CharsWritten;
		glGetProgramiv (_shader_program, GL_INFO_LOG_LENGTH, &i32InfoLogLength);

		char* pszInfoLog = new char[i32InfoLogLength];
		glGetProgramInfoLog (_shader_program, i32InfoLogLength, &i32CharsWritten, pszInfoLog);

		printf ("Failed to link program: %s", pszInfoLog);

		delete [] pszInfoLog;
		return false;
	}

	// Sets the sampler2D variable to the first texture unit
	//glUniform1i (glGetUniformLocation (_shader_program, "sampler2d"), 0);

    // Get uniform locations
	_modelview_location = glGetUniformLocation (_shader_program, "g_matModelView");
	_projview_location  = glGetUniformLocation (_shader_program, "g_matProj");

	return true;
}

//--------------------------------------------------------------------------------------
// Name: PlaneDestroy
// Desc: free plane memory
//--------------------------------------------------------------------------------------
void GLPlane::PlaneDestroy (void)
{
	// Frees the texture
	glDeleteTextures(1, &_texture);

	// Frees the OpenGL handles for the program and the 2 shaders
	glDeleteProgram	(_shader_program);
	glDeleteShader		(_vertex_shader);
	glDeleteShader		(_frag_shader);

	return;
}


//--------------------------------------------------------------------------------------
// Name: PlaneGPUBufferAccess
// Desc: access the gpu buffer to write directly
//--------------------------------------------------------------------------------------
void PlaneGPUBufferAccess (void)
{
	glTexDirectVIV (0, 720, 480, 0, NULL);
	
	return;
}