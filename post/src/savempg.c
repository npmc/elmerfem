//=============================================================================
// savempg.c
//
// Module for compressing and saving ElmerPost-pictures in MPEG1 format
//
// Compile e.g. as follows (you'll need ffmpeg installed in $FFMPEG):
//
//   MinGW: 
//
//     > gcc -shared -O -I$FFMPEG/include -L$FFMPEG/lib -o savempg.dll 
//               savempg.c -lopengl32 -ltcl84 -lavcodec -lavutil -lz
//
//   Linux:
//
//     > more or less the same (-lGL -ltcl -lavcodec -lavutil -lz)
//
// Note that the libraries required depend on the libavcodec build.
//
// Copy the shared library into $ELMER_POST_HOME/modules and run ElmerPost
//
// Usage:
//
//   Elmer-Post: savempg status file
//
// where status is either "start", "append" or "stop". File defaults to 
// "elmerpost.mpg" (this argument is neglected unless status == start).
//
// Parsed together from screensave.c && the ffmpeg samples
//
// Written by: ML, 30. Sept. 2007
//=============================================================================
#ifdef HAVE_AV_CONFIG_H
#undef HAVE_AV_CONFIG_H
#endif

#if defined(WIN32) || defined(win32)
#include <windows.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <GL/gl.h>
#include <tcl.h>

#if !(defined(WIN32) || defined(win32))
#include <errno.h>
extern int errno;
#endif

#include <ffmpeg/avcodec.h>

#define INBUF_SIZE    4096

#define STATE_READY   0
#define STATE_STARTED 1

#undef MPEG4

void SetMessage( Tcl_Interp *interp, char *message ) {
#if defined(WIN32) || defined(win32)
  sprintf( interp->result, "savempg: %s\n", message );
#else
  sprintf( interp->result, "savempg: %s:\n%s\n", message, strerror(errno) );
#endif
}

static int SaveMPG( ClientData cl,Tcl_Interp *interp,int argc,char **argv ) {
  static AVCodec *codec;
  static AVCodecContext *c = NULL;
  static AVFrame *picture, *RGBpicture;
  static int out_size, size, stride, y, outbuf_size;
  static int nx, ny, ox, oy, viewp[4];
  static int i_state = STATE_READY;
  static int initialized = 0;
  static int count_frames = 0;
  static uint8_t *outbuf, *picture_buf;
  static char fname[256];
  static FILE *outfile;
  static unsigned char *buffer, *rowbuffer;
  static char *state;
  static int bitrate = 400000;

  if( argc<2 ) {
    SetMessage( interp, "too few arguments" );
    return TCL_ERROR;
  }

  state = argv[1];
  if( !strncmp( state, "bitrate", 7 ) ) {
    if( argc >= 3 )
      bitrate = atoi( argv[2] );
    return TCL_OK;
  } 
  //===========================================================================
  //                           START COMPRESSION
  //===========================================================================
  else if( !strncmp( state, "start", 5 ) ) {
    
    // Can't start if already running:
    //----------------------------------
    if( i_state != STATE_READY ) {
      SetMessage( interp, "already started" );
      return TCL_ERROR;
    }

    // Detemine output file name:
    //---------------------------
    if( argc < 3 ) {
#if defined(MPEG4)
      strcpy( fname, "elmerpost.raw" );
#else
      strcpy( fname, "elmerpost.mpg" );
#endif
    } else {
      strncpy( fname, argv[2], 256 );
    }
    
    // Open output file:
    //------------------
    if( (outfile = fopen(fname, "wb")) == NULL ) {
      SetMessage( interp, "can't open file" );
      return TCL_ERROR;
    }

    fprintf( stdout, "Saving frames to: %s\n", fname );
    fprintf( stdout, "libavcodec version: %s\n", 
	     AV_STRINGIFY(LIBAVCODEC_VERSION) );
    fprintf( stdout, "libavutil version: %s\n", 
	     AV_STRINGIFY(LIBAVUTIL_VERSION) );
    fflush( stdout );

    count_frames = 0;

    // Determine size:
    //-----------------
    glGetIntegerv( GL_VIEWPORT, viewp );
    ox = viewp[0];
    oy = viewp[1];
    nx = viewp[2]+1;
    ny = viewp[3]+1;
    stride = 3*nx;

    // Must be even:
    //--------------
    if( nx % 2 || ny % 2 ) {
      SetMessage( interp, "view port must be even" );
      fprintf( stdout, "State: stopped\n" );
      fflush( stdout );
      fclose( outfile );
      return TCL_ERROR;
    }

    // Allocate memory for RGB data:
    //------------------------------
    if ( !(buffer = (unsigned char *)malloc(3*nx*ny)) ||
	 !(rowbuffer = (unsigned char *)malloc(3*nx)) ) {
      SetMessage( interp, "can't allocate memory" );
      fclose( outfile );
      return TCL_ERROR;
    }

    // Initialize libavcodec:
    //-----------------------
    if( !initialized ) {
      avcodec_init();
      avcodec_register_all();
      initialized = 1;
    }

    // Choose MPEG1:
    //--------------
#if defined(MPEG4)
    codec = avcodec_find_encoder( CODEC_ID_MPEG4 );
#else
    codec = avcodec_find_encoder( CODEC_ID_MPEG1VIDEO );
#endif

    if( !codec ) {
      SetMessage( interp, "can't find codec" );
      free( buffer );
      free( rowbuffer );
      fclose( outfile );
      return TCL_ERROR;
    }
    
    // Init codec context etc.:
    //--------------------------
    c = avcodec_alloc_context();
    picture = avcodec_alloc_frame();
    RGBpicture = avcodec_alloc_frame();

    c->bit_rate = bitrate;
    c->width = nx;
    c->height = ny;
    c->time_base = (AVRational){ 1, 25 };
    c->gop_size = 10;
    c->max_b_frames = 1;
    c->pix_fmt = PIX_FMT_YUV420P;
  
    if( avcodec_open( c, codec ) < 0 ) {
      SetMessage( interp, "can't open codec" );
      free( buffer );
      free( rowbuffer );
      fclose( outfile );
      return TCL_ERROR;
    }

    outbuf_size = 200000;
    size = c->width * c->height;

    if( !(outbuf = malloc(outbuf_size)) ||
	!(picture_buf = malloc(3*(size/2))) ) {
      SetMessage( interp, "can't allocate memory" );
      free( buffer );
      free( rowbuffer );
      fclose( outfile );      
      return TCL_ERROR;
    }
    
    picture->data[0] = picture_buf;
    picture->data[1] = picture->data[0] + size;
    picture->data[2] = picture->data[1] + size / 4;
    picture->linesize[0] = c->width;
    picture->linesize[1] = c->width / 2;
    picture->linesize[2] = c->width / 2;
    
    // Set state "started":
    //----------------------
    i_state = STATE_STARTED;
    fprintf( stdout, "State: started\n" );
    fflush( stdout );
    
    return TCL_OK;
  } 

  //===========================================================================
  //                             COMPRESS FRAME
  //===========================================================================
  else if( !strncmp( state, "append", 6) ) {

    // Can't compress if status != started:
    //-------------------------------------
    if( i_state != STATE_STARTED ) {
      SetMessage( interp, "not started" );
      return TCL_ERROR;
    }

    // Read RGB data:
    //---------------
    glReadBuffer( GL_FRONT );
    glReadPixels( ox, oy, nx, ny, GL_RGB, GL_UNSIGNED_BYTE, buffer );

    // The picture is upside down - flip it:
    //---------------------------------------
    for( y=0; y<ny/2; y++ ) {
      unsigned char *row1 = buffer + stride * y;
      unsigned char *row2 = buffer + stride * (ny-1-y);
      memcpy( rowbuffer, row1, stride );
      memcpy( row1, row2, stride );
      memcpy( row2, rowbuffer, stride );
    }

    // Convert to YUV:
    //----------------
    RGBpicture->data[0] = buffer;
    RGBpicture->data[1] = buffer;
    RGBpicture->data[2] = buffer;
    RGBpicture->linesize[0] = stride;
    RGBpicture->linesize[1] = stride;
    RGBpicture->linesize[2] = stride;

    img_convert( (AVPicture*)picture, PIX_FMT_YUV420P, 
		 (AVPicture*)RGBpicture, PIX_FMT_RGB24, 
		 nx, ny );

    // Encode frame:
    //--------------
    out_size = avcodec_encode_video( c, outbuf, outbuf_size, picture );
    count_frames++;

    fprintf( stdout, "encoded frame %5d (size:%6d bytes)\n", 
	     count_frames, out_size );
    fflush( stdout );

    fwrite( outbuf, 1, out_size, outfile );
    
    return TCL_OK;
  } 

  //===========================================================================
  //                           STOP COMPRESSION
  //===========================================================================
  else if( !strncmp( state, "stop", 4) ) {

    // Can't stop if status != started:
    //---------------------------------
    if( i_state != STATE_STARTED ) {
      SetMessage( interp, "not started" );
      return TCL_ERROR;
    }

    // Get the delayed frames:
    //-------------------------
    for( ; out_size; ) {
      fflush( stdout );
      out_size = avcodec_encode_video( c, outbuf, outbuf_size, NULL );
      count_frames++;
      fprintf( stdout, "wrote frame   %5d (size:%6d bytes)\n", 
	       count_frames, out_size );
      fwrite( outbuf, 1, out_size, outfile );
    }
    
    // Add sequence end code:
    //-----------------------
#if !defined(MPEG4)
    outbuf[0] = 0x00;
    outbuf[1] = 0x00;
    outbuf[2] = 0x01;
    outbuf[3] = 0xb7;
    fwrite( outbuf, 1, 4, outfile );
#endif
    fclose( outfile );

    free( picture_buf );
    free( outbuf );
    
    // Finalize:
    //-----------
    avcodec_close( c );
    av_free( c );
    av_free( picture );
    av_free( RGBpicture );
    
    i_state = STATE_READY;
    fprintf( stdout, "State: stopped\n" );
    fflush( stdout );

    free( buffer );
    free( rowbuffer );
    
    return TCL_OK;
  } 

  //===========================================================================
  //                              UNKNOWN STATE
  //===========================================================================
  else {
    SetMessage( interp, "unknown request" );
    return TCL_ERROR;
  }

  // We should never ever arrive at this point:
  //-------------------------------------------
  SetMessage( interp, "unknown error" );
  return TCL_ERROR;
}

#if defined(WIN32) || defined(win32)
__declspec(dllexport)
#endif
     
int Savempg_Init( Tcl_Interp *interp ) {
  Tcl_CreateCommand( interp, "savempg", (Tcl_CmdProc *)SaveMPG,
		     (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL );
  return TCL_OK;
}
