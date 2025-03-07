/***************************************************************************/
/*                                                                         */
/*  GLBuilder.h                                                            */
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



#if !defined(AFX_GLBUILDER_H__0F2DA94E_7293_494C_B936_8CB72098E626__INCLUDED_)
#define AFX_GLBUILDER_H__0F2DA94E_7293_494C_B936_8CB72098E626__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>
using std::string;
using std::vector;

#include "BuilderNative.h"

class CGLBuilder  
{
public:
	typedef struct EXTENSION_TAG
	{
		EXTENSION_KIND	kind;
		bool			active;
		std::string		extensionName;
		std::vector<std::string>  dependencies;
	} EXTENSION;


	static const char* core_profile;
	static const char* compatibility_profile;
	static const char* full_profile;


public:
	CGLBuilder();
	virtual ~CGLBuilder();

	const vector<EXTENSION>& getExtensions(void) const { return extensions; };

	//!	Activates all registered extensions.
	void activateAll(void);
	
	//!	Deactivates all registered extensions.
	void activateNone(void);

	void activateDebug(bool activate) { m_buildDebug = activate; };
	void activateRedist(bool activate) { m_buildRedist = activate; };
    void activateCompute(bool activate) { m_buildCompute = activate; };
	void activatePhysX(bool activate) { m_buildPhysX = activate; };

	//!	Returns false if extension not found or if error.
	bool activateExtension(const string &extension,bool activate);

	//!	Returns false if extension not found or if not active
	bool isExtensionActive(const string &extension) const;

	//! A Valid OGL Context must be active and the string of extensions
	//! has to be queried via glGetString and then passed to this method
	void glQueryExtensions(const string &strExtensions);

	//!	Specifies the version of OpenGL required and returns
	//!	a string representing the versions as if they were extensions
	string setVersion(const string &strVersion);

    //! This method checks the consistency of the set of extensions.
	//! It should be called prior to any header generation because extensions settings are modified if needed.
    //! if forceDependency is true, all dependent extensions are forced, even if not actually supported by hardware.
    //! otherwise, an error is returned.
    //! Return : true if no consistency error is found.
	bool checkConsistency(bool forceDependency);

	//! Generates the extension header file
	bool writeHeader(const string& filename);



private:
	//!	Helper to register Raptor extensions.
	void addExtension(EXTENSION_KIND kind, const std::string& extensionName);

	//!	Helper to register Raptor extensions.
	void addExtension(EXTENSION_KIND kind, const std::string& extensionName, const std::string& extensionDependency);

	//!	Helper to register Raptor extensions.
	void addExtension(EXTENSION_KIND kind, const std::string& extensionName, const std::string& extensionDependency, const std::string& extensionDependency2);


	std::vector<EXTENSION>	extensions;

    bool	m_buildDebug;
    bool	m_buildRedist;
    bool	m_buildCompute;
	bool	m_buildPhysX;

	int		majorVersion;
	int		minorVersion;
	int		releaseNumber;
};

#endif // !defined(AFX_GLBUILDER_H__0F2DA94E_7293_494C_B936_8CB72098E626__INCLUDED_)
