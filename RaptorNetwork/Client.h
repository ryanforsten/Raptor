// Client.h: interface for the CClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIENT_H__A2920B8F_12E4_11D3_9142_D3B83905F198__INCLUDED_)
#define AFX_CLIENT_H__A2920B8F_12E4_11D3_9142_D3B83905F198__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

using std::string;
using std::stringstream;

#if !defined(AFX_NETWORK_H__AC9D546D_A00A_4BFC_AC0C_288BE137CD20__INCLUDED_)
    #include "Network.h"
#endif



template <class ClientSocket_T> 
class CClient  
{
public:
    CClient(Network::CLIENT_MODEL mode = Network::FDPLX_CLIENT);
	virtual ~CClient();

	bool connectToServer(const std::string& address="localhost",unsigned short port=1025);
	bool disconnectServer(void);

	virtual unsigned short  getPort() const {return m_socket.getPort();};
	virtual unsigned int  getAddr() const {return m_socket.getIP();};

	void write(void *data,size_t size);
	void read(void *&data,size_t& size);


protected:
	Network::CLIENT_MODEL m_mode;

	ClientSocket_T	m_socket;
};





////////////////////////////////////////////////////////////////////////
//
//	Implementation
//
//
template <class ClientSocket_T> 
CClient<ClientSocket_T>::CClient(Network::CLIENT_MODEL mode)
	:m_mode(mode)
{
}

template <class ClientSocket_T> 
CClient<ClientSocket_T>::~CClient()
{
	disconnectServer();
}


template <class ClientSocket_T> 
void CClient<ClientSocket_T>::write(void *data,size_t size)
{
	m_socket.safeWrite(data,size);
}

template <class ClientSocket_T> 
void CClient<ClientSocket_T>::read(void *&data,size_t& size)
{
	m_socket.safeRead(data,size);
}

template <class ClientSocket_T> 
bool CClient<ClientSocket_T>::connectToServer(const std::string& address, unsigned short port)
{
	int nb_attempts = 0;
	bool result = false;

	while (!result && (nb_attempts < Network::getNbConnectAttempts()))
	{
		result = m_socket.connect(address,port);
		nb_attempts++;

		if (!result)
		{
			stringstream msg;
			msg << "Connection failed, attempt ";
			msg << nb_attempts;
			msg << "/";
			msg << Network::getNbConnectAttempts();
			Network::userOutput(INetworkLogger::NETWORK_WARNING,msg.str());
		}
	}

	if (!result)
	{
		Network::userOutput(INetworkLogger::NETWORK_ERROR,
							Network::networkErrors("Client sock connect failed"));
		return false;
	}

	return result;
}

template <class ClientSocket_T> 
bool CClient<ClientSocket_T>::disconnectServer(void)
{
	m_socket.shutdown();
	m_socket.close();

	return true;
}

#endif // !defined(AFX_CLIENT_H__A2920B8F_12E4_11D3_9142_D3B83905F198__INCLUDED_)
