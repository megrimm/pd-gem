/*-----------------------------------------------------------------

GEM - Graphics Environment for Multimedia

capture an image using unicap

Copyright (c) 2001-2012 IOhannes m zmölnig. forum::für::umläute. IEM. zmoelnig@iem.at
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.
	
-----------------------------------------------------------------*/

#ifndef _INCLUDE_GEMPLUGIN__VIDEOUNICAP_VIDEOUNICAP_H_
#define _INCLUDE_GEMPLUGIN__VIDEOUNICAP_VIDEOUNICAP_H_

#include "plugins/videoBase.h"

#include <map>
#define private unicap_private
#include <unicap.h>
#undef private
#include "Gem/ThreadMutex.h"
/*-----------------------------------------------------------------
  -------------------------------------------------------------------
  CLASS
  pix_video
    
  grabs a video via UNICAP (http://unicap-imaging.org/)
    
  KEYWORDS
  pix

  -----------------------------------------------------------------*/
namespace gem { namespace plugins {
 class GEM_EXPORT videoUNICAP : public videoBase {
 public:
  //////////
  // Constructor
  videoUNICAP(void);
  
  //////////
  // Destructor
  virtual ~videoUNICAP(void);
  
  ////////
  // open the video-device
  virtual bool           openDevice(gem::Properties&writeprops);
  virtual void          closeDevice(void);
    
  //////////
  // Start up the video device
  // [out] int - returns 0 if bad
  virtual bool	    	startTransfer(void);
  //////////
  // Stop the video device
  // [out] int - returns 0 if bad
  virtual bool	   	stopTransfer(void);

  //////////
  // get the next frame
  virtual pixBlock    *getFrame(void);
  virtual void releaseFrame(void);

  //////////
  // Set the video properties
  virtual bool	    	setColor(int);

  virtual bool defaultFormat(void);

  virtual std::vector<std::string>enumerate(void);

  virtual bool enumProperties(gem::Properties&readable,
			      gem::Properties&writeable);
  virtual void setProperties(gem::Properties&writeprops);
  virtual void getProperties(gem::Properties&readprops);

 protected:
  std::vector<unicap_device_t>m_devices;
  std::map<std::string, std::vector<unsigned int> >m_name2devices;
  unicap_handle_t m_handle;

  void newFrame (unicap_handle_t handle, 
		 unicap_data_buffer_t * buffer);


  gem::thread::Mutex mutex;


  private:
  static void newFrameCB (unicap_event_t event, 
			  unicap_handle_t handle, 
			  unicap_data_buffer_t * buffer, 
			  void *usr_data);
 };
};};

#endif	// for header file

