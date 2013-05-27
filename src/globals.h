/*
 *  globals.h
 *  versatubes
 *
 *  Created by 2Roqs on 01/05/12.
 *  Copyright 2012 2Roqs. All rights reserved.
 *
 */

#pragma once
#include "osc.h"
#include "animations.h"
#include "timeline.h"
#include "versatubes.h"

class OSC;
class testApp;
class Globals
{
	public:
		Globals					();


		static Globals*			instance();
		OSC*					mp_osc;
		AnimationManager*		mp_animationManager;
		Timeline*				mp_timeline;
		Versatubes*				mp_versatubes;
		testApp*				mp_app;

		float					M_getClientBass		(int indexClient, int indexChannel);
		float					M_getClientMedium	(int indexClient, int indexChannel);
		float					M_getClientHigh		(int indexClient, int indexChannel);
		
		int						M_getIndexBass		();
		int						M_getIndexMedium	();
		int						M_getIndexHigh		();
	
		bool					m_isAutoReloadScript;		

	private:
		static Globals*			smp_instance;
};
