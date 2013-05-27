#pragma once

#include "ofMain.h"
#include "ofxJavascript.h"
#include "ofxUI.h"
#include "ofxUIMovingGraphMulti.h"
#include "osc.h"
#include "versatubes.h"
#include "animations.h"
#include "timeline.h"
#include "js.h"

class groupUIClient
{
	public:
		groupUIClient()
		{
			mp_eqLeft = 0;
			mp_eqRight = 0;
			mp_label = 0;
		}

		ofxUIMovingGraphMulti*	mp_eqLeft;
		ofxUIMovingGraphMulti*	mp_eqRight;
		ofxUILabel*				mp_label;
}; 

class testApp : public ofBaseApp{

	public:
		testApp			();
		~testApp		();

		void setup();
		void update();
		void draw();
		void exit();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void applyColor		();

		void gui			();
		void guiEvent		(ofxUIEventArgs &e);
		void guiUpdateLive	();
		void guiUpdate		();
		
		void setupJS				();
		void versatubesSetColorJS	();
		void versatubesUpdateJS		(float dt);
		
		void versatubesSetColorNext (ofColor cNext, Poco::DateTime next);
		
		Versatubes*			mp_versatubes;
		float				m_versatubesLength;
		int					m_versatubesNbLeds;
		ofxUICanvas*		gui1;
		ofxUICanvas*		gui2;
		ofxUICanvas*		gui3;
		AnimationManager	m_animationManager;
		OSC					m_osc;
		Timeline			m_timeline;

		bool				m_bRenderMask;
		bool				m_bRenderVersatubes;
		float				m_scaleVersatubes;
		float				m_intervalTimeline;
		int					m_indexBass,m_indexMedium,m_indexHigh;

		bool				m_bTimelineColors,m_bTryColors;
		ofColor				m_tryColor;
		ofColor				m_defaultColor;
		ofColor				m_currentColor;
		ofColor				m_timelineColor,m_timelineColorStart, m_timelineColorNext;
		Poco::DateTime		m_colorDateStart, m_colorDateNext;
		float				m_colorChangeInterval;
		
		string				m_oscClientLabel;

		// GUI
		groupUIClient		m_groupUIClient[3];
		ofxUIDropDownList*	mp_ddlAnimations;
		ofxUILabel*			mp_lblAnimationCurrent,*mp_lblAnimationTitle;
		ofxUITextInput		*mp_teBass,*mp_teMedium,*mp_teHigh;
		ofxUIRotarySlider	*mp_rsBass,*mp_rsMedium,*mp_rsHigh;
		ofxUISlider			*mp_sliderRed,*mp_sliderGreen,*mp_sliderBlue;		
		

};

