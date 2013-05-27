/*
 *  js.cpp
 *  versatubes
 *
 *  Created by 2Roqs on 04/05/12.
 *  Copyright 2012 2Roqs. All rights reserved.
 *
 */

#include "js.h"
#include "globals.h"

//--------------------------------------------------------------
bool initJS()
{
	// Some functions
	ofxJSDefineFunctionGlobal("getClientBass",					&getClientBass,					2); // name, function pointer, args number
	ofxJSDefineFunctionGlobal("getClientMedium",				&getClientMedium,				2); // name, function pointer, args number
	ofxJSDefineFunctionGlobal("getClientHigh",					&getClientHigh,					2); // name, function pointer, args number
	ofxJSDefineFunctionGlobal("getClientBassMean",				&getClientBassMean,				1); // name, function pointer, args number
	ofxJSDefineFunctionGlobal("getClientMediumMean",			&getClientMediumMean,			1); // name, function pointer, args number
	ofxJSDefineFunctionGlobal("getClientHighMean",				&getClientHighMean,				1); // name, function pointer, args number
	ofxJSDefineFunctionGlobal("getClientCurrentIndex",			&getClientCurrentIndex,			0); // name, function pointer, args number
	ofxJSDefineFunctionGlobal("getClientCurrentSceneId",		&getClientCurrentSceneId,		0); // name, function pointer, args number

	ofxJSDefineFunctionGlobal("nowLocalFormatUnderscore",		&nowLocalFormatUnderscore,		0); // name, function pointer, args number
	ofxJSDefineFunctionGlobal("getElapsedSeconds",				&getElapsedSeconds,				0); // name, function pointer, args number
	ofxJSDefineFunctionGlobal("getRemainingSeconds",			&getRemainingSeconds,			0); // name, function pointer, args number
	ofxJSDefineFunctionGlobal("padNumberZero",					&padNumberZero,					2); // name, function pointer, args number

	ofxJSDefineFunctionGlobal("ofNoise",						&ofNoise,						3); // name, function pointer, args number


	ofxJSDefineFunctionGlobal("loadShader",						&loadShader,					1); // name, function pointer, args number
	ofxJSDefineFunctionGlobal("beginShader",					&beginShader,					0); // name, function pointer, args number
	ofxJSDefineFunctionGlobal("endShader",						&endShader,						0); // name, function pointer, args number
	ofxJSDefineFunctionGlobal("setUniform1fShader",				&setUniform1fShader,			2); // name, function pointer, args number
	ofxJSDefineFunctionGlobal("setUniform2fShader",				&setUniform2fShader,			3); // name, function pointer, args number

	ofxJSDefineFunctionGlobal("enableVersatubesRow",			&enableVersatubesRow,			2); // name, function pointer, args number
	ofxJSDefineFunctionGlobal("enableVersatubesRowAll",			&enableVersatubesRowAll,		1); // name, function pointer, args number


	// Load globals
	ofxJSScript* pScript = ofxJSLoadFromData("versatubes.js", "versatubes"); // "exampleId" is used for error reporting
	if (pScript){
		bool evalOk = ofxJSEval(pScript);
		if (evalOk){
			//printf("Loaded \"versatubes.js\"\n");
		}
	}
	
	return true; // TODO err checking
}

// ofxJSBOOL funcName(ofxJSContext* cx, ofxJSObject* obj, uintN argc, ofxJSValue* argv, ofxJSValue* retVal)
//--------------------------------------------------------------
ofxJSDefineFunctionCpp(getClientBass){
	if (argc == 2){
		int indexClient		= ofxJSValue_TO_int(argv[0]);
		int indexChannel	= ofxJSValue_TO_int(argv[1]);

		float val = Globals::instance()->M_getClientBass(indexClient,indexChannel);
		*retVal = float_TO_ofxJSValue(val);

		return JS_TRUE;
	}
	return JS_FALSE;
}

//--------------------------------------------------------------
ofxJSDefineFunctionCpp(getClientMedium){
	if (argc == 2){
		int indexClient		= ofxJSValue_TO_int(argv[0]);
		int indexChannel	= ofxJSValue_TO_int(argv[1]);

		float val = Globals::instance()->M_getClientMedium(indexClient,indexChannel);
		*retVal = float_TO_ofxJSValue(val);

		return JS_TRUE;
	}
	return JS_FALSE;
}

//--------------------------------------------------------------
ofxJSDefineFunctionCpp(getClientBassMean){
	if (argc == 1){
		int indexClient		= ofxJSValue_TO_int(argv[0]);

		float val = 0.5f*(Globals::instance()->M_getClientBass(indexClient,0)+Globals::instance()->M_getClientBass(indexClient,1));
		*retVal = float_TO_ofxJSValue(val);

		return JS_TRUE;
	}
	return JS_FALSE;
}

//--------------------------------------------------------------
ofxJSDefineFunctionCpp(getClientMediumMean){
	if (argc == 1){
		int indexClient		= ofxJSValue_TO_int(argv[0]);

		float val = 0.5f*(Globals::instance()->M_getClientMedium(indexClient,0)+Globals::instance()->M_getClientMedium(indexClient,1));
		*retVal = float_TO_ofxJSValue(val);

		return JS_TRUE;
	}
	return JS_FALSE;
}

//--------------------------------------------------------------
ofxJSDefineFunctionCpp(getClientHighMean){
	if (argc == 1){
		int indexClient		= ofxJSValue_TO_int(argv[0]);

		float val = 0.5f*(Globals::instance()->M_getClientHigh(indexClient,0)+Globals::instance()->M_getClientHigh(indexClient,1));
		*retVal = float_TO_ofxJSValue(val);

		return JS_TRUE;
	}
	return JS_FALSE;
}

//--------------------------------------------------------------
ofxJSDefineFunctionCpp(getClientHigh){
	if (argc == 2){
		int indexClient		= ofxJSValue_TO_int(argv[0]);
		int indexChannel	= ofxJSValue_TO_int(argv[1]);

		float val = Globals::instance()->M_getClientHigh(indexClient,indexChannel);
		*retVal = float_TO_ofxJSValue(val);

		return JS_TRUE;
	}
	return JS_FALSE;
}

//--------------------------------------------------------------
ofxJSDefineFunctionCpp(getClientCurrentIndex){
	*retVal = int_TO_ofxJSValue( Globals::instance()->mp_osc->M_getIndexClientActive() );
	return JS_TRUE;
}

//--------------------------------------------------------------
ofxJSDefineFunctionCpp(getClientCurrentSceneId){
	*retVal = int_TO_ofxJSValue( Globals::instance()->mp_osc->M_getSceneIdClientActive() );
	return JS_TRUE;
}


//--------------------------------------------------------------
ofxJSDefineFunctionCpp(nowLocalFormatUnderscore){
	string s = nowLocalFormatUnderscore();
	*retVal = string_TO_ofxJSValue(s);
	return JS_TRUE;
}

//--------------------------------------------------------------
ofxJSDefineFunctionCpp(getElapsedSeconds){
	*retVal = int_TO_ofxJSValue(0);
	if (Globals::instance()->mp_timeline){
		*retVal = int_TO_ofxJSValue(Globals::instance()->mp_timeline->M_getElapsedSeconds());
	}
	return JS_TRUE;
}

//--------------------------------------------------------------
ofxJSDefineFunctionCpp(getRemainingSeconds){
	*retVal = int_TO_ofxJSValue(0);
	if (Globals::instance()->mp_timeline){
		*retVal = int_TO_ofxJSValue(Globals::instance()->mp_timeline->M_getRemainingSeconds());
	}
	return JS_TRUE;
}

ofxJSDefineFunctionCpp(padNumberZero){

	if (argc == 2)
	{
		int value	= ofxJSValue_TO_int(argv[0]);
		int pad		= ofxJSValue_TO_int(argv[1]);
		
		s_jsTemp = "%0"+ofToString(pad)+"d";
		
		sprintf(s_jsStringBuffer, s_jsTemp.c_str(), value);
		s_jsTemp = s_jsStringBuffer;
		
		*retVal = string_TO_ofxJSValue(s_jsTemp);
	
		return JS_TRUE;
	}
	
	return JS_FALSE;
}


//--------------------------------------------------------------
ofxJSDefineFunctionCpp(ofNoise){
	float val = 0.0f;

	if (argc == 1)
		val = ofNoise( ofxJSValue_TO_float(argv[0]) );
	else
	if (argc == 2)
		val = ofNoise( ofxJSValue_TO_float(argv[0]), ofxJSValue_TO_float(argv[1]) );
	else
	if (argc == 3)
		val = ofNoise( ofxJSValue_TO_float(argv[0]), ofxJSValue_TO_float(argv[1]), ofxJSValue_TO_float(argv[2]) );

	*retVal = float_TO_ofxJSValue(val);
	
	return JS_TRUE;
}

//--------------------------------------------------------------
ofxJSDefineFunctionCpp(loadShader)
{
	Animation* pAnimation = Globals::instance()->mp_animationManager->mp_animationCurrent;
	if (argc==1 && pAnimation)
	{
		pAnimation->M_loadShader(ofxJSValue_TO_string(argv[0]));
		return JS_TRUE;
	}

	return JS_FALSE;
}

//--------------------------------------------------------------
ofxJSDeclareFunctionCpp(beginShader)
{
	Animation* pAnimation = Globals::instance()->mp_animationManager->mp_animationCurrent;
	if (pAnimation)
		pAnimation->M_beginShader();
	return JS_TRUE;
}

//--------------------------------------------------------------
ofxJSDeclareFunctionCpp(endShader)
{
	Animation* pAnimation = Globals::instance()->mp_animationManager->mp_animationCurrent;
	if (pAnimation)
		pAnimation->M_endShader();

	return JS_TRUE;
}

//--------------------------------------------------------------
ofxJSDeclareFunctionCpp(setUniform1fShader)
{
	Animation* pAnimation = Globals::instance()->mp_animationManager->mp_animationCurrent;
	if (argc==2 && pAnimation)
	{
		pAnimation->m_shader.setUniform1f(ofxJSValue_TO_string(argv[0]).c_str(), ofxJSValue_TO_float(argv[1]));
		return JS_TRUE;
	}
	return JS_FALSE;
}

//--------------------------------------------------------------
ofxJSDeclareFunctionCpp(setUniform2fShader)
{
	Animation* pAnimation = Globals::instance()->mp_animationManager->mp_animationCurrent;
	if (argc==3 && pAnimation)
	{
		pAnimation->m_shader.setUniform2f(ofxJSValue_TO_string(argv[0]).c_str(), ofxJSValue_TO_float(argv[1]), ofxJSValue_TO_float(argv[2]));
		return JS_TRUE;
	}
	return JS_FALSE;
}

//--------------------------------------------------------------
ofxJSDeclareFunctionCpp(enableVersatubesRow)
{
	if (argc==2 && Globals::instance()->mp_versatubes)
	{
		Globals::instance()->mp_versatubes->M_enableRow(ofxJSValue_TO_int(argv[0]), ofxJSValue_TO_bool(argv[1]));
		return JS_TRUE;
	}

	return JS_FALSE;
}

//--------------------------------------------------------------
ofxJSDeclareFunctionCpp(enableVersatubesRowAll)
{
	if (argc==1 && Globals::instance()->mp_versatubes)
	{
		Globals::instance()->mp_versatubes->M_enableRowAll(ofxJSValue_TO_bool(argv[0]));
		return JS_TRUE;
	}

	return JS_FALSE;
}
