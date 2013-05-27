/*
 *  osc.h
 *  versatubes
 *
 *  Created by 2Roqs on 30/04/12.
 *  Copyright 2012 2Roqs. All rights reserved.
 *
 */

#pragma once
#include "ofxOsc.h"
#include "utils.h"

class OSCClient
{
	public:
		OSCClient		(string ip);

		void			M_setBassMediumLow				(float bl, float ml, float ll, float br, float mr, float lr);
		void			M_setupSender					();
		void			M_sendMessage					(ofxOscMessage&);
		void			M_sendEqualizerIndexes			(int indexBass, int indexMedium, int indexHigh);
		void			M_setAmplification				(float ampBass, float ampMedium, float ampHigh);
		bool			M_isClientActive				();
		void			M_setSoundInactivityThreshold	(float th);

		string			m_ip;
		ofxOscSender	m_sender;
		Poco::DateTime	m_lastDateMessage;		// Time of the last message we received


		float			m_bass[2],m_medium[2],m_low[2];
		bool			m_bStoreData;

		vector<float>	m_listBass;
		vector<float>	m_listMedium;
		vector<float>	m_listLow;
		int				m_bufferSize;

		float			m_ampBass,m_ampMedium,m_ampHigh;
		
		bool			m_isConnected;
		float			m_soundInactivityTh;
		
		int				m_sceneId;

	private:
		void			M_storeData			(vector<float>&,float);
};


class OSC
{
	public:
		OSC					();


		void				M_setup							(int port);
		void				M_setupSender					(int port);
		bool				M_hasClient						(string ip);
		OSCClient*			M_getClientByIp					(string ip);
		OSCClient*			M_getClient						(int index);
		int					M_getClientNb					();
		void				M_update						();
		void				M_sendEqualizerIndexes			(int indexBass, int indexMedium, int indexHigh);
		void				M_setAmplification				(float ampBass, float ampMedium, float ampHigh);
		int					M_getIndexClientActive			(){return m_indexClientActive;}
		int					M_getSceneIdClientActive		();
		OSCClient*			M_getClientActive				();
		void				M_changeIndexClientActive		();
		void				M_setSoundInactivityThreshold	(int th);

		vector<OSCClient*>	m_listClients;
		ofxOscReceiver		m_receiver;
		
		
		
	private:
		vector<OSCClient*>::iterator itClients;
		ofxOscMessage		m_oscMessage;
		string				m_ipClient;
		int					m_indexClientActive;
		float				m_ampBass,m_ampMedium,m_ampHigh;
};