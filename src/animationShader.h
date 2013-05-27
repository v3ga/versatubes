/*
 *  animationShader.h
 *  versatubes
 *
 *  Created by Julien on 10/05/12.
 *  Copyright 2012 2Roqs. All rights reserved.
 *
 */

#pragma once

#include "animations.h"

class AnimationShader : public Animation
{
	public:
		AnimationShader					(string name);
	
		void			VM_enter		();
		void			VM_update		(float dt);
		void			VM_draw			(float w, float h);
		void			VM_exit			();

};
