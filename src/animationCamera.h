/*
 *  AnimationCamera.h
 *  versatubes
 *
 *  Created by 2Roqs on 07/05/12.
 *  Copyright 2012 2Roqs. All rights reserved.
 *
 */

#pragma once


#include "animations.h"

class AnimationCamera : public Animation
{
	public:
		AnimationCamera					(string name);
	
		void			VM_enter		();
		void			VM_update		(float dt);
		void			VM_draw			(float w, float h);
		void			VM_exit			();

//		ofTexture		m_texImage;
		ofVideoGrabber 		vidGrabber;
};
