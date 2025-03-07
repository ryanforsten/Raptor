// TestDoc.h : interface of the CTestDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTDOC_H__864FB149_93D7_4EE3_BCFE_7CE132610911__INCLUDED_)
#define AFX_TESTDOC_H__864FB149_93D7_4EE3_BCFE_7CE132610911__INCLUDED_

#include "System/RenderEntryPoint.h"
#include "Raptordll.h"

RAPTOR_NAMESPACE

class CTestDoc : public  CRenderEntryPoint
{
public:
    CTestDoc(const RAPTOR_HANDLE&,CRaptorDisplay*);
    virtual ~CTestDoc();

	void GLInitContext(void);

    virtual void glRender();


protected:
    CRaptorDisplay	*m_pDisplay;
    RAPTOR_HANDLE	m_wnd;
};

#endif // !defined(AFX_TESTDOC_H__864FB149_93D7_4EE3_BCFE_7CE132610911__INCLUDED_)
