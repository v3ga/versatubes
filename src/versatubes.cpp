/*
 *  versatubes.cpp
 *  versatubes
 *
 *  Created by 2Roqs on 30/04/12.
 *  Copyright 2012 2Roqs. All rights reserved.
 *
 */

#include "versatubes.h"

#define ____________Tube____________
//--------------------------------------------------------------
Tube::Tube(int id_, int length_, int nbLeds_, float xA_, float yA_, float xB_, float yB_)
{
	m_id		= id_;
	m_length	= length_;
	m_nbLeds	= nbLeds_;
	m_label		= "";
	m_A.set(xA_,yA_);
	m_B.set(xB_,yB_);
	m_dir = m_B-m_A;
	m_dir.normalize();
	M_setOffset(0.1f);

	m_isLockOnOff = false;
	M_setOn();
	
	m_xPixels = m_yPixels = 0;

	// Horizontal bar
	m_xPixelsDir = 1;
	m_yPixelsDir = 0;
}

//--------------------------------------------------------------
void Tube::M_setOffset(float offset)
{
	m_offset = offset;
	m_Aoffset = m_A + m_offset*m_length*m_dir;
	m_Boffset = m_B - m_offset*m_length*m_dir;
	m_step.x = (m_Boffset.x-m_Aoffset.x)/float(m_nbLeds-1);
	m_step.y = (m_Boffset.y-m_Aoffset.y)/float(m_nbLeds-1);
}

//--------------------------------------------------------------
void Tube::M_display(bool isOffset)
{
	if (m_isOn){
		if (!isOffset)
			ofLine(m_A.x,m_A.y,m_B.x,m_B.y);
		else
			ofLine(m_Aoffset.x,m_Aoffset.y,m_Boffset.x,m_Boffset.y);
	}
}

//--------------------------------------------------------------
void Tube::M_sample(ofPixels& src, ofPixels& dst, int border)
{
	if (!m_isOn) return;
	
	m_pointSample = m_Aoffset;
	m_pointSample.x += border;
	m_pointSample.y += border;
	ofColor_<unsigned short> colorPixel;
	int xDst=m_xPixels;
	int yDst=m_yPixels;
	for (int i=0;i<m_nbLeds;i++)
	{
		colorPixel = src.getColor((int)m_pointSample.x, (int)m_pointSample.y);
		dst.setColor(xDst, yDst, colorPixel);
	
		m_pointSample += m_step;
		xDst+=m_xPixelsDir;
		yDst+=m_yPixelsDir;
	}
}

#define ____________Versatubes____________
//--------------------------------------------------------------
Versatubes::Versatubes(float lengthTube_, int nbLeds_)
{
	m_lengthTube	= lengthTube_;
	m_nbLedsTube	= nbLeds_;
	m_nbRows		= 4;
	m_nbCols		= 14;
	m_fboBorder		= 20;

	printf("new Versatubes(%.2f,%d)\n", m_lengthTube, m_nbLedsTube);
}

//--------------------------------------------------------------
Versatubes::~Versatubes()
{
	
}

//--------------------------------------------------------------
void Versatubes::VM_build()
{
	m_hRow		= m_lengthTube * sin(ofDegToRad(60.0f));

	float x = 0.0f, y=0.0f,xRow = 0.0f;
	int indexTube=0;
	for (int i=0;i<m_nbRows;i++)
	{
		TubeGroup* pTubeGroupRow = new TubeGroup();
		m_listTubeGroupRow.push_back(pTubeGroupRow);

		x = i*m_lengthTube/2.0f;
		xRow = x;
		
		int xPixelsLigne1 = 0;
		int xPixelsLigne2 = 0;
		for (int j=0;j<m_nbCols;j++)
		{
			// Tube horizontal (01)
			Tube* pTube1 = new Tube(indexTube++,m_lengthTube,m_nbLedsTube, x,y,x+m_lengthTube,y);
			m_listTubes.push_back(pTube1);
			pTubeGroupRow->M_addTube(pTube1);

			// Tubes verticaux (02 & 03)
			Tube* pTube2 = new Tube(indexTube++,m_lengthTube,m_nbLedsTube, x,y,x+m_lengthTube/2.0f,y+m_hRow);
			m_listTubes.push_back(pTube2);

			Tube* pTube3 = new Tube(indexTube++,m_lengthTube,m_nbLedsTube, x+m_lengthTube,y,x+m_lengthTube/2.0f,y+m_hRow);
			m_listTubes.push_back(pTube3);


			if (i==m_nbRows-1){
				pTube2->M_setOff();
				pTube2->M_lockOnOff(true);
			
				pTube3->M_setOff();
				pTube3->M_lockOnOff(true);
			}

			if (j == m_nbCols-1){
				pTube1->M_setOff();
				pTube1->M_lockOnOff(true);

				pTube3->M_setOff();
				pTube3->M_lockOnOff(true);
			}

			if (pTube1->m_isOn){
				pTube1->m_xPixels = xPixelsLigne1;
				pTube1->m_yPixels = 2*i;
				xPixelsLigne1 += pTube1->m_nbLeds;
			}

			if (pTube2->m_isOn){
				pTube2->m_xPixels = xPixelsLigne2;
				pTube2->m_yPixels = 2*i+1;
				xPixelsLigne2 += pTube2->m_nbLeds;
			}

			if (pTube3->m_isOn){
				pTube3->m_xPixels = xPixelsLigne2;
				pTube3->m_yPixels = 2*i+1;
				xPixelsLigne2 += pTube3->m_nbLeds;
			}
		
			x+=m_lengthTube;
		} // end row

		y+=m_hRow;
	}
		

	// Compute size
	float xmax=0.0f,ymax = 0.0f;
	vector<Tube*>::iterator it;
	Tube* pTube=0;
	for (it = m_listTubes.begin(); it != m_listTubes.end(); ++it){
		pTube = *it;
		if (pTube->m_isOn){
			if (!m_boundingBox.inside(pTube->m_A))
				m_boundingBox.set(0,0,pTube->m_A.x,pTube->m_A.y);
			if (!m_boundingBox.inside(pTube->m_B))
				m_boundingBox.set(0,0,pTube->m_B.x,pTube->m_B.y);
		}
	}
	
	// Canvas + Mask
	M_makeCanvas();
	M_makeMask();
	
	// Pixels (to be sent to hardware)
//	m_pixels.allocate(m_listTubes.size(),m_nbLedsTube,1);	
	m_pixels.allocate(27*m_nbLedsTube,8,1);	
	m_texPixels.allocate(m_pixels.getWidth(), m_pixels.getHeight(), GL_RGBA);
	for (int i=0;i<m_listTubes.size();i++){
//		m_listTubes[i]->m_xPixels = i;
//		m_listTubes[i]->m_yPixels = 0;
	}
}

//--------------------------------------------------------------
void Versatubes::M_readPixels()
{
	// Download pixels
	m_fboCanvas.readToPixels(m_fboCanvasPixels);	
}

//--------------------------------------------------------------
void Versatubes::M_sample()
{
	M_readPixels();
	
	m_pixels.set(0);
	vector<Tube*>::iterator it;
	for (it = m_listTubes.begin(); it != m_listTubes.end(); ++it){
		(*it)->M_sample(m_fboCanvasPixels, m_pixels, m_fboBorder);
	}
	m_texPixels.loadData(m_pixels);
}


//--------------------------------------------------------------
void Versatubes::VM_display()
{
	vector<Tube*>::iterator it;
	for (it = m_listTubes.begin(); it != m_listTubes.end(); ++it){
		(*it)->M_display(true);
	}
}

//--------------------------------------------------------------
void Versatubes::M_displayMask(float x, float y)
{
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	ofPushStyle();
	ofSetColor(255,100);
	ofPushMatrix();
		ofTranslate(x,y,0);
		m_fboMask.draw(0,0);
	ofPopMatrix();
	ofPopStyle();
	ofDisableBlendMode();
}

//--------------------------------------------------------------
void Versatubes::M_displayCanvas(float x, float y)
{
	ofPushMatrix();
		ofTranslate(x,y,0);
		m_fboCanvas.draw(0,0);
	ofPopMatrix();
}

//--------------------------------------------------------------
void Versatubes::M_displayCanvasWithMask(float x, float y)
{
	m_fboCanvasWithMask.begin();

		ofClear(0,0,0,255);
		ofSetColor(255,255); // TODO : check this ? 

		m_fboCanvas.draw(0,0);

		glEnable(GL_BLEND);
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		m_fboMask.draw(0,0);
		glDisable(GL_BLEND);
		ofDisableBlendMode();
	
	m_fboCanvasWithMask.end();

	ofPushMatrix();
		ofTranslate(x,y,0);
		m_fboCanvasWithMask.draw(0,0);
	ofPopMatrix();
}

//--------------------------------------------------------------
void Versatubes::M_displayPixels(int x, int y)
{
	m_texPixels.draw(x,y);
}


//--------------------------------------------------------------
void Versatubes::M_makeCanvas()
{
	m_fboCanvas.allocate((int)M_getWidth()+m_fboBorder*2,(int)M_getHeight()+m_fboBorder*2);
	m_fboCanvas.begin();
	ofClear(0,0,0,0);
	m_fboCanvas.end();
	m_fboCanvasPixels.allocate(m_fboCanvas.getWidth(),m_fboCanvas.getHeight(),4);

	m_fboCanvasWithMask.allocate(m_fboCanvas.getWidth(),m_fboCanvas.getHeight());
}

//--------------------------------------------------------------
void Versatubes::M_makeMask()
{
	m_fboMask.allocate((int)M_getWidth()+m_fboBorder*2,(int)M_getHeight()+m_fboBorder*2);
	m_fboMask.begin();
		ofClear(0,0,0,0);
		ofSetLineWidth(2);
		ofTranslate(0.5f*(m_fboMask.getWidth()-M_getWidth()), 0.5f*(m_fboMask.getHeight()-M_getHeight()));
		VM_display();
	m_fboMask.end();
}

//--------------------------------------------------------------
void Versatubes::M_setOffset(float offset)
{
	offset = ofClamp(offset,0.0f,0.2f);	

	vector<Tube*>::iterator it;
	for (it = m_listTubes.begin(); it != m_listTubes.end(); ++it){
		(*it)->M_setOffset(offset);
	}
}

//--------------------------------------------------------------
void Versatubes::M_enableRow(int index, bool is)
{
	if (index < m_listTubeGroupRow.size())
	{
		TubeGroup* pGroup = m_listTubeGroupRow[index];
		
		vector<Tube*>::iterator it;
		for (it = pGroup->m_listTubes.begin(); it != pGroup->m_listTubes.end(); ++it)
			(*it)->M_setOn(is);
	}
}

//--------------------------------------------------------------
void Versatubes::M_enableRowAll(bool is)
{
	int nb = m_listTubeGroupRow.size();
	for (int i=0;i<nb;i++)
		M_enableRow(i,is);
	M_makeMask();
}



