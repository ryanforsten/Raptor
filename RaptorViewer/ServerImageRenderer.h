#if !defined(AFX_SERVERIMAGERENDERER_H__C9720F3B_1B29_482C_84C8_1A990CEC0EBD__INCLUDED_)
#define AFX_SERVERIMAGERENDERER_H__C9720F3B_1B29_482C_84C8_1A990CEC0EBD__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#if !defined(AFX_OBJECT3D_H__DB24F017_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "GLHierarchy/Object3D.h"
#endif
#if !defined(AFX_RAPTORINSTANCE_H__D5297BB6_098A_4082_96AA_36A78E76F18C__INCLUDED_)
	#include "RaptorNetwork/RaptorNetwork.h"
#endif
#if !defined(AFX_BASICOBJECTS_H__2E0B717D_862A_4096_9D99_8EE0654F4678__INCLUDED_)
	#include "ToolBox/BasicObjects.h"
#endif

RAPTOR_NAMESPACE_BEGIN
class ITextureObject;
RAPTOR_NAMESPACE_END

RAPTOR_NAMESPACE



class CServerImageRenderer : public CBasicObjects::CRectangle
{
public:
	CServerImageRenderer(size_t width,size_t height);
	virtual ~CServerImageRenderer(void);

	bool hasImagesAvailable(void) const;

	void glInitImage();

	void glUpdateImage();

	void setImageData(CRaptorNetwork::IMAGE_COMMAND *);


private:
	CServerImageRenderer();
	virtual void glClipRender(void);
	
	CRaptorMutex	tMutex;

	ITextureObject	*m_pImage;
	size_t			m_serverWidth;
	size_t			m_serverHeight;

	std::vector<CRaptorNetwork::IMAGE_COMMAND *> m_pImageDatas;
	
};


#endif // !defined(AFX_SERVERIMAGERENDERER_H__C9720F3B_1B29_482C_84C8_1A990CEC0EBD__INCLUDED_)

