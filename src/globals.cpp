/*
 *  globals.cpp
 *  versatubes
 *
 *  Created by 2Roqs on 01/05/12.
 *  Copyright 2012 2Roqs. All rights reserved.
 *
 */

#include "globals.h"
#include "testApp.h"


Globals* Globals::smp_instance = 0;

Globals::Globals()
{
	mp_osc = 0;
	mp_animationManager = 0;
	mp_timeline = 0;
	mp_versatubes = 0;
	mp_app = 0;
	m_isAutoReloadScript = false;
}

Globals* Globals::instance(){
	if (smp_instance == 0){
		smp_instance = new Globals();
	}
	return smp_instance; 
}


float Globals::M_getClientBass(int indexClient, int indexChannel)
{
	if (mp_osc){
		OSCClient* pClient = mp_osc->M_getClient(indexClient);
		if (pClient){
			if (indexChannel<2)
				return pClient->m_bass[indexChannel];
		}
	}
	return 0.0f;
}

float Globals::M_getClientMedium(int indexClient, int indexChannel)
{
	if (mp_osc){
		OSCClient* pClient = mp_osc->M_getClient(indexClient);
		if (pClient){
			if (indexChannel<2)
				return pClient->m_medium[indexChannel];
		}
	}
	return 0.0f;

}

float Globals::M_getClientHigh(int indexClient, int indexChannel)
{
	if (mp_osc){
		OSCClient* pClient = mp_osc->M_getClient(indexClient);
		if (pClient){
			if (indexChannel<2)
				return pClient->m_low[indexChannel];
		}
	}
	return 0.0f;

}

		
int Globals::M_getIndexBass()
{
	if (mp_app)
		return mp_app->m_indexBass;
	return 0;
}

int Globals::M_getIndexMedium()
{
	if (mp_app)
		return mp_app->m_indexMedium;
	return 0;
}

int Globals::M_getIndexHigh()
{
	if (mp_app)
		return mp_app->m_indexHigh;
	return 0;
}

