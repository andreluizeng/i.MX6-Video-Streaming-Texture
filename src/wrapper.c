#include "wrapper.h"

int TexDirectVIVMap (GLenum Target, GLsizei Width, GLsizei Height, GLenum Format, GLvoid **Logical, GLvoid **Physical)
{

	glTexDirectVIVMap (Target, Width, Height, Format, Logical, Physical);
	
	return glGetError ();
}

int TexDirectVIV (GLenum Target, GLsizei Width, GLsizei Height, GLenum Format, GLvoid **Logical)
{

	glTexDirectVIV (Target, Width, Height, Format, Logical);

	return glGetError ();
}
