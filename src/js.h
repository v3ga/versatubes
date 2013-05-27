/*
 *  js.h
 *  versatubes
 *
 *  Created by 2Roqs on 04/05/12.
 *  Copyright 2012 2Roqs. All rights reserved.
 *
 */

#pragma once
#include "ofMain.h"
#include "ofxJavascript.h"

bool initJS();

ofxJSDeclareFunctionCpp(getClientBass);
ofxJSDeclareFunctionCpp(getClientMedium);
ofxJSDeclareFunctionCpp(getClientHigh);
ofxJSDeclareFunctionCpp(getClientBassMean);
ofxJSDeclareFunctionCpp(getClientMediumMean);
ofxJSDeclareFunctionCpp(getClientHighMean);
ofxJSDeclareFunctionCpp(getClientCurrentIndex);
ofxJSDeclareFunctionCpp(getClientCurrentSceneId);


ofxJSDeclareFunctionCpp(nowLocalFormatUnderscore);
ofxJSDeclareFunctionCpp(getElapsedSeconds);
ofxJSDeclareFunctionCpp(getRemainingSeconds);
ofxJSDeclareFunctionCpp(padNumberZero);

ofxJSDeclareFunctionCpp(ofNoise);

ofxJSDeclareFunctionCpp(loadShader);
ofxJSDeclareFunctionCpp(beginShader);
ofxJSDeclareFunctionCpp(endShader);
ofxJSDeclareFunctionCpp(setUniform1fShader);
ofxJSDeclareFunctionCpp(setUniform2fShader);

ofxJSDeclareFunctionCpp(enableVersatubesRow);
ofxJSDeclareFunctionCpp(enableVersatubesRowAll);

static char s_jsStringBuffer[2048];
static string s_jsTemp;





