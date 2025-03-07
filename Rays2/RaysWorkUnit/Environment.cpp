// Environment.cpp: implementation of the CEnvironment class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Subsys/CodeGeneration.h"

#include "RaysWorkUnit.h"
#include "Environment.h"
#include "Raytracer.h"
#include "Texture.h"


CEnvironment	*CEnvironment::m_pEnvironment = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEnvironment::CEnvironment():
	m_pEnvironMap(NULL)
{
	tmpClr.r = 0;
	tmpClr.g = 0;
	tmpClr.b = 0;
	tmpClr.a = 1.0;
}

CEnvironment::~CEnvironment()
{

}


CEnvironment* CEnvironment::GetInstance(void)
{
	if (m_pEnvironment == NULL)
	{
		m_pEnvironment = new CEnvironment();
	}

	return m_pEnvironment;
}

CColor::RGBA& CEnvironment::GetLocalColor(const CGenericVector<float>	&direction)
{
	if (m_pEnvironMap != NULL)
	{
		float	theta;

		theta = (float)sqrt((direction.X() * direction.X()) + (direction.Z() * direction.Z()));
		theta = (float)(atan2(direction.Y() , theta));

		tmpVect.Y() = (float)(UN_SUR_2_PI * (PI + theta));
		
		theta = (float)atan2(direction.Z(),direction.X());
		tmpVect.X() = (float)(UN_SUR_2_PI * (PI + theta));

		tmpVect.X() *= 2;
		tmpVect.Y() *= 2;

		tmpClr = m_pEnvironMap->GetMapColor(tmpVect);
	}

	return tmpClr;
}