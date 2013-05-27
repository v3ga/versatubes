/*
 *  animationImage.h
 *  versatubes
 *
 *  Created by 2Roqs on 30/04/12.
 *  Copyright 2012 2Roqs. All rights reserved.
 *
 */
 
#include "animations.h"

class AnimationImage : public Animation
{
	public:
		AnimationImage					(string name, string imageName);
	
		void			VM_enter		();
		void			VM_update		(float dt);
		void			VM_draw			(float w, float h);
		void			VM_exit			();

//		ofTexture		m_texImage;

};

// TODO : implement :)