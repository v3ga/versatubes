#include "testApp.h"
#include "ofxJSOpenGL.h"
#include "ofxJSGlobalFunc.h"
#include "globals.h"

//--------------------------------------------------------------
#include "animationCamera.h"
#include "animationShader.h"

//--------------------------------------------------------------
testApp::testApp() : ofBaseApp()
{
	mp_versatubes = 0;
	m_bRenderMask = true;
	m_bRenderVersatubes = false;
	m_scaleVersatubes = 1.0f;

	mp_ddlAnimations = 0;
	mp_lblAnimationCurrent = 0;
	mp_lblAnimationTitle = 0;

	m_versatubesLength	= 60.0;
	m_versatubesNbLeds	= 18;
	
	m_intervalTimeline	= 10.0;
	
	gui1 = 0;
	gui2 = 0;
	gui3 = 0;

	mp_teBass = 0;
	mp_teMedium = 0;
	mp_teHigh = 0;

	mp_rsBass = mp_rsMedium = mp_rsHigh = 0;	
	m_indexBass = m_indexMedium = m_indexHigh = 0;

	mp_sliderRed = mp_sliderGreen = mp_sliderBlue = 0;
	
	m_defaultColor.set(255.0, 255.0);
	m_tryColor.set(255.0, 255.0);
	
	m_currentColor	= m_defaultColor;
	m_timelineColor	= m_defaultColor;

	m_colorChangeInterval = 60.0*60.0;
}

//--------------------------------------------------------------
testApp::~testApp()
{
}


//--------------------------------------------------------------
void testApp::setup(){
	ofLogLevel(OF_LOG_NOTICE);

	// Initialize JS
	int err = ofxJSInitialize();

	if (err == 0){

		// JS
		setupJS();

		// Globals
		Globals::instance()->mp_app = this;
		Globals::instance()->mp_osc = &m_osc;
		Globals::instance()->mp_animationManager = &m_animationManager;
		Globals::instance()->mp_timeline = &m_timeline;

		// Versatubes
		mp_versatubes = new Versatubes(m_versatubesLength,m_versatubesNbLeds); // length, nbLeds
		mp_versatubes->VM_build();
		Globals::instance()->mp_versatubes = mp_versatubes;

		// Animations (not scripted)
//		m_animationManager.M_addAnimation( new AnimationCamera("Camera") );
//		m_animationManager.M_addAnimation( new AnimationShader("Shader") );
		m_animationManager.M_groupAnimationByTheme();

		// Timeline
		//m_timeline.M_init("timeline.xml", m_intervalTimeline);
		//m_timeline.M_programFor(12);
		string dateEnd  = "2012-05-20 23:59:59";

		m_timeline.M_addColorToPalette( ofColor(255,0,0) ); // rouge
		m_timeline.M_addColorToPalette( ofColor(0,255,0) ); // vert
		m_timeline.M_addColorToPalette( ofColor(255,255,255) );
		m_timeline.M_addColorToPalette( ofColor(255,255,255) ); // blanc
		m_timeline.M_addColorToPalette( ofColor(255,255,255) ); // blanc
		m_timeline.M_addColorToPalette( ofColor(244,180,2) ); // Jaune

		m_timeline.M_programUntil(dateEnd);
		m_timeline.M_programColorChangeEvery(m_colorChangeInterval, dateEnd);
		m_timeline.M_sortEventByDate();
//		m_timeline.M_print();

		// Colors
		versatubesSetColorNext(m_defaultColor, nowLocal()+Poco::Timespan(3600,0));

		// UI
		gui();
		
		// OSC
		m_osc.M_setup(12345);
		
		// Update controls
		guiUpdate();

		// OF init
		ofSetVerticalSync(true);
		ofBackground(0,255);
	}
}

//--------------------------------------------------------------
void testApp::setupJS()
{
	initJS();
	
	// Read properties from JS
	ofxJSValue retVal;
	ofxJSCallFunctionNameGlobal_NoArgs("getVersatubesLength", retVal);
	m_versatubesLength = ofxJSValue_TO_float(retVal);

	ofxJSCallFunctionNameGlobal_NoArgs("getVersatubesNbLeds", retVal);
	m_versatubesNbLeds = ofxJSValue_TO_int(retVal);

	ofxJSCallFunctionNameGlobal_NoArgs("getTimelineInterval", retVal);
	m_intervalTimeline = ofxJSValue_TO_float(retVal);

	ofxJSCallFunctionNameGlobal_NoArgs("getVersatubesColorChangeInterval", retVal);
	m_colorChangeInterval = ofxJSValue_TO_float(retVal);


	// Scripts
	ofDirectory dirScripts("Scripts");
	if (dirScripts.exists())
	{
		dirScripts.listDir();			
		printf("DIR %s [%d file(s)]\n", dirScripts.path().c_str(),dirScripts.size());
	
		vector<ofFile> files = dirScripts.getFiles();
		vector<ofFile>::iterator it;
		for (it = files.begin(); it != files.end(); ++it){
			if ((*it).getExtension() == "js")
			{
				Animation* pAnimation = new Animation((*it).getFileName(), (*it).getAbsolutePath());
				
				// Eval script to extract informations from it
				if (pAnimation->M_loadScript( (*it).getAbsolutePath().c_str() )){
					
					// Theme
					ofxJSValue retValTheme,retValAutoClear;
					retValTheme = int_TO_ofxJSValue(1);
					retValAutoClear = bool_TO_ofxJSValue(true); 
					//ofxJSCallFunctionNameGlobal_NoArgs_IfExists("getTheme", retVal);
					if (pAnimation->mp_obj){
						ofxJSCallFunctionNameObject_NoArgs_IfExists(pAnimation->mp_obj,"getTheme",		retValTheme);
						ofxJSCallFunctionNameObject_NoArgs_IfExists(pAnimation->mp_obj,"getAutoClear",	retValAutoClear);
					}
					int theme = ofxJSValue_TO_int(retValTheme);
					bool isAutoClear = ofxJSValue_TO_bool(retValAutoClear);
					printf("- [%s], theme=%d, autoclear=%s\n", (*it).getFileName().c_str(),theme, isAutoClear ? "true" : "false");

					pAnimation->M_setTheme(theme);
					pAnimation->m_isAutoClear = isAutoClear;
					
					m_animationManager.M_addAnimation(pAnimation);
				}
			}
		}
	}
	dirScripts.close();

	// Call setup
	ofxJSValue retValSetup;
	ofxJSCallFunctionNameGlobal_NoArgs("setupApp", retValSetup);
}


//--------------------------------------------------------------
void testApp::versatubesSetColorJS()
{
	ofxJSValue retVal;
	ofxJSValue args[3];
	args[0] = float_TO_ofxJSValue(m_currentColor.r);
	args[1] = float_TO_ofxJSValue(m_currentColor.g);
	args[2] = float_TO_ofxJSValue(m_currentColor.b);
	ofxJSCallFunctionNameGlobal_IfExists("versatubesSetColor", args,3,retVal);
}

//--------------------------------------------------------------
void testApp::versatubesUpdateJS(float dt)
{
	ofxJSValue retVal;
	ofxJSValue args[1];
	args[0] = float_TO_ofxJSValue(dt);
	ofxJSCallFunctionNameGlobal_IfExists("updateApp", args,1,retVal);
}

//--------------------------------------------------------------
void testApp::versatubesSetColorNext(ofColor c, Poco::DateTime next)
{
	m_timelineColorStart	= m_timelineColor;
	m_timelineColorNext		= c;	
	
	m_colorDateStart	= nowLocal();
	m_colorDateNext		= next;
}


//--------------------------------------------------------------
void testApp::gui()
{
	// ==============================================================
	// gui1
	// ==============================================================
	float dim = 16;
	int widthDefault = 320;
	float h = 10;

	gui1 = new ofxUICanvas(0,h,widthDefault,1200);
	gui1->addWidgetDown(new ofxUILabel("360 000 | Nuits sonores 2012", OFX_UI_FONT_LARGE)); 

    gui1->addWidgetDown(new ofxUIFPS(OFX_UI_FONT_SMALL)); 

    gui1->addWidgetDown(new ofxUISpacer(widthDefault, 2)); 
	gui1->addWidgetDown(new ofxUILabel("RENDERING", OFX_UI_FONT_MEDIUM)); 
	gui1->addWidgetDown(new ofxUIToggle( dim, dim, false, "Fullscreen")); 	
	gui1->addWidgetRight(new ofxUIToggle( dim, dim, m_bRenderMask, "Mask")); 	
	gui1->addWidgetRight(new ofxUIToggle( dim, dim, false, "Versatubes")); 	

    gui1->addWidgetDown(new ofxUISpacer(widthDefault, 2)); 
	gui1->addWidgetDown(new ofxUILabel("ANIMATION", OFX_UI_FONT_MEDIUM)); 
	gui1->addWidgetDown(new ofxUIToggle( dim, dim, true, "Timeline")); 	
	gui1->addWidgetRight(new ofxUISlider( 232, dim, 0.5, 3.0, 2.0f, "Transition" )); 	

    gui1->addWidgetDown(new ofxUISpacer(widthDefault, 2)); 
	gui1->addWidgetDown(new ofxUILabel("SCRIPT", OFX_UI_FONT_MEDIUM)); 
	gui1->addWidgetDown(new ofxUILabelButton( 100, false, "Reload",OFX_UI_FONT_SMALL)); 	
	gui1->addWidgetRight(new ofxUILabelButton( 100, false, "Edit",OFX_UI_FONT_SMALL)); 	
	gui1->addWidgetRight(new ofxUILabelToggle( 100, Globals::instance()->m_isAutoReloadScript, "Livecoding",OFX_UI_FONT_SMALL)); 	

    gui1->addWidgetDown(new ofxUISpacer(widthDefault, 2)); 
	gui1->addWidgetDown(new ofxUILabel("VERSATUBES", OFX_UI_FONT_MEDIUM)); 
	gui1->addWidgetDown(new ofxUISlider(widthDefault, dim, 1.0, 2.0, m_scaleVersatubes, "Scale")); 
	gui1->addWidgetDown(new ofxUISlider(widthDefault, dim, 0.0, 0.2, 0.0f, "Offset")); 

    gui1->addWidgetDown(new ofxUISpacer(widthDefault, 2)); 
	gui1->addWidgetDown(new ofxUILabel("COLORS", OFX_UI_FONT_MEDIUM)); 
	gui1->addWidgetDown(new ofxUIToggle( dim, dim, false, "TimelineColors")); 	
	gui1->addWidgetRight(new ofxUIToggle( dim, dim, false, "TryColors")); 	

	mp_sliderRed	= (ofxUISlider*) gui1->addWidgetDown(new ofxUISlider( 232, dim, 0.0, 255, 255.0, "ColorRed" )); 	
	mp_sliderGreen	= (ofxUISlider*) gui1->addWidgetDown(new ofxUISlider( 232, dim, 0.0, 255, 255.0, "ColorGreen" )); 	
	mp_sliderBlue	= (ofxUISlider*) gui1->addWidgetDown(new ofxUISlider( 232, dim, 0.0, 255, 255.0, "ColorBlue" )); 	
	

	gui1->disableAppEventCallbacks();
	ofAddListener(gui1->newGUIEvent, this, &testApp::guiEvent); 

	gui1->autoSizeToFitWidgets();
	gui1->loadSettings("GUI/guiSettings.xml");

	// ==============================================================
	// gui2
	// ==============================================================
	gui2 = new ofxUICanvas(gui1->getRect()->getWidth()+10,h,widthDefault+10,1200);

    vector<string> animsNames; 
	vector<Animation*>& anims = m_animationManager.m_listAnimations;
	vector<Animation*>::iterator animsIt;
	for (animsIt = anims.begin(); animsIt != anims.end(); ++animsIt){
		animsNames.push_back( (*animsIt)->m_name );
	}

    gui2->addWidgetDown(new ofxUISpacer(0,0,widthDefault, 2)); 
	mp_lblAnimationTitle = (ofxUILabel*) gui2->addWidgetDown(new ofxUILabel("ANIMATIONS", OFX_UI_FONT_MEDIUM)); 

	mp_ddlAnimations = 	new ofxUIDropDownList(widthDefault, "Animations", animsNames, OFX_UI_FONT_SMALL);
	mp_ddlAnimations->setAutoClose(true);
    gui2->addWidgetDown(mp_ddlAnimations); 

	
//	gui2->autoSizeToFitWidgets();
//	gui2->disableAppEventCallbacks();
	ofAddListener(gui2->newGUIEvent, this, &testApp::guiEvent); 

	// ==============================================================
	// gui3
	// ==============================================================
	gui3 = new ofxUICanvas(gui2->getRect()->getX()+gui2->getRect()->getWidth()+10,h,widthDefault,1200);
    gui3->addWidgetDown(new ofxUISpacer(0,0,widthDefault, 2)); 
	gui3->addWidgetDown(new ofxUILabel("CLIENTS", OFX_UI_FONT_MEDIUM)); 

	char clientLabel[128];
	for (int i=0;i<3;i++){
		sprintf(clientLabel, "-",i);
		m_groupUIClient[i].mp_label		= new ofxUILabel(clientLabel, OFX_UI_FONT_MEDIUM);
	
		m_groupUIClient[i].mp_eqLeft	= new ofxUIMovingGraphMulti(150, 64, 256, "WAVEFORM_CLIENT_L");
		m_groupUIClient[i].mp_eqRight	= new ofxUIMovingGraphMulti(150, 64, 256, "WAVEFORM_CLIENT_R");

		m_groupUIClient[i].mp_eqLeft->addBuffer("Bass",		ofColor(255,0,0), 0, 1);
		m_groupUIClient[i].mp_eqLeft->addBuffer("Medium",	ofColor(0,255,0), 0, 1);
		m_groupUIClient[i].mp_eqLeft->addBuffer("Low",		ofColor(0,0,255), 0, 1);

		m_groupUIClient[i].mp_eqRight->addBuffer("Bass",	ofColor(255,0,0), 0, 1);
		m_groupUIClient[i].mp_eqRight->addBuffer("Medium",	ofColor(0,255,0), 0, 1);
		m_groupUIClient[i].mp_eqRight->addBuffer("Low",		ofColor(0,0,255), 0, 1);


		gui3->addWidgetDown(m_groupUIClient[i].mp_label); 
		gui3->addWidgetDown(m_groupUIClient[i].mp_eqLeft); 
		gui3->addWidgetRight(m_groupUIClient[i].mp_eqRight); 
	}


	gui3->addWidgetDown(new ofxUISpacer(0,0,widthDefault, 2)); 
	gui3->addWidgetDown(new ofxUILabel("EQUALIZER", OFX_UI_FONT_MEDIUM)); 

	mp_teBass	= (ofxUITextInput*) gui3->addWidgetDown(new ofxUITextInput(widthDefault/3-6, "Bass",		"10", OFX_UI_FONT_SMALL)); 
	mp_teMedium = (ofxUITextInput*) gui3->addWidgetRight(new ofxUITextInput(widthDefault/3-6, "Medium",	"65", OFX_UI_FONT_SMALL)); 
	mp_teHigh	= (ofxUITextInput*) gui3->addWidgetRight(new ofxUITextInput(widthDefault/3-6, "High",		"120", OFX_UI_FONT_SMALL)); 

	gui3->addWidgetDown(new ofxUISpacer(0,0,widthDefault, 2)); 
	gui3->addWidgetDown(new ofxUILabel("SIGNAL", OFX_UI_FONT_MEDIUM)); 

	mp_rsBass	= (ofxUIRotarySlider*) gui3->addWidgetDown(new ofxUIRotarySlider(dim*4, 0.0, 3.0, 1.0f, "BassAmp")); 
	mp_rsMedium = (ofxUIRotarySlider*) gui3->addWidgetRight(new ofxUIRotarySlider(dim*4, 0.0, 3.0, 1.0f, "MediumAmp")); 
	mp_rsHigh	= (ofxUIRotarySlider*) gui3->addWidgetRight(new ofxUIRotarySlider(dim*4, 0.0, 3.0, 1.0f, "HighAmp")); 

	gui3->addWidgetDown(new ofxUISlider( 232, dim, 0.1, 0.6, 0.25, "Inactivity" )); 	

	//gui3->disableAppEventCallbacks();
	ofAddListener(gui3->newGUIEvent, this, &testApp::guiEvent); 
	gui3->autoSizeToFitWidgets();

	gui3->loadSettings("GUI/gui3Settings.xml");

}


//--------------------------------------------------------------
void testApp::exit()
{
	if (gui1){
		gui1->exit();
		gui1->saveSettings("GUI/guiSettings.xml");     
	}
	
	if (gui2){
		gui2->exit();
	}

	if (gui3){
		gui3->exit();
		gui3->saveSettings("GUI/gui3Settings.xml");     
	}

	delete mp_versatubes;
    delete gui1; 
    delete gui2; 
	delete gui3;

	ofxJSFinalize();
}

//--------------------------------------------------------------
void testApp::applyColor()
{
	Poco::Timespan	deltaStart = nowLocal() - m_colorDateStart;
	Poco::Timespan	duration = m_colorDateNext - m_colorDateStart;
			
	float t = (float)deltaStart.totalMilliseconds() / (float)duration.totalMilliseconds();
	t = ofClamp(t,0.0,1.0);
	
	m_timelineColor = m_timelineColorStart.getLerped(m_timelineColorNext,t);
//	printf("deltaStart.seconds()=%d, duration.seconds()=%d, t=%.2f\n", deltaStart.totalSeconds(),duration.totalSeconds(),t);
//	printf("color=(%.1f,%.1f,%.1f), t=%.2f\n", (float)m_timelineColor.r,(float)m_timelineColor.g,(float)m_timelineColor.b,t);

	if (m_bTryColors){
		m_currentColor = m_tryColor;
	}
	else
	{
		if (m_bTimelineColors){
			m_currentColor = m_timelineColor;
		}
	}
	versatubesSetColorJS();
}

//--------------------------------------------------------------
void testApp::update(){

	// dt
	float dt = (float) ofGetLastFrameTime();

	// Color
	applyColor();
	
	// Script update
	versatubesUpdateJS(dt);

	// Timeline
	m_timeline.M_update();

	// OSC
	m_osc.M_update();

	// Animation
	m_animationManager.M_update(dt);
	
	// Update UI
	guiUpdateLive();
}

//--------------------------------------------------------------
void testApp::draw(){

	if (mp_versatubes)
	{
		// Draw in the canvas
		mp_versatubes->M_beginCanvas();
			if (m_animationManager.M_isAutoClear())
				ofClear(0,0,0,255);
			
			ofSetColor(m_currentColor,255);
			ofEnableBlendMode(OF_BLENDMODE_ALPHA);
			
			int wCanvas = mp_versatubes->m_fboCanvas.getWidth()-mp_versatubes->m_fboBorder*2;
			int hCanvas = mp_versatubes->m_fboCanvas.getHeight()-mp_versatubes->m_fboBorder*2;
			ofPushMatrix();
				ofTranslate(mp_versatubes->m_fboBorder,mp_versatubes->m_fboBorder,0);
				m_animationManager.M_drawCanvas(float(wCanvas), float(hCanvas));
			ofPopMatrix();
			m_animationManager.M_drawTransition(mp_versatubes->m_fboCanvas.getWidth(),mp_versatubes->m_fboCanvas.getHeight());

			ofDisableBlendMode();

		mp_versatubes->M_endCanvas();


		// Sample it
		mp_versatubes->M_sample();
		
		// Draw on screen
		float s = m_scaleVersatubes;
		ofPushMatrix();

			glDisable(GL_DEPTH_TEST);       
			float xCanvas = 0.5f*(ofGetViewportWidth()-mp_versatubes->m_fboCanvas.getWidth()*s);
			float yCanvas = 0.5f*(ofGetViewportHeight()-mp_versatubes->m_fboCanvas.getHeight()*s);
			ofTranslate(xCanvas,yCanvas,0);
			ofScale(s,s,1);
			if (m_bRenderMask)
				mp_versatubes->M_displayCanvasWithMask(0,0);		
			else{
				ofSetColor(m_currentColor,255);
				mp_versatubes->M_displayCanvas(0,0);		
			}

			if (m_bRenderVersatubes){
				mp_versatubes->M_displayMask(0,0);
			}

		// Bounding box
		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		ofPushStyle();
		ofSetColor(255,0,0,100);
		ofSetLineWidth(1);
		ofNoFill();
		ofRect(0,0,mp_versatubes->m_fboCanvas.getWidth(),mp_versatubes->m_fboCanvas.getHeight());
		ofPopStyle();
		ofDisableBlendMode();

		ofPopMatrix();

		// Pixels for the versatubes
		ofSetColor(m_currentColor,255);
		mp_versatubes->M_displayPixels(0,0);

	}

	if (gui1)
		gui1->draw();
}

//--------------------------------------------------------------
void testApp::guiUpdate()
{
//	if (mp_lblAnimationCurrent && m_animationManager.mp_animationCurrent) 
//		mp_lblAnimationCurrent->setLabel(m_animationManager.mp_animationCurrent->m_name);

	if (mp_lblAnimationTitle && m_animationManager.mp_animationCurrent){
		string animName = m_animationManager.mp_animationCurrent->m_name;
		if (animName.size()>=22)
			animName = animName.substr(0,22) + "...";
		string s = "ANIMATIONS / " + animName;
		mp_lblAnimationTitle->setLabel(s);
	}

}

//--------------------------------------------------------------
void testApp::guiUpdateLive()
{
	gui1->update();

	for (int i=0;i<3;i++){
		OSCClient* pClient = m_osc.M_getClient(i);
		if (pClient){
			m_oscClientLabel = pClient->m_ip+" / Scene "+ofToString(pClient->m_sceneId);
		
			m_groupUIClient[i].mp_label->setLabel(m_oscClientLabel);
		
			if (m_groupUIClient[i].mp_eqLeft){
				m_groupUIClient[i].mp_eqLeft->addPoint(0, pClient->m_bass[0]);
				m_groupUIClient[i].mp_eqLeft->addPoint(1, pClient->m_medium[0]);
				m_groupUIClient[i].mp_eqLeft->addPoint(2, pClient->m_low[0]);
			}
			
			if (m_groupUIClient[i].mp_eqRight){
				m_groupUIClient[i].mp_eqRight->addPoint(0, pClient->m_bass[1]);
				m_groupUIClient[i].mp_eqRight->addPoint(1, pClient->m_medium[1]);
				m_groupUIClient[i].mp_eqRight->addPoint(2, pClient->m_low[1]);
			}
		}
	}
}

//--------------------------------------------------------------
void testApp::guiEvent(ofxUIEventArgs &e)
{
	string name = e.widget->getName(); 
//printf("Event from %s\n",name.c_str());

	if(name == "BassAmp" || name == "MediumAmp" || name == "HighAmp")
	{
		float ampBass = mp_rsBass->getScaledValue();
		float ampMedium = mp_rsMedium->getScaledValue();
		float ampHigh = mp_rsHigh->getScaledValue();
	
		m_osc.M_setAmplification(ampBass, ampMedium, ampHigh);
	
	}

	if(name == "Scale")
	{
		m_scaleVersatubes = ((ofxUISlider *) e.widget)->getScaledValue();
	}
	else
	if(name == "Offset")
	{
		if (mp_versatubes){
			mp_versatubes->M_makeMask();
			mp_versatubes->M_setOffset( ((ofxUISlider *) e.widget)->getScaledValue() );
		}
	}
	else
	if(name == "Transition")
	{
		m_animationManager.M_setTransitionDuration( ((ofxUISlider *) e.widget)->getScaledValue() );
	}
	else
	if(name == "Mask")
	{
		ofxUIToggle *toggle = (ofxUIToggle *) e.widget; 
		m_bRenderMask = toggle->getValue(); 
	}
	if(name == "Versatubes")
	{
		ofxUIToggle *toggle = (ofxUIToggle *) e.widget; 
		m_bRenderVersatubes = toggle->getValue(); 
	}
	else
	if (name == "Reload"){
		m_animationManager.M_reloadScript();
	}
	else
	if (name == "Edit"){
		m_animationManager.M_editScript();
	}
	else
	if (name == "Livecoding"){
		Globals::instance()->m_isAutoReloadScript = ((ofxUILabelToggle *) e.widget)->getValue();
	}
	else
	if (name == "Fullscreen"){
		ofxUIToggle *btn = (ofxUIToggle*) e.widget; 
		ofSetFullscreen(btn->getValue());
	}
	else
	if (name == "Animations"){
		//ofxUIDropDownList *ddl = (ofxUIDropDownList*) e.widget; 
	}
	else
	if (name == "Timeline"){
		m_timeline.M_setActive(  ((ofxUIToggle *) e.widget)->getValue() );
		//ofxUIDropDownList *ddl = (ofxUIDropDownList*) e.widget; 
	}
	else
	if (name == "Bass" || name == "Medium" || name == "High")
	{
		m_indexBass		= atoi(mp_teBass->getTextString().c_str());
		m_indexMedium	= atoi(mp_teMedium->getTextString().c_str());
		m_indexHigh		= atoi(mp_teHigh->getTextString().c_str());
		
		m_osc.M_sendEqualizerIndexes(m_indexBass, m_indexMedium, m_indexHigh);
	}
	else
	if (name=="Inactivity")
	{
		m_osc.M_setSoundInactivityThreshold( ((ofxUISlider *) e.widget)->getScaledValue() );
	}
	else
	if (name == "ColorRed" || name == "ColorGreen" || name == "ColorBlue"){
		float r = mp_sliderRed->getScaledValue();
		float g = mp_sliderGreen->getScaledValue();
		float b = mp_sliderBlue->getScaledValue();

		m_tryColor.set(r,g,b,255.0);
	}

	else
	if (name == "TimelineColors")
	{
		m_bTimelineColors = ((ofxUIToggle *) e.widget)->getValue(); 
	}
	else
	if (name == "TryColors")
	{
		m_bTryColors = ((ofxUIToggle *) e.widget)->getValue(); 
	}
	else
	{
		if (!m_timeline.m_isActive)
		{
			if (e.widget->getKind() == OFX_UI_WIDGET_LABELTOGGLE){
				if (e.widget->getParent() == mp_ddlAnimations){
					m_animationManager.M_setAnimationDirect(mp_ddlAnimations->selected);
					//printf("%s\n",mp_ddlAnimations->selected.c_str());
					guiUpdate();
				}
			}
		}
	}

}

//--------------------------------------------------------------



//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if (key == ' '){
		if (mp_versatubes)
			ofSaveImage(mp_versatubes->m_fboCanvasPixels,"temp.png");
	}
	else
	if (key == 's'){
		ofImage img;
		img.grabScreen(0,0,ofGetViewportWidth(),ofGetViewportHeight());
		img.saveImage("screenshot.jpg");
	}
	else
	if (key == 'h'){
//		if (gui1) gui1->toggleVisible(); 
		if (gui2) gui2->toggleVisible(); 
		if (gui3) gui3->toggleVisible(); 
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	//updateLayout();
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

