/***************************************************************************/
/*                                                                         */
/*  ShaderLibrary.cpp                                                      */
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


#if !defined(AFX_SHADERLIBRARY_H__E2A8C35E_23A4_4AD1_8467_884E6B183B4F__INCLUDED_)
	#include "ShaderLibrary.h"
#endif
#if !defined(AFX_RAPTORDATAMANAGER_H__114BFB19_FA00_4E3E_879E_C9130043668E__INCLUDED_)
	#include "DataManager/RaptorDataManager.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
    #include "System/Raptor.h"
#endif
#if !defined(AFX_OBJECTFACTORY_H__7F891C52_9E32_489C_B09C_5E5803522D91__INCLUDED_)
	#include "GLHierarchy/ObjectFactory.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_SHADERPROGRAM_H__936BEC73_3903_46CE_86C9_9CA0005B31F5__INCLUDED_)
	#include "GLHierarchy/ShaderProgram.h"
#endif
#if !defined(AFX_BLINNSHADER_H__7C69D948_B286_4840_8101_00E84647D812__INCLUDED_)
	#include "BlinnShader.h"
#endif
#if !defined(AFX_PHONGSHADER_H__C6904117_2A9C_42A8_A20B_A71A22A9C00B__INCLUDED_)
	#include "PhongShader.h"
#endif
#if !defined(AFX_BUMPSHADER_H__6201C4A1_1F09_41C4_836F_2AAC79D36A42__INCLUDED_)
	#include "BumpShader.h"
#endif
#if !defined(AFX_EMBMSHADER_H__99A5AF45_D5C7_4F43_851C_A31FC52DB237__INCLUDED_)
	#include "EMBMShader.h"
#endif
#if !defined(AFX_AOCOMPUTESHADER_H__7CD66380_1000_47A3_AA98_47E0EDBD728E__INCLUDED_)
	#include "AOComputeShader.h"
#endif
#if !defined(AFX_CONTEXTMANAGER_H__F992F5F0_D8A5_475F_9777_B0EB30E7648E__INCLUDED_)
	#include "Subsys/ContextManager.h"
#endif



RAPTOR_NAMESPACE_BEGIN

static const size_t NB_FACTORY_SHADERS = 44;
static CShaderLibrary::factory_shader fsh[NB_FACTORY_SHADERS] = 
	{ { "BUMP_TEX_SHADER", "bump.fp", "FragmentProgram" },
	  { "EMBM_TEX_SHADER", "embm.fp", "FragmentProgram" },
	  { "BUMP_VTX_SHADER", "bump.vp", "VertexProgram" },
	  { "EMBM_VTX_SHADER", "embm.vp", "VertexProgram" },
	  { "PROJECTION_TEX_SHADER", "projection.fp", "FragmentProgram" },
	  { "SHADOWMAP_TEX_SHADER", "shadowmap.fp", "FragmentProgram" },
	  { "SHADOWMAP_TEX_SHADER_PCF", "shadowmap_pcf.fp", "FragmentProgram" },
	  { "SHADOWMAP_TEX_SHADER_PCF_4X", "shadowmap_pcf_4x.fp", "FragmentProgram" },
	  { "SHADOWMAP_TEX_SHADER_PCF_16X", "shadowmap_pcf_16x.fp", "FragmentProgram" },
	  { "PPIXEL_BLINN_VTX_PROGRAM", "blinn.vs", "VertexShader" },
	  { "PPIXEL_BLINN_TEX_PROGRAM", "blinn.ps", "FragmentShader" },
	  { "PPIXEL_PHONG_VTX_PROGRAM", "phong.vs", "VertexShader" },
	  { "PPIXEL_PHONG_TEX_PROGRAM", "phong.ps", "FragmentShader" },
	  { "PPIXEL_BUMP_VTX_PROGRAM", "bump.vs", "VertexShader" },
	  { "PPIXEL_BUMP_TEX_PROGRAM", "bump.ps", "FragmentShader" },
	  { "AMBIENT_OCCLUSION_VTX_PROGRAM", "AO.vs", "VertexShader" },
	  { "AMBIENT_OCCLUSION_TEX_PROGRAM", "AO.ps", "FragmentShader" },
	  { "TEXTURE_QUAD_VTX_PROGRAM", "tquad.vs", "VertexShader" },
	  { "TEXTURE_QUAD_TEX_PROGRAM", "tquad.ps", "FragmentShader" },
	  { "TEXTURE_QUAD_GEO_PROGRAM", "tquad.gs", "GeometryShader" },
	  { "BLENDER_8X", "blender_8x.fp", "FragmentProgram" },
	  { "BLENDER_8X_XOFFSETS", "blenderX_8x.vp", "VertexProgram" },
	  { "BLENDER_8X_YOFFSETS", "blenderY_8x.vp", "VertexProgram" },
	  { "BLENDER_8X_TEX_PROGRAM", "blenderX_8x.ps", "FragmentShader" },
	  { "BLENDER_8Y_TEX_PROGRAM", "blenderY_8x.ps", "FragmentShader" },
	  { "EMPTY_PROGRAM", "empty.vs", "VertexShader" },
	  { "DIFFUSE_PROGRAM", "diffuse.ps", "FragmentShader" },
	  { "FULL_SCREEN_GEO_PROGRAM", "blender_8x.gs", "GeometryShader" },
	  { "PARTICLE_VTX_PROGRAM", "particle.vs", "VertexShader" },
	  { "PARTICLE2D_GEO_PROGRAM", "particle2D.gs", "GeometryShader" },
	  { "PARTICLE3D_GEO_PROGRAM", "particle3D.gs", "GeometryShader" },
	  { "PARTICLE3D_TEX_PROGRAM", "particle3D.ps", "FragmentShader" },
	  { "FONT2D_VTX_PROGRAM", "font2D.vs", "VertexShader" },
	  { "FONT2D_GEO_PROGRAM", "font2D.gs", "GeometryShader" },
	  { "FONT2D_TEX_PROGRAM", "font2D.ps", "FragmentShader" },
	  { "VECTORFONT_VTX_PROGRAM", "vector_font.vs", "VertexShader" },
	  { "VECTORFONT_GEO_PROGRAM", "vector_font.gs", "GeometryShader" },
	  { "VECTORFONT_TEX_PROGRAM", "vector_font.ps", "FragmentShader" },
	  { "BOX_VTX_PROGRAM", "box.vs", "VertexShader" },
	  { "FILLEDBOX_GEO_PROGRAM", "box.gs", "GeometryShader" },
	  { "WIREDBOX_GEO_PROGRAM", "box_line.gs", "GeometryShader" },
	  { "BOX_TEX_PROGRAM", "box.ps", "FragmentShader" },
	  { "FLAT_VTX_SDHADER", "flat_shading.vs", "VertexShader" },
	  { "FLAT_TEX_SDHADER", "flat_shading.ps", "FragmentShader" } };

RAPTOR_NAMESPACE_END



RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShaderLibrary::CShaderLibrary()
	:s_initialized(false),
	m_pBlinnShader(NULL), 
	m_pPhongShader(NULL),
	m_pBumpShader(NULL),
	m_pEMBMShader(NULL),
	m_pAOComputeShader(NULL)
{

}

CShaderLibrary::~CShaderLibrary()
{
	map<std::string, factory_shader>::iterator itr = s_factoryShaders.begin();
	while (itr != s_factoryShaders.end())
	{
		CPersistence *p = CPersistence::FindObject((*itr).first);
		if (NULL != p)
			if (p->getId().isSubClassOf(CShaderProgram::CShaderProgramClassID::GetClassId()))
			{
				CShaderProgram *program = (CShaderProgram *)p;
				delete program;
			}
		itr++;
	}

	//	TODO: delete shaders allocated from glAddToLibrary.
	s_factoryShaders.clear();

	//	Delete global shaders allocated from init.
	if (NULL != m_pBlinnShader)
		m_pBlinnShader->releaseReference();
	if (NULL != m_pPhongShader)
		m_pPhongShader->releaseReference();
	if (NULL != m_pBumpShader)
		m_pBumpShader->releaseReference();
	if (NULL != m_pEMBMShader)
		m_pEMBMShader->releaseReference();
	if (NULL != m_pAOComputeShader)
		m_pAOComputeShader->releaseReference();
}

void CShaderLibrary::getFactoryShaders(vector<std::string> & res)
{
	res.clear();

	map<std::string, factory_shader>::iterator itr = s_factoryShaders.begin();
	while (itr != s_factoryShaders.end())
	{
		itr++;
		res.push_back((*itr).first);
	}
}

bool CShaderLibrary::glAddToLibrary(const std::string& shader_name,
									const std::string& shader_source_file,
									const std::string& class_name)
{
	if (shader_name.empty() || shader_source_file.empty() || class_name.empty())
		return false;

	if (s_factoryShaders.find(shader_name) != s_factoryShaders.end())
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		RAPTOR_WARNING(	CShader::CShaderClassID::GetClassId(),
						"Raptor ShaderLibrary cannot import already existing shader type");
#endif
		return false;
	}

	CObjectFactory *pFactory = CObjectFactory::GetInstance();
	const CPersistentObject & po = pFactory->createObject(class_name);
	CPersistence* persistence = po;
	if (persistence->getId().ClassName() == class_name)
	{
		persistence->setName(shader_name);
		CShaderProgram *program = static_cast<CShaderProgram*>(persistence);
		if (program->glLoadProgramFromFile(shader_source_file))
		{
			factory_shader fs;
			fs.class_name = STRDUP(class_name.c_str());
			fs.shader_fname = NULL;
			fs.shader_name = STRDUP(shader_name.c_str());
			s_factoryShaders.insert(map<std::string, factory_shader>::value_type(fs.shader_name, fs));
		}

		return true;
	}
	else
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
				RAPTOR_WARNING(CShader::CShaderClassID::GetClassId(), "Raptor ShaderLibrary cannot import unknown shader type");
#endif
		return false;
	}
}


bool CShaderLibrary::glLoadShadersFromDataPackage()
{
	CShaderProgram *program = NULL;
	CRaptorDataManager *dataManager = CRaptorDataManager::GetInstance();

	CObjectFactory *pFactory = CObjectFactory::GetInstance();

	//! First export Raptor shaders interfaces
	std::string shader_path = dataManager->exportFile("Raptor.glsl");
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if (shader_path.empty())
	{
		std::string msg = "ShaderLibrary cannot find mandatory shader interface: Raptor.glsl";
		RAPTOR_FATAL(CShader::CShaderClassID::GetClassId(), msg);
	}
#endif

	for (size_t nb_shaders = 0; nb_shaders < NB_FACTORY_SHADERS; nb_shaders++)
	{
		factory_shader &fs = fsh[nb_shaders];
		shader_path = dataManager->exportFile(fs.shader_fname);
		if (!shader_path.empty())
		{
			const CPersistentObject & po = pFactory->createObject(fs.class_name);
			CPersistence* persistence = po;
			if (NULL == persistence)
			{
				std::string msg = "ShaderLibrary unsupported shader class type: ";
				msg += fs.class_name;
				RAPTOR_ERROR(CShader::CShaderClassID::GetClassId(), msg);
			}
			else if (persistence->getId().ClassName() == fs.class_name)
			{
				persistence->setName(fs.shader_name);
				program = static_cast<CShaderProgram*>(persistence);
				if (program->glLoadProgramFromFile(shader_path))
					s_factoryShaders.insert(map<std::string, factory_shader>::value_type(fs.shader_name, fs));
#ifdef RAPTOR_DEBUG_MODE_GENERATION
				std::string msg = "ShaderLibrary loaded shader type ";
				msg += fs.class_name;
				msg += " from file ";
				msg += shader_path;
				msg += " named ";
				msg += fs.shader_name;
				RAPTOR_NO_ERROR(CShader::CShaderClassID::GetClassId(),msg);
#endif
			}
		}
		else
		{
			std::string msg = "ShaderLibrary cannot find mandatory shader: ";
			msg += fs.shader_fname;
			RAPTOR_FATAL(CShader::CShaderClassID::GetClassId(), msg);
		}
	}

	return true;
}

bool CShaderLibrary::glInitFactory(void)
{
	if (s_initialized)
		return true;

	CContextManager *manager = CContextManager::GetInstance();
	if (CContextManager::INVALID_CONTEXT == manager->glGetCurrentContext())
		return false;

	s_initialized = glLoadShadersFromDataPackage();
    
	CATCH_GL_ERROR

	if (s_initialized)
	{
		CBlinnShader *pBlinnShader = new CBlinnShader();
		pBlinnShader->glInit();
		m_pBlinnShader = pBlinnShader;

		CPhongShader *pPhongShader = new CPhongShader();
		pPhongShader->glInit();
		m_pPhongShader = pPhongShader;
		
		CBumpShader *pBumpShader = new CBumpShader();
		pBumpShader->glInit();
		m_pBumpShader = pBumpShader;

		CEMBMShader *pEMBMShader = new CEMBMShader();
		pEMBMShader->glInit();
		m_pEMBMShader = pEMBMShader;

		CAOComputeShader *pAOComputeShader = new CAOComputeShader();
		pAOComputeShader->glInit();
		m_pAOComputeShader = pAOComputeShader;

		CATCH_GL_ERROR
	}

	return s_initialized;
}

