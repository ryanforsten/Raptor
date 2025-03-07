// 3DSceneObject.h: interface for the C3DSceneObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_3DSCENEOBJECT_H__96A34268_AD58_4F73_B633_F6C3E92FE0A9__INCLUDED_)
#define AFX_3DSCENEOBJECT_H__96A34268_AD58_4F73_B633_F6C3E92FE0A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif
#if !defined(AFX_LIGHTATTRIBUTES_H__B0A3AF95_90DC_4185_9747_B7F631DDB2BF__INCLUDED_)
	#include "LightAttributes.h"
#endif
#if !defined(AFX_LIGHT_H__AA8BABD6_059A_4939_A4B6_A0A036E12E1E__INCLUDED_)
	#include "GLHierarchy/Light.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CLight;
RAPTOR_INTERFACE IRaptorPipeline;
class CObject3D;
class CVulkanCommandBuffer;
class CShader;


class C3DSceneObject  
{
public:
    //! Max number of supported rendering passes
	enum { NB_PASSES = 8 };


	C3DSceneObject(CObject3D* obj);

	//! Destructor.
	virtual ~C3DSceneObject();

	//!	Returns the 'real' embedded objet.
	CObject3D* getObject(void) const;

    //! this method renders the real object using the previous occlusion query
    //! Up to NB_PASSES are managed.
    bool glRenderPass(	unsigned int passNumber,
						const vector<CLight*> &lights,
						GLboolean proceedLights);

	void vkRender(	CVulkanCommandBuffer& commandBuffer,
					VkBuffer vertexBinding,
					VkBuffer indexBinding);

    //! This method chooses the appropriate light for self object rendering.
	//! The lights selected are the first and less then CLightAttributes::MAX_LIGHTS lights
	//! that contribute to shading taking into account attenuation (> 1/256 rgba)
    void selectLights(const vector<CLight*> &lights,const CGenericMatrix<float>& transform);

    //! This method renders the selected lights.
    //! - The list in parameter is the list of all lights that will be switched on or off, depending on the 
    //!   list of effective lights for this object.
	//!	- selectLights must have been called for current rendering.
	void glRenderLights(GLboolean proceedLights,
						const vector<CLight*> &lights);

	//! This method renders the selected lights into a shader buffer at provided offset.
	//!	- selectLights must have been called for current rendering.
	size_t glRenderLights(CLight::R_LightProducts *buffer, uint8_t* uniform, bool proceedLights);

	//! This method renders initial occlusion bbox 
	void glRenderBBoxOcclusion(unsigned int passNumber);


	//!	Specific object ordering
	struct zorder
	{
		bool operator()(C3DSceneObject* const x, C3DSceneObject* const y) const
		{
			if (x->z_order == y->z_order)
				return x->object.handle() < y->object.handle();
			else
				return (x->z_order < y->z_order); 
		};
	};

	//!	storage for z_ordering	( need multipass ? )
	float			z_order;
	float			z_span;

	//!	The object bounding box buffer index.
	uint64_t bbox;

	//!	active lights for next rendering
	CLight          *effectiveLights[CLightAttributes::MAX_LIGHTS];
	
	//!	Occlusion queries
	unsigned int	visibilityQuery[NB_PASSES];
	int				passVisibility[NB_PASSES];

	typedef struct
	{
		uint8_t		*uniform;
		uint64_t	bufferOffset;
		CShader		*shader;
		size_t		index;
	} lightShaderbloc;
	std::vector<lightShaderbloc> lightShaders;

	size_t initShaders(size_t base);


private:
	//!	Default constructor.
	C3DSceneObject();

	//!	The 'real' embedded object
	RAPTOR_HANDLE	object;

	//!	Object's rendering pipeline
	IRaptorPipeline *m_pPipeline;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_3DSCENEOBJECT_H__96A34268_AD58_4F73_B633_F6C3E92FE0A9__INCLUDED_)

