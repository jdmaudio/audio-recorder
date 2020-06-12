#include <gst/gst.h>
#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

/* Forward definition for the message handling*/
static gboolean bus_call (GstBus *bus, GstMessage *msg, gpointer data);
static gboolean timerfun(GstElement *pipeline);


/* Main function for audio pipeline initialization and looping streaming process  */
int
main (int argc, char *argv[]) {

    extern char *optarg; extern int optind;
    GMainLoop *loop;
    GstElement *pipeline, *audio_source, *wav_enc, *filesink; 
    GstBus *bus;
    guint bus_watch_id;
    GstCaps *caps;
    gboolean ret;

    char* fname;
    int rectime = 2;

    int c = 0;
    int tflag = 0, fflag = 0;

    while ((c = getopt(argc, argv, "t:f:")) != -1)
    switch (c) {
    case 'f':
	fname = optarg;
        fflag = 1;
        break;
    case 't':
        rectime = atoi(optarg);
        tflag = 1;
    }

    if (fflag == 0) {
    	fprintf(stderr, "%s: missing -f option\n", argv[0]);
        exit(1);
    }

    /* Initialization of gstreamer */
    gst_init (NULL, NULL);
    loop = g_main_loop_new (NULL, FALSE);

    /* Elements creation */
    pipeline     = gst_pipeline_new ("audio_stream");
    audio_source = gst_element_factory_make ("alsasrc", "audio_source");
    wav_enc      = gst_element_factory_make ("wavenc", "wav_encoder");
    filesink     = gst_element_factory_make ("filesink", "file_sink");

    if (!pipeline) {
        g_printerr ("Audio: Pipeline couldn't be created\n");
        return -1;
    }
    if (!audio_source) {
        g_printerr ("Audio: alsasrc couldn't be created\n");
        return -1;
    }
    if (!audio_source) {
        g_printerr ("Audio: wavenc couldn't be created\n");
        return -1;
    }
    if (!filesink) {
        g_printerr ("Audio: Output file couldn't be created\n");
        return -1;
    }

    /* Sets properties on source and sink */
    g_object_set (G_OBJECT (audio_source), "device", "default", NULL);
    g_object_set (G_OBJECT (filesink), "location", fname, NULL);

	/* Add a bus watch, so we get notified when a message arrives */
    bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
    bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);
    gst_object_unref (bus);

	/* Add elements to a bin */
    gst_bin_add_many (GST_BIN(pipeline), audio_source, wav_enc, filesink, NULL);

    caps = gst_caps_new_simple ("audio/x-raw", "format", G_TYPE_STRING, "S16LE",  "layout", G_TYPE_STRING, "interleaved", "rate", G_TYPE_INT, (int)384000, "channels", G_TYPE_INT, (int)1, NULL);
    ret = gst_element_link_filtered (audio_source, wav_enc, caps);
    if (!ret) {
        g_print ("audio_source and wav encoder couldn't be linked\n");
        gst_caps_unref (caps);
        return FALSE;
    }
    
    ret = gst_element_link(wav_enc, filesink);
    if (!ret) {
        g_print ("wave encoder and file sink couldn't be linked\n");
        return FALSE;
    }

    ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
		g_printerr ("Unable to set the pipeline to the playing state.\n");
		return FALSE;
	}

	g_print("Record time set to: %i seconds \n",rectime);

	/* Set record timer running */
	g_timeout_add (1000*rectime,(GSourceFunc)timerfun, pipeline);
	
    g_print ("streaming...\n");
    g_main_loop_run (loop);

    g_print ("Returned, stopping stream\n");
    gst_element_set_state (pipeline, GST_STATE_NULL);

    g_print ("Deleting pipeline\n");
    gst_object_unref (GST_OBJECT (pipeline));
    g_source_remove (bus_watch_id);
    g_main_loop_unref (loop);

    return 0;
}

/* Callback for processing messages from GStreamer */
static gboolean bus_call (GstBus *bus, GstMessage *msg, gpointer data){
  
  GMainLoop *loop = (GMainLoop *) data;

  switch (GST_MESSAGE_TYPE (msg)) {

    case GST_MESSAGE_EOS:
      g_print ("End of stream\n");
      g_main_loop_quit (loop);
      break;

    case GST_MESSAGE_ERROR: {
      gchar  *debug;
      GError *error;

      gst_message_parse_error (msg, &error, &debug);
      g_free (debug);

      g_printerr ("Error: %s\n", error->message);
      g_error_free (error);

      g_main_loop_quit (loop);
      break;
    }
    default:
      break;
  }

  return TRUE;
}

/* Called when timer reaches time */
static gboolean timerfun(GstElement *pipeline)
{
	gst_element_send_event(pipeline, gst_event_new_eos());
	return FALSE;
}
