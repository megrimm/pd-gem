/* ------------------------------------------------------------------
 * GEM - Graphics Environment for Multimedia
 *
 *  Copyright (c) 2002-2011 IOhannes m zmölnig. forum::für::umläute. IEM. zmoelnig@iem.at
 *     zmoelnig@iem.kug.ac.at
 *  For information on usage and redistribution, and for a DISCLAIMER
 *  OF ALL WARRANTIES, see the file, "GEM.LICENSE.TERMS"
 *
 *  this file has been generated...
 * ------------------------------------------------------------------
 */

#ifndef _INCLUDE__GEM_OPENGL_GEMGLPOLYGONMODE_H_
#define _INCLUDE__GEM_OPENGL_GEMGLPOLYGONMODE_H_

#include "Base/GemGLBase.h"

/*
 CLASS
        GEMglPolygonMode
 KEYWORDS
        openGL  0
 DESCRIPTION
        wrapper for the openGL-function
        "glPolygonMode( GLenum face, GLenum mode)"
 */

class GEM_EXTERN GEMglPolygonMode : public GemGLBase
{
  CPPEXTERN_HEADER(GEMglPolygonMode, GemGLBase);

public:
  // Constructor
  GEMglPolygonMode (int, t_atom*); // CON

protected:
  // Destructor
  virtual ~GEMglPolygonMode ();
  // Do the rendering
  virtual void  render (GemState *state);

  // variables
  GLenum        face;           // VAR
  virtual void  faceMess(t_atom);       // FUN

  GLenum        mode;           // VAR
  virtual void  modeMess(t_atom);       // FUN


private:

  // we need some inlets
  t_inlet *m_inlet[2];

  // static member functions
  static void    faceMessCallback (void*,t_symbol*,int,t_atom*);
  static void    modeMessCallback (void*,t_symbol*,int,t_atom*);
};
#endif // for header file
