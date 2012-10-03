#ifndef __wrapper_h__
#define __wrapper_h__

#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <GLES2/gl2ext.h>
#include <EGL/eglext.h>

 #ifdef __cplusplus
 extern "C" {
 #endif 

int TexDirectVIVMap (GLenum Target, GLsizei Width, GLsizei Height, GLenum Format, GLvoid **Logical, GLvoid **Physical);
int TexDirectVIV (GLenum Target, GLsizei Width, GLsizei Height, GLenum Format, GLvoid **Logical);

 #ifdef __cplusplus
 }
 #endif 

#endif