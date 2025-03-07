// WMVStreaming.h: interface for the CwMVStreaming class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WMVSTREAMING_H__14310AA1_ECB0_48C7_B69F_C0816C83B1CC__INCLUDED_)
#define AFX_WMVSTREAMING_H__14310AA1_ECB0_48C7_B69F_C0816C83B1CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_ANIMATOR_H__077150E3_D826_11D3_9142_9866F8B4457F__INCLUDED_)
    #include "Engine/Animator.h"
#endif

#if defined(_WIN32)
    //	non standard structures used
    #pragma warning( disable : 4201 )  
	#include <wmsdk.h>
#endif



RAPTOR_NAMESPACE

#if defined(_WIN32)
class CWMVStreaming : public CAnimator::IVideoIO
{
public:
    CWMVStreaming()
        :frameLength(0),streamPos(0),locked(false),m_pReader(NULL),m_pBuffer(NULL),
		m_width(0),m_height(0),m_wNumStream(0),lastFrameTime(0){};
    virtual ~CWMVStreaming() {};

	virtual std::string getKind(void) const { return "WMV"; };
	virtual unsigned int getWidth() const { return m_width; };
	virtual unsigned int getHeight() const { return m_height; };
    virtual float getFrameLength() const { return frameLength; };

    virtual CAnimator::IVideoIO*   newInstance(void) const { return new CWMVStreaming; };
	virtual bool releaseInstance(void) { delete this; return true;};

	virtual bool openReader(const std::string &fname);
    virtual bool readFrame(unsigned char *& readBuffer);
    virtual void seekFrame(unsigned int framePos);
    virtual unsigned int getFramePosition(void) const { return streamPos; };
    virtual bool closeReader(void);

    virtual bool openRecorder(const std::string& fname,unsigned int width,unsigned int height);
    virtual bool writeFrame(const unsigned char *writeBuffer);
    virtual bool closeRecorder(void);

private:
    float			frameLength;
	float			lastFrameTime;
    LONG			streamPos;
    bool			locked;
	IWMSyncReader*  m_pReader;
	INSSBuffer*		m_pBuffer;
	WORD			m_wNumStream;

	unsigned int	m_width;
	unsigned int	m_height;
};
#endif


#endif	// !defined(AFX_WMVSTREAMING_H__14310AA1_ECB0_48C7_B69F_C0816C83B1CC__INCLUDED_)


