/*
 *  animationShader.cpp
 *  versatubes
 *
 *  Created by Julien on 10/05/12.
 *  Copyright 2012 2Roqs. All rights reserved.
 *
 */

#include "animationShader.h"

AnimationShader::AnimationShader(string name) : Animation(name)
{
}

void AnimationShader::VM_enter()
{
	M_loadShader("noise");
}

void AnimationShader::VM_update(float dt)
{
}

void AnimationShader::VM_draw(float w, float h)
{
	m_shader.begin();
		m_shader.setUniform1f("timeValX", ofGetElapsedTimef() * 0.1 );
		m_shader.setUniform1f("timeValY", -ofGetElapsedTimef() * 0.18 );
		m_shader.setUniform2f("mouse", 0, 0 );

printf("%.3f", ofGetElapsedTimef());
		ofSetColor(200,200,200);
		ofEllipse(w/2,h/2,200,200);

	m_shader.end();
}

void AnimationShader::VM_exit()
{
}


