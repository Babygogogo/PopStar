#include "TitleMenuScript.h"

//////////////////////////////////////////////////////////////////////////
//Definition of TitleMenuScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct TitleMenuScript::TitleMenuScriptImpl
{
	TitleMenuScriptImpl();
	~TitleMenuScriptImpl();
};

TitleMenuScript::TitleMenuScriptImpl::TitleMenuScriptImpl()
{
}

TitleMenuScript::TitleMenuScriptImpl::~TitleMenuScriptImpl()
{
}

//////////////////////////////////////////////////////////////////////////
//Implementation of TitleMenuScript.
//////////////////////////////////////////////////////////////////////////
TitleMenuScript::TitleMenuScript() : pimpl{ std::make_unique<TitleMenuScriptImpl>() }
{
}

TitleMenuScript::~TitleMenuScript()
{
}

const std::string & TitleMenuScript::getType() const
{
	return Type;
}

const std::string TitleMenuScript::Type = "TitleMenuScript";
