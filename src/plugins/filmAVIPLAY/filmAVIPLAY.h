/* -----------------------------------------------------------------

GEM - Graphics Environment for Multimedia

Load an digital video (like AVI, Mpeg, Quicktime) into a pix block 
(OS independant parent-class)

Copyright (c) 1997-1999 Mark Danks. mark@danks.org
Copyright (c) Günther Geiger. geiger@epy.co.at
Copyright (c) 2001-2003 IOhannes m zmoelnig. forum::für::umläute. IEM. zmoelnig@iem.kug.ac.at
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.

-----------------------------------------------------------------*/

#ifndef INCLUDE_FILMAVIPLAY_H_
#define INCLUDE_FILMAVIPLAY_H_
#include "plugins/film.h"

#if defined (_WIN32) & defined (HAVE_LIBAVIPLAY)
   // un windows there are other ways...
#undef HAVE_LIBAVIPLAY
#endif

#ifdef HAVE_LIBAVIPLAY
   /* this used to be <avifile/avifile.h> 
    * but on my system it changed to <avifile-0.7/avifile.h>
    * so we now find the correct path via "configure"
    */

// ugly hack, since avifile.h does weird things if HAVE_CONFIG_H is defined
# undef HAVE_CONFIG_H

# include "avifile.h"
# include "infotypes.h"
# include "image.h"

   // some version checking...
# ifndef IMG_FMT_RGB24
#  undef HAVE_LIBAVIPLAY
# endif // IMG_FMT_RGB24

#endif

/*-----------------------------------------------------------------
  -------------------------------------------------------------------
  CLASS
  filmAVIPLAY
    
  film-loader class for AVIPLAY(linux)
    
  KEYWORDS
  pix film movie
    
  DESCRIPTION

  -----------------------------------------------------------------*/
namespace gem { namespace plugins {
class GEM_EXPORT filmAVIPLAY : public gem::plugins::film {
 public:
  
  //////////
  // Constructor
  filmAVIPLAY(void);

  //////////
  // Destructor
  ~filmAVIPLAY();

#ifdef HAVE_LIBAVIPLAY
  //////////
  // open a movie up
  virtual bool open(const std::string filename, int format=0);
  //////////
  // close the movie file
  virtual void close(void);

  //////////
  // get the next frame
  virtual pixBlock* getFrame();

  //////////
  // set the next frame to read;
  virtual int changeImage(int imgNum, int trackNum=-1);

 protected:
  IAviReadFile *m_avifile;
  IAviReadStream  *m_avistream;


# ifdef AVM_BEGIN_NAMESPACE
  avm::CImage *m_aviimage;
# else
  CImage *m_aviimage;
# endif
#endif //AVIPLAY
  unsigned char *m_rawdata;
  long           m_rawlength;
};};};

#endif	// for header file
