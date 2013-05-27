/*
 *  AnimationCamera.cpp
 *  versatubes
 *
 *  Created by 2Roqs on 07/05/12.
 *  Copyright 2012 2Roqs. All rights reserved.
 *
 */

#include "animationCamera.h"

AnimationCamera::AnimationCamera(string name) : Animation(name)
{
}

void AnimationCamera::VM_enter()
{
	int camWidth 		= 320;	// try to grab at this size. 
	int camHeight 		= 240;
	
	vidGrabber.setVerbose(true);
	vidGrabber.initGrabber(camWidth,camHeight);
}

void AnimationCamera::VM_update(float dt)
{
	vidGrabber.grabFrame();
	if (vidGrabber.isFrameNew()){
	}
}

void AnimationCamera::VM_draw(float w, float h)
{
	float wCam = vidGrabber.getWidth();
	float hCam = vidGrabber.getHeight();
	float scale = w/wCam;

	ofPushMatrix();
		float y = (h-hCam*scale)/2.0;
		ofTranslate(0,y,0);
		ofScale(scale, scale, 1.0f);
		vidGrabber.draw(0,0);
	ofPopMatrix();

}

void AnimationCamera::VM_exit()
{
	vidGrabber.close();
}


