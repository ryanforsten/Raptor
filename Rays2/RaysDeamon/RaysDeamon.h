// RaysDeamon.h : main header file for the RAYSDEAMON application
//

#if !defined(AFX_RAYSDEAMON_H__1FD417A3_0293_47C1_B3C3_DD773362F2E1__INCLUDED_)
#define AFX_RAYSDEAMON_H__1FD417A3_0293_47C1_B3C3_DD773362F2E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../Messages.h"			// io messages IDs and structs

#if !defined(AFX_SERVER_H__A2920B8C_12E4_11D3_9142_D3B83905F198__INCLUDED_)
    #include "RaptorNetwork/Server.h"
#endif
#if !defined(AFX_SERVERSOCKET_H__A2920B8D_12E4_11D3_9142_D3B83905F198__INCLUDED_)
	#include "RaptorNetwork/ServerSocket.h"
#endif
#if !defined(AFX_CLIENTSOCKET_H__A2920B8E_12E4_11D3_9142_D3B83905F198__INCLUDED_)
	#include "RaptorNetwork/ClientSocket.h"
#endif
#if !defined(AFX_CLIENT_H__A2920B8F_12E4_11D3_9142_D3B83905F198__INCLUDED_)
	#include "RaptorNetwork/Client.h"
#endif

#include "Raptordll.h"
RAPTOR_NAMESPACE


class RaysCmdLine;

class CRaysDeamon : public CServer<CServerSocket,CClientSocket>,
					public server_base_t::request_handler_t
{
public:
	CRaysDeamon();
	virtual ~CRaysDeamon();

	bool start(	const CCmdLineParser& cmdline );
	
	//!	Return this as request handler.
	virtual server_base_t::request_handler_t &getRequestHandler(const iosock_base_t& client) const;
	
	//!	Implements Server stop.
	virtual bool stopServer(void);

	//!	Request Deamon stop.
	void requestExit() { m_bExit = true; };

	//!	Return exit status.
	bool doExit(void) const { return m_bExit; };


	typedef struct WORKUNITSTRUCT
	{
		std::string		path;
		unsigned int	nbProcs;
		unsigned int	nbProcsAvailable;
		bool			active;
		float			jobDone;	// percentage of job actually done
		CClient<CClientSocket> *connection;// connection to the work unit
	} work_unit_struct_t;

	typedef struct Message_reg_t
	{
		MSGSTRUCT		msg;
		unsigned char	*raw_data;
	} MSGREGSTRUCT;
	typedef MSGREGSTRUCT* LPMSGREGSTRUCT;


private:
	typedef struct request_t
	{
		request_handler_t::request_id id;
		bool			reply;
		size_t			size;
		MSGSTRUCT		*msg;
	} request;


	//!	Implements Server request callback
	virtual bool handleRequest(request_handler_t::request_id id,const void *data,size_t size);

	//! Implements Server reply request
	virtual bool handleReply(request_handler_t::request_id id, const void *&data,size_t &size);

	//!	Implements Client close callback
	virtual size_t onNewClient(const CClientSocket &client);

	//!	Implements Client close callback
	virtual bool onClientClose(const CClientSocket &client);

	//!	Process DMN_DISPATCHJOB
	void dispatchJob(request &rq);

	//!	Process DMN_DISPATCHJOB
	void objPlugin(request &rq);

	//!	Process DMN_STATUS
	void dmnStatus(request &rq);

	//!	Exit deamon request
	bool	m_bExit;

	//! Array of registered work units
	std::vector<WORKUNITSTRUCT> m_WorkUnits;

	//!	Replies queue
	std::vector<request> m_replies;

	//!	Mutex to protect replies accesses
	CRaptorMutex		m_mutex;
};

#endif // !defined(AFX_RAYSDEAMON_H__1FD417A3_0293_47C1_B3C3_DD773362F2E1__INCLUDED_)
