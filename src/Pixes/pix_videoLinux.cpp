////////////////////////////////////////////////////////
//
// GEM - Graphics Environment for Multimedia
//
// zmoelnig@iem.kug.ac.at
//
// Implementation file
//
//    Copyright (c) 1997-1998 Mark Danks.
//    Copyright (c) G�nther Geiger.
//    Copyright (c) 2001-2002 IOhannes m zmoelnig. forum::f�r::uml�ute. IEM
//    For information on usage and redistribution, and for a DISCLAIMER OF ALL
//    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.
//
/////////////////////////////////////////////////////////
/*
    this is an attempt at a Linux version of pix_video by Miller Puckette.
    Anyone conversant in c++ will probably howl at this.  I'm uncertain of
    several things.
    
    First, the #includes I threw in pix_video.h may not all be necessary; I
    notice that far fewer are needed for the other OSes.
    
    Second, shouldn't the os-dependent state variables be "private"?  I
    followed the lead of the other os-dependent state variables.  Also,
    I think the indentation is goofy but perhaps there's some reason for it.

    Third, I probably shouldn't be using sprintf to generate filenames; I
    don't know the "modern" c++ way to do this.
    
    Fourth, I don't know why some state variables 
    show up as "arguments" in the pix_video :: pix_video().
     
    This code is written with the "bttv" device in mind, which memory mapes
    images up to 24 bits per pixel.  So we request the whole 24 and don't
    settle for anything of lower quality (nor do we offer anything of higher
    quality; it seems that Gem is limited to 32 bits per pixel including
    alpha.)  We take all video images to be opaque by setting the alpha
    channel to 255.

*/
#ifdef __linux__
    
#include "pix_videoLinux.h"
#include "Base/GemCache.h"

CPPEXTERN_NEW_WITH_TWO_ARGS(pix_videoLinuxT,t_floatarg, A_DEFFLOAT, t_floatarg, A_DEFFLOAT)

#define BYTESIN 3

/////////////////////////////////////////////////////////
//
// pix_videoLinuxT
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_videoLinuxT :: pix_videoLinuxT(t_floatarg w = 320, t_floatarg h = 240) :
     m_width((int)w),
     m_height((int)h),
     m_channel(COMPOSITEIN),
     m_norm(VIDEO_MODE_AUTO),
     m_devicenum(DEVICENO)
{
  if (!m_width)m_width=64;
  if (!m_height)m_height=64;

  //  m_pixBlock.image.reallocate();
  m_pixBlock.image.notowned=1;
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_videoLinuxT :: ~pix_videoLinuxT()
{
  if (m_haveVideo)stopTransfer();
}

/////////////////////////////////////////////////////////
// render
//
/////////////////////////////////////////////////////////
void *pix_videoLinuxT :: capturing(void*you)
{
  pix_videoLinuxT *me=(pix_videoLinuxT *)you;
  unsigned char *pixp;
  while(me->m_continue_thread){

  me->frame = !me->frame;

    me->vmmap[me->frame].width = me->m_pixBlock.image.xsize + 
      me->myleftmargin + me->myrightmargin;
    me->vmmap[me->frame].height = me->m_pixBlock.image.ysize + 
      me->mytopmargin + me->mybottommargin;
    
   /* syncing */
    if (ioctl(me->tvfd, VIDIOCSYNC, &me->vmmap[me->frame].frame) < 0)
    {
	perror("VIDIOCSYNC");
	//me->m_haveVideo = 0;me->stopTransfer();
    }

    /* capturing */
    if (ioctl(me->tvfd, VIDIOCMCAPTURE, &me->vmmap[me->frame]) < 0)
      {
    	if (errno == EAGAIN)
	  fprintf(stderr, "can't sync (no video source?)\n");
    	else 
	  perror("VIDIOCMCAPTURE");
	if (ioctl(me->tvfd, VIDIOCMCAPTURE, &me->vmmap[me->frame]) < 0)
	  perror("VIDIOCMCAPTURE2");
      
	post("frame %d %d, format %d, width %d, height %d\n",
	     me->frame, me->vmmap[me->frame].frame, me->vmmap[me->frame].format,
	     me->vmmap[me->frame].width, me->vmmap[me->frame].height);

	//me->stopTransfer();me->m_haveVideo = 0;
      }
    me->m_frame_ready = 1;
    me->last_frame=me->frame;
  }
  return NULL;
}
void pix_videoLinuxT :: render(GemState *state){
  unsigned char *newimage = videobuf + vmbuf.offsets[last_frame];
  int row, column;
 
  if (!m_frame_ready)  m_pixBlock.newimage = 0;
  else {
    m_pixBlock.image.data=newimage;
    m_pixBlock.newimage = 1;
  }
  state->image = &m_pixBlock;
}

/////////////////////////////////////////////////////////
// startRendering
//
/////////////////////////////////////////////////////////
void pix_videoLinuxT :: startRendering()
{
     startTransfer();
     m_pixBlock.newimage = 0;
}

/////////////////////////////////////////////////////////
// stopRendering
//
/////////////////////////////////////////////////////////
void pix_videoLinuxT :: stopRendering()
{
     stopTransfer();
}

/////////////////////////////////////////////////////////
// postrender
//
/////////////////////////////////////////////////////////
void pix_videoLinuxT :: postrender(GemState *state)
{
    m_pixBlock.newimage = 0;
    state->image = NULL;
}

/////////////////////////////////////////////////////////
// startTransfer
//
/////////////////////////////////////////////////////////
int pix_videoLinuxT :: startTransfer()
{
  post("starting transfer");
    char buf[256];
    int i, dataSize;
    frame = 0;
    int width, height;
    
    skipnext = 0;
    if (m_devicenum<0){
      sprintf(buf, "/dev/video");
    } else sprintf(buf, "/dev/video%d", m_devicenum);
    if ((tvfd = open(buf, O_RDWR)) < 0)
    {
	perror(buf);
	goto closit;
    }
    if (ioctl(tvfd, VIDIOCGCAP, &vcap) < 0)
    {
	perror("get capabilities");
	goto closit;
    }
    post("cap: name %s type %d channels %d maxw %d maxh %d minw %d minh %d\n",
    	vcap.name, vcap.type,  vcap.channels,  vcap.maxwidth,  vcap.maxheight,
	    vcap.minwidth,  vcap.minheight);
    if (ioctl(tvfd, VIDIOCGPICT, &vpicture) < 0)
    {
	perror("VIDIOCGCAP");
	goto closit;
    }
    
    post("picture: brightness %d depth %d palette %d\n",
	    vpicture.brightness, vpicture.depth, vpicture.palette);

    for (i = 0; i < vcap.channels; i++)
    {
	vchannel.channel = i;
	if (ioctl(tvfd, VIDIOCGCHAN, &vchannel) < 0)
	{
	    perror("VDIOCGCHAN");
	    goto closit;
	}
    	printf("channel %d name %s type %d flags %d\n",
    	    vchannel.channel, vchannel.name, 
	    vchannel.type, vchannel.flags);
    }
    vchannel.channel = m_channel;
    if (ioctl(tvfd, VIDIOCGCHAN, &vchannel) < 0)
    {
	perror("VDIOCGCHAN");
	goto closit;
    }

    vchannel.norm = m_norm;
    if (ioctl(tvfd, VIDIOCSCHAN, &vchannel) < 0)
    {
	perror("VDIOCSCHAN");
	goto closit;
    }


    	/* get mmap numbers */
    if (ioctl(tvfd, VIDIOCGMBUF, &vmbuf) < 0)
    {
	perror("VIDIOCGMBUF");
	goto closit;
    }
    post("buffer size %d, frames %d, offset %d %d\n", vmbuf.size,
    	vmbuf.frames, vmbuf.offsets[0], vmbuf.offsets[1]);
    if (!(videobuf = (unsigned char *)
    	mmap(0, vmbuf.size, PROT_READ|PROT_WRITE, MAP_SHARED, tvfd, 0)))
    {
	perror("mmap");
	goto closit;
    }

    width = (m_width > vcap.minwidth) ? m_width :  vcap.minwidth;   
    width = (width > vcap.maxwidth) ? vcap.maxwidth : width;
    height = m_height > vcap.minheight ? m_height : vcap.minheight;
    height = (height > vcap.maxheight) ? vcap.maxheight : height;

    for (i = 0; i < NBUF; i++)
    {
    	vmmap[i].format = VIDEO_PALETTE_RGB32;
    	vmmap[i].width = width;
    	vmmap[i].height = height;
	vmmap[i].frame  = i;
    }
    if (ioctl(tvfd, VIDIOCMCAPTURE, &vmmap[frame]) < 0)
    {
    	if (errno == EAGAIN)
	    fprintf(stderr, "can't sync (no video source?)\n");
    	else 
	    perror("VIDIOCMCAPTURE");
	goto closit;
    }
    post("frame %d %d, format %d, width %d, height %d\n",
    	frame, vmmap[frame].frame, vmmap[frame].format,
    	vmmap[frame].width, vmmap[frame].height);

    	/* fill in image specifics for Gem pixel object.  Could we have
	just used RGB, I wonder? */
    m_pixBlock.image.xsize = width;
    m_pixBlock.image.ysize = height;
    m_pixBlock.image.csize = 4;
    m_pixBlock.image.format = GL_RGBA;
    m_pixBlock.image.type = GL_UNSIGNED_BYTE;
    //    m_pixBlock.image.reallocate();
    myleftmargin = 0;
    myrightmargin = 0;
    mytopmargin = 0;
    mybottommargin = 0;
    
    m_haveVideo = 1;

    /* create thread */
    m_continue_thread = 1;
    m_frame_ready = 0;
    post("GEM: pix_video: Opened video connection %X", tvfd);
    post("vmmap %X", vmmap);
    pthread_create(&m_thread_id, 0, capturing, this);
    return(1);

closit:
    if (tvfd >= 0)
    {
    	close(tvfd);
	tvfd = -1;
    }
    m_haveVideo = 0;
    return(0);
}

/////////////////////////////////////////////////////////
// stopTransfer
//
/////////////////////////////////////////////////////////
int pix_videoLinuxT :: stopTransfer()
{
  /* close the v4l device and dealloc buffer */
    /* terminate thread if there is one */
    if(m_continue_thread){
      void *dummy;
      m_continue_thread = 0;
      pthread_join (m_thread_id, &dummy);
    }
    munmap(videobuf, vmbuf.size);
    if (tvfd) close(tvfd);
    tvfd = 0;
    m_haveVideo = 0;
    return(1);

}

/////////////////////////////////////////////////////////
// dimenMess
//
/////////////////////////////////////////////////////////
void pix_videoLinuxT :: dimenMess(int x, int y, int leftmargin, int rightmargin,
    int topmargin, int bottommargin)
{

    int xtotal = x + leftmargin + rightmargin;
    int ytotal = y + topmargin + bottommargin;
    if (xtotal > 844)
    	post("x dimensions too great");
    else if (xtotal < vcap.minwidth || x < 1 ||
    	leftmargin < 0 || rightmargin < 0)
    	    post("x dimensions too small");
    if (ytotal > vcap.maxheight)
    	post("y dimensions too great");
    else if (ytotal < vcap.minheight || y < 1 ||
	     topmargin < 0 || bottommargin < 0)
      post("y dimensions too small");

    myleftmargin = leftmargin;
    myrightmargin = rightmargin;
    mytopmargin = topmargin;
    mybottommargin = bottommargin;

    m_width=x;
    m_height=y;
    m_pixBlock.image.xsize = x;
    m_pixBlock.image.ysize = y;

    //    m_pixBlock.image.reallocate();
    skipnext = 1;
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_videoLinuxT :: obj_setupCallback(t_class *classPtr)
{
  class_addcreator((t_newmethod)_classpix_videoLinuxT,gensym("pix_video"),A_DEFFLOAT,A_DEFFLOAT,A_NULL);
  pix_video::real_obj_setupCallback(classPtr);
  class_addmethod(classPtr, (t_method)&pix_videoLinuxT::freqMessCallback,
		  gensym("freq"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_videoLinuxT::modeMessCallback,
		  gensym("mode"), A_GIMME, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_videoLinuxT::channelMessCallback,
		  gensym("channel"), A_FLOAT, A_NULL);
}


void pix_videoLinuxT :: modeMess(int argc, t_atom *argv)
{
  int mode=m_channel;
  char c=0;
  if (argc==1){
    if (argv->a_type==A_FLOAT)mode=atom_getint(argv);
    else if (argv->a_type==A_SYMBOL)c=toupper(*argv->a_w.w_symbol->s_name);
    else return;
  } else if (argc==2){
    if (argv->a_type==A_FLOAT && (argv+1)->a_type==A_SYMBOL){
      mode=atom_getint(argv);
      c=toupper(*(argv+1)->a_w.w_symbol->s_name);
    } else if ((argv+1)->a_type==A_FLOAT && argv->a_type==A_SYMBOL){
      mode=atom_getint(argv+1);
      c=toupper(*argv->a_w.w_symbol->s_name);
    } else return;
  }

  stopTransfer();

  switch (c){
  case 'p':
  case 'P':
    m_norm = VIDEO_MODE_PAL;
    break;
  case 'n':
  case 'N':
    m_norm = VIDEO_MODE_NTSC;
    break;
  default:
    error("pix_video: unknown norm");
    break;
  }
  m_channel = (int) mode;
  vchannel.channel = m_channel;

  if(gem_amRendering)startTransfer();
}

void pix_videoLinuxT :: channelMess(int c)
{
  if(gem_amRendering)stopTransfer();

  m_channel = c;
  vchannel.channel = m_channel;

  if(gem_amRendering)startTransfer();
}

void pix_videoLinuxT :: deviceMess(int d)
{
  if(gem_amRendering)stopTransfer();
  m_devicenum=d;
  if(gem_amRendering)startTransfer();
}

void pix_videoLinuxT :: freqMess(t_floatarg c)
{
     int freq = (int) c;
     vtuner.tuner = m_channel;
     if (ioctl(tvfd,VIDIOCGTUNER,&vtuner) < 0) {
	  post("Error setting frequency -- no tuner");
	  return;
     }
     
     if (ioctl(tvfd,VIDIOCSFREQ,&freq) < 0) {
	  post("Error setting frequency");
	  return;
     }
}

void pix_videoLinuxT :: modeMessCallback(void *data, t_symbol* norm, int argc, t_atom *argv)
{

  //if (argc==1 || argc==2)
    GetMyClass(data)->modeMess(argc, argv);
}


void pix_videoLinuxT :: freqMessCallback(void *data, t_floatarg f)
{
    GetMyClass(data)->freqMess(f);
}

void pix_videoLinuxT :: channelMessCallback(void *data, t_floatarg f)
{
    GetMyClass(data)->channelMess((int)f);
}

void pix_videoLinuxT :: deviceMessCallback(void *data, t_floatarg f)
{
    GetMyClass(data)->deviceMess((int)f);
}
#endif
