/*-----------------------------------------------------------------
LOG
GEM - Graphics Environment for Multimedia

Clamp pixel values to a threshold

Copyright (c) 1997-1998 Mark Danks. mark@danks.org
Copyright (c) G�nther Geiger. geiger@epy.co.at
Copyright (c) 2001-2002 IOhannes m zmoelnig. forum::f�r::uml�ute. IEM. zmoelnig@iem.kug.ac.at
Copyright (c) 2002 James Tittle & Chris Clepper
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "LICENSE.txt" in this distribution.

-----------------------------------------------------------------*/

#ifndef INCLUDE_pix_drum_H_
#define INCLUDE_pix_drum_H_

#include "Base/GemPixObj.h"

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    pix_drum
    
    Clamp pixel values to a threshold

KEYWORDS
    pix
    
DESCRIPTION

    Inlet for a list - "vec_thresh"
    Inlet for a float - "ft1"

    "vec_thresh" - The threshold vector
    "ft1" - Set all thresholds to one value
   
-----------------------------------------------------------------*/
class GEM_EXTERN pix_drum : public GemPixObj
{
    CPPEXTERN_HEADER(pix_drum, GemPixObj)

    public:

        //////////
        // Constructor
    	pix_drum();
    	
    protected:
    	
    	//////////
    	// Destructor
    	virtual ~pix_drum();
    	virtual void 	processRGBAImage(imageStruct &image);
    	virtual void 	processGrayImage(imageStruct &image);
    	virtual void 	processYUVImage(imageStruct &image);
		
#ifdef __VEC__

    	// Do the processing
    	virtual void 	processYUVAltivec(imageStruct &image);
#endif  

    	void	    	vecBoundsMess(int argc, t_atom *argv);
    	void	    	vecThreshMess(int argc, t_atom *argv);
    	
		t_outlet *outlet1;
		t_outlet *outlet2;
		t_outlet *outlet3;
		t_outlet *outlet4;
    	unsigned char  	m_thresh[4];
        unsigned char	m_Y;
		int head, bottom, right, left, mode, pix_dist, min_width, pix_dist_ctr;
		int Xsize, Ysize;
		float thresh, min_height;
    
    private:
    
    	//////////
    	// Static member functions
    	static void 	vecBoundsMessCallback(void *data, t_symbol *, int argc, t_atom *argv);
    	static void 	vecThreshMessCallback(void *data, t_symbol *, int argc, t_atom *argv);
};

#endif	// for header file
