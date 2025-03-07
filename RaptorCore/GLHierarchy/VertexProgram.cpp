/***************************************************************************/
/*                                                                         */
/*  VertexProgram.cpp                                                      */
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


#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_VERTEXPROGRAM_H__F2D3BBC6_87A1_4695_B667_2B8C3C4CF022__INCLUDED_)
	#include "VertexProgram.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_OBJECTFACTORY_H__7F891C52_9E32_489C_B09C_5E5803522D91__INCLUDED_)
	#include "ObjectFactory.h"
#endif
#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif


RAPTOR_NAMESPACE

IMPLEMENT_CLASS_ID(CVertexProgram, vertexId)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CVertexProgram::CVertexProgram(const std::string& name)
	:CShaderProgram(vertexId,name)
{
    m_bValid = false;
	m_handle.handle(0);	// default openGL vertex processing pipeline
	m_handle.hClass(CVertexProgram::CVertexProgramClassID::GetClassId().ID());
}

CVertexProgram::CVertexProgram(const CVertexProgram& shader)
	:CShaderProgram(shader)
{
	m_bValid = shader.m_bValid;
}

CVertexProgram* CVertexProgram::glClone()
{
	return new CVertexProgram(*this);
}

CVertexProgram::~CVertexProgram()
{
#if defined(GL_ARB_vertex_program)
	if (!CRaptorInstance::GetInstance().isVertexProgramReady())
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		Raptor::GetErrorManager()->generateRaptorError(	CVertexProgram::CVertexProgramClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_ERROR,
														"Raptor Vertex programs are not initialised : no program were created with a valid GL context");
#endif
	}
	else
	{
        glStop();

		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		GLuint program = m_handle.glhandle();
		if (pExtensions->glIsProgramARB(program))
			pExtensions->glDeleteProgramsARB(1,&program);
	}
#endif
}

//
//	Management of vertex attributes
//

void RAPTOR_FASTCALL CVertexProgram::glVertex(const GL_COORD_VERTEX &v)
{
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

	if (m_handle.glhandle() == 0)
		glVertex4fv(v);
#if defined(GL_ARB_vertex_program)
	else if (CRaptorInstance::GetInstance().isVertexProgramReady())
		pExtensions->glVertexAttrib4fvARB(CProgramParameters::POSITION, v);
#endif

	//A call within a display list generates an error !!!
	//CATCH_GL_ERROR
}

void RAPTOR_FASTCALL CVertexProgram::glVertexWeightEXT(float w)
{
#if defined(GL_ARB_vertex_program)
	if (CRaptorInstance::GetInstance().isVertexProgramReady())
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		pExtensions->glVertexAttrib1fARB(CProgramParameters::WEIGHTS, w);
	}
#endif
	CATCH_GL_ERROR
}

void RAPTOR_FASTCALL CVertexProgram::glNormal(const GL_COORD_VERTEX &n)
{
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

	if (m_handle.glhandle() == 0)
		glNormal3fv(n);
#if defined(GL_ARB_vertex_program)
	else if (CRaptorInstance::GetInstance().isVertexProgramReady())
		pExtensions->glVertexAttrib4fvARB(CProgramParameters::NORMAL, n);
#endif

	//A call within a display list generates an error !!!
	//CATCH_GL_ERROR
}

void RAPTOR_FASTCALL CVertexProgram::glColor(const CColor::RGBA &c)
{
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

	if (m_handle.glhandle() == 0)
		glColor4fv(c);
#if defined(GL_ARB_vertex_program)
	else if (CRaptorInstance::GetInstance().isVertexProgramReady())
		pExtensions->glVertexAttrib4fvARB(CProgramParameters::PRIMARY_COLOR, c);
#endif

	//A call within a display list generates an error !!!
	//CATCH_GL_ERROR
}

void RAPTOR_FASTCALL CVertexProgram::glSecondaryColorEXT(const CColor::RGBA &c)
{
#if defined(GL_ARB_vertex_program)
	if (CRaptorInstance::GetInstance().isVertexProgramReady())
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		pExtensions->glVertexAttrib4fvARB(CProgramParameters::SECONDARY_COLOR, c);
	}
#endif

	CATCH_GL_ERROR
}

void RAPTOR_FASTCALL CVertexProgram::glFogCoordEXT(float f)
{
#if defined(GL_ARB_vertex_program)
	if (CRaptorInstance::GetInstance().isVertexProgramReady())
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		pExtensions->glVertexAttrib1fARB(CProgramParameters::FOG_COORDINATE, f);
	}
#endif

	CATCH_GL_ERROR
}

void RAPTOR_FASTCALL CVertexProgram::glMultiTexCoord(CProgramParameters::GL_VERTEX_ATTRIB tmu, const GL_COORD_VERTEX &t)
{
	// Assert the tmu is valid
	if (tmu < CProgramParameters::TEXCOORD0)
		return;

	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	if (m_handle.glhandle() == 0)
	{
#if defined(GL_ARB_multitexture)
		pExtensions->glMultiTexCoord4fvARB(tmu - CProgramParameters::TEXCOORD0 + GL_TEXTURE0_ARB, t);
#else
		glTexCoord4fv(t);
#endif
	}
#if defined(GL_ARB_vertex_program)
	else if (CRaptorInstance::GetInstance().isVertexProgramReady())
		pExtensions->glVertexAttrib4fvARB(tmu,t);
#endif

	//A call within a display list generates an error !!!
	//CATCH_GL_ERROR
}

void RAPTOR_FASTCALL CVertexProgram::glAdditionalVertexParam(const GL_COORD_VERTEX &v)
{
#if defined(GL_ARB_vertex_program)
	if (CRaptorInstance::GetInstance().isVertexProgramReady())
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		pExtensions->glVertexAttrib4fvARB(CProgramParameters::ADDITIONAL_PARAM1, v);
	}
#endif

	CATCH_GL_ERROR
}

void RAPTOR_FASTCALL CVertexProgram::glAdditionalVertexParam2(const GL_COORD_VERTEX &v)
{
#if defined(GL_ARB_vertex_program)
	if (CRaptorInstance::GetInstance().isVertexProgramReady())
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		pExtensions->glVertexAttrib4fvARB(CProgramParameters::ADDITIONAL_PARAM2, v);
	}
#endif

	CATCH_GL_ERROR
}

void RAPTOR_FASTCALL CVertexProgram::glPointParameter(float minSize, float maxSize, const GL_COORD_VERTEX& attenuation)
{
#if defined(GL_EXT_point_parameters)
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

	if (minSize >= 0.0)
		pExtensions->glPointParameterfEXT(GL_POINT_SIZE_MIN_EXT,minSize);
	if (maxSize >= 0.0)
		pExtensions->glPointParameterfEXT(GL_POINT_SIZE_MAX_EXT,maxSize);
	
	pExtensions->glPointParameterfvEXT(GL_DISTANCE_ATTENUATION_EXT,attenuation);

#endif

	CATCH_GL_ERROR
}


void CVertexProgram::glProgramParameter(unsigned int numParam, const GL_COORD_VERTEX &v) const
{
#if defined(GL_ARB_vertex_program)
	if (CRaptorInstance::GetInstance().isVertexProgramReady())
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		pExtensions->glProgramLocalParameter4fvARB(GL_VERTEX_PROGRAM_ARB, numParam, v);
	}
#endif

	CATCH_GL_ERROR
}

void CVertexProgram::glProgramParameter(unsigned int numParam, const CColor::RGBA &v) const
{
#if defined(GL_ARB_vertex_program)
	if (CRaptorInstance::GetInstance().isVertexProgramReady())
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		pExtensions->glProgramLocalParameter4fvARB(GL_VERTEX_PROGRAM_ARB, numParam, v);
	}
#endif

	CATCH_GL_ERROR
}

void CVertexProgram::glProgramParameter(unsigned int numParam, const CGenericVector<float> &v) const
{
#if defined(GL_ARB_vertex_program)
	if (CRaptorInstance::GetInstance().isVertexProgramReady())
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		pExtensions->glProgramLocalParameter4fvARB(GL_VERTEX_PROGRAM_ARB, numParam, v.vector());
	}
#endif

	CATCH_GL_ERROR
}




//
//	Vertex Shader management
//
void CVertexProgram::glRender(void)
{
	if (m_handle.glhandle() == 0)
		return;

#if defined(GL_ARB_vertex_program)
	if (CRaptorInstance::GetInstance().isVertexProgramReady())
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		if (pExtensions->glIsProgramARB(m_handle.glhandle()))
		{
			glEnable(GL_VERTEX_PROGRAM_ARB);

			pExtensions->glBindProgramARB(GL_VERTEX_PROGRAM_ARB, m_handle.glhandle());

			if (m_bApplyParameters)
			{
				for (unsigned int i=0;i<m_parameters.getNbParameters();i++)
				{
					const CProgramParameters::CParameterBase& param_value = m_parameters[i];
					const GL_COORD_VERTEX &vector = ((const CProgramParameters::CParameter<GL_COORD_VERTEX>&)param_value).p;
					pExtensions->glProgramLocalParameter4fvARB(	GL_VERTEX_PROGRAM_ARB, i, vector);
				}
				m_bApplyParameters = false;
			}
		}
	}
#endif

	CATCH_GL_ERROR
}

void CVertexProgram::glStop(void)
{
#if defined(GL_ARB_vertex_program)
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	pExtensions->glBindProgramARB(GL_VERTEX_PROGRAM_ARB,0);
	glDisable(GL_VERTEX_PROGRAM_ARB);
#endif

	CATCH_GL_ERROR
}

bool CVertexProgram::glLoadProgram(const std::string &program)
{
	m_bValid = false;
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

#if defined(GL_ARB_vertex_program)
	if (CRaptorInstance::GetInstance().isVertexProgramReady())
	{
		GLuint hProgram = m_handle.glhandle();
		if (pExtensions->glIsProgramARB(hProgram))
			pExtensions->glDeleteProgramsARB(1, &hProgram);
	}
#endif

	// default openGL vertex processing pipeline
	string ogl = "opengl";
    for (unsigned int i=0;i<program.size()&&i<6;i++)
	    ogl[i] = toupper(program[i]);

	if (ogl == "OPENGL")
	{
		m_handle.handle(0);
        m_bValid = true;
	}
#if defined(GL_ARB_vertex_program)
	else if (CRaptorInstance::GetInstance().isVertexProgramReady())
	{
        //!    In case of a previous error, we need to initialize error checking
        //!    to be sure that the error detected will only be due to shader loading.
        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
        {
			Raptor::GetErrorManager()->generateRaptorError(CVertexProgram::CVertexProgramClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_WARNING,
															"Raptor encountered errors before loading vertex shader, check with debug infos.");
            while (err != GL_NO_ERROR)
                err = ::glGetError();
        }

		GLuint hd = 0;
		pExtensions->glGenProgramsARB(1,&hd);
		pExtensions->glBindProgramARB(GL_VERTEX_PROGRAM_ARB,hd);
		m_handle.handle(hd);

		pExtensions->glProgramStringARB(GL_VERTEX_PROGRAM_ARB,
										GL_PROGRAM_FORMAT_ASCII_ARB,
										(GLsizei)program.size(),
										(void*)(program.data()));
		err = glGetError();
		
		if (err != GL_NO_ERROR)// && (err == GL_INVALID_OPERATION))
		{
			int pos = 0;
			glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB,&pos);
			const unsigned char * str = glGetString(GL_PROGRAM_ERROR_STRING_ARB);

            vector<CRaptorMessages::MessageArgument> args;
            CRaptorMessages::MessageArgument arg1;
            arg1.arg_int = pos;
            args.push_back(arg1);
            CRaptorMessages::MessageArgument arg2;
            arg2.arg_sz = (const char*)str;
            args.push_back(arg2);

			Raptor::GetErrorManager()->generateRaptorError(CVertexProgram::CVertexProgramClassID::GetClassId(),
                                                            CRaptorErrorManager::RAPTOR_ERROR,
											                CRaptorMessages::ID_PROGRAM_ERROR,
															__FILE__, __LINE__, args); 
		}

		m_bValid = ((err == GL_NO_ERROR) && glGetProgramStatus());
		if (!m_bValid)
		{
			RAPTOR_WARNING(CVertexProgram::CVertexProgramClassID::GetClassId(),
						   CRaptorMessages::ID_NO_GPU_PROGRAM)
			RAPTOR_WARNING(CVertexProgram::CVertexProgramClassID::GetClassId(),
						   getName().data())
		}
		
		// Unbind program to avoid side effects
		glStop();
	}
#endif

	CATCH_GL_ERROR

	return m_bValid;
}

std::string CVertexProgram::glGetProgramString(void) const
{
	if (m_handle.glhandle() == 0)
		return "";

	if (!CRaptorInstance::GetInstance().isVertexProgramReady())
		return "";

#if defined(GL_ARB_vertex_program)
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

	if (pExtensions->glIsProgramARB(m_handle.glhandle()))
		pExtensions->glBindProgramARB(GL_VERTEX_PROGRAM_ARB, m_handle.glhandle());
	else
		return "";

	int value = 0;
	pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_LENGTH_ARB, &value);
	char *source = new char[value];
	pExtensions->glGetProgramStringARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_STRING_ARB, source);

	std::string program_source = source;
	delete[] source;
	return program_source;
#else
	return "";
#endif
}

bool CVertexProgram::glGetProgramStatus(void) const
{
	if (m_handle.glhandle() == 0)
		return false;

	if (!CRaptorInstance::GetInstance().isVertexProgramReady())
		return false;

#if defined(GL_ARB_vertex_program)
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

	if (pExtensions->glIsProgramARB(m_handle.glhandle()))
		pExtensions->glBindProgramARB(GL_VERTEX_PROGRAM_ARB, m_handle.glhandle());
	else
		return false;

	int value = 0;
	pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB, &value);
	if (value < 1)
	{
		GL_VERTEX_PROGRAM_CAPS caps;
		if (glGetProgramCaps(caps))
		{
			pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_INSTRUCTIONS_ARB, &value);
			if (value > caps.max_instructions)
				return false;

			pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_NATIVE_INSTRUCTIONS_ARB, &value);
			if (value > caps.max_native_instructions)
				return false;

			pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_TEMPORARIES_ARB, &value);
			if (value > caps.max_temporaries)
				return false;

			pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_NATIVE_TEMPORARIES_ARB, &value);
			if (value > caps.max_native_temporaries)
				return false;

			pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_PARAMETERS_ARB, &value);
			if (value > caps.max_parameters)
				return false;

			pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_NATIVE_PARAMETERS_ARB, &value);
			if (value > caps.max_native_parameters)
				return false;

			pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_ATTRIBS_ARB, &value);
			if (value > caps.max_attribs)
				return false;

			pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_NATIVE_ATTRIBS_ARB, &value);
			if (value > caps.max_native_attribs)
				return false;

			pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_ADDRESS_REGISTERS_ARB, &value);
			if (value > caps.max_address_registers)
				return false;

			pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB, &value);
			if (value > caps.max_native_address_registers)
				return false;

			pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB, &value);
			if (value < 1)
				return false;
			return true;
		}
		else
			return false;
	}
	else
#endif
		return true;
}

bool CVertexProgram::glGetProgramCaps(GL_VERTEX_PROGRAM_CAPS& caps)
{
	if (CRaptorInstance::GetInstance().isVertexProgramReady())
	{
#if defined(GL_ARB_vertex_program)
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

		pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB,GL_MAX_PROGRAM_INSTRUCTIONS_ARB,&caps.max_instructions);
		pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB,GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB,&caps.max_native_instructions);
		pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB,GL_MAX_PROGRAM_TEMPORARIES_ARB,&caps.max_temporaries);
		pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB,GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB,&caps.max_native_temporaries);
		pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB,GL_MAX_PROGRAM_PARAMETERS_ARB,&caps.max_parameters);
		pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB,GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB,&caps.max_local_parameters);
		pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB,GL_MAX_PROGRAM_ENV_PARAMETERS_ARB,&caps.max_env_parameters);
		pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB,GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB,&caps.max_native_parameters);
		pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB,GL_MAX_PROGRAM_ATTRIBS_ARB,&caps.max_attribs);
		pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB,GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB,&caps.max_native_attribs);
		pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB,GL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB,&caps.max_address_registers);
		pExtensions->glGetProgramivARB(GL_VERTEX_PROGRAM_ARB,GL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB,&caps.max_native_address_registers);
		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS_ARB,&caps.max_vertex_attribs);
		glGetIntegerv(GL_MAX_PROGRAM_MATRICES_ARB,&caps.max_program_matrices);
		glGetIntegerv(GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB,&caps.max_program_matrix_stack_depth);
        GLboolean bVal = GL_FALSE;
        glGetBooleanv(GL_VERTEX_PROGRAM_POINT_SIZE_ARB,&bVal);
        caps.vertex_program_point_size = (bVal == GL_TRUE);
        glGetBooleanv(GL_VERTEX_PROGRAM_TWO_SIDE_ARB,&bVal);
        caps.vertex_program_two_side = (bVal == GL_TRUE);

		return true;
#else
		return false;
#endif
	}
	else
		return false;
}

