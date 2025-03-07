/***************************************************************************/
/*                                                                         */
/*  Light.cpp                                                              */
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


#include "Subsys/CodeGeneration.h"


#if !defined(AFX_LIGHT_H__AA8BABD6_059A_4939_A4B6_A0A036E12E1E__INCLUDED_)
	#include "Light.h"
#endif
#if !defined(AFX_3DENGINE_H__DB24F018_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "Engine/3DEngine.h"
#endif
#if !defined(AFX_ITEXTUREOBJECT_H__3AA8C89E_BB23_483C_A547_C8A4CC53E551__INCLUDED_)
	#include "GLHierarchy/ITextureObject.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "TextureSet.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif
#if !defined(AFX_LIGHTATTRIBUTES_H__B0A3AF95_90DC_4185_9747_B7F631DDB2BF__INCLUDED_)
	#include "Subsys/LightAttributes.h"
#endif
#if !defined(AFX_OBJECTFACTORY_H__7F891C52_9E32_489C_B09C_5E5803522D91__INCLUDED_)
	#include "GLHierarchy/ObjectFactory.h"
#endif
#if !defined(AFX_OBJECT3DINSTANCE_H__A2627662_F5F9_11D3_9142_CFEB8E9F2745__INCLUDED_)
	#include "GLHierarchy/Object3DInstance.h"
#endif
#if !defined(AFX_PROJECTOR_H__0AEE2092_215F_40FA_BBAE_7D8A2F5A482F__INCLUDED_)
    #include "GLHierarchy/Projector.h"
#endif
#if !defined(AFX_LIGHTGLOW_H__577C39B3_EE0B_4A07_8974_BC250BA2960A__INCLUDED_)
    #include "LightGlow.h"
#endif
#if !defined(AFX_LIGHTFLARE_H__373B5695_C92B_4ED8_8DDF_81273BF34FE3__INCLUDED_)
	#include "LightFlare.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif

RAPTOR_NAMESPACE

IMPLEMENT_CLASS_ID(CLight, lightID)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CLight::CLight(const std::string& name)
	:CMaterial(	CGL_NO_MATERIAL,CGL_NO_MATERIAL,CGL_NO_MATERIAL,
				0.0, CGL_NO_MATERIAL, lightID, name),
				m_pProjector(NULL), m_pGlow(NULL), m_pFlare(NULL)
{
	m_pAttributes = new CLightAttributes();
}

CLight::~CLight()
{
	if (m_pProjector != NULL)
		m_pProjector->unregisterDestruction(this);
	if (m_pGlow != NULL)
	{
		m_pGlow->unregisterDestruction(this);
		delete m_pGlow;
	}
	if (NULL != m_pFlare)
	{
		m_pFlare->unregisterDestruction(this);
		delete m_pFlare;
	}
	
	glDeActivate();

	if (m_pAttributes->m_uiFlare != 0)
		glDeleteLists(m_pAttributes->m_uiFlare,1);

	delete m_pAttributes;
}

void CLight::unLink(const CPersistence* obj)
{
	if (obj == static_cast<CPersistence*>(m_pProjector))
		m_pProjector = NULL;
	else if (obj == static_cast<CPersistence*>(m_pGlow))
		m_pGlow = NULL;
	else if (obj == static_cast<CPersistence*>(m_pFlare))
		m_pFlare = NULL;
	else
		CMaterial::unLink(obj);
}

GL_COORD_VERTEX RAPTOR_FASTCALL CLight::getLightDirection(void) const 
{ 
	return GL_COORD_VERTEX(	m_pAttributes->m_direction.X(),
							m_pAttributes->m_direction.Y(),
                            m_pAttributes->m_direction.Z(),
                            1.0f); 
};

void RAPTOR_FASTCALL CLight::setAttenuation(float quadratic,float linear, float constant)
{ 
	m_pAttributes->m_spotParams[0] = quadratic; 
	m_pAttributes->m_spotParams[1] = linear; 
	m_pAttributes->m_spotParams[2] = constant;
    
    //  1 / ( a.x^2 + b.x + c ) = 1 / 256
    float a = quadratic;
    float b = linear;
    float c = constant - m_pAttributes->m_threshold;
	float dmax = 0;
    
    if (a == 0)
    {
        if (b == 0)
            //  there is no solution with a single constant. return a large value
            //  enough to reach every object in a large scene.
            dmax = FLT_MAX; //constant;
        else
            dmax = MAX(0,-c / b);
    }
    else
    {
        if (b == 0)
            dmax = sqrt(-c / a);
        else
        {
            float delta = (b*b - 4*a*c);
            dmax = MAX((-b + sqrt(delta)) / (2*a),(-b - sqrt(delta)) / (2*a));
        }
    }

	m_pAttributes->m_dMax = dmax;
}

void RAPTOR_FASTCALL CLight::setSpotTreshold(float t)
{
    if ((t > 0.0f) && (t < 1.0f))
    {
        m_pAttributes->m_threshold = 1.0f / t;
        setAttenuation(m_pAttributes->m_spotParams[0],m_pAttributes->m_spotParams[1],m_pAttributes->m_spotParams[2]);
    }
}

GL_COORD_VERTEX RAPTOR_FASTCALL CLight::getSpotParams(void) const 
{ 
	return m_pAttributes->m_spotParams;
};

void RAPTOR_FASTCALL CLight::setCutOff(float cutoff) 
{ 
	m_pAttributes->m_spotParams[3] = cutoff; 

	if (m_pProjector)
		m_pProjector->setCutOff(cutoff);
};


void CLight::glActivate(bool spot)
{
	m_pAttributes->glActivate(this,spot);
	if (spot && (0 != m_pProjector))
		m_pProjector->glActivate(true);

    m_pAttributes->notify(this,CLightObserver::ACTIVATE);

	CATCH_GL_ERROR
}

void CLight::glDeActivate(void)
{
	m_pAttributes->glDeActivate();
	if (0 != m_pProjector)
		m_pProjector->glActivate(false);
	m_pAttributes->notify(this,CLightObserver::DEACTIVATE);

	CATCH_GL_ERROR
}

bool CLight::isActive(void)
{
    return (0 != m_pAttributes->m_hwMapping);

    // Do not perform the test below, because it could return true with another light enabled
    // or even with direct gl commands. An active light is then reduced to a valid HW mapping value
    /*
	if (0 == m_pAttributes->m_hwMapping)
		return false;
	else
		return (GL_TRUE == glIsEnabled(m_pAttributes->m_hwMapping));
    */
}

void RAPTOR_FASTCALL CLight::glProject(bool grab)
{
    CGenericMatrix<float> viewTransform;
    CGenericMatrix<float> eyeTransform;

    if (grab)
    {    
        glGetTransposeFloatv(GL_MODELVIEW_MATRIX,viewTransform);
        glGetTransposeFloatv(GL_PROJECTION_MATRIX,eyeTransform);
    }
    else
    {
        viewTransform = C3DEngine::Get3DEngine()->getModelview();
        eyeTransform = C3DEngine::Get3DEngine()->getProjection();
    }

    m_pAttributes->m_viewPosition = viewTransform * m_pAttributes->m_position;
    m_pAttributes->m_eyePosition = eyeTransform * m_pAttributes->m_viewPosition;
}

GL_COORD_VERTEX RAPTOR_FASTCALL CLight::getLightPosition(void) const
{
	GL_COORD_VERTEX lpos;
    memcpy(&lpos,m_pAttributes->m_position.vector(),sizeof(GL_COORD_VERTEX));

	return lpos; 
}

const CGenericVector<float>& RAPTOR_FASTCALL CLight::getLightViewPosition(void) const
{
    return m_pAttributes->m_viewPosition;
}

GL_COORD_VERTEX RAPTOR_FASTCALL CLight::getLightClipPosition(void) const
{
    GL_COORD_VERTEX lpos;
    lpos.x = m_pAttributes->m_eyePosition.X() / m_pAttributes->m_eyePosition.H();
    lpos.y = m_pAttributes->m_eyePosition.Y() / m_pAttributes->m_eyePosition.H();
    lpos.z = m_pAttributes->m_eyePosition.Z() / m_pAttributes->m_eyePosition.H();
    lpos.h = 1.0f;

	return lpos; 
}

float CLight::getLightAttenuation(CGenericVector<float> atPosition) const
{
    CGenericVector<float> dst = atPosition - m_pAttributes->m_viewPosition;

    float d = dst.Norm();
    float att = 1.0f / ( m_pAttributes->m_spotParams[0]*d*d + m_pAttributes->m_spotParams[1]*d + m_pAttributes->m_spotParams[2] );

    return att;
}

void RAPTOR_FASTCALL CLight::setLightPosition(const GL_COORD_VERTEX& position) 
{ 
	m_pAttributes->m_position = position;
	if (0 != m_pProjector)
		m_pProjector->setPosition(m_pAttributes->m_position);

    m_pAttributes->notify(this,CLightObserver::POSITION);
}

void RAPTOR_FASTCALL CLight::setLightDirection(const GL_COORD_VERTEX& direction) 
{ 
	m_pAttributes->m_direction = direction;
	if (0 != m_pProjector)
		m_pProjector->setDirection(m_pAttributes->m_direction);

	m_pAttributes->m_direction.Normalize();
}

void CLight::setGlow(CLightGlow *G)
{
	if (m_pGlow != NULL)
		m_pGlow->unregisterDestruction(this);

	m_pGlow = G;
	G->registerDestruction(this);
}

void CLight::setProjector(CProjector *P)
{
	if (m_pProjector != NULL)
		m_pProjector->unregisterDestruction(this);

	m_pProjector = P;
	P->registerDestruction(this);
}

void CLight::setLightVolume(float size)
{
	if (NULL == m_pFlare)
	{
		m_pFlare = new CLightFlare;
		m_pFlare->registerDestruction(this);
	}

	m_pFlare->setLightVolumeSize(size);
}

void CLight::glRender(void)
{
    GLenum hwMapping = m_pAttributes->m_hwMapping;

	if (0 != hwMapping)
	{
		glLightfv(hwMapping,GL_POSITION,m_pAttributes->m_position.vector());
		glLightfv(hwMapping,GL_AMBIENT, M.ambient);
		glLightfv(hwMapping,GL_DIFFUSE, M.diffuse);
		glLightfv(hwMapping,GL_SPECULAR, M.specular);

		if (m_pAttributes->m_spot)
		{
			glLightfv(hwMapping,GL_SPOT_DIRECTION,m_pAttributes->m_direction.vector());
			glLightfv(hwMapping,GL_SPOT_EXPONENT,&M.shininess);
			glLightfv(hwMapping,GL_SPOT_CUTOFF,&m_pAttributes->m_spotParams[3]);
			glLightfv(hwMapping,GL_CONSTANT_ATTENUATION,&m_pAttributes->m_spotParams[2]);
			glLightfv(hwMapping,GL_LINEAR_ATTENUATION,&m_pAttributes->m_spotParams[1]);
			glLightfv(hwMapping,GL_QUADRATIC_ATTENUATION,&m_pAttributes->m_spotParams[0]);

			if (m_pProjector != NULL)
				m_pProjector->glRender();
		}
	}

	CATCH_GL_ERROR
}

void CLight::glRenderEffects(void)
{
	glLoadIdentity();
	glTranslatef(m_pAttributes->m_viewPosition.X(),
				 m_pAttributes->m_viewPosition.Y(),
				 m_pAttributes->m_viewPosition.Z());

	if (NULL != m_pGlow)
		m_pGlow->glRender();

	if (NULL == m_pFlare)
		return;

	float V = m_pFlare->getLightVolumeSize();
	if (V == 0.0f)
		return;

	glPushAttrib(GL_ENABLE_BIT);

	//  Project light volume to compute total surface, and then compare it to visible surface.
	GL_COORD_VERTEX down_left(-V, -V, 0.0f, 1.0f);
	GL_COORD_VERTEX up_right(V, V, 0.0f, 1.0f);
	C3DEngine::Get3DEngine()->glProject(down_left);
	C3DEngine::Get3DEngine()->glProject(up_right);

#if defined(GL_ARB_occlusion_query)
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	GLuint query = m_pAttributes->m_visibilityQuery;
	if (query == 0)
	{
		pExtensions->glGenQueriesARB(1, &query);
		m_pAttributes->m_visibilityQuery = query;
	}
	else
		pExtensions->glGetQueryObjectuivARB(query, GL_QUERY_RESULT_ARB, &m_pAttributes->m_volumeVisibility);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDisable(GL_TEXTURE_2D);
	pExtensions->glBeginQueryARB(GL_SAMPLES_PASSED_ARB, query);
	glBegin(GL_TRIANGLE_STRIP);
		glVertex3f(-V, -V, 0.0f);
		glVertex3f(+V, -V, 0.0f);
		glVertex3f(-V, +V, 0.0f);
		glVertex3f(+V, +V, 0.0f);
	glEnd();
	pExtensions->glEndQueryARB(GL_SAMPLES_PASSED_ARB);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	GLint nbSamples = 1;
#if defined(GL_ARB_multisample)
	GLint sampleBuffers = 0;
	glGetIntegerv(GL_SAMPLE_BUFFERS_ARB, &sampleBuffers);
	if (sampleBuffers > 0)
		glGetIntegerv(GL_SAMPLES_ARB, &nbSamples);
#endif

	float surface = nbSamples * (up_right.x - down_left.x) * (up_right.y - down_left.y);
	float scale = (m_pAttributes->m_volumeVisibility / surface);
	m_pAttributes->m_fLightVolumeVisibility = scale;

	if (scale < 0.5) scale = 0.0f;  //TODO : use a treshold parameter instead of this constant value.

#else
	float surface = (up_right.x - down_left.x) * (up_right.y - down_left.y);
	//  TODO : find real scale
	//float scale = surface / (m_pAttributes->m_fLightVolumeSize*m_pAttributes->m_fLightVolumeSize);
	float scale = 1.0f;
	m_pAttributes->m_fLightVolumeVisibility = scale;
#endif

	if (scale > 0)
	{
		glScalef(scale, scale, scale);

		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);

		float dx = -2 * m_pAttributes->m_viewPosition.X() / (scale * m_pFlare->getNbFlares());
		float dy = -2 * m_pAttributes->m_viewPosition.Y() / (scale * m_pFlare->getNbFlares());
		
		m_pFlare->glRender(dx, dy);
	}

	glPopAttrib();

	CATCH_GL_ERROR
}

float CLight::getLightDMax(void) const
{
    return m_pAttributes->m_dMax;
}

float CLight::getLightVisibility(void) const
{
    return m_pAttributes->m_fLightVolumeVisibility;
}

//!   This method should be for debug only because it uses OpenGL direct mode
//! instead of vertex arrays as in CObject3D. It should be replaced or disappear.
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	void CLight::glRenderLightBBox(void)
	{
		float att = m_pAttributes->m_dMax;
    
		const CGenericVector<float>& x = m_pAttributes->m_position;

		float xmin = x.X() - att;
		float xmax = x.X() + att;
		float ymin = x.Y() - att;
		float ymax = x.Y() + att;
		float zmin = x.Z() - att;
		float zmax = x.Z() + att;
	            
		glPushAttrib(GL_ENABLE_BIT);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		glColor4f(1.0f,1.0f,1.0f,0.3f);
		glBegin(GL_QUADS);
			glVertex3f(xmin,ymin,zmin);	glVertex3f(xmax,ymin,zmin);glVertex3f(xmax,ymax,zmin);glVertex3f(xmin,ymax,zmin);
			glVertex3f(xmin,ymin,zmax);	glVertex3f(xmax,ymin,zmax);glVertex3f(xmax,ymax,zmax);glVertex3f(xmin,ymax,zmax);
			glVertex3f(xmin,ymax,zmin);	glVertex3f(xmin,ymax,zmax);glVertex3f(xmax,ymax,zmax);glVertex3f(xmax,ymax,zmin);
			glVertex3f(xmin,ymin,zmin);	glVertex3f(xmin,ymin,zmax);glVertex3f(xmax,ymin,zmax);glVertex3f(xmax,ymin,zmin);
			glVertex3f(xmin,ymin,zmin);	glVertex3f(xmin,ymin,zmax);glVertex3f(xmin,ymax,zmax);glVertex3f(xmin,ymax,zmin);
			glVertex3f(xmax,ymax,zmin);	glVertex3f(xmax,ymax,zmax);glVertex3f(xmax,ymin,zmax);glVertex3f(xmax,ymin,zmin);
		glEnd();
		glPopAttrib();
	}
#endif

CLight::R_LightProduct CLight::computeLightProduct(const CMaterial &material)
{
	CLight::R_LightProduct lp;

	lp.ambient = material.getAmbient() * M.ambient;
	lp.diffuse = material.getDiffuse() * M.diffuse;
	lp.specular = material.getSpecular() * M.specular;
	lp.enable = shader_true;
	const CGenericVector<float, 4> &p = m_pAttributes->m_viewPosition;
	lp.position = GL_COORD_VERTEX(p.X(), p.Y(), p.Z(), p.H());
	lp.attenuation = m_pAttributes->m_spotParams;

	return lp;
}

bool CLight::exportObject(CRaptorIO& o)
{
	CMaterial::exportObject(o);

	return true;
}

bool CLight::importSpotParams(CRaptorIO& io)
{
    string name;
    io >> name;

	string data = io.getValueName();
    while (io.hasMoreValues())
    {
        if (data == "treshold")
        {
            float t = 0;
            io >> t;
            setSpotTreshold(t);
        }
        else if (data == "Attenuation")
        {
            GL_COORD_VERTEX pos;
			io >> pos;
            setAttenuation(pos.x,pos.y, pos.z);
            setCutOff(pos.h);
        }
        else
			io >> name;
		
		data = io.getValueName();
	}
	io >> name;

    return true;
}

bool CLight::importObject(CRaptorIO& io)
{
    bool activate = false;

    string name;
    io >> name;

	string data = io.getValueName();
    while (io.hasMoreValues())
    {
		if (data == "name")
			CPersistence::importObject(io);
        else if (data == "active")
            io >> activate; 
        else if (data == "volumesize")
        {
            float vSize = 0;
            io >> vSize; 
            setLightVolume(vSize);
        }
		else if (data == "Material")
			CMaterial::importObject(io);
		else if (data == "Position")
		{
			GL_COORD_VERTEX pos;
			io >> pos;
			m_pAttributes->m_position = pos;
		}
		else if (data == "Direction")
		{
			GL_COORD_VERTEX pos;
			io >> pos;
			m_pAttributes->m_direction = pos;
		}
		else if (data == "Spot")
            importSpotParams(io);
        else if (data == "Glow")
		{
			if (NULL == m_pGlow)
			{
				m_pGlow = new CLightGlow;
				m_pGlow->registerDestruction(this);
			}
            m_pGlow->importObject(io);
		}
		else if (data == "Flare")
		{
			if (NULL == m_pFlare)
			{
				m_pFlare = new CLightFlare;
				m_pFlare->registerDestruction(this);
			}
			m_pFlare->importObject(io);
		}
		else
			io >> name;

		data = io.getValueName();
	}
	io >> name;

	return true;
}

