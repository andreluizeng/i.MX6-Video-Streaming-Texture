#include "gstcontrol.h"


//-------------------------------------------------------------------------------
// Name: GSTInit
// Arguments: argc, argv (command line arguments)
// Description: gstreamer initialization
// usage: object->GSTInit (&argc, &argv);
//-------------------------------------------------------------------------------
bool GSTVideoControl::GSTInit (void)
{
	
	__loop = g_main_loop_new (NULL, FALSE);
	
	if (! __loop)
	{
		printf ("\nError creating the g_main_loop_new");
		printf ("\nAborting...");
		return false;

	}
	
	__gst_uri = (char *) malloc (sizeof (char) * 100);
	
	if (! __gst_uri)
	{
		printf ("\nError allocating memory for the URI string");
		printf ("\nAborting...");
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------
// Name: GSTBuildPipeline
// Arguments: video_sink 		("fakesink", "mfw_v4lsink", "vro4l2sink", etc..)
//            handoffHandlerFunc	(a function that will be used a callback (copy buffer for ex.))
//            buscall_func 	 	(bus message func) 
// Description: pipeline creation
// usage: object->GSTBuildPipeline ("fakesink", NULL, BusCall1 (GstBus *bus, GstMessage *msg, gpointer data, gboolean ));
//-------------------------------------------------------------------------------
bool GSTVideoControl::GSTBuildPipeline (char *video_sink, GCallback handoffHandlerFunc, gboolean (*buscall_func) (GstBus *, GstMessage *, gpointer))
{
	GstElement* pFfConv 		= NULL;
	GstElement* pSinkBin 		= NULL;
	GstPad*     pFfConvSinkPad 	= NULL;
	GstPad*     pSinkPad 		= NULL;



    	__pipeline = NULL;
	__pipeline = gst_pipeline_new ("gst-player");

    	__bin = gst_element_factory_make ("playbin2", "bin");
    	__videosink = gst_element_factory_make (video_sink, "videosink");

	if (handoffHandlerFunc != NULL)
	{
		g_object_set (G_OBJECT (__videosink), "sync", TRUE, "signal-handoffs", TRUE, NULL);
		g_signal_connect (__videosink, "handoff", handoffHandlerFunc, NULL);
	}

    	g_object_set (G_OBJECT (__bin), "video-sink", __videosink, NULL);
	g_object_set (G_OBJECT (__bin), "volume", 0.5, NULL);

	__GSTLoopFunctionPTR = buscall_func;
        __bus = gst_pipeline_get_bus (GST_PIPELINE (__pipeline));
        gst_bus_add_watch (__bus, (*buscall_func), __loop);
	gst_object_unref (__bus);
	
	if (! __gst_uri)
	{
		printf ("\nError: URI not set");
		return false;
	}
       	g_object_set (G_OBJECT (__bin), "uri", __gst_uri, NULL);
	
	
	// colorspace conversion (only for fakesink)
	// it is add in a new bin, and then this bin is added to the first one (above)
	
	if (! strcmp (video_sink, "fakesink"))
	{
		pFfConv = gst_element_factory_make ("ffmpegcolorspace", "ffconv");

		if (!pFfConv)
		{
			printf("Couldn't create pFfConv\n");
		}
	
	
		//Put the fake sink and caps fiter into a single bin
		pSinkBin = gst_bin_new("SinkBin");
		if (!pSinkBin)
		{
			printf("Couldn't create pSinkBin\n");
			return FALSE;
		}
		gst_bin_add_many (GST_BIN (pSinkBin), pFfConv, __videosink, NULL);
		
		// RGB
		//gst_element_link_filtered (pFfConv, __videosink, gst_caps_new_simple ("video/x-raw-rgb","bpp",G_TYPE_INT,16, NULL));
		
		// YUV12
		gst_element_link_filtered (pFfConv, __videosink, gst_caps_new_simple ("video/x-raw-yuv", "format", GST_TYPE_FOURCC, GST_MAKE_FOURCC ('Y', 'V', '1', '2'), NULL));

		//In order to link the sink bin to the play been we have to create
		//a ghost pad that points to the capsfilter sink pad
		pFfConvSinkPad = gst_element_get_static_pad(pFfConv, "sink");
		if (!pFfConvSinkPad)
		{
			printf("Couldn't create pFfCovSinkPad\n");
			return FALSE;
		}

		pSinkPad = gst_ghost_pad_new( "sink", pFfConvSinkPad );
		if (!pSinkPad)
		{
			printf("Couldn't create pSinkPad\n");
			return FALSE;
		}
		gst_element_add_pad(pSinkBin, pSinkPad);
		gst_object_unref( pFfConvSinkPad);
 
		// force the SinkBin to be used as the video sink
		g_object_set (G_OBJECT (__bin), "video-sink", pSinkBin, NULL);
	}

	gst_bin_add (GST_BIN (__pipeline), __bin);
	
    	gst_element_set_state (__pipeline, GST_STATE_PAUSED);

	
	return TRUE;
}

//-------------------------------------------------------------------------------
// Name: GSTSetUri
// Arguments: *uri (string/path)
// Description: set the filestream to play
// usage: object->GSTSetUri ("lalalala.avi");
//-------------------------------------------------------------------------------
void GSTVideoControl::GSTSetURI(char* uri)
{
	snprintf (__gst_uri, 100, "%s", uri);
	g_object_set (G_OBJECT (__bin), "uri", __gst_uri, NULL);
	
	return;
}

//-------------------------------------------------------------------------------
// Name: __GSTLoopFunction
// Arguments: none
// Description: bus call watch
// FIXME: usage: should not be called anywhere (only for the thread)
//-------------------------------------------------------------------------------
void GSTVideoControl::GSTLoopFunction (void)
{
	while (1)
	{	
		while ((__bus_msg = gst_bus_pop (__bus))) 
		{
			// Call your bus message handler
			(*__GSTLoopFunctionPTR) (__bus, __bus_msg, NULL);
			gst_message_unref (__bus_msg);
		}
		
		usleep (10);
	}
	
	return;
}

bool GSTVideoControl::GSTThreadCreate (void* (*ptr)(void *))
{
	if (pthread_create (&__gst_loop_thread, 0, ptr, this)) return false;
	else return true;
}

//-------------------------------------------------------------------------------
// Name: GSTPlay
// Arguments: none
// Description: start playing
// usage: object->GSTPlay ();
//-------------------------------------------------------------------------------
void GSTVideoControl::GSTPlay(void)
{
	if (__pipeline)
	{
		gst_element_set_state (__pipeline, GST_STATE_PLAYING);
	}
	
    	else
	{
		printf ("\nError: no pipeline created\n");
	}
	
	
	return;
}

//-------------------------------------------------------------------------------
// Name: GSTStop
// Arguments: none
// Description: stop playing (needs to create another pipeline to play again)
// usage: object->GSTStop ();
//-------------------------------------------------------------------------------
void GSTVideoControl::GSTStop(void)
{
    	if (__pipeline)
    	{
        	gst_element_set_state (__pipeline, GST_STATE_NULL);
        	gst_object_unref (GST_OBJECT (__pipeline));
        	__pipeline = NULL;
    	}
    	else
	{
		printf ("\nError: no pipeline created\n");
	}
	
	return;
}

//-------------------------------------------------------------------------------
// Name: GSTPause
// Arguments: none
// Description: pause playing
// usage: object->GSTPause ();
//-------------------------------------------------------------------------------
void GSTVideoControl::GSTPause(void)
{
	if (__pipeline)
	{
		gst_element_set_state (__pipeline, GST_STATE_PAUSED);
	}
	
    	else
	{
		printf ("\nError: no pipeline created\n");
	}
	
	
	return;
}

//-------------------------------------------------------------------------------
// Name: GSTSeekAbsolute
// Arguments: value (position in time - miliseconds)
// Description: seek to a determined value
// usage: object->GSTSeekAbsolute(1000); 
//-------------------------------------------------------------------------------
void GSTVideoControl::GSTSeekAbsolute(guint64 value)
{
	guint64 nanoseconds;
	nanoseconds = value * 1000 * 1000;
	if (__pipeline)
	{
		GSTPause ();
		
		gst_element_seek (	__pipeline, 
					1.0,
					GST_FORMAT_TIME,
					GST_SEEK_FLAG_FLUSH,
					GST_SEEK_TYPE_SET, 
					nanoseconds,
					GST_SEEK_TYPE_NONE, 
					GST_CLOCK_TIME_NONE
				);
				
		GSTPlay ();
	}
	
    	else
	{
		printf ("\nError: no pipeline created\n");
	}
	
	
	return;
}

//-------------------------------------------------------------------------------
// Name: GSTFastForward
// Arguments: value (speed)
// Description: fast forward
// usage: object->GSTFastForward(2);
//-------------------------------------------------------------------------------
void GSTVideoControl::GSTFastForward(float value)
{
	gint64 x;
	
	if (__bin)
	{
		x = GSTQueryPosition();
		
		GSTPause ();
		
		gst_element_seek (	__pipeline,
					value,
					GST_FORMAT_TIME,
					GST_SEEK_FLAG_FLUSH,
					GST_SEEK_TYPE_SET, 
					x,
					GST_SEEK_TYPE_NONE, 
					GST_CLOCK_TIME_NONE
				);
		GSTPlay ();	
	}
	
    	else
	{
		printf ("\nError: no BIN created\n");
	}
	

	return;
}


//-------------------------------------------------------------------------------
// Name: GSTFastRewind
// Arguments: value (speed)
// Description: fast Rewind
// usage: object->GSTFastRewind(2);
//FIXME: not working
//-------------------------------------------------------------------------------
void GSTVideoControl::GSTFastRewind(float value)
{
	gint64 x;
	
	if (__bin)
	{
		x = GSTQueryPosition();
		
		GSTPause ();
		
		gst_element_seek (	__pipeline,
					-value,
					GST_FORMAT_TIME,
					GST_SEEK_FLAG_FLUSH,
					GST_SEEK_TYPE_SET, x,
					GST_SEEK_TYPE_NONE, 
					GST_CLOCK_TIME_NONE
				);
		GSTPlay ();	
	}
	
    	else
	{
		printf ("\nError: no BIN created\n");
	}
	

	return;
}

//-------------------------------------------------------------------------------
// Name: GSTQueryDuration
// Arguments: None
// Description: Returns the video total time duration
// usage: uint64 value = object->GSTQueryDuration ();
//-------------------------------------------------------------------------------
guint64 GSTVideoControl::GSTQueryDuration (void)
{
    	GstFormat format = GST_FORMAT_TIME;
    	gint64 cur = 0;

    	gst_element_query_duration (__pipeline, &format, &cur);

	if (format != GST_FORMAT_TIME)
        	return GST_CLOCK_TIME_NONE;

    	return cur;
}

//-------------------------------------------------------------------------------
// Name: GSTQueryPosition
// Arguments: None
// Description: Returns the video current position
// usage: uint64 value = object->GSTQueryPosition ();
//-------------------------------------------------------------------------------
guint64 GSTVideoControl::GSTQueryPosition (void)
{
            GstFormat format = GST_FORMAT_TIME;
            gint64 cur = 0;
        
            gst_element_query_position (__pipeline, &format, &cur);

        if (format != GST_FORMAT_TIME)
                return GST_CLOCK_TIME_NONE;

            return cur;
}


//-------------------------------------------------------------------------------
// Name: GSTGetPadWidth
// Arguments: GstPad
// Description: Returns the video's pad width
// usage: int width = object->GSTGetPadWidth (GstPad *)pPad);
//-------------------------------------------------------------------------------
gint GSTVideoControl::GSTGetPadWidth (GstPad* pVideoPad)
{
	gint iWidth = 0;
	GstCaps* pCaps = NULL;
	GstStructure* pStructure = NULL;

	pCaps = gst_pad_get_negotiated_caps (pVideoPad);
	if (pCaps)
	{
		pStructure = gst_caps_get_structure (pCaps, 0);
		gst_structure_get_int (pStructure, "width", &iWidth);
	}
	else
		g_print ("gst_pad_width() - Could not get caps for the pad!\n");

	return iWidth;
}


//-------------------------------------------------------------------------------
// Name: GSTGetPadHeight
// Arguments: GstPad
// Description: Returns the video's pad height
// usage: int width = object->GSTGetPadheight (GstPad *)pPad);
//-------------------------------------------------------------------------------
gint GSTVideoControl::GSTGetPadHeight (GstPad* pVideoPad)
{
	gint iHeight = 0;
	GstCaps* pCaps = NULL;
	GstStructure* pStructure = NULL;

	pCaps = gst_pad_get_negotiated_caps (pVideoPad);
	if (pCaps)
	{
		pStructure = gst_caps_get_structure (pCaps, 0);
		gst_structure_get_int (pStructure, "height", &iHeight);
	}
	else
		g_print ("gst_pad_height() - Could not get caps for the pad!\n");

	return iHeight;
}


//-------------------------------------------------------------------------------
// Name: GstDeInit
// Arguments: none
// Description: destroy the object
// usage: object->GSTFastRewind(2);
//-------------------------------------------------------------------------------
void GSTVideoControl::GSTDeInit(void)
{
	if (__pipeline)
	{
		gst_object_unref (GST_OBJECT (__pipeline));
	  	gst_element_set_state (__pipeline, GST_STATE_NULL);
	}

	
	if (__gst_uri) free (__gst_uri);
	
	pthread_cancel(__gst_loop_thread);
	gst_deinit();

	return;
}