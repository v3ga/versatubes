/*
 *  osc.cpp
 *  versatubes
 *
 *  Created by 2Roqs on 30/04/12.
 *  Copyright 2012 2Roqs. All rights reserved.
 *
 */

#include "osc.h"
#include "globals.h"

#define ____________OSCClient____________
//--------------------------------------------------------------
OSCClient::OSCClient(string ip_)
{
	m_ip = ip_;
	m_bufferSize = 60*60*1; // 60 fps * 60 seconds * (n) minutes
	m_bStoreData = true;
	m_isConnected = true;	// created when received a message from client

	M_setAmplification(1.0f,1.0f,1.0f);
	m_soundInactivityTh = 0.25f;
	
	m_sceneId = 1;
}


//--------------------------------------------------------------
void OSCClient::M_setupSender()
{
	m_sender.setup(m_ip, 12344);
}

//--------------------------------------------------------------
void OSCClient::M_sendMessage(ofxOscMessage& m)
{
	m_sender.sendMessage(m);
}


//--------------------------------------------------------------
void OSCClient::M_sendEqualizerIndexes(int indexBass, int indexMedium, int indexHigh)
{
	ofxOscMessage m;
		m.setAddress( "/eq/indexes" );

		m.addIntArg(indexBass);
		m.addIntArg(indexMedium);
		m.addIntArg(indexHigh);
	M_sendMessage(m);
}

//--------------------------------------------------------------
void OSCClient::M_setAmplification(float ampBass, float ampMedium, float ampHigh)
{
	m_ampBass=ampBass;
	m_ampMedium=ampMedium;
	m_ampHigh=ampHigh;
	
	//printf("M_setAmplification(%.2f,%.2f,%.2f)\n",m_ampBass,m_ampMedium,m_ampHigh);
}


//--------------------------------------------------------------
void OSCClient::M_setBassMediumLow(float bl, float ml, float ll, float br, float mr, float lr)
{
	m_lastDateMessage  = nowLocal();

	m_bass[0]		= ofClamp(m_ampBass*bl,0.0,1.0);
	m_medium[0]		= ofClamp(m_ampMedium*ml,0.0,1.0);
	m_low[0]		= ofClamp(m_ampHigh*ll,0.0,1.0);

	m_bass[1]		= ofClamp(m_ampBass*br,0.0,1.0);
	m_medium[1]		= ofClamp(m_ampMedium*mr,0.0,1.0);
	m_low[1]		= ofClamp(m_ampHigh*lr,0.0,1.0);
	
	
	if (m_bStoreData)
	{
		M_storeData(m_listBass,		0.5f*(m_bass[0]+m_bass[1]));
		M_storeData(m_listMedium,	0.5f*(m_medium[0]+m_medium[1]));
		M_storeData(m_listLow,		0.5f*(m_low[0]+m_low[1]));
	}
}




//--------------------------------------------------------------
void OSCClient::M_storeData(vector<float>& list,float val)
{
	list.push_back(val); 
	if( list.size() >= m_bufferSize )
		list.erase(list.begin(), list.begin()+1);
}

//--------------------------------------------------------------
bool OSCClient::M_isClientActive()
{
	// Check connectivity
	if (m_isConnected)
	{
		Poco::DateTime now = nowLocal();
		Poco::Timespan delta = now - m_lastDateMessage;
		if (delta.totalSeconds()>15){
			m_isConnected = false;
			printf(">>> WARNING client %s disconnected\n", m_ip.c_str());
		}
	}	

	// Check sound
	bool isSoundActivity = false;
	float total = 0.0f;
	if (m_isConnected){
		vector<float>::iterator it;
		for (it = m_listMedium.begin(); it != m_listMedium.end() ; ++it){
			total += *it;
		}
		total /= (float)m_bufferSize;
		if (total >= m_soundInactivityTh){
			isSoundActivity = true;
		}
	}

	bool is = (m_isConnected && isSoundActivity);
	printf("OSCClient::M_isClientActive()\n  - TOTAL = %.2f\n  - CONNECTED=%s\n  - ACTIVE=%s\n", total, m_isConnected?"true":"false", is?"true":"false");
	
	return is;
}

//--------------------------------------------------------------
void OSCClient::M_setSoundInactivityThreshold(float th)
{
	m_soundInactivityTh = th;
}

#define ____________OSC____________
//--------------------------------------------------------------
OSC::OSC()
{
	m_indexClientActive = 0;
	m_ampBass = m_ampMedium = m_ampHigh = 1.0f;
}

//--------------------------------------------------------------
void OSC::M_setup(int port)
{
	m_receiver.setup(port);
}


//--------------------------------------------------------------
void OSC::M_sendEqualizerIndexes(int indexBass, int indexMedium, int indexHigh)
{
	for (itClients = m_listClients.begin(); itClients != m_listClients.end(); ++itClients){
		(*itClients)->M_sendEqualizerIndexes(indexBass, indexMedium, indexHigh);
	}
}

//--------------------------------------------------------------
void OSC::M_update()
{
	// OSC messages
	if (m_receiver.hasWaitingMessages()){
		while(m_receiver.getNextMessage(&m_oscMessage))
		{
			m_ipClient = m_oscMessage.getRemoteIp();
		
			if (!M_hasClient(m_ipClient))
			{
				// Create client
				OSCClient* pNewClient = new OSCClient(m_ipClient);

				// Run osc sender
				pNewClient->M_setupSender();
				
				// Send the current values
				pNewClient->M_sendEqualizerIndexes(Globals::instance()->M_getIndexBass(),Globals::instance()->M_getIndexMedium(),Globals::instance()->M_getIndexHigh());
				
				// Apply some preset values
				pNewClient->M_setAmplification(m_ampBass,m_ampMedium,m_ampHigh);	
				
				// Scene Id
				if (m_oscMessage.getNumArgs() == 7)
					pNewClient->m_sceneId = m_oscMessage.getArgAsInt32(6);
				
				// Save
				m_listClients.push_back(pNewClient);
				printf("OSC > new client connected %s\n", m_ipClient.c_str());
			}

			OSCClient* pClient = M_getClientByIp(m_ipClient);
			if (pClient){
				pClient->M_setBassMediumLow(
											m_oscMessage.getArgAsFloat(0),
											m_oscMessage.getArgAsFloat(1),
											m_oscMessage.getArgAsFloat(2),

											m_oscMessage.getArgAsFloat(3),
											m_oscMessage.getArgAsFloat(4),
											m_oscMessage.getArgAsFloat(5)
											);
			}
		}
	}

	// Check activity
/*	for (itClients = m_listClients.begin(); itClients != m_listClients.end(); ++itClients)
	{
		(*itClients)->M_isClientActive();	
	}
*/
}

//--------------------------------------------------------------
bool OSC::M_hasClient(string ip)
{
	for (itClients = m_listClients.begin(); itClients != m_listClients.end(); ++itClients)
	{
		if ((*itClients)->m_ip == ip)
			return true;
	}	
	return false;
}

//--------------------------------------------------------------
OSCClient* OSC::M_getClientByIp(string ip)
{
	for (itClients = m_listClients.begin(); itClients != m_listClients.end(); ++itClients)
	{
		if ((*itClients)->m_ip == ip)
			return *itClients;
	}	
	return 0;
}

//--------------------------------------------------------------
OSCClient* OSC::M_getClient(int index)
{
	if (index < m_listClients.size())
		return m_listClients[index];
	return 0;
}

//--------------------------------------------------------------
int OSC::M_getClientNb()
{
	return m_listClients.size();
}

//--------------------------------------------------------------
OSCClient* OSC::M_getClientActive()
{
	return M_getClient(m_indexClientActive);
}

//--------------------------------------------------------------
void OSC::M_changeIndexClientActive()
{
	// Take next
	int nbClients = m_listClients.size();
	if (m_listClients.size()>0){
		int indexPrev = m_indexClientActive;
		m_indexClientActive = (m_indexClientActive+1)%nbClients;
	}
}

//--------------------------------------------------------------
int OSC::M_getSceneIdClientActive()
{
	OSCClient* pClient = M_getClientActive();
	if (pClient)
		return pClient->m_sceneId;
	return 1;
}


//--------------------------------------------------------------
void OSC::M_setAmplification(float ampBass, float ampMedium, float ampHigh)
{
	m_ampBass = ampBass;
	m_ampMedium = ampMedium;
	m_ampHigh = ampHigh;

	for (itClients = m_listClients.begin(); itClients != m_listClients.end(); ++itClients)
		(*itClients)->M_setAmplification(ampBass, ampMedium, ampHigh);
}

//--------------------------------------------------------------
void OSC::M_setSoundInactivityThreshold	(int th)
{
	for (itClients = m_listClients.begin(); itClients != m_listClients.end(); ++itClients)
		(*itClients)->M_setSoundInactivityThreshold(th);
}




