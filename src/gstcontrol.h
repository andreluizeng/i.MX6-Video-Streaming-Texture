#ifndef __GSTCONTROL_H__
#define __GSTCONTROL_H__

#include <gst/gst.h>
#include <gst/interfaces/xoverlay.h>
#include <pthread.h>
#include <string.h>
#include <iostream>

class GSTVideoControl
{
	
	public:
		
		bool		GSTInit 		(void);
		bool 		GSTBuildPipeline 	(char *video_sink, GCallback handoffHandlerFunc, gboolean (*buscall_func) (GstBus *, GstMessage *, gpointer));
		void		GSTSetURI		(char *uri);
		void		GSTPlay		 	(void);
		void 		GSTStop 		(void);
		void 		GSTPause 		(void);
		void   		GSTSeekAbsolute  	(guint64 value);
		void 		GSTFastForward 		(float value);
		void 		GSTFastRewind 		(float value);

		guint64 	GSTQueryDuration 	(void);
		guint64 	GSTQueryPosition 	(void);

		gint 		GSTGetPadWidth 		(GstPad* pVideoPad);
		gint 		GSTGetPadHeight		(GstPad* pVideoPad);
		
		void		GSTDeInit 		(void);		

		bool 		GSTThreadCreate (void* (*ptr)(void *));
		
		// FIXME: these the variable and function below should be in the private area
		void 		GSTLoopFunction	(void);


	
	private:
		
		char 		*__gst_uri;
		pthread_t 	__gst_loop_thread;
		GstElement 	*__pipeline;
		GstElement	*__bin;
		GstElement 	*__videosink;
		GMainLoop 	*__loop;
		GstBus 		*__bus;
		GstMessage 	*__bus_msg;
		
		gboolean	(*__GSTLoopFunctionPTR) (GstBus *, GstMessage *, gpointer); // pointer to function


		
};

//void *GSTThreadCreate (void *obj, int *thread_id);
//void GSTCreateThread (GSTControl *obj);

#endif