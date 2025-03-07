/***************************************************************************/
/*                                                                         */
/*  CmdLineParser.cpp                                                      */
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

#if !defined(AFX_CMDLINEPARSER_H__D7D8768A_3D97_491F_8493_588972A3CF62__INCLUDED_)
	#include "CmdLineParser.h"
#endif



CCmdLineParser::CCmdLineParser(void)
{
}

CCmdLineParser::~CCmdLineParser(void)
{
	for (unsigned int i=0;i<m_options.size();i++)
		delete m_options[i];
}

bool CCmdLineParser::removeOption(const std::string &name)
{
	for (unsigned int o = 0; o<m_options.size(); o++)
	{
		CCommandLineOption* cmdline = m_options[o];
		if ((cmdline->getName() == name) || (cmdline->getShort() == name))
		{
			CCommandLineOption* last = m_options.back();
			m_options[o] = last;
			m_options.pop_back();
			return true;
		}
	}

	return false;
}

bool CCmdLineParser::parse(int argc, char *argv[])
{
	int i = 0;
	while (i+1 < argc)
	{
		const char *arg = argv[i];
		if (arg[0] == '-')
		{
			arg = arg + 1;

			if (arg[0] == '-')
			{
				arg = arg + 1;
				for (unsigned int o=0;o<m_options.size();o++)
				{
					if (m_options[o]->getName() == arg)
					{
						i++;
						m_options[o]->parse(argv[i]);
						break;
					}
				}
			}
			else for (unsigned int o=0;o<m_options.size();o++)
			{
				if (m_options[o]->getShort() == arg)
				{
					i++;
					m_options[o]->parse(argv[i]);
					break;
				}
			}
		}
		i++;
	}

	return true;
}

bool CCmdLineParser::setValue(const std::string& optionName, const char* str_value)
{
	for (unsigned int o = 0; o<m_options.size(); o++)
	{
		CCommandLineOption* cmdline = m_options[o];
		if ((cmdline->getName() == optionName) ||
			(cmdline->getShort() == optionName))
		{
			//!	Need to avoid cast to check improper type.
			cmdline->parse(str_value);

			return true;
		}
	}

	return false;
}


template <>
CCmdLineParser::CCommandLineOptionValue<const char*>::CCommandLineOptionValue(const std::string &name,
																			  const std::string &shortname,
																			  const char* defaultValue)
	:CCmdLineParser::CCommandLineOption(name, shortname), m_value(STRDUP(defaultValue))
{
}

template <>
CCmdLineParser::CCommandLineOptionValue<const char*>::~CCommandLineOptionValue()
{
	if (m_value != NULL)
		free((void*)m_value);
}

template <>
CCmdLineParser::CCommandLineOptionValue<char*>::CCommandLineOptionValue(const std::string &name,
																		const std::string &shortname,
																		char* defaultValue)
	:CCmdLineParser::CCommandLineOption(name, shortname), m_value(NULL)
{
	m_value = STRDUP(defaultValue);
}

template <>
CCmdLineParser::CCommandLineOptionValue<char*>::~CCommandLineOptionValue()
{
	if (m_value != NULL)
		free((void*)m_value);
}

template <>
bool CCmdLineParser::CCommandLineOptionValue<unsigned int>::parse(const char* argv)
{
	if (NULL == argv)
		return false;

	m_value = atoi(argv);
	return true;
}

template <>
bool CCmdLineParser::CCommandLineOptionValue<unsigned short>::parse(const char* argv)
{
	if (NULL == argv)
		return false;

	m_value = (unsigned short)(0xffff & atoi(argv));
	return true;
}

template <>
bool CCmdLineParser::CCommandLineOptionValue<const char*>::parse(const char* argv)
{
	if (NULL == argv)
		return false;

	if (m_value != NULL)
		free((void*)m_value);

	m_value = STRDUP(argv);
	return true;
}

template <>
bool CCmdLineParser::CCommandLineOptionValue<char*>::parse(const char* argv)
{
	if (NULL == argv)
		return false;

	if (m_value != NULL)
		free((void*)m_value);

	m_value = STRDUP(argv);
	return true;
}

template <>
bool CCmdLineParser::CCommandLineOptionValue<std::string>::parse(const char* argv)
{
	if (NULL == argv)
		return false;

	m_value = std::string(argv);
	return true;
}

template <>
bool CCmdLineParser::CCommandLineOptionValue<std::vector<unsigned int>>::parse(const char* argv)
{
	if (NULL == argv)
		return false;

	m_value.push_back((unsigned int)(0xffff & atoi(argv)));
	return true;
}

template <>
bool CCmdLineParser::CCommandLineOptionValue<std::vector<std::string>>::parse(const char* argv)
{
	if (NULL == argv)
		return false;

	m_value.push_back(std::string(argv));
	return true;
}

template <>
bool CCmdLineParser::CCommandLineOptionValue<CCmdLineParser::NO_VALUE_OPTION_t>::parse(const char* argv)
{
	if (NULL == argv)
		return false;

	m_value = CCmdLineParser::NO_VALUE_VALUE;

	return true;
}