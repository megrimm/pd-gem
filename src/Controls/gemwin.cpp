////////////////////////////////////////////////////////
//
// GEM - Graphics Environment for Multimedia
//
// zmoelnig@iem.kug.ac.at
//
// Implementation file
//
//    Copyright (c) 1997-2000 Mark Danks.
//    Copyright (c) G�nther Geiger.
//    Copyright (c) 2001-2002 IOhannes m zmoelnig. forum::f�r::uml�ute. IEM
//    For information on usage and redistribution, and for a DISCLAIMER OF ALL
//    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.
//
/////////////////////////////////////////////////////////

#include "gemwin.h"

#include "Base/GemGL.h"
 
#ifdef __APPLE__
# include <Carbon/carbon.h>
# include <AvailabilityMacros.h>
#endif // __APPLE__

#include "Base/GemMath.h"
#include "Base/GemMan.h"

CPPEXTERN_NEW_WITH_ONE_ARG(gemwin, t_floatarg, A_DEFFLOAT)

static bool StillHaveGemWin(bool up) {
  static int ref_counter = 0;

  if (up){
    ref_counter++;
    if (ref_counter==1)return false;
  } else {
    ref_counter--;
    if (ref_counter<0)ref_counter=0;
    return (ref_counter!=0);
  }
  return true;
}

/////////////////////////////////////////////////////////
//
// gemwin
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
gemwin :: gemwin(t_floatarg framespersecond)
{
  if (framespersecond <= 0.)
    framespersecond = 20.;
  GemMan::frameRate((float)framespersecond);
  if(!StillHaveGemWin(true))GemMan::resetState();
  m_FrameRate       = outlet_new(this->x_obj, 0);
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
gemwin :: ~gemwin()
{
  if(!StillHaveGemWin(false))
    GemMan::destroyWindow();
}

/////////////////////////////////////////////////////////
// bangMess
//
/////////////////////////////////////////////////////////
void gemwin :: bangMess()
{
  if ( GemMan::windowExists() )
    {
      GemMan::swapBuffers();
    }
  else 
    error("GEM: no window");
}

/////////////////////////////////////////////////////////
// intMess
//
/////////////////////////////////////////////////////////
void gemwin :: intMess(int state)
{
  if (state)
    GemMan::startRendering();
  else
    GemMan::stopRendering();
}
/////////////////////////////////////////////////////////
// renderMess
//
/////////////////////////////////////////////////////////
void gemwin :: renderMess()
{
	if (GemMan::getRenderState())
		GemMan::render(NULL);
	else
		error("GEM: not in render mode");
}
/////////////////////////////////////////////////////////
// titleMess
//
/////////////////////////////////////////////////////////
void gemwin :: titleMess(t_symbol* s)
{
  GemMan::m_title = s->s_name;
}
/////////////////////////////////////////////////////////
// createMess
//
/////////////////////////////////////////////////////////
void gemwin :: createMess(t_symbol* s)
{
  char* disp = NULL;

  if (s != &s_) 
    disp = s->s_name;

  if ( !GemMan::windowExists() )  {
    GemMan::createContext(disp);
    if ( !GemMan::createWindow(disp) )  {
      error("GEM: gemwin: no window made");
      return;
    }
    GemMan::swapBuffers();	
    GemMan::swapBuffers();	
  } else
    error("GEM: gemwin: window already made");
}

/////////////////////////////////////////////////////////
// bufferMess
//
/////////////////////////////////////////////////////////
void gemwin :: bufferMess(int buf)
{
  if (buf == 1)
    GemMan::m_buffer = 1;
  else
    GemMan::m_buffer = 2;
}


/////////////////////////////////////////////////////////
// stereoMess
//
/////////////////////////////////////////////////////////
void gemwin :: stereoMess(int mode)
{
  if (mode<0){
    error("GEM: stereo-mode must not be %d", mode);
    return;
  }
  if (mode>1) {
    error("GEM: only stereo-modes 1/0 are allowed!!!");
    return;
  }

  GemMan::m_stereo = mode;
}

/////////////////////////////////////////////////////////
// fullscreenMess
//
/////////////////////////////////////////////////////////
void gemwin :: fullscreenMess(int on)
{
    GemMan::m_fullscreen = on;
}

/////////////////////////////////////////////////////////
// menuBarMess		1 = show	-1 = hide, but autoshow
//					0 = hide, but neverShow
/////////////////////////////////////////////////////////
void gemwin :: menuBarMess(int on)
{
  GemMan::m_menuBar = on;
#ifdef __APPLE__
  if (on == 0) {
    SetSystemUIMode( kUIModeAllHidden, kUIOptionDisableAppleMenu |
									   kUIOptionDisableProcessSwitch |
									   kUIOptionDisableSessionTerminate |
									   kUIOptionDisableForceQuit );
  }else if (on > 0) {
    SetSystemUIMode( kUIModeNormal, NULL);
  }else if (on < 0) {
    SetSystemUIMode( kUIModeAllHidden, kUIOptionAutoShowMenuBar );
  }
#endif
}

/////////////////////////////////////////////////////////
// secondScreen
//
/////////////////////////////////////////////////////////
void gemwin :: secondscreenMess(int on)
{
  GemMan::m_secondscreen = on;
}

/////////////////////////////////////////////////////////
// dimensionsMess
//
/////////////////////////////////////////////////////////
void gemwin :: dimensionsMess(int width, int height)
{
  if (width <= 0) {
    error("GEM: gemwin: width must be greater than 0");
    return;
  }
    
  if (height <= 0 ) {
    error ("GEM: gemwin: height must be greater than 0");
    return;
  }
  GemMan::m_width = width;
  GemMan::m_height = height;
}
/////////////////////////////////////////////////////////
// offsetMess
//
/////////////////////////////////////////////////////////
void gemwin :: offsetMess(int x, int y)
{
  GemMan::m_xoffset = x;
  GemMan::m_yoffset = y;
}
/////////////////////////////////////////////////////////
// colorMess
//
/////////////////////////////////////////////////////////
void gemwin :: colorMess(float red, float green, float blue, float alpha)
{
#ifdef __APPLE__
  if ( !GemMan::windowExists() )  {
    post("GEM: gemwin: !windowExists");
    return;
  }
#endif
  glClearColor(red, green, blue, alpha);
  GemMan::m_clear_color[0] = red;
  GemMan::m_clear_color[1] = green;
  GemMan::m_clear_color[2] = blue;
  GemMan::m_clear_color[3] = alpha;
}
/////////////////////////////////////////////////////////
// clearmaskMess
//
/////////////////////////////////////////////////////////
void gemwin :: clearmaskMess(float bitmask)
{
  GemMan::m_clear_mask = (GLbitfield) bitmask;
}

/////////////////////////////////////////////////////////
// ambientMess
//
/////////////////////////////////////////////////////////
void gemwin :: ambientMess(float red, float green, float blue, float alpha)
{
  GemMan::m_mat_ambient[0] = red;
  GemMan::m_mat_ambient[1] = green;
  GemMan::m_mat_ambient[2] = blue;
  GemMan::m_mat_ambient[3] = blue;

}

/////////////////////////////////////////////////////////
// specularMess
//
/////////////////////////////////////////////////////////
void gemwin :: specularMess(float red, float green, float blue, float alpha)
{
  GemMan::m_mat_specular[0] = red;
  GemMan::m_mat_specular[1] = green;
  GemMan::m_mat_specular[2] = blue;
  GemMan::m_mat_specular[2] = alpha;

}

/////////////////////////////////////////////////////////
// shininessMess
//
/////////////////////////////////////////////////////////
void gemwin :: shininessMess(float val)
{
  GemMan::m_mat_shininess = val;
}

/////////////////////////////////////////////////////////
// fogDensityMess
//
/////////////////////////////////////////////////////////
void gemwin :: fogDensityMess(float val)
{
  if (val < 0.f)
    val = 0.f;

  GemMan::m_fog = val;
}

/////////////////////////////////////////////////////////
// fogRangeMess
//
/////////////////////////////////////////////////////////
void gemwin :: fogRangeMess(float start, float end)
{
  if (start < 0.f)
    start= 0.f;

  if (end < 0.f)
    end  = 0.f;

  GemMan::m_fogStart = start;
  GemMan::m_fogEnd = end;
}

/////////////////////////////////////////////////////////
// fogColorMess
//
/////////////////////////////////////////////////////////
void gemwin :: fogColorMess(float red, float green, float blue, float alpha)
{
  GemMan::m_fogColor[0] = red;
  GemMan::m_fogColor[1] = green;
  GemMan::m_fogColor[2] = blue;
  GemMan::m_fogColor[3] = alpha;
}

/////////////////////////////////////////////////////////
// fogModeMess
//
/////////////////////////////////////////////////////////
void gemwin :: fogModeMess(int mode)
{
  switch (mode)
    {
    case 0 :
      GemMan::m_fogMode = GemMan::FOG_OFF;
      break;

    case 1 :
      GemMan::m_fogMode = GemMan::FOG_LINEAR;
      break;

    case 2 :
      GemMan::m_fogMode = GemMan::FOG_EXP;
      break;

    case 3 :
      GemMan::m_fogMode = GemMan::FOG_EXP2;
      break;

    default :
      error("GEM : fogmode must be 0, 1, 2 or 3");
      break;
    }
}

/////////////////////////////////////////////////////////
// cursorMess
//
/////////////////////////////////////////////////////////
void gemwin :: cursorMess(float setting)
{
  GemMan :: cursorOnOff((int)setting);
}

/////////////////////////////////////////////////////////
// topmostMess
//
/////////////////////////////////////////////////////////
void gemwin :: topmostMess(float topmost)
{
  GemMan::topmostOnOff((int) topmost);
}


/////////////////////////////////////////////////////////
// cursorMess
//
/////////////////////////////////////////////////////////
void gemwin :: blurMess(float setting)
{
  if (setting>=0.f && setting <= 1.f)
    GemMan :: m_motionBlur = setting;
}

/////////////////////////////////////////////////////////
// fpsMess
//
/////////////////////////////////////////////////////////
void gemwin :: fpsMess()
{
//GemMan::m_fps = 0;
 // post("gemwin: fps %d",GemMan::fps);
    outlet_float(m_FrameRate,GemMan :: fps);
}

/////////////////////////////////////////////////////////
// fsaaMess
//
/////////////////////////////////////////////////////////
void gemwin :: fsaaMess(int value)
{
    if (value == 2 || value == 4 || value == 8){
        GemMan :: fsaa = value;
    }
    else{
        GemMan :: fsaa = value;
    }
}


/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void gemwin :: obj_setupCallback(t_class *classPtr)
{
  class_addbang(classPtr, (t_method)&gemwin::bangMessCallback);
  class_addfloat(classPtr, (t_method)&gemwin::floatMessCallback);

  class_addmethod(classPtr, (t_method)&gemwin::renderMessCallback,
		  gensym("render"), A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::titleMessCallback,
		  gensym("title"), A_DEFSYM ,A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::createMessCallback,
		  gensym("create"), A_DEFSYM ,A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::createStereoMessCallback,
		  gensym("createStereo"), A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::createStereoMessCallback,
		  gensym("createstereo"), A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::bufferMessCallback,
		  gensym("buffer"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::fullscreenMessCallback,
		  gensym("fullscreen"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::menuBarMessCallback,
		  gensym("menubar"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::secondscreenMessCallback,
		  gensym("secondscreen"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::dimensionsMessCallback,
		  gensym("dimen"), A_FLOAT, A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::offsetMessCallback,
		  gensym("offset"), A_FLOAT, A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::colorMessCallback,
		  gensym("color"), A_GIMME, A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::clearmaskMessCallback,
		  gensym("clearmask"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::perspectiveMessCallback,
		  gensym("perspec"), A_GIMME, A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::viewMessCallback,
		  gensym("view"), A_GIMME, A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::fogMessCallback,
		  gensym("fog"), A_GIMME, A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::fogColorMessCallback,
		  gensym("fogcolor"), A_GIMME, A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::fogModeMessCallback,
		  gensym("fogmode"), A_FLOAT, A_NULL);

  class_addmethod(classPtr, (t_method)&gemwin::ambientMessCallback,
		  gensym("ambient"), A_GIMME, A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::specularMessCallback,
		  gensym("specular"), A_GIMME, A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::shininessMessCallback,
		  gensym("shininess"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::cursorMessCallback,
		  gensym("cursor"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::blurMessCallback,
		  gensym("blur"), A_FLOAT, A_NULL);

  // just call GemMan directly
  class_addmethod(classPtr, (t_method)&gemwin::resetMessCallback,
		  gensym("reset"), A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::destroyMessCallback,
		  gensym("destroy"), A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::printMessCallback,
		  gensym("print"), A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::profileMessCallback, 
		  gensym("profile"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::lightingMessCallback, 
		  gensym("lighting"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::stereoMessCallback, 
		  gensym("stereo"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::stereoSepMessCallback, 
		  gensym("stereoSep"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::stereoFocMessCallback, 
		  gensym("stereoFoc"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::stereoSepMessCallback, 
		  gensym("stereosep"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::stereoFocMessCallback, 
		  gensym("stereofoc"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::stereoLineMessCallback, 
		  gensym("stereoLine"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::stereoLineMessCallback, 
		  gensym("stereoline"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::borderMessCallback, 
		  gensym("border"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::frameMessCallback,
		  gensym("frame"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::fpsMessCallback,
		  gensym("fps"), A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::topmostMessCallback,
		  gensym("topmost"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&gemwin::fsaaMessCallback,
		  gensym("FSAA"), A_FLOAT, A_NULL);
}
void gemwin :: printMessCallback(void *)
{
  GemMan::printInfo();
}
void gemwin :: profileMessCallback(void *, t_floatarg state)
{
  GemMan::m_profile = (int)state;
}
void gemwin :: lightingMessCallback(void *, t_floatarg state)
{
  GemMan::lightingOnOff((int)state);
}
void gemwin :: fogMessCallback(void *data, t_symbol *, int argc, t_atom *argv)
{
  switch (argc)
    {
    case (1):
      GetMyClass(data)->fogDensityMess(atom_getfloat(&argv[0]));
      break;
    case (2):
      GetMyClass(data)->fogRangeMess(atom_getfloat(&argv[0]),atom_getfloat(&argv[1]));
      break;
    default:
      error("GEM : fog message needs 1 or 2 arguments");
    }
}
void gemwin :: fogColorMessCallback(void *data, t_symbol*s,int argc, t_atom*argv)
{
  float red, green, blue, alpha=1.f;
  if(argc<3){
    error("gemwin: \"color\" expects 3 or 4 values");
    return;
  }
  switch(argc){
  case 4:
    alpha=atom_getfloat(argv+3);
  case 3:
    red=  atom_getfloat(argv);
    green=atom_getfloat(argv+1);
    blue= atom_getfloat(argv+2);
    GetMyClass(data)->fogColorMess((float)red, (float)green, (float)blue, (float)alpha);
    break;
  default:
    error("gemwin: \"color\" expects 3 or 4 values");
  }
}
void gemwin :: fogModeMessCallback(void *data, t_float val)
{
  GetMyClass(data)->fogModeMess((int)val);
}
void gemwin :: stereoMessCallback(void *, t_floatarg state)
{
  int mode = (int)state;
  if (mode<0 || mode>2){
    error("GEM: possible stereo-modes are: 0, 1, 2");
    return;
  }
  GemMan::m_stereo = mode;
}
void gemwin :: stereoSepMessCallback(void *, t_floatarg state)
{
  GemMan::m_stereoSep = state;
}
void gemwin :: stereoFocMessCallback(void *, t_floatarg state)
{
  GemMan::m_stereoFocal = state;
}
void gemwin :: stereoLineMessCallback(void *, t_floatarg state)
{
  GemMan::m_stereoLine = (state!=0.0);
}
void gemwin :: borderMessCallback(void *, t_floatarg state)
{
  GemMan::m_border = (int)state;
}
void gemwin :: destroyMessCallback(void *)
{
  GemMan::destroyWindow();
}
void gemwin :: resetMessCallback(void *)
{
  GemMan::resetState();
}
void gemwin :: frameMessCallback(void *, t_floatarg rate)
{
  GemMan::frameRate((float)rate);
}
void gemwin :: perspectiveMessCallback(void *, t_symbol *, int argc, t_atom *argv)
{
  if (argc != 6)
    {
      error("GEM: perspec message needs 6 arguments");
      return;
    }
  GemMan::m_perspect[0] = atom_getfloat(&argv[0]);	// left
  GemMan::m_perspect[1] = atom_getfloat(&argv[1]);	// right
  GemMan::m_perspect[2] = atom_getfloat(&argv[2]);	// bottom
  GemMan::m_perspect[3] = atom_getfloat(&argv[3]);	// top
  GemMan::m_perspect[4] = atom_getfloat(&argv[4]);	// front
  GemMan::m_perspect[5] = atom_getfloat(&argv[5]);	// back
}
void gemwin :: viewMessCallback(void *, t_symbol *, int argc, t_atom *argv)
{
  const float DEG2RAD = 0.01745329251994f;
  float azimuth = 0.f;
  float theta = 0.f;
  float distance = 1.f;

  if (GemMan::m_stereoFocal > 0)
    {
      distance = GemMan::m_stereoFocal;
    }

  switch (argc)
    {
      // setting all lookat values directly
    case 9 :
      GemMan::m_lookat[0] = atom_getfloat(&argv[0]);	// eyex
      GemMan::m_lookat[1] = atom_getfloat(&argv[1]);	// eyey
      GemMan::m_lookat[2] = atom_getfloat(&argv[2]);	// eyez
      GemMan::m_lookat[3] = atom_getfloat(&argv[3]);	// centerx
      GemMan::m_lookat[4] = atom_getfloat(&argv[4]);	// centery
      GemMan::m_lookat[5] = atom_getfloat(&argv[5]);	// centerz
      GemMan::m_lookat[6] = atom_getfloat(&argv[6]);	// upx
      GemMan::m_lookat[7] = atom_getfloat(&argv[7]);	// upy
      GemMan::m_lookat[8] = atom_getfloat(&argv[8]);	// upz
      break;

    case 5 :
      theta	= (float)DEG2RAD * atom_getfloat(&argv[4]);
		
    case 4 :
      azimuth = (float)DEG2RAD * atom_getfloat(&argv[3]);
		
      // just have position
    case 3 :
      {
	const float dx =  (float)(cos(theta) * sinf(azimuth));
	const float dy =  (float)(sin(theta));
	const float dz = -(float)(cos(theta) * cosf(azimuth));

	GemMan::m_lookat[0] = atom_getfloat(&argv[0]);		// eyex
	GemMan::m_lookat[1] = atom_getfloat(&argv[1]);		// eyey
	GemMan::m_lookat[2] = atom_getfloat(&argv[2]);		// eyez
	GemMan::m_lookat[3] = GemMan::m_lookat[0] + dx * distance;	// centerx
	GemMan::m_lookat[4] = GemMan::m_lookat[1] + dy * distance;	// centery
	GemMan::m_lookat[5] = GemMan::m_lookat[2] + dz * distance;	// centery
	GemMan::m_lookat[6] = -dx*dy;					// upx
	GemMan::m_lookat[7] = dx*dx+dz*dz;				// upy
	GemMan::m_lookat[8] = -dy*dz;					// upz
      }
      break;

    default:
      error("GEM: view message needs 3, 4, 5 or 9 arguments");

      // note :: LATER set the StereoView ...
    }
}

void gemwin :: bangMessCallback(void *data)
{
  GetMyClass(data)->bangMess();
}
void gemwin :: floatMessCallback(void *data, t_float state)
{
  GetMyClass(data)->intMess((int)state);
}
void gemwin :: renderMessCallback(void *data)
{
  GetMyClass(data)->renderMess();
}
void gemwin :: titleMessCallback(void *data, t_symbol* disp)
{
  GetMyClass(data)->titleMess(disp);
}
void gemwin :: createMessCallback(void *data, t_symbol* disp)
{
  GetMyClass(data)->createMess(disp);
}
void gemwin :: createStereoMessCallback(void *data)
{
  error("GEM: 'createStereo' is deprecated and does not work any more");
  error("GEM:   use 'stereo 1' + 'create' instead");

}
void gemwin :: bufferMessCallback(void *data, t_floatarg buf)
{
  GetMyClass(data)->bufferMess((int)buf);
}
void gemwin :: fullscreenMessCallback(void *data, t_floatarg on)
{
  GetMyClass(data)->fullscreenMess((int)on);
}
void gemwin :: menuBarMessCallback(void *data, t_floatarg on)
{
  GetMyClass(data)->menuBarMess((int)on);
}
void gemwin :: secondscreenMessCallback(void *data, t_floatarg on)
{
  GetMyClass(data)->secondscreenMess((int)on);
}
void gemwin :: dimensionsMessCallback(void *data, t_floatarg width, t_floatarg height)
{
  GetMyClass(data)->dimensionsMess((int)width, (int)height);
}
void gemwin :: offsetMessCallback(void *data, t_floatarg x, t_floatarg y)
{
  GetMyClass(data)->offsetMess((int)x, (int)y);
}
void gemwin :: colorMessCallback(void *data, t_symbol*s, int argc, t_atom*argv)
{
  float red, green, blue, alpha=0.f;
  switch(argc){
  case 4:
    alpha=atom_getfloat(argv+3);
  case 3:
    red=  atom_getfloat(argv);
    green=atom_getfloat(argv+1);
    blue= atom_getfloat(argv+2);
    GetMyClass(data)->colorMess((float)red, (float)green, (float)blue, (float)alpha);
    break;
  default:
    error("gemwin: \"color\" expects 3 or 4 values");
  }
}
void gemwin :: clearmaskMessCallback(void *data, t_floatarg bitmask)
{
  GetMyClass(data)->clearmaskMess((float)bitmask);
}
void gemwin :: ambientMessCallback(void *data, t_symbol*s,int argc, t_atom*argv)
{
  float red, green, blue, alpha=1.f;
  switch(argc){
  case 4:
    alpha=atom_getfloat(argv+3);
  case 3:
    red=  atom_getfloat(argv);
    green=atom_getfloat(argv+1);
    blue= atom_getfloat(argv+2);
    GetMyClass(data)->ambientMess((float)red, (float)green, (float)blue, (float)alpha);
    break;
  default:
    error("gemwin: \"ambient\" expects 3 or 4 values");
  }
}
void gemwin :: specularMessCallback(void *data, t_symbol*s,int argc, t_atom*argv)
{
  float red, green, blue, alpha=1.f;
  switch(argc){
  case 4:
    alpha=atom_getfloat(argv+3);
  case 3:
    red=  atom_getfloat(argv);
    green=atom_getfloat(argv+1);
    blue= atom_getfloat(argv+2);
    GetMyClass(data)->specularMess((float)red, (float)green, (float)blue, (float)alpha);
    break;
  default:
    error("gemwin: \"specular\" expects 3 or 4 values");
  }
}
void gemwin :: shininessMessCallback(void *data, t_floatarg val)
{
  GetMyClass(data)->shininessMess((float)val);
}
void gemwin :: cursorMessCallback(void *data, t_floatarg val)
{
  GetMyClass(data)->cursorMess((float)val);
}
void gemwin :: topmostMessCallback(void *data, t_floatarg val)
{
  GetMyClass(data)->topmostMess((float)val);
}
void gemwin :: blurMessCallback(void *data, t_floatarg val)
{
  GetMyClass(data)->blurMess((float)val);
}
void gemwin :: fpsMessCallback(void *data)
{
  GetMyClass(data)->fpsMess();
}

void gemwin :: fsaaMessCallback(void *data, t_floatarg val)
{
  GetMyClass(data)->fsaaMess((int) val);
}

