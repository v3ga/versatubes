/*
 *  ofxUIMovingGraphMulti.h
 *  versatubes
 *
 *  Created by Julien on 30/04/12.
 *  Copyright 2012 2Roqs. All rights reserved.
 *
 */

#pragma once

#include "ofxUIWidget.h"


class ofxUIMovingGraphBuffer
{
public:
	ofxUIMovingGraphBuffer(string _name, ofColor _color, float _min, float _max, int _bufferSize)
	{
		name = _name;
		color = _color;
		max = _max;
		min = _min;
		bufferSize = _bufferSize;
		for (int i=0;i<bufferSize;i++)
			addPoint(0.5f*(max+min));	
	}
	
	void addPoint(float _point)
	{
        buffer.push_back(_point); 
        if( buffer.size() >= bufferSize ){
            buffer.erase(buffer.begin(), buffer.begin()+1);
        }	        
	}

	string			name;
    vector<float>	buffer;
	int				bufferSize; 
	float			max, min; 
	ofColor			color;
};

class ofxUIMovingGraphMulti : public ofxUIWidget
{
public:    
    ofxUIMovingGraphMulti(float x, float y, float w, float h, int _bufferSize, string _name)
    {
        rect = new ofxUIRectangle(x,y,w,h); 
		init(w,h,_bufferSize, _name);
    //    init(w, h, _buffer, _bufferSize, _min, _max, _name);
    }
    
    ofxUIMovingGraphMulti(float w, float h, int _bufferSize, string _name)
    {
        rect = new ofxUIRectangle(0,0,w,h); 
		init(w,h,_bufferSize, _name);
        //init(w, h, _buffer, _bufferSize, _min, _max, _name);
    }    
	
	void init(float w, float h,int _bufferSize, string _name)
	{
		name = _name; 				
		kind = OFX_UI_WIDGET_MOVINGGRAPH; 

		paddedRect = new ofxUIRectangle(-padding, -padding, w+padding*2.0, h+padding*2.0);
		paddedRect->setParent(rect); 
		
        draw_fill = true; 
	
		bufferSize = _bufferSize; 
		scale = rect->getHeight()*.5; 
        inc = rect->getWidth()/((float)bufferSize-1.0);         
	}
	   
    virtual void drawFill()
    {
        if(draw_fill)
        {			
			ofNoFill(); 
/*			if(draw_fill_highlight)
			{
				ofSetColor(color_fill_highlight); 
			}        
			else 
			{
				ofSetColor(color_fill); 		 	
			}
*/			
			ofSetLineWidth(1.0); 
			for (listBuffersIt = listBuffers.begin(); listBuffersIt != listBuffers.end(); ++listBuffersIt)
			{
				ofxUIMovingGraphBuffer* pBuffer = *listBuffersIt;
			
				ofSetColor(pBuffer->color); 		 	
				glPushMatrix();
					glTranslatef(rect->getX(), rect->getY()+scale, 0);
					ofBeginShape();		
					for (int i = 0; i < bufferSize; i++)
					{					
						ofVertex(inc*(float)i, ofMap(pBuffer->buffer[i], pBuffer->min, pBuffer->max, scale, -scale, true)); 
					}
					ofEndShape();
				glPopMatrix();
			}
//			ofSetLineWidth(1); 
		}
    }
		        
	void setParent(ofxUIWidget *_parent)
	{
		parent = _parent; 
	}
	
	void addBuffer(string _name, ofColor _color, float _min,float _max)
	{
		ofxUIMovingGraphBuffer* pBuffer = new ofxUIMovingGraphBuffer(_name,_color,_min,_max,bufferSize);
		listBuffers.push_back(pBuffer);
	}
	
	void addPoint(int _index, float _point)
	{
		if (_index < listBuffers.size()){
			ofxUIMovingGraphBuffer* pBuffer = listBuffers[_index];
			pBuffer->addPoint(_point);
		}
	}
	

private:
	vector<ofxUIMovingGraphBuffer*>					listBuffers;
	vector<ofxUIMovingGraphBuffer*>::iterator		listBuffersIt;

	float	scale, inc; 
	int		bufferSize; 
}; 
