/***************************************************************************/
/*                                                                         */
/*  OmniShadowMap.h                                                        */
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


#if !defined(AFX_OMNISHADOWMAP_H__FB391794_C7C1_404B_A146_061A62252C5D__INCLUDED_)
#define AFX_OMNISHADOWMAP_H__FB391794_C7C1_404B_A146_061A62252C5D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if !defined(AFX_ENVIRONMENT_H__9EA164E8_2589_4CC0_B0EA_6C95FED9F04A__INCLUDED_)
	#include "Engine/Environment.h"
#endif
#ifndef __SIMD_H__
	#include "Subsys/SimdLib/simd.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class IViewPoint;
class CRaptorDisplay;
class ITextureObject;
class CVertexProgram;
class CFragmentProgram;


class COmniShadowMap : public CEnvironment  
{
public:
	COmniShadowMap(C3DScene& rScene);
	virtual ~COmniShadowMap();

    virtual ENVIRONMENT_KIND    getKind(void) const { return OMNI_SHADOW_MAP; };

    virtual bool glInitEnvironment(const vector<C3DSceneObject*> &object);

    virtual void glRender(const CLight* currentLight,const vector<C3DSceneObject*>& objects);

    virtual void glRenderTexture(void);

	//!	Create resources.
	bool glInitialize(uint32_t width, uint32_t height);


private:
	//! Implements CEnvironment
	virtual void unLink(const CPersistence *object);

	//! Implements CEnvironment
	virtual void unLinkEnvironment(void);

	//! Implements CEnvironment
	virtual void addObject(C3DSceneObject* object);

	void glRenderMap(const CLight* currentLight,const vector<C3DSceneObject*>& objects);

    void glRenderShadow(const vector<C3DSceneObject*>& objects);


	CGenericMatrix<float>	m_lightProjection;

    IViewPoint*			m_pViewPoint;
    CRaptorDisplay*		m_pShadowCubeMap;
    ITextureObject*		m_pShadowTexture;
	CVertexProgram*		m_pVSShadowMap;
	CFragmentProgram*	m_pFSShadowMap;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_OMNISHADOWMAP_H__FB391794_C7C1_404B_A146_061A62252C5D__INCLUDED_)

