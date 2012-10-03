/************************-***************************************************
 *   Copyright (C) 2011 by Andre L. V. da Silva   									*
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

//--------------------------------------------------------------------------------------
// File: main.cpp
//--------------------------------------------------------------------------------------
#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <fcntl.h>
#include <malloc.h>
#include <math.h>
#include <stdlib.h>

#include <termios.h>
#include <unistd.h>
#include <time.h>

#include <pthread.h>

#include "gstcontrol.h"
#include "glutils.h"
#include "glplane.h"

GLUtils *window = new GLUtils;

GLPlane *plane_video1 = new GLPlane;

GSTVideoControl *gst_video1_object = new GSTVideoControl;

int thread_id;

float matProj[16] = {0};
float matModel[9] = {0};


// video 1
gchar*	g_pcFrameBuffer1 = NULL;

void *WrapFunction1 (void *obj);
gboolean BusCall1 (GstBus *bus, GstMessage *msg, gpointer data);
void on_handoff1 (GstElement* pFakeSink, GstBuffer* pBuffer, GstPad* pPad);

int w;
int h;


void Render (void);
//--------------------------------------------------------------------------------------
// Name: main()
// Desc: main function (entry point)
//--------------------------------------------------------------------------------------
int main (int argc, char **argv)
{
	char filepath[100];
	
	struct timeval start, end;
	long mtime, seconds, useconds;    
		
	strcpy (filepath, "file:///home/gst-api/Sherlock.mp4");
	w = 1920;
	h = 1080;

	switch (argc)
	{
		case 2 :	// set the file path
				if (! strcmp (argv[1], "?"))
				{
					printf ("\n-----------------------------------\n");
					printf ("\nUsage: %s <file complete path> <width> <height>\n", argv[0]);
					printf ("\n-----------------------------------\n\n");
					return 0;
				}
				
				else
				{
					snprintf(filepath, 100, "file://%s", argv[1]);
				}
				break;
				
		case 3: 	snprintf(filepath, 100, "file://%s", argv[1]);
				w = atoi (argv[2]);
				break;

		case 4: 	snprintf(filepath, 100, "file://%s", argv[1]);
				w = atoi (argv[2]);
				h = atoi (argv[3]);
				break;
				
		default: break;

	}


	printf ("\nVideo file: %s", filepath);
	printf ("\nwidth: %d", w);
	printf ("\nheight: %d\n", h);

	window->GLInit();
	window->GenPerspectiveMatrix (0.5, 1.0, -1.0, 1.0, matProj);
	
	gst_init (&argc, &argv);
	
	gst_video1_object->GSTInit();

	plane_video1->PlaneCreateTex(w, h);
	plane_video1->PlaneCreate(w, h);
	plane_video1->PlaneSetProjMatrix(matProj);
	// g_pcFrameBuffer1 = (gchar*) malloc (w * h * 2); // rgb
	g_pcFrameBuffer1 = (gchar*) malloc (w * h * 3/2);   // yuv
	gst_video1_object->GSTSetURI(filepath);

	// create the pipe line using the mfw_v4lsin, no callback function and the message bus BusCall1
	gst_video1_object->GSTBuildPipeline((char *)"fakesink", (GCallback)on_handoff1, BusCall1);
	// create the thread for this GSTVideoControl (thread used for Buscall and loop)
	if (! gst_video1_object->GSTThreadCreate(WrapFunction1))
		return 0;

	// Change the GSTcontrol to playing state
	gst_video1_object->GSTPlay();

	
	// keep the app runing until the end_of_stream is not reached.
	while (!window->Kbhit ())
	{
		gettimeofday(&start, NULL);
		Render ();	
		gettimeofday(&end, NULL);
		
		// compute and print the elapsed time in millisec
		seconds  = end.tv_sec  - start.tv_sec;
		useconds = end.tv_usec - start.tv_usec;
		mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;

		printf( "\nProcess time = %ld ms\n\n", mtime);
	}
	
	gst_video1_object->GSTStop();
	
	free (g_pcFrameBuffer1);

	window->GLEnd();
	
	plane_video1->PlaneDestroy();
	
	gst_video1_object->GSTDeInit();
	
	return 0;
}



// VIDEO 1
// message bus for the current GSTVideoControl object, must have replicated for each created object
gboolean BusCall1 (GstBus *bus, GstMessage *msg, gpointer data)
{
  	switch (GST_MESSAGE_TYPE (msg))
	{
		case GST_MESSAGE_EOS:	g_print ("End of stream\n");
					gst_video1_object->GSTSeekAbsolute(0);
					break;

    		case GST_MESSAGE_ERROR:{
						gchar  *debug;
      						GError *error;
						gst_message_parse_error (msg, &error, &debug);
      						g_free (debug);
	
      						g_printerr ("Error: %s\n", error->message);
      						g_error_free (error);
	
					}
		      		
    		default:		break;
  	}

  return TRUE;

}

// just to create the pthread inside the GSTVideoControl class, this wrap function must be replicate for every object
void *WrapFunction1 (void *obj)
{
	GSTVideoControl *aux = reinterpret_cast <GSTVideoControl *> (obj);
	aux->GSTLoopFunction();
	
	return NULL;

}

//handoff function, called every frame
void on_handoff1 (GstElement* pFakeSink, GstBuffer* pBuffer, GstPad* pPad)
{
	int video_w;
	int video_h;
	
	video_w = gst_video1_object->GSTGetPadWidth (pPad);
	video_h = gst_video1_object->GSTGetPadHeight (pPad);
	
	gst_buffer_ref (pBuffer);
	//memmove (g_pcFrameBuffer1, GST_BUFFER_DATA (pBuffer), video_w * video_h * 2);
	g_memmove (g_pcFrameBuffer1, GST_BUFFER_DATA (pBuffer), video_w * video_h * 3/2);
	gst_buffer_unref (pBuffer);
}


void Render (void)
{
	
	
	static float rotationx1 = 0;
	static float rotationy1 = 0;
	static float rotationz1 = 0;

		
	glClearColor (1.0, 1.0, 1.0, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (rotationx1 == 360) rotationx1 = 0;
	else rotationx1 = rotationx1 + 0.3;
	if (rotationy1 == 360) rotationy1 = 0;
	else rotationy1 = rotationy1 + 0.7;
	if (rotationz1 == 360) rotationz1 = 0;
	else rotationz1 = rotationz1 + 0.5;

	
	//plane_video1->PlaneMove(PLANE_X_AXIS, -350);
	//plane_video1->PlaneMove(PLANE_Y_AXIS, -350);
	plane_video1->PlaneMove(PLANE_Z_AXIS, -3000);
	plane_video1->PlaneRotate(PLANE_X_AXIS, rotationx1);
	plane_video1->PlaneRotate(PLANE_Y_AXIS, rotationy1);
	plane_video1->PlaneRotate(PLANE_Z_AXIS, rotationz1);
	plane_video1->PlaneSetTexBuf(g_pcFrameBuffer1, w, h);
	plane_video1->PlaneDraw();
	
	// Swap Buffers.
	// Brings to the native display the current render surface.
	eglSwapBuffers (window->egldisplay, window->eglsurface);
	assert (eglGetError () == EGL_SUCCESS);
	return;
}