/*
 *  animations.h
 *  versatubes
 *
 *  Created by 2Roqs on 30/04/12.
 *  Copyright 2012 2Roqs. All rights reserved.
 *
 */
 
#pragma once

#include "ofMain.h"
#include "ofxJavascript.h"
#include "ofxTween.h"

class Animation
{
	public:
		Animation				(string name);
		Animation				(string name, string pathAbsScript);
		Animation				();
		virtual ~Animation		();

				void			M_zeroAll				();
		virtual	void			VM_setArgs				(string args);
		virtual	void			VM_enter				();
		virtual void			VM_update				(float dt);
		virtual void			VM_draw					(float w, float h);
		virtual	void			VM_exit					();

		void					M_setTheme				(int theme){m_theme=theme;}

		bool					M_loadScript			(const char* pathAbs);
		bool					M_reloadScript			();
		void					M_deleteScript			();
		
		bool					M_isScript				(){return mp_script!=0;}

		// Shader
		void					M_beginShader			();
		void					M_endShader				();
		void					M_loadShader			(string name);
		ofShader				m_shader;
		
	
		string					m_name;
		string					m_pathAbsScript;
		ofxJSScript*			mp_script;

		// TODO : SAFE with this ?
		JSObject*				mp_obj;

		bool					m_isAutoReloadScript;
		float					m_tReloadScript;
		Poco::Timestamp			m_timestampFileScript;
		
		bool					m_isAutoClear;
		
		// Theme
		int						m_theme;
		enum{
			THEME_UNKNOWN			= 0,
			THEME_SCENE				= 1,
			THEME_ECOUTE			= 2,
			THEME_CONTEMPLATION		= 3
		};
};


class AnimationManager
{
	public:
								AnimationManager		();
								~AnimationManager		();
	
		void					M_setAnimation			(int index, string args="");
		void					M_setAnimation			(string name, string args="");
		void					M_setAnimation			(Animation*, string args="");
		void					M_setAnimationDirect	(string name, string args="");

		Animation*				M_getAnimation			(int index);
		Animation*				M_getAnimationByName	(string name);
		Animation*				M_getAnimationByThemeRnd(int theme);
		string					M_getAnimationNameRnd	();
		void					M_addAnimation			(Animation*);
		void					M_groupAnimationByTheme	();

		bool					M_isAutoClear			();

		void					M_update				(float dt);
		void					M_drawCanvas			(float w, float h);
		void					M_drawTransition		(float w, float h);

		void					M_setTransitionDuration	(float d){m_transitionDuration=d;}

		bool					M_editScript			();
		bool					M_reloadScript			();

		vector<Animation*>		m_listAnimations;
		Animation*				mp_animationCurrent;
		Animation*				mp_animationNext;
		string					m_animationNextArgs;
		
		map<int,vector<Animation*>*>		m_listAnimationsTheme;

		int						m_state;
		void					M_changeState			(int newState);

		enum{
			STATE_PLAY				= 0,
			STATE_TRANSITION_IN		= 1,
			STATE_TRANSITION_OUT	= 2
		};

		// Transition
		ofxTween				m_transitionTween;
		float					m_transitionDuration;
		ofxEasingBack			easingback;
		ofxEasingLinear			easinglinear;
		
	private:
		void					M_changeAnimation		(Animation*,string args="");
		

};