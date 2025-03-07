/***************************************************************************/
/*                                                                         */
/*  RaptorComputeMemory.cpp                                                */
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



#include "StdAfx.h"

#if !defined(AFX_RAPTOR_COMPUTE_H__7FDED2CB_7AA2_4D6E_BF22_50923FD45212__INCLUDED_)
	#include "RaptorCompute.h"
#endif
#if !defined(AFX_RAPTOR_COMPUTE_MANAGER_H__876B757E_63F1_4E8A_847E_205C3AE2E132__INCLUDED_)
	#include "RaptorComputeManager.h"
#endif
#if !defined(AFX_RAPTOR_COMPUTE_MEMORY_H__525371EA_0090_40AA_9889_333824045A7A__INCLUDED_)
	#include "RaptorComputeMemory.h"
#endif
#if !defined(AFX_RAPTOR_COMPUTE_TASK_H__F05CF7F9_151A_4760_885A_19FF9C648278__INCLUDED_)
	#include "RaptorComputeTask.h"
#endif
#if !defined(AFX_RAPTOR_COMPUTE_BUFFER_OBJECT_H__6AB1B061_4A89_4F5C_BCFE_01C26304A434__INCLUDED_)
	#include "RaptorComputeBufferObject.h"
#endif


#include <sstream>

const CRaptorComputeBufferObject* CRaptorComputeBufferObject::getBuffer(const CRaptorComputeMemory::IBufferObject *bo)
{
	std::map<const CRaptorComputeMemory::IBufferObject*,const CRaptorComputeBufferObject*>::iterator it = 
		CRaptorComputeBufferObject::s_pBuffers.find(bo);
	if (CRaptorComputeBufferObject::s_pBuffers.end() == it)
		return NULL;
	else
		return (*it).second;
}

bool CRaptorComputeBufferObject::addBuffer(const CRaptorComputeBufferObject* cb)
{
	const CRaptorComputeMemory::IBufferObject* bo = cb;
	std::map<const CRaptorComputeMemory::IBufferObject*,const CRaptorComputeBufferObject*>::iterator it = 
		CRaptorComputeBufferObject::s_pBuffers.find(bo);
	if (CRaptorComputeBufferObject::s_pBuffers.end() != it)
		return false;
	else
		s_pBuffers[bo] = cb;
	return true;
}

bool CRaptorComputeBufferObject::removeBuffer(const CRaptorComputeMemory::IBufferObject *bo)
{
	std::map<const CRaptorComputeMemory::IBufferObject*,const CRaptorComputeBufferObject*>::iterator it = 
		CRaptorComputeBufferObject::s_pBuffers.find(bo);
	if (CRaptorComputeBufferObject::s_pBuffers.end() == it)
		return false;
	else
		s_pBuffers.erase(it);
	return true;
}


std::map<const CRaptorComputeMemory::IBufferObject*,const CRaptorComputeBufferObject*> CRaptorComputeBufferObject::s_pBuffers;
std::map<unsigned int,CRaptorComputeMemory*>	CRaptorComputeMemory::s_pMemories;
static map<unsigned int,cl_context>	s_pContext;

typedef struct Buffer_Mem_Link_t
{
	CRaptorComputeMemory::IBufferObject *buffer;
	CRaptorComputeMemory *memory;
} Buffer_Mem_Link;

void CL_CALLBACK notifyMemDestruction(cl_mem memobj,void *user_data)
{
	Buffer_Mem_Link_t *link = (Buffer_Mem_Link_t*)user_data;

	stringstream str;
	str << "Desctruction of buffer object: " << memobj;
	str << " ManagerBuffer: " << link->buffer;
	str << " Memory: " << link->memory;
	RAPTOR_NO_ERROR(CRaptorComputeManager::COpenCLClassID::GetClassId(),
					str.str());

	delete link;
}

CRaptorComputeMemory::CRaptorComputeMemory(	unsigned int numPlatform,
											unsigned int numDevice)
	:maxSizes(NULL),maxAllocatable(NULL)
{
	CRaptorComputeManager &manager = CRaptorComputeManager::GetInstance();
	vector<CRaptorCompute::PLATFORM> platforms = manager.getPlatforms();

	maxSizes = 0;
	maxAllocatable = 0;

	if (numPlatform < platforms.size())
	{
		const CRaptorCompute::PLATFORM &p = platforms[numPlatform];
		if (numDevice < p.devices.size())
		{
			maxSizes = p.devices[numDevice].global_memory_size;
			maxAllocatable = p.devices[numDevice].global_memory_size;
		}
	}

	mID = ((numPlatform & 0xFFFF) << 16) + (numDevice & 0xFFFF);
}

CRaptorComputeMemory::~CRaptorComputeMemory(void)
{
	static map<unsigned int,cl_context>::iterator itc = s_pContext.find(mID);
	if (s_pContext.end() != itc)
	{
		::clReleaseContext((*itc).second);
		s_pContext.erase(itc);
	}

	map<unsigned int,CRaptorComputeMemory*>::iterator it = s_pMemories.find(mID);
	if (s_pMemories.end() != it)
	{
		s_pMemories.erase(it);
	}
}

CRaptorComputeMemory& CRaptorComputeMemory::GetInstance(unsigned int numPlatform,
														unsigned int numDevice)
{
	unsigned int num_P_D = ((numPlatform & 0xFFFF) << 16) + (numDevice & 0xFFFF);

	map<unsigned int,CRaptorComputeMemory*>::iterator it = s_pMemories.find(num_P_D);

	CRaptorComputeMemory *pMem = NULL;
	if (it == s_pMemories.end())
	{
		pMem = new CRaptorComputeMemory(numPlatform,numDevice);
		s_pMemories[num_P_D] = pMem;
	}
	else
		pMem = (*it).second;

	CRaptorComputeManager &manager = CRaptorComputeManager::GetInstance();
	cl_context ctx = manager.getContext(numPlatform,numDevice);
	::clRetainContext(ctx);
	s_pContext[num_P_D] = ctx;

	return *pMem;
}

CRaptorComputeMemory::IBufferObject*
CRaptorComputeMemory::clCreateBuffer(	size_t size,
										void *initialData,
										CRaptorComputeMemory::IBufferObject::BUFFER_KIND kind)
{
	CRaptorComputeBufferObject *res = NULL;

	if (CRaptorComputeMemory::IBufferObject::LOCAL_BUFFER == kind)
	{
		res = new CRaptorComputeBufferObject(size);
		CRaptorComputeBufferObject::addBuffer(res);
		return res;
	}
	
	if (CRaptorComputeMemory::IBufferObject::COMPUTE_BUFFER != kind)
		return res;

	static map<unsigned int,cl_context>::iterator itc = s_pContext.find(mID);
	if ((0 == size) || (s_pContext.end() == itc))
		return res;

	//	TODO : use current context instead
	if (size > maxAllocatable)
		return res;

	// allocate the buffer
	cl_int errcode = CL_SUCCESS;
	cl_mem buffer = NULL;

	if (initialData == NULL)
	{
		buffer = ::clCreateBuffer(	(*itc).second,
									CL_MEM_READ_WRITE, 
									size, 
									NULL, 
									&errcode);
	}
	else
	{
		buffer = ::clCreateBuffer(	(*itc).second,
									CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR, 
									size, 
									initialData, 
									&errcode);
	}

	if (CL_SUCCESS == errcode)
	{
		res = new CRaptorComputeBufferObject();

		res->address = buffer;
		res->m_size = size;
		res->m_storage = kind;

		Buffer_Mem_Link_t *link = new Buffer_Mem_Link_t;
		link->buffer = res;
		link->memory = this;
		errcode = ::clSetMemObjectDestructorCallback(buffer,notifyMemDestruction,link);

		//	TODO : use current context instead
		maxAllocatable -= size;
		CRaptorComputeBufferObject::addBuffer(res);
	}

	return res;
}

CRaptorComputeMemory::IBufferObject*
CRaptorComputeMemory::clCreateBuffer(IDeviceMemoryManager::IBufferObject *glBuffer)
{
	CRaptorComputeBufferObject *res = NULL;

	static map<unsigned int,cl_context>::iterator itc = s_pContext.find(mID);

	if ((NULL == glBuffer) || (s_pContext.end() == itc))
		return res;

	if ((glBuffer->getSize() > 0) && (glBuffer->getBufferId() > 0))
	{
		cl_int errCode = CL_SUCCESS;
		cl_mem buffer = NULL;

		// TODO: provide flags consistency from glBuffer kind:
		// DYNAMIC / READ_WRITE ? STREAM / READ_ONLY ? ...
		buffer = ::clCreateFromGLBuffer((*itc).second,
										CL_MEM_READ_WRITE,
										glBuffer->getBufferId(),
										&errCode);

		if (CL_SUCCESS == errCode)
		{
			res = new CRaptorComputeBufferObject();

			res->address = buffer;
			res->m_size = glBuffer->getSize();
			res->m_storage = CRaptorComputeMemory::IBufferObject::INTEROP_COMPUTE_BUFFER;

			Buffer_Mem_Link_t *link = new Buffer_Mem_Link_t;
			link->buffer = res;
			link->memory = this;
			errCode = ::clSetMemObjectDestructorCallback(buffer,notifyMemDestruction,link);

			//	TODO : use current context instead
			maxAllocatable -= glBuffer->getSize();
			CRaptorComputeBufferObject::addBuffer(res);
		}
	}

	return res;
}

bool CRaptorComputeMemory::clDestroyBuffer(CRaptorComputeMemory::IBufferObject *&bo)
{
	const CRaptorComputeBufferObject* cb = CRaptorComputeBufferObject::getBuffer(bo);
	if (NULL == cb)
		return false;

	if ((cb->address != NULL) && (cb->getSize() > 0))
	{
		cl_uint memcount = 0;
		::clGetMemObjectInfo (	cb->address,
								CL_MEM_REFERENCE_COUNT,
								sizeof(cl_uint),
								&memcount,
								NULL);
		if (memcount == 0)
		{
			RAPTOR_ERROR(	CRaptorComputeManager::COpenCLClassID::GetClassId(),
							"Release buffer object unallocated or already released.");
			return false;
		}
		else
		{
			cl_int res = ::clReleaseMemObject(cb->address);
			//	TODO : use current context instead
			if ((res == CL_SUCCESS) && (cb->address == NULL))
			{
				maxAllocatable += cb->getSize();
				delete cb;
				bo = NULL;
				CRaptorComputeBufferObject::removeBuffer(bo);
			}
			return (res == CL_SUCCESS);
		}
	}
	else if ((bo->getStorage() == IBufferObject::LOCAL_BUFFER) && 
			(bo->getSize() > 0))
	{
		delete cb;
		bo = NULL;
		CRaptorComputeBufferObject::removeBuffer(bo);
		return true;
	}
	else
		return false;
}
