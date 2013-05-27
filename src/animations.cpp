/*
 *  animations.cpp
 *  versatubes
 *
 *  Created by 2Roqs on 30/04/12.
 *  Copyright 2012 2Roqs. All rights reserved.
 *
 */

#include "animations.h"
#include "globals.h"
#include "utils.h"


#define ____________Animation____________
//--------------------------------------------------------------
Animation::Animation()
{
	M_zeroAll();
}

//--------------------------------------------------------------
Animation::Animation(string name_)
{
	M_zeroAll();
	m_name			= name_;
}

//--------------------------------------------------------------
Animation::Animation(string name_,string pathAbsScript_)
{
	M_zeroAll();
	m_name			= name_;
	m_pathAbsScript = pathAbsScript_;

	mp_obj = JS_NewObject(ofxJSGetContext(), NULL, NULL, ofxJSGetGlobalObj());
//	JS_AddObjectRoot(ofxJSGetContext(), &mp_obj);
	JS_AddNamedRoot(ofxJSGetContext(), &mp_obj, m_name.c_str());
}

//--------------------------------------------------------------
Animation::~Animation(){
//	printf("~Animation()\n");
	delete mp_script;
}

//--------------------------------------------------------------
void Animation::M_zeroAll()
{
	mp_script		= 0;
	mp_obj			= 0;
	m_theme			= THEME_UNKNOWN;
	m_isAutoClear	= true;
}

//--------------------------------------------------------------
bool Animation::M_loadScript(const char* s)
{
//	printf("M_loadScript() > path='%s'\n", s);
	string pathAbs = s;
	if (pathAbs != "")
	{
		mp_script = ofxJSLoad(pathAbs,"___tmpScript___");
		if (mp_script){

//			if (ofxJSEval(mp_script))
			if (ofxJSEvalOnObject(mp_script, mp_obj))
			{
//				printf("OK > evaluated '%s'\n", s);
				//ofxJSValue retVal;
				return true;
			}
			else{
				printf("ERROR > cannot eval script '%s'\n", s);
				M_deleteScript();
			}
		}
		else{
			printf("ERROR > cannot load script '%s'\n", s);
			M_deleteScript();
		}
	}
	else
		printf("ERROR > path for script is empty...\n");

	return false;
}

//--------------------------------------------------------------
bool Animation::M_reloadScript()
{
	return M_loadScript(m_pathAbsScript.c_str());
}

//--------------------------------------------------------------
void Animation::VM_setArgs(string args)
{
}

//--------------------------------------------------------------
void Animation::M_beginShader()
{
	m_shader.begin();
}

//--------------------------------------------------------------
void Animation::M_endShader()
{
	m_shader.end();
}

//--------------------------------------------------------------
void Animation::M_loadShader(string name)
{
	string v = "Shaders/"+name+".vert";
	string f = "Shaders/"+name+".frag";
	m_shader.load(v,f);
}



//--------------------------------------------------------------
void Animation::VM_enter()
{
	if (M_reloadScript())
	{
		ofxJSValue retVal;
	//	ofxJSCallFunctionNameGlobal_NoArgs_IfExists("setup", retVal);
		if (mp_obj){
			ofxJSCallFunctionNameObject_NoArgs_IfExists(mp_obj, "setup", retVal);
		}
	}
}

//--------------------------------------------------------------
void Animation::VM_exit()
{
	ofxJSValue retVal;
	if (mp_obj){
//		ofxJSCallFunctionNameObject_NoArgs_IfExists(mp_obj, "setup", retVal);
		ofxJSCallFunctionNameObject_NoArgs_IfExists(mp_obj, "exit", retVal);
	}

	M_deleteScript();	
}

//--------------------------------------------------------------
void Animation::M_deleteScript()
{
	if (mp_script){
		delete mp_script;
		mp_script=0;
	}
}

//--------------------------------------------------------------
void Animation::VM_update(float dt)
{
	if (mp_script)
	{
		// Reload
		if (Globals::instance()->m_isAutoReloadScript){
			m_tReloadScript += dt;
			if (m_tReloadScript>=1.0f){
				//M_reloadScript();
				// Check time stamp of file
				ofFile fileScript(m_pathAbsScript);
				Poco::File& pocoFileScript = fileScript.getPocoFile();
				Poco::Timestamp timestamp = pocoFileScript.getLastModified();
				if (timestamp > m_timestampFileScript){
					m_timestampFileScript = timestamp;
					M_reloadScript();
				}
			}
		}

		// Call update
		ofxJSValue retVal;
		ofxJSValue args[1];
		args[0] = float_TO_ofxJSValue(dt);
//		ofxJSCallFunctionNameGlobal_IfExists("update", args,1,retVal);

		if (mp_obj){
			ofxJSCallFunctionNameObject_IfExists(mp_obj,"update", args,1,retVal);
		}

	}
}

//--------------------------------------------------------------
void Animation::VM_draw(float w, float h)
{
	if (mp_script){
		ofxJSValue retVal;
		ofxJSValue args[2];
		args[0] = float_TO_ofxJSValue(w);
		args[1] = float_TO_ofxJSValue((h+1));
//		ofxJSCallFunctionNameGlobal_IfExists("draw", args,2,retVal);
		if (mp_obj){
			ofxJSCallFunctionNameObject_IfExists(mp_obj,"draw", args,2,retVal);
		}

	}
}

#define ____________AnimationManager____________

//--------------------------------------------------------------
AnimationManager::AnimationManager()
{
	mp_animationCurrent = 0;	
	mp_animationNext	= 0;
	m_state				= STATE_PLAY;
	m_transitionDuration = 2.0f;
	//m_transitionTween.setFrameBasedAnimation(true);
}

//--------------------------------------------------------------
AnimationManager::~AnimationManager()
{
	vector<Animation*>::iterator it;
	for (it = m_listAnimations.begin(); it != m_listAnimations.end(); ++it){
		delete *it;
	}
}

//--------------------------------------------------------------
void AnimationManager::M_setAnimation(int index, string args)
{
	Animation* pAnimation = M_getAnimation(index);
	M_setAnimation(pAnimation, args);
}

//--------------------------------------------------------------
void AnimationManager::M_setAnimation(string name, string args)
{
	vector<Animation*>::iterator it;
	for (it = m_listAnimations.begin(); it != m_listAnimations.end(); ++it){
		if (name == (*it)->m_name){
			M_setAnimation(*it,args);
			break;
		}
	}
}

//--------------------------------------------------------------
void AnimationManager::M_setAnimation(Animation* pAnimation, string args)
{
	mp_animationNext = pAnimation;
	m_animationNextArgs = args;
	M_changeState(STATE_TRANSITION_OUT);
}

//--------------------------------------------------------------
void AnimationManager::M_setAnimationDirect(string name, string args)
{
	vector<Animation*>::iterator it;
	for (it = m_listAnimations.begin(); it != m_listAnimations.end(); ++it){
		if (name == (*it)->m_name){
			M_changeAnimation(*it, args);
			break;
		}
	}
}


//--------------------------------------------------------------
void AnimationManager::M_changeAnimation(Animation* pAnimationNext, string args)
{
	if (pAnimationNext)
	{
		if (mp_animationCurrent)
			mp_animationCurrent->VM_exit();

		mp_animationCurrent = pAnimationNext;
		mp_animationCurrent->VM_setArgs(args);
		mp_animationCurrent->VM_enter();
	}
}

//--------------------------------------------------------------
Animation* AnimationManager::M_getAnimation(int index)
{
	if (index < m_listAnimations.size())
		return m_listAnimations[index];
	return 0;
}

//--------------------------------------------------------------
Animation* AnimationManager::M_getAnimationByName(string name)
{
	vector<Animation*>::iterator it;
	for (it = m_listAnimations.begin(); it != m_listAnimations.end(); ++it){
		if ((*it)->m_name == name)
			return *it;
	}	
	return 0;
}

//--------------------------------------------------------------
Animation* AnimationManager::M_getAnimationByThemeRnd(int theme)
{
	int nb = m_listAnimationsTheme[theme]->size();
	if (nb>0)
	{
		int rnd = (int)ofRandom(0,nb);
		return m_listAnimationsTheme[theme]->at(rnd); 
	}	


/*	
	vector<Animation*>::iterator it;
	for (it = m_listAnimations.begin(); it != m_listAnimations.end(); ++it){
		if ((*it)->m_name == name)
			return *it;
	}	
	return 0;
*/
	return 0;
}


//--------------------------------------------------------------
void AnimationManager::M_addAnimation(Animation* pAnimation)
{
	m_listAnimations.push_back(pAnimation);
}

//--------------------------------------------------------------
string AnimationManager::M_getAnimationNameRnd()
{
	int index = (int)ofRandom(m_listAnimations.size()-1);
	return m_listAnimations[index]->m_name;
}

//--------------------------------------------------------------
void AnimationManager::M_groupAnimationByTheme()
{
	m_listAnimationsTheme[Animation::THEME_UNKNOWN]			= new vector<Animation*>();	
	m_listAnimationsTheme[Animation::THEME_SCENE]			= new vector<Animation*>();	
	m_listAnimationsTheme[Animation::THEME_ECOUTE]			= new vector<Animation*>();	
	m_listAnimationsTheme[Animation::THEME_CONTEMPLATION]	= new vector<Animation*>();	

	vector<Animation*>::iterator it;
	for (it = m_listAnimations.begin(); it != m_listAnimations.end(); ++it){
		m_listAnimationsTheme[(*it)->m_theme]->push_back( *it );
	}	
	
	printf("------------------------------------\n");
	for (int i=0;i<4;i++){
		printf("> %d\n", i);
		for (it = m_listAnimationsTheme[i]->begin(); it != m_listAnimationsTheme[i]->end(); ++it)
		{
			printf("  %s\n", (*it)->m_name.c_str());
		}
	}
	printf("------------------------------------\n");
}


//--------------------------------------------------------------
bool AnimationManager::M_isAutoClear()
{
	if (mp_animationCurrent)
		return mp_animationCurrent->m_isAutoClear;
	return true;
}

//--------------------------------------------------------------
void AnimationManager::M_update(float dt)
{
	// Update current animation
	if (mp_animationCurrent)
		mp_animationCurrent->VM_update(dt);
		
		
	// Update transition tween
	if (m_state == STATE_TRANSITION_OUT || m_state == STATE_TRANSITION_IN)
		m_transitionTween.update();

	// tween end ? 
	if (m_transitionTween.isCompleted())
	{
		if (m_state == STATE_TRANSITION_OUT) 
			M_changeState(STATE_TRANSITION_IN);
		else
		if (m_state == STATE_TRANSITION_IN) 
			M_changeState(STATE_PLAY);
	}
}

//--------------------------------------------------------------
void AnimationManager::M_drawCanvas(float w, float h)
{
	// Draw animation
	if (mp_animationCurrent)
		mp_animationCurrent->VM_draw(w,h);
}


//--------------------------------------------------------------
void AnimationManager::M_drawTransition(float w, float h)
{
	// Draw transition
	if (m_state == STATE_TRANSITION_OUT || m_state == STATE_TRANSITION_IN)
	{
		//printf("tween=%.2f\n", m_transitionTween.getTarget(0));
	
		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		ofPushStyle();
			ofSetColor(0,0,0, 255*m_transitionTween.getTarget(0));
			ofRect(0,0,w,h);
		ofPopStyle();
		ofDisableBlendMode();
	}
}

//--------------------------------------------------------------
bool AnimationManager::M_editScript()
{
	if (mp_animationCurrent && mp_animationCurrent->M_isScript()){
		ofLVOpenProgram(mp_animationCurrent->m_pathAbsScript);
	}
}

//--------------------------------------------------------------
bool AnimationManager::M_reloadScript()
{
	if (mp_animationCurrent)
		return mp_animationCurrent->M_reloadScript();
}


//--------------------------------------------------------------
void AnimationManager::M_changeState(int newState)
{
	if (m_state == STATE_PLAY)
	{
		if (newState == STATE_TRANSITION_OUT)
		{
			m_transitionTween.setParameters(1, easinglinear, ofxTween::easeOut, 0, 1, m_transitionDuration*1000.0f, 0);
			m_transitionTween.start();
			m_state = newState;
			
//			printf("STATE_PLAY > STATE_TRANSITION_OUT\n");
		}
	}
	else
	if (m_state == STATE_TRANSITION_OUT)
	{
		if (newState == STATE_TRANSITION_IN)
		{
			//if (mp_animationNext != mp_animationCurrent)
			if (mp_animationNext)
			{
				M_changeAnimation(mp_animationNext,m_animationNextArgs);
				mp_animationNext = 0;

				m_transitionTween.setParameters(1, easinglinear, ofxTween::easeOut, 1, 0, m_transitionDuration*1000.0f, 0);
				m_transitionTween.start();
			}
			m_state = newState;
		
//			printf("STATE_TRANSITION_OUT > STATE_TRANSITION_IN\n");
		}
	}
	else
	if (m_state == STATE_TRANSITION_IN)
	{
		if (newState == STATE_PLAY)
		{
			m_state = newState;
//			printf("STATE_TRANSITION_IN > STATE_PLAY\n");
		}
	}
}

