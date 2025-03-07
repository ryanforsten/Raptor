/***************************************************************************/
/*                                                                         */
/*  RaptorClient.cpp                                                       */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "stdafx.h"

#if !defined(AFX_RAPTORSERVER_H__4595FAB9_F62A_4C9C_8356_9E204107AA14__INCLUDED_)
	#include "RaptorClient.h"
#endif
#if !defined(AFX_CMDLINEPARSER_H__D7D8768A_3D97_491F_8493_588972A3CF62__INCLUDED_)
	#include "ToolBox/CmdLineParser.h"
#endif
	
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
    #include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTORCONFIG_H__29B753B8_17DE_44DF_A4D2_9D19C5AC53D5__INCLUDED_)
    #include "System/RaptorConfig.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif
#if !defined(AFX_RAPTORCONSOLE_H__27656611_2DF3_4416_8124_F608CFAC2122__INCLUDED_)
	#include "System/RaptorConsole.h"
#endif
#if !defined(AFX_RAPTORAPPLICATION_H__E102DE21_30F2_4478_8C5A_55F780A0B33E__INCLUDED_)
	#include "System/RaptorApplication.h"
#endif
#if !defined(AFX_ANIMATOR_H__077150E3_D826_11D3_9142_9866F8B4457F__INCLUDED_)
	#include "Engine/Animator.h"
#endif
#if !defined(AFX_IVIEWPOINT_H__82071851_A036_4311_81CB_01E7E25F19E1__INCLUDED_)
	#include "Engine/IViewPoint.h"
#endif
#if !defined(AFX_3DSCENE_H__E597E752_BAD4_415D_9C00_8C59D139D32B__INCLUDED_)
	#include "Engine/3DScene.h"
#endif
#if !defined(AFX_IRENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)
	#include "GLHierarchy/IRenderingProperties.h"
#endif

#if !defined(AFX_SERVERIMAGERENDERER_H__C9720F3B_1B29_482C_84C8_1A990CEC0EBD__INCLUDED_)
	#include "ServerImageRenderer.h"
#endif
#if !defined(AFX_IMAGING_H__BD40E48F_EE12_49CF_BFBD_93658FCD0529__INCLUDED_)
	#include "ToolBox/Imaging.h"
#endif


RAPTOR_NAMESPACE


#ifdef WIN32
	DWORD processClientsThread(void* pParam)
#else
	void* processClientsThread(void* pParam)
#endif
{
	CRaptorClient *pRaptorClient = (CRaptorClient*)pParam;

	while (pRaptorClient->isRunning())
	{
		pRaptorClient->queryServerImage();
	}

	CServerImageRenderer* pImage = pRaptorClient->getImage();
	if (NULL != pImage)
		pImage->setImageData(NULL);

#ifdef WIN32
	ExitThread(0);
#else	// Linux environment
	pthread_exit(0);
#endif
}

CServerImageRenderer* CRaptorClient::getImage(void) const
{
	// Can never bu NULL after a call to run.
	return m_pImage;
}

CRaptorClient::CRaptorClient(void)
	:m_Client(NULL), client_id(NULL),
	m_pDisplay(NULL),m_pImage(NULL), m_bIsRunning(false)
{
}

CRaptorClient::~CRaptorClient(void)
{
}

bool CRaptorClient::load(const std::string& fname)
{
	bool loadResult = true;
	CRaptorIO *io = CRaptorIO::Create(fname.data(),CRaptorIO::DISK_READ,CRaptorIO::BINARY);
	
	std::streampos fsize = io->getSize();
	if (io->getStatus() == CRaptorIO::IO_OK)
	{
		std::cout << "Sending file " << fname << " to server." << std::endl;
		
		CRaptorNetwork::DATA_COMMAND cmd = CRaptorNetwork::getDataPackageCommand();
		size_t datasize = (size_t)fsize + (size_t)cmd.command.requestLen;
		cmd.size = fsize;
		cmd.packnameLen = fname.size();
		strcpy(cmd.packname, fname.c_str());

		uint8_t *buffer = new uint8_t[datasize];
		memcpy(buffer, &cmd, (size_t)cmd.command.requestLen);
		io->read(&buffer[(size_t)cmd.command.requestLen], fsize);
		
		m_Client->write(buffer, datasize);
		delete[] buffer;
	}
	else
	{
		std::cout << "Unable to send file " << fname << " to server: file does not exist or is unreadable" << std::endl;
		loadResult = false;
	}

	delete io;
	return loadResult;
}

static const int MAX_QUERIES = 5;
static int pending_queries = 0;

void CRaptorClient::queryServerImage(void)
{
	size_t size = 0;
	void *out = NULL;
	m_Client->read(out, size);

	if ((NULL == out) || (0 == size))
		return;

	CRaptorNetwork::SERVER_COMMAND *command = (CRaptorNetwork::SERVER_COMMAND*)out;

	const CRaptorNetwork::SESSION_COMMAND &cmd = CRaptorNetwork::getOpenSessionCommand();
	if (!strncmp(command->command, cmd.command.command, cmd.command.commandLen))
	{
		CRaptorNetwork::SESSION_COMMAND *session = (CRaptorNetwork::SESSION_COMMAND*)command;
		client_id = session->id;

		std::cout << "Received session_id: " << client_id << std::endl;
		return;
	}

	const CRaptorNetwork::IMAGE_COMMAND& cmd2 = CRaptorNetwork::getImageCommand();
	if (!strncmp(command->command, cmd2.command.command, cmd2.command.commandLen))
	{
		if (m_pImage == NULL)
			return;

		std::cout << "Received image_data: " << client_id << std::endl;
		
		pending_queries--;

		m_pImage->setImageData((CRaptorNetwork::IMAGE_COMMAND*)out);

		return;
	}
}

void CRaptorClient::glRender()
{
	m_pDisplay->glvkBindDisplay(m_window);

	m_pImage->glUpdateImage();

	m_pDisplay->glRender();

	m_pDisplay->glvkUnBindDisplay();

	if (pending_queries < MAX_QUERIES)
	{
		const CRaptorNetwork::SERVER_COMMAND& cmd = CRaptorNetwork::getRenderCommand();
		m_Client->write((void*)&cmd,cmd.requestLen);
		pending_queries++;
	}
}

bool CRaptorClient::run(uint32_t width, uint32_t height,
						uint32_t r_width, uint32_t r_height)
{
	if (NULL == m_Client)
		return false;

	bool res = false;

    CRaptorDisplayConfig glcs;
	glcs.width = width;
	glcs.height = height;
	glcs.x = 500;
	glcs.y = 200;
	glcs.caption = "Raptor Viewer";
	glcs.acceleration = CRaptorDisplayConfig::HARDWARE;
	glcs.double_buffer = true;
	glcs.depth_buffer = true;
	glcs.depth_buffer = true;
	glcs.display_mode = CGL_RGBA | CGL_DEPTH;

	m_window = Raptor::glCreateWindow(glcs,m_pDisplay);
	if (m_window.handle() == 0)
	{
		RAPTOR_FATAL(	CPersistence::CPersistenceClassID::GetClassId(),
						"Raptor Render Server has no resources: hardware OpenGL rendering not supported, exiting...");
		return false;
	}

	IViewPoint *vp = m_pDisplay->getViewPoint();
	vp->setViewVolume(-1.0,1.0,-1.0,1.0,1.0,100.0,IViewPoint::ORTHOGRAPHIC);

	IRenderingProperties &props = m_pDisplay->getRenderingProperties();
	props.clear(CGL_RGBA|CGL_DEPTH);
	props.setTexturing(IRenderingProperties::ENABLE);
	props.setLighting(IRenderingProperties::DISABLE);
	props.setCullFace(IRenderingProperties::DISABLE);

	res = m_pDisplay->glvkBindDisplay(m_window);
	if (res)
	{
		vp->glvkRenderViewPointModel();

		CRaptorConsole *pConsole = Raptor::GetConsole();
		pConsole->glInit();
		pConsole->showStatus(true);
		pConsole->showFPS(true);
		pConsole->activateConsole(true);
		
		glClearColor(0.2f,0.8f,0.9f,1.0f);
		m_pImage = new CServerImageRenderer(r_width, r_height);
		m_pImage->glInitImage();
		
		C3DScene *pScene = m_pDisplay->getRootScene();
		pScene->addObject(m_pImage);

		res = m_pDisplay->glvkUnBindDisplay();
		if (res)
		{
			CRaptorApplication *pApplication = CRaptorApplication::CreateApplication();
			pApplication->setRootWindow(m_window);
			pApplication->grabCursor(false);
			pApplication->run();
		}
	}

	return res;
}

bool CRaptorClient::start(const CCmdLineParser &cmdLine)
{
	//	initialize Raptor classes and settings
    CRaptorConfig config;
	config.m_bRelocation = true;
	config.m_uiPolygons = 65536;
	config.m_uiVertices = 65536;
	config.m_uiUniforms = 65536;
	config.m_logFile = "Raptor_Viewer.log";
	config.m_bAutoDestroy = false;

	bool res = Raptor::glInitRaptor(config);
	if (!res)
	{
		std::cout << "Failed to initialize Raptor layer." << std::endl;
		return false;
	}

	CAnimator::SetAnimator(new CAnimator());
	CImaging::installImagers();

	if (res)
    {
		RAPTOR_NO_ERROR(CPersistence::CPersistenceClassID::GetClassId(),
						"Raptor Toolbox initialized. ");
		std::cout << "Raptor Toolbox initialized. " << std::endl;
    }
    else
    {
		RAPTOR_FATAL(	CPersistence::CPersistenceClassID::GetClassId(),
						"Failed to initialize Raptor. ");
		std::cout << "Failed to initialize Raptor. " << std::endl;
    }

	if (m_Client == NULL)
		m_Client = new CClient<CClientSocket>();

	string addrStr = "127.0.0.1";
	unsigned short port = 2048;
	cmdLine.getValue("port",port);
	cmdLine.getValue("host_addr",addrStr);
	res = m_Client->connectToServer(addrStr,port);
	if (res)
	{
		stringstream str;
		str << "Raptor Viewer ready on port ";
		str << port;
		str << " at host ";
		str << addrStr;
		RAPTOR_NO_ERROR(CPersistence::CPersistenceClassID::GetClassId(),str.str());
	}
	else
	{
		stringstream str;
		str << "Raptor Viewer couldn't connect to server on port ";
		str << port;
		str << " at host ";
		str << addrStr;
		RAPTOR_FATAL(CPersistence::CPersistenceClassID::GetClassId(),str.str());
		return false;
	}

	CRaptorNetwork::SESSION_COMMAND cmd = CRaptorNetwork::getOpenSessionCommand();
	cmdLine.getValue("r_width",cmd.width);
	cmdLine.getValue("r_height",cmd.height);
	m_Client->write((void*)&cmd,cmd.command.requestLen);

	unsigned long int ui_threadID = 0;
	m_bIsRunning = true;
#ifdef WIN32
	m_hThread = CreateThread(NULL,
							 0,
							 (LPTHREAD_START_ROUTINE)processClientsThread,
							 this,
							 0,
							 &ui_threadID);
#else // Linux environment
	pthread_create(&ui_threadID, NULL, processClientsThread, this);
	m_hThread = (void*)ui_threadID;
#endif
	
	return (NULL != m_hThread);
}

bool CRaptorClient::stop(void)
{
	if (m_Client == NULL)
		return false;
	else
	{
		if (0 != m_hThread)
		{
			m_bIsRunning = false;
#ifdef WIN32
			WaitForSingleObject((HANDLE)m_hThread, INFINITE);
			CloseHandle((HANDLE)m_hThread);
#else // Linux environment
			void *ret = NULL;
			pthread_join((pthread_t)m_hThread, &ret);
#endif
			m_hThread = 0;
		}
		
		m_Client->disconnectServer();
		return Raptor::glQuitRaptor();
	}
}



