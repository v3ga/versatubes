/*
 *  versatubes.h
 *  versatubes
 *
 *  Created by 2Roqs on 30/04/12.
 *  Copyright 2012 2Roqs. All rights reserved.
 *
 */
#include "ofMain.h"


#pragma once

class Tube
{
	public:
		
						Tube			(int id, int length, int nbLeds, float xA, float yA, float xB, float yB);

		int				m_id;

		ofPoint			m_A,m_B;
		ofPoint			m_Aoffset,m_Boffset;
		ofPoint			m_dir;
		float			m_offset;

		bool			m_isOn;
		bool			m_isLockOnOff;
		string			m_label;

		float			m_length;
		int				m_nbLeds;
		
		// Final pixels
		int				m_xPixels,m_yPixels;
		int				m_xPixelsDir,m_yPixelsDir;

		void			M_setOn			(bool is=true){if (m_isLockOnOff == false) m_isOn=is;}
		void			M_setOff		(){M_setOn(false);}
		void			M_lockOnOff		(bool is){m_isLockOnOff = is;}
		void			M_setOffset		(float offset);
		void			M_display		(bool isOffset=false);
		void			M_sample		(ofPixels& src, ofPixels& dst, int border=0);

	private:
		ofPoint			m_step;
		ofPoint			m_pointSample;
};

class TubeGroup
{
	public:
		vector<Tube*>	m_listTubes;

		void			M_addTube				(Tube* pTube){m_listTubes.push_back(pTube);}
};


class Versatubes
{
	public:
						Versatubes				(float lengthTube, int nbLeds);
		virtual			~Versatubes				();

		virtual void	VM_build				();
		virtual	void	VM_display				();

		int				M_getRowsNb				(){return m_nbRows;}
		int				M_getColsNb				(){return m_nbCols;}

		float			M_getWidth				(){return m_boundingBox.width;}
		float			M_getHeight				(){return m_boundingBox.height;}
		
		void			M_beginCanvas			(){m_fboCanvas.begin();}
		void			M_endCanvas				(){m_fboCanvas.end();}

		void			M_displayMask			(float x, float y);
		void			M_displayCanvas			(float x, float y);
		void			M_displayCanvasWithMask	(float x, float y);
		void			M_displayPixels			(int x, int y);
		void			M_readPixels			();
		void			M_sample				();
		void			M_setOffset				(float offset);
		
		void			M_enableRow				(int index, bool is);
		void			M_enableRowAll			(bool is);
		
		// Tube groups
		vector<TubeGroup*> m_listTubeGroupRow;

		// Bounding box
		ofRectangle		m_boundingBox;

		// Mask
		ofFbo			m_fboMask;

		// Canvas
		enum{
			CANVAS_DISPLAY_MASK		= 0,
			CANVAS_DISPLAY_MOTIF	= 1
		};
		ofFbo			m_fboCanvas;
		int				m_fboCanvasDisplayMode;
		ofPixels		m_fboCanvasPixels;

		// Composition (canvas + mask)
		// Drawing only
		ofFbo			m_fboCanvasWithMask;

		// Borders
		int				m_fboBorder;

		// Final pixels
		ofPixels		m_pixels;
		ofTexture		m_texPixels;

		void			M_makeMask		();

	private:
		vector<Tube*>	m_listTubes;
		float			m_lengthTube;
		int				m_nbLedsTube;

		int				m_nbRows;
		int				m_nbCols;
		float			m_hRow;

		void			M_makeCanvas	();
		
};